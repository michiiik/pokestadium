#!/usr/bin/env python3
"""Transfer the US symbol map to JP by keeping each symbol's offset within its
subsegment: jp_vram = jp_subseg_vram + (us_vram - us_subseg_vram).

Overlay (fragments/*) interiors are reordered between versions, so those and
symbols with no JP subsegment are left commented out. Auto-names that would
move are dropped, since splat generates the same names for other JP addresses.

usage: tools/jp_transfer_symbols.py [repo_root]
writes: linker_scripts/jp/symbol_addrs{,_code,_ultralib}.txt
"""
import pathlib
import re
import sys

import yaml

ROOT = pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else ".")


def load_yaml(ver):
    return yaml.safe_load((ROOT / f"yamls/{ver}/header.yaml").read_text()
                          + (ROOT / f"yamls/{ver}/rom.yaml").read_text())["segments"]


def norm(s):
    if isinstance(s, dict):
        return (s.get("start"), s.get("type") or "", s.get("name") or "",
                s.get("subsegments") or [], s.get("vram"))
    return (s[0], s[1] if len(s) > 1 else "", s[2] if len(s) > 2 else "", [], None)


def collect(segments):
    """name -> [(rom, vram, size)] for every subsegment."""
    subs = {}

    def walk(entries, g_vram, g_off):
        entries = [norm(s) for s in entries]
        for i, (off, typ, name, nested, own_vram) in enumerate(entries):
            if nested:
                walk(nested, g_vram, g_off)
                continue
            if off is None:
                if name and own_vram is not None:
                    subs.setdefault(name, []).append((None, own_vram, 0))
                continue
            nxt = next((o for o, *_ in entries[i + 1:] if o is not None), None)
            if name and nxt is not None and nxt > off:
                vram = own_vram if own_vram is not None else (
                    g_vram + (off - g_off) if g_vram is not None else None)
                subs.setdefault(name, []).append((off, vram, nxt - off))

    for g in segments:
        if isinstance(g, dict):
            walk(g.get("subsegments") or [], g.get("vram"), g.get("start"))
    return subs


us_subs = collect(load_yaml("us"))
jp_subs = collect(load_yaml("jp"))

sym_re = re.compile(r"^(\w+)\s*=\s*0x([0-9A-Fa-f]+);(.*)$")
auto_re = re.compile(r"(?:func|D|jtbl|B|L|sub|code|data)_([0-9A-Fa-f]{6,8})")

seen_vram = {}
for fname in ("symbol_addrs_code.txt", "symbol_addrs.txt", "symbol_addrs_ultralib.txt"):
    src = ROOT / "linker_scripts/us" / fname
    lines, n_ok = [], 0
    for line in src.read_text().splitlines():
        m = sym_re.match(line.strip())
        if not m:
            if not line.strip() or line.lstrip().startswith("//"):
                lines.append(line)
            continue
        name, us_vram, rest = m.group(1), int(m.group(2), 16), m.group(3)
        hit = next(((nm, rom, vram) for nm, cands in us_subs.items()
                    for rom, vram, size in cands
                    if vram is not None and size and vram <= us_vram < vram + size), None)
        if hit is None or hit[1] is None:
            lines.append(f"// {name} = 0x{us_vram:08X};{rest} // UNTRANSFERRED (no JP subsegment)")
            continue
        nm, _, us_svram = hit
        if nm not in jp_subs:
            lines.append(f"// {name} = 0x{us_vram:08X};{rest} // UNTRANSFERRED ({nm} not in JP yaml)")
            continue
        jp_vram = jp_subs[nm][0][1] + (us_vram - us_svram)
        if nm.startswith("fragments/"):
            lines.append(f"// {name} = 0x{jp_vram:08X};{rest} "
                         f"// OVERLAY, offset not preserved - needs per-function location")
            continue
        a = auto_re.fullmatch(name)
        if a and int(a.group(1), 16) != jp_vram:
            continue
        # splat rejects duplicate vrams
        if jp_vram in seen_vram:
            lines.append(f"// {name} = 0x{jp_vram:08X};{rest} // duplicate of {seen_vram[jp_vram]}")
            continue
        seen_vram[jp_vram] = name
        lines.append(f"{name} = 0x{jp_vram:08X};{rest}")
        n_ok += 1
    (ROOT / "linker_scripts/jp" / fname).write_bytes(("\n".join(lines) + "\n").encode())
    print(f"{fname}: {n_ok} transferred")
