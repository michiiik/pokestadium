#!/usr/bin/env python3
"""Match a shared C file against the JP ROM and move it into the JP build.

The file is compiled for a version and its .text/.data/.rodata are searched for
in that version's ROM with relocated fields masked. Every relocated field is
then read back from the ROM, which gives the address of each symbol the file
references. The file matches when all sections are found and every symbol gets
one consistent address.

  list                  candidate files and their state
  check FILE [VERSION]  check FILE against jp (default) or us, with a diff on failure
  apply FILE [--keep]   check jp and us, then put FILE into the JP build and
                        verify the ROM checksum, rolling back on failure
                        unless --keep

Run from the repo root with the venv python and make on PATH.
"""
import fcntl
import os
import re
import shutil
import struct
import subprocess
import sys
import tempfile
from pathlib import Path

import rabbitizer
import yaml

ROOT = Path.cwd()
CROSS = os.environ.get("CROSS", "mips-linux-gnu-")
YAML = {v: ROOT / f"yamls/{v}/rom.yaml" for v in ("us", "jp")}
C_LD = ROOT / "linker_scripts/jp/c_files.ld"
RESTORE = ROOT / "build/jp_match_restore"
TRACKED = (ROOT / "yamls/jp/rom.yaml", ROOT / "linker_scripts/jp/c_files.ld",
           ROOT / "linker_scripts/jp/undefined_syms.ld")
SECTIONS = (".text", ".data", ".rodata")
R_32, R_26, R_HI16, R_LO16 = 2, 4, 5, 6
MASK = {R_32: 0xFFFFFFFF, R_26: 0x03FFFFFF, R_HI16: 0xFFFF, R_LO16: 0xFFFF}


class Fail(Exception):
    pass


def run(*cmd, quiet=True):
    if cmd[0] == "make":
        cmd = (*cmd, f"PYTHON={sys.executable}")
    r = subprocess.run(cmd, capture_output=True, text=True)
    if r.returncode and not quiet:
        print(r.stdout[-3000:], r.stderr[-3000:])
    return r


def stem(src):
    return re.sub(r"^src/|\.c$", "", str(src))


def prefix(src):
    return "jpc_" + re.sub(r"\W", "_", "src/" + stem(src)) + "__"


# ---------------------------------------------------------------- inputs

def compile_obj(src, version):
    obj = ROOT / "build" / "src" / (stem(src) + ".o")
    obj.unlink(missing_ok=True)
    r = run("make", f"VERSION={version}", "RUN_CC_CHECK=0", str(obj.relative_to(ROOT)))
    if r.returncode or not obj.exists():
        raise Fail("compile failed:\n" + (r.stdout + r.stderr)[-2000:])
    out = Path(tempfile.mkdtemp()) / obj.name
    shutil.copy(obj, out)
    obj.unlink()
    return out


def read_elf(path):
    d = path.read_bytes()
    shoff = struct.unpack_from(">I", d, 0x20)[0]
    shentsize, shnum, shstrndx = struct.unpack_from(">HHH", d, 0x2E)
    secs = []
    for i in range(shnum):
        f = struct.unpack_from(">10I", d, shoff + i * shentsize)
        secs.append({"name": f[0], "type": f[1], "off": f[4], "size": f[5], "link": f[6], "info": f[7]})

    def cstr(sec, o):
        s = sec["off"] + o
        return d[s:d.index(b"\0", s)].decode()

    for s in secs:
        s["name"] = cstr(secs[shstrndx], s["name"]).replace(".jpcbss", ".bss")
        s["data"] = d[s["off"]:s["off"] + s["size"]] if s["type"] != 8 else b""
    symtab = next(s for s in secs if s["type"] == 2)
    syms = []
    for i in range(symtab["size"] // 16):
        n, val, size, info, other, shndx = struct.unpack_from(">IIIBBH", d, symtab["off"] + i * 16)
        name = cstr(secs[symtab["link"]], n) if n else (secs[shndx]["name"] if info & 15 == 3 else "")
        syms.append({"name": name, "value": val, "size": size, "shndx": shndx, "func": info & 15 == 2})
    rels = {}
    for s in secs:
        if s["type"] == 9:
            rels[secs[s["info"]]["name"]] = [(o, i & 0xFF, i >> 8) for o, i in struct.iter_unpack(">II", s["data"])]
    return {s["name"]: s for s in secs}, secs, syms, rels


def load_groups(version):
    segs = yaml.safe_load((ROOT / f"yamls/{version}/header.yaml").read_text()
                          + YAML[version].read_text())["segments"]
    tops = []
    for g in segs:
        if isinstance(g, dict):
            tops.append((g["start"], g.get("vram"), g.get("name")))
        elif isinstance(g, list):
            tops.append((g[0], None, None))
    out = []
    for (s, v, n), (e, *_) in zip(tops, tops[1:]):
        if v is not None and e > s:
            out.append((s, e, v, n))
    return out


def yaml_hint(version, name):
    """rom offsets of the entries named after the file, by section class."""
    hint = {}
    cls = {"c": ".text", "asm": ".text", ".data": ".data", "data": ".data",
           ".rodata": ".rodata", "rodata": ".rodata"}
    for m in re.finditer(r"- \[0x([0-9A-Fa-f]+),\s*([.\w]+),\s*([^\],\s]+)", YAML[version].read_text()):
        if m.group(3) == name and m.group(2) in cls:
            hint.setdefault(cls[m.group(2)], int(m.group(1), 16))
    return hint


# ---------------------------------------------------------------- matching

def pattern(data, rels):
    mask = bytearray(b"\xff" * len(data))
    for off, typ, _ in rels:
        if typ in MASK:
            m = MASK[typ].to_bytes(4, "big")
            for k in range(4):
                mask[off + k] &= ~m[k] & 0xFF
    parts = []
    for b, m in zip(data, mask):
        if m == 0xFF:
            parts.append(re.escape(bytes([b])))
        elif m == 0:
            parts.append(b".")
        else:
            lo = b & m
            parts.append(b"[" + re.escape(bytes([lo])) + b"-" + re.escape(bytes([lo | (~m & 0xFF)])) + b"]")
    return re.compile(b"".join(parts), re.S), mask


def find_all(rx, rom, lo=0, hi=None):
    hi = len(rom) if hi is None else hi
    pos, out = lo, []
    while True:
        m = rx.search(rom, pos, hi)
        if not m:
            return out
        if m.start() % 4 == 0:
            out.append(m.start())
        pos = m.start() + 1


def group_of(groups, off):
    return next((g for g in groups if g[0] <= off < g[1]), None)


def candidates(sec, rels, rom, groups, hint, after=None):
    rx, _ = pattern(sec["data"], rels)
    if after:
        g = group_of(groups, after)
        hits = find_all(rx, rom, after, g[1] if g else None)
    else:
        hits = find_all(rx, rom)
    ref = hint if hint is not None else (after or 0)
    return sorted(hits, key=lambda h: abs(h - ref))


def own_sec(sym, secs):
    return secs[sym["shndx"]]["name"] if 0 < sym["shndx"] < len(secs) else None


def mode(xs):
    return max(set(xs), key=xs.count)


def sext16(x):
    return x - 0x10000 if x & 0x8000 else x


def derive(loc, objsecs, rels, syms, rom, groups):
    """Address of every referenced symbol, read back from the ROM."""
    vals, lo_checks = {}, []

    def note(sym, v, where):
        vals.setdefault(sym, []).append((v & 0xFFFFFFFF, where))

    for sname, (r, vram) in loc.items():
        pending = {}
        for off, typ, si in rels.get(sname, []):
            o = objsecs[sname]["data"]
            ow = struct.unpack_from(">I", o, off)[0]
            rw = struct.unpack_from(">I", rom, r + off)[0]
            where = f"{sname}+0x{off:X}"
            if typ == R_32:
                note(si, rw - ow, where)
            elif typ == R_26:
                note(si, (((rw & 0x3FFFFFF) << 2) | ((vram + off) & 0xF0000000)) - ((ow & 0x3FFFFFF) << 2), where)
            elif typ == R_HI16:
                pending.setdefault(si, []).append((ow, rw, where))
            elif typ == R_LO16:
                if pending.get(si):
                    for hi_o, hi_r, w in pending.pop(si):
                        a_obj = ((hi_o & 0xFFFF) << 16) + sext16(ow & 0xFFFF)
                        a_rom = ((hi_r & 0xFFFF) << 16) + sext16(rw & 0xFFFF)
                        note(si, a_rom - a_obj, w)
                else:
                    lo_checks.append((si, ow, rw, where))
            else:
                raise Fail(f"unsupported relocation type {typ} at {where}")
    return vals, lo_checks


def check(src, version="jp", quiet=False):
    src = Path(src)
    name = stem(src)
    text = src.read_text(errors="replace")
    if re.search(r"GLOBAL_ASM|INCLUDE_ASM", text):
        raise Fail("file has GLOBAL_ASM functions, match those first")
    obj = compile_obj(src, version)
    objsecs, secs, syms, rels = read_elf(obj)
    rom = (ROOT / f"baseroms/{version}/baserom.z64").read_bytes()
    groups = load_groups(version)
    hint = yaml_hint(version, name)

    def implied(locd):
        vals, _ = derive(locd, objsecs, rels, syms, rom, groups)
        out = {}
        for si, vs in vals.items():
            own = own_sec(syms[si], secs)
            if own in (".text", ".data", ".rodata", ".bss"):
                out.setdefault(own, []).extend(v - syms[si]["value"] for v, _ in vs)
        return out

    def vram_of(r):
        g = group_of(groups, r)
        return g[2] + (r - g[0])

    loc, after, known = {}, None, {}
    for sname in SECTIONS:
        sec = objsecs.get(sname)
        if not sec or not sec["size"]:
            continue
        hits = candidates(sec, rels.get(sname, []), rom, groups, hint.get(sname), after)
        if not hits:
            ref = hint.get(sname, after)
            if sname == ".text":
                text_report(sec, objsecs, secs, rels.get(sname, []), rom, groups, ref, syms)
            else:
                diff(sname, sec, rels.get(sname, []), rom, groups, ref, syms)
            raise Fail(f"{sname} not found in the {version} ROM")
        r = hits[0]
        if len(hits) > 1 and rels.get(sname):
            # mostly-relocation sections (jump tables) match in many places,
            # so pick the one whose addresses agree with what is known
            def score(h):
                imp = implied({sname: (h, vram_of(h))})
                return sum(bs.count(known.get(s, mode(bs))) for s, bs in imp.items())
            r = max(hits[:300], key=lambda h: (score(h), -hits.index(h)))
        loc[sname] = (r, vram_of(r))
        for s2, bs in implied(loc).items():
            known[s2] = mode(bs)
        after = r + sec["size"]

    # a constant vram offset across sections means the yaml group start is off
    group_fix = None
    imp = implied(loc)
    deltas = {(mode(imp[s]) - v) & 0xFFFFFFFF for s, (r, v) in loc.items() if s in imp}
    groups_hit = {group_of(groups, r)[0] for r, v in loc.values()}
    if len(deltas) == 1 and deltas != {0} and len(groups_hit) == 1:
        d = deltas.pop()
        d = d - (1 << 32) if d & 0x80000000 else d
        gstart = groups_hit.pop()
        if abs(d) <= 0x1000:
            group_fix = (gstart, gstart - d)
            loc = {s: (r, v + d) for s, (r, v) in loc.items()}
        else:
            r, v = next(iter(loc.values()))
            raise Fail(f"the code matches at rom 0x{r:X}, but its references put it at vram "
                       f"0x{(v + d) & 0xFFFFFFFF:08X} while the yaml group there gives 0x{v:08X}. "
                       "The JP yaml groups around this file are misplaced; skip this file.")

    vals, lo_checks = derive(loc, objsecs, rels, syms, rom, groups)
    bases = {s: v for s, (r, v) in loc.items()}
    externs, bss, bad = {}, set(), []
    for si, vs in vals.items():
        sym = syms[si]
        own = own_sec(sym, secs)
        got = {v - sym["value"] if own else v for v, _ in vs}
        if len(got) > 1:
            bad.append(f"  {sym['name'] or own}: {', '.join(f'0x{v:08X} at {w}' for v, w in vs[:4])}")
            continue
        v = got.pop()
        if own in bases:
            if v != bases[own]:
                bad.append(f"  {sym['name'] or own}: refs imply {own} at 0x{v:08X}, found at 0x{bases[own]:08X}")
        elif own == ".bss":
            bss.add(v)
        elif own is None:
            externs[sym["name"]] = v
        else:
            bad.append(f"  {sym['name']}: unexpected section {own}")
    for si, ow, rw, where in lo_checks:
        sym = syms[si]
        own = secs[sym["shndx"]]["name"] if 0 < sym["shndx"] < len(secs) else None
        base = bases.get(own) if own in bases else (next(iter(bss)) if own == ".bss" and bss else externs.get(sym["name"]))
        if base is not None and ((base + (sym["value"] if own else 0) + sext16(ow & 0xFFFF)) - sext16(rw & 0xFFFF)) & 0xFFFF:
            bad.append(f"  {sym['name'] or own}: %lo mismatch at {where}")
    if len(bss) > 1:
        bad.append("  .bss: references imply different addresses " + ", ".join(f"0x{b:08X}" for b in bss))
    if bad:
        raise Fail("addresses are inconsistent, so the code differs:\n" + "\n".join(bad[:30]))
    result = {"name": name, "group_fix": group_fix,
              "loc": {s: (r, v, objsecs[s]["size"]) for s, (r, v) in loc.items()},
              "bss": next(iter(bss), None), "bss_size": objsecs[".bss"]["size"] if ".bss" in objsecs else 0,
              "externs": externs}
    if not quiet:
        for s, (r, v, size) in result["loc"].items():
            print(f"  {s:8s} rom 0x{r:X} vram 0x{v:08X} size 0x{size:X}")
        if result["bss"] is not None:
            print(f"  .bss     vram 0x{result['bss']:08X}")
        print(f"  {len(externs)} external symbols resolved")
        if group_fix:
            print(f"  yaml group start 0x{group_fix[0]:X} is really 0x{group_fix[1]:X} (fixed on apply)")
    return result


def text_report(sec, objsecs, secs, rels, rom, groups, hint, syms, show=24):
    """Per function: where it matches, or an instruction diff against the ROM."""
    if hint is None:
        print(".text: no entry named after this file in the yaml to diff against")
        return
    text_idx = next(i for i, s in enumerate(secs) if s["name"] == ".text")
    funcs = sorted((s for s in syms if s["func"] and s["shndx"] == text_idx and s["size"]),
                   key=lambda s: s["value"])
    data = sec["data"]
    _, mask = pattern(data, rels)
    reloc_at = {o: syms[si]["name"].split("__", 1)[-1] for o, t, si in rels}
    lo, hi = max(0, hint - 0x4000), hint + len(data) + 0x4000
    from jpmatch import masked_word
    ok = {}
    for f in funcs:
        o, n = f["value"], f["size"]
        fr = [(r - o, t, si) for r, t, si in rels if o <= r < o + n]
        hits = find_all(pattern(data[o:o + n], fr)[0], rom, lo, hi)
        if hits:
            ok[o] = min(hits, key=lambda h: abs(h - hint - o))
    for f in funcs:
        o, n = f["value"], f["size"]
        name = f["name"].split("__", 1)[-1]
        if o in ok:
            print(f"  OK    {name} at rom 0x{ok[o]:X}")
            continue
        # the JP version sits in the gap between the matched neighbours
        nxt = min((k for k in ok if k > o), default=None)
        prv = max((k for k in ok if k < o), default=None)
        size_of = {f2["value"]: f2["size"] for f2 in funcs}
        if prv is not None:
            start = ok[prv] + size_of[prv]
        elif nxt is not None:
            start = max(0, ok[nxt] - (nxt - o) * 3 // 2)
        else:
            start = hint + o
        if nxt is not None:
            end = ok[nxt]
        else:
            end = start + (len(data) - o) * 3 // 2
        exact = prv is not None and nxt is not None
        if prv is None:
            for p in range(start + 8, end, 4):
                pad = p
                while pad < end and pad % 16 and struct.unpack_from(">I", rom, pad)[0] == 0:
                    pad += 4
                if (struct.unpack_from(">I", rom, p - 8)[0] == 0x03E00008 and pad % 16 == 0
                        and pad > p and pad < end):
                    start = pad
                    break
        cw = [masked_word(w) for w in struct.unpack_from(f">{n // 4}I", data, o)]
        win = rom[start:end]
        rw = [masked_word(w) for w in struct.unpack_from(f">{len(win) // 4}I", win)]
        sm = __import__("difflib").SequenceMatcher(None, cw, rw, autojunk=False)
        same = sum(b.size for b in sm.get_matching_blocks())
        where = "between the matched neighbours" if exact else "(estimated, a neighbour is unmatched)"
        print(f"  DIFF  {name} (C 0x{n:X} bytes): JP rom 0x{start:X}-0x{end:X} {where}, "
              f"{same}/{len(cw)} instructions line up")
        g = group_of(groups, start)
        vram = g[2] + (start - g[0]) if g else 0
        if same * 2 < len(cw):
            # too different to diff usefully: show the JP code itself
            print("    JP code in that range (a new function starts after `jr $ra` and its delay slot;"
                  " files start on 16-byte boundaries after zero padding):")
            words = struct.unpack_from(f">{len(win) // 4}I", win)
            for j, w in enumerate(words[:96]):
                print(f"      0x{start + j * 4:X} {vram + j * 4:08X}: {dis(w, vram + j * 4)}")
                if j and words[j - 1] == 0x03E00008:
                    print("      ----")
            print(f"    compiled C for {name}:")
            for i in range(n // 4):
                rel = f"  [{reloc_at[o + i * 4]}]" if o + i * 4 in reloc_at else ""
                print(f"      C+0x{i * 4:03X}: {dis(struct.unpack_from('>I', data, o + i * 4)[0], vram + i * 4)}{rel}")
            continue
        shown = 0
        for tag, i1, i2, j1, j2 in sm.get_opcodes():
            if tag == "equal" or shown >= show or (tag == "insert" and i1 == len(cw) and not exact):
                continue
            for k in range(max(i2 - i1, j2 - j1)):
                i, j = i1 + k, j1 + k
                left = dis(struct.unpack_from(">I", data, o + i * 4)[0], vram + j * 4) if i < i2 else ""
                right = dis(struct.unpack_from(">I", win, j * 4)[0], vram + j * 4) if j < j2 else ""
                rel = f"  [{reloc_at[o + i * 4]}]" if i < i2 and o + i * 4 in reloc_at else ""
                print(f"    {tag:7s} C+0x{i * 4:03X}: {left:34s} ROM+0x{j * 4:03X}: {right}{rel}")
                shown += 1


def dis(word, vram):
    return rabbitizer.Instruction(word, vram=vram).disassemble()


def diff(sname, sec, rels, rom, groups, hint, syms, show=16):
    data = sec["data"]
    _, mask = pattern(data, rels)
    if hint is None:
        print(f"{sname}: no entry named after this file in the yaml to diff against")
        return
    n = len(data) // 4

    def score(start):
        return sum(1 for i in range(min(n, 64))
                   if all((data[i * 4 + k] ^ rom[start + i * 4 + k]) & mask[i * 4 + k] == 0 for k in range(4)))

    start = max(range(max(0, hint - 0x800), hint + 0x800, 4), key=score)
    g = group_of(groups, start)
    vram = g[2] + (start - g[0]) if g else 0
    reloc_at = {o: (t, syms[s]["name"]) for o, t, s in rels}
    bad = [i for i in range(n)
           if any((data[i * 4 + k] ^ rom[start + i * 4 + k]) & mask[i * 4 + k] for k in range(4))]
    print(f"{sname}: best alignment at rom 0x{start:X}, {len(bad)} of {n} words differ "
          f"(size 0x{len(data):X}, the yaml region may be shorter or longer)")
    for i in bad[:show]:
        o = i * 4
        ow = struct.unpack_from(">I", data, o)[0]
        rw = struct.unpack_from(">I", rom, start + o)[0]
        if sname == ".text":
            left, right = dis(ow, vram + o), dis(rw, vram + o)
        else:
            left, right = f"{ow:08X} {data[o:o + 4]!r}", f"{rw:08X} {rom[start + o:start + o + 4]!r}"
        rel = f"  [{reloc_at[o][1]}]" if o in reloc_at else ""
        print(f"  +0x{o:04X}  C: {left:40s} ROM: {right}{rel}")


# ---------------------------------------------------------------- applying

def set_range(lines, start, end, typ, name):
    """Make [start, end) one yaml entry, keeping whatever follows at end."""
    ent = re.compile(r"^(\s*)- \[0x([0-9A-Fa-f]+),\s*([.\w]+),\s*([^\]\s,]+)")
    idx = [(i, int(m.group(2), 16), m) for i, l in enumerate(lines) if (m := ent.match(l))]
    before = [e for e in idx if e[1] < start]
    cover = [e for e in idx if e[1] <= end][-1]
    inside = [e for e in idx if start <= e[1] < end]
    if not before and not inside:
        raise Fail(f"no yaml entry covers 0x{start:X}")
    other_c = [e for e in inside + [cover] if e[2].group(3) in ("c", ".data", ".rodata") and e[2].group(4) != name]
    if any(e[1] >= start or e is cover and e[1] < start for e in other_c):
        raise Fail(f"0x{start:X}-0x{end:X} overlaps a file already in C")
    indent = (inside or before)[-1][2].group(1)
    new = [f"{indent}- [0x{start:X}, {typ}, {name}]"]
    if cover[1] < end:
        new.append(f"{indent}- [0x{end:X}, {cover[2].group(3)}, {cover[2].group(4)}_{end:X}]")
    pos = inside[0][0] if inside else before[-1][0] + 1
    drop = {e[0] for e in inside}
    out = [l for i, l in enumerate(lines[:pos]) if i not in drop] + new
    out += [l for i, l in enumerate(lines[pos:], pos) if i not in drop]
    return out


def fix_group_start(lines, old, new):
    """Move a group start, which is also its first entry's offset. Entries
    passed over are absorbed by the group or segment on the other side."""
    ent = re.compile(r"^\s*- \[0x([0-9A-Fa-f]+)")
    si = next(i for i, l in enumerate(lines) if re.match(rf"^\s+start: 0x0*{old:X}\b", l, re.I))
    fi = next(i for i in range(si, len(lines)) if (m := ent.match(lines[i])) and int(m.group(1), 16) == old)
    gend = next((i for i in range(fi + 1, len(lines)) if re.match(r"^  - ", lines[i])), len(lines))
    gstart = max(i for i in range(si + 1) if re.match(r"^  - ", lines[i]))
    if new > old:
        drop = {i for i in range(fi + 1, gend) if (m := ent.match(lines[i])) and int(m.group(1), 16) <= new}
        if len(drop) == sum(1 for i in range(fi + 1, gend) if ent.match(lines[i])):
            raise Fail(f"moving group start 0x{old:X} to 0x{new:X} would empty the group")
    else:
        prev = [i for i in range(gstart) if (m := ent.match(lines[i])) and int(m.group(1), 16) >= new]
        drop = set(prev)
        if prev and not [i for i in range(gstart) if (m := ent.match(lines[i])) and int(m.group(1), 16) < new]:
            raise Fail(f"cannot move group start 0x{old:X} back to 0x{new:X}")
    lines[si] = re.sub(r"0x[0-9A-Fa-f]+", f"0x{new:X}", lines[si], count=1)
    lines[fi] = re.sub(r"0x[0-9A-Fa-f]+", f"0x{new:X}", lines[fi], count=1)
    return [l for i, l in enumerate(lines) if i not in drop]


def ld_block(res):
    p = prefix("src/" + res["name"] + ".c")
    lines = [f"/* {res['name']} */"]
    for n, v in sorted(res["externs"].items()):
        lines.append(f"{n} = 0x{v:08X};")
    if res["bss"] is not None:
        sec = p + "bss"
        lines.append(f"SECTIONS {{ .{sec} 0x{res['bss']:08X} (NOLOAD) : "
                     f"{{ build/src/{res['name']}.o(.jpcbss) build/src/{res['name']}.o(COMMON) }} }}")
    lines.append(f"/* end {res['name']} */")
    return "\n".join(lines) + "\n"


def build_jp():
    for n in re.findall(r"- \[0x[0-9A-Fa-f]+,\s*c,\s*([^\]\s,]+)", YAML["jp"].read_text()):
        (ROOT / f"build/src/{n}.o").unlink(missing_ok=True)
    r = run("make", "VERSION=jp", "extract")
    if r.returncode:
        raise Fail("extract failed:\n" + r.stderr[-2000:])
    for _ in range(3):
        r = run("make", "VERSION=jp", "rom", "-k")
        log = r.stdout + r.stderr
        if "undefined reference" not in log:
            break
        lf = Path(tempfile.mkdtemp()) / "link.log"
        lf.write_text(log)
        run(sys.executable, "tools/jp_undefined_syms.py", str(lf))
        for f in ("build/pokestadium-jp.elf", "build/linker_scripts/jp/undefined_syms.ld"):
            Path(f).unlink(missing_ok=True)
    if "pokestadium-jp.z64: OK" not in log:
        raise Fail("JP ROM does not match after apply:\n" + log[-2500:])


def restore():
    """Undo an apply that failed or was killed."""
    if RESTORE.exists():
        for p in TRACKED:
            if (RESTORE / p.name).exists():
                shutil.copy(RESTORE / p.name, p)
        shutil.rmtree(RESTORE)
        return True
    return False


def apply(src, keep=False):
    name = stem(src)
    print("check us")
    check(src, "us", quiet=True)
    print("check jp")
    res = check(src, "jp")
    RESTORE.mkdir(parents=True, exist_ok=True)
    for p in TRACKED:
        shutil.copy(p, RESTORE / p.name)
    try:
        lines = YAML["jp"].read_text().split("\n")
        types = {".text": "c", ".data": ".data", ".rodata": ".rodata"}
        if res["group_fix"]:
            lines = fix_group_start(lines, *res["group_fix"])
        for s, (r, v, size) in sorted(res["loc"].items(), key=lambda x: x[1][0]):
            lines = set_range(lines, r, r + size, types[s], name)
        # leftover asm/bin pieces keep the old name, which now means the C file
        keep = re.compile(rf"^(\s*- \[0x([0-9A-Fa-f]+),\s*(?!c,|\.data,|\.rodata,)[.\w]+,\s*){re.escape(name)}(?=[\]\s,])")
        lines = [keep.sub(lambda m: f"{m.group(1)}{name}_{int(m.group(2), 16):X}", l) for l in lines]
        YAML["jp"].write_bytes("\n".join(lines).encode())
        ld = C_LD.read_text()
        ld = re.sub(rf"/\* {re.escape(name)} \*/.*?/\* end {re.escape(name)} \*/\n", "", ld, flags=re.S)
        C_LD.write_bytes((ld + ld_block(res)).encode())
        print("build jp")
        build_jp()
    except BaseException:
        if not keep:
            restore()
        raise
    shutil.rmtree(RESTORE)
    print(f"applied {src}: JP ROM matches")


# ---------------------------------------------------------------- listing

def listing():
    from jpmatch import mask_stream
    us = (ROOT / "baseroms/us/baserom.z64").read_bytes()
    jpm = mask_stream((ROOT / "baseroms/jp/baserom.z64").read_bytes())
    applied = set(re.findall(r"- \[0x[0-9A-Fa-f]+,\s*c,\s*([^\]\s,]+)", YAML["jp"].read_text()))
    ents = [(int(m.group(1), 16), m.group(2), m.group(3)) for m in
            re.finditer(r"- \[0x([0-9A-Fa-f]+),\s*([.\w]+)(?:,\s*([^\]\s,]+))?", YAML["us"].read_text())]
    rows = []
    for i, (off, typ, name) in enumerate(ents):
        if typ != "c" or not (ROOT / f"src/{name}.c").exists():
            continue
        end = next((o for o, *_ in ents[i + 1:] if o > off), off)
        text = (ROOT / f"src/{name}.c").read_text(errors="replace")
        if name in applied:
            state = "applied"
        elif re.search(r"GLOBAL_ASM|INCLUDE_ASM", text):
            state = "has GLOBAL_ASM"
        else:
            win = mask_stream(us[off:off + min(end - off, 256)])
            state = "likely shared" if len(win) >= 16 and jpm.find(win) >= 0 else "differs"
        rows.append((state, end - off, name))
    order = {"applied": 0, "likely shared": 1, "differs": 2, "has GLOBAL_ASM": 3}
    for state, size, name in sorted(rows, key=lambda r: (order[r[0]], r[1])):
        print(f"{state:15s} 0x{size:6X}  src/{name}.c")


def main():
    sys.path.insert(0, str(ROOT / "tools"))
    if len(sys.argv) < 2 or sys.argv[1] not in ("list", "check", "apply"):
        print(__doc__)
        return 2
    with open(Path(tempfile.gettempdir()) / "jp_match.lock", "w") as lock:
        fcntl.flock(lock, fcntl.LOCK_EX)
        if restore():
            print("note: an earlier apply was interrupted; its yaml and linker script changes were undone")
        try:
            if sys.argv[1] == "list":
                listing()
            elif sys.argv[1] == "check":
                check(sys.argv[2], sys.argv[3] if len(sys.argv) > 3 else "jp")
                print("MATCH")
            else:
                apply(sys.argv[2], "--keep" in sys.argv)
        except Fail as e:
            print(f"FAIL: {e}")
            return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
