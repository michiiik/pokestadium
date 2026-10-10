#!/usr/bin/env python3
"""Generate yamls/jp/rom.yaml from the US yaml and yamls/jp/segment_transfer.csv.

ROM offsets go through a piecewise-linear map between monotone anchors.
Subsegment vrams are remapped through their group's rom range; segment vram
bases are left alone.

usage: tools/jp_remap_yaml.py [repo_root]
"""
import csv
import pathlib
import re
import sys

ROOT = pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else ".")
ROM_SIZE = 0x2000000

rows = list(csv.DictReader((ROOT / "yamls/jp/segment_transfer.csv").open()))

anchors = []
for r in rows:
    jp = r["raw_jp_off"] or r["masked_jp_off"]
    if jp:
        anchors.append((int(r["us_off"], 16), int(jp, 16)))
anchors.sort()

clean = [(0, 0)]
for u, j in anchors:
    if u > clean[-1][0] and j > clean[-1][1]:
        clean.append((u, j))
clean.append((ROM_SIZE, ROM_SIZE))
print(f"anchors: {len(anchors)} located -> {len(clean)} monotone")


def remap(u):
    for a, b in zip(clean, clean[1:]):
        if a[0] <= u <= b[0]:
            span = b[0] - a[0]
            if span == 0:
                return a[1]
            return a[1] + round((u - a[0]) * (b[1] - a[1]) / span)
    return u


src = (ROOT / "yamls/us/rom.yaml").read_text()
sub_re = re.compile(r"^(\s*)- \[0x([0-9A-Fa-f]+)(.*?)\s*$")
# Unnamed entries are named after their offset by splat, so pin the US name.
bare_re = re.compile(r"^(\s{2,})- \[0x([0-9A-Fa-f]+),\s*([A-Za-z_.]+)\s*\](\s*#.*)?$")
start_re = re.compile(r"(\w*start: 0x)([0-9A-Fa-f]+)")
vram_re = re.compile(r"(vram: 0x)([0-9A-Fa-f]+)")
name_re = re.compile(r"^\s*- name: ")
subseg_re = re.compile(r"^\s*subsegments:")

state = {"prev": 0, "sub": 0, "start": 0, "vram": 0, "named": 0}
seg = {"rom_us": None, "rom_jp": None, "vram_us": None, "in_sub": False}


def hexfmt(val, orig):
    return f"{val:0{len(orig)}X}" if val < 16 ** len(orig) else f"{val:X}"


def rewrite(orig_hex):
    val = max(remap(int(orig_hex, 16)), state["prev"])
    state["prev"] = val
    return hexfmt(val, orig_hex)


def remap_vram(v):
    if v == 0 or v >= 0xA0000000:
        return None
    if seg["rom_us"] is None or seg["vram_us"] is None or seg["rom_jp"] is None:
        return None
    rom_equiv = seg["rom_us"] + (v - seg["vram_us"])
    return seg["vram_us"] + (remap(rom_equiv) - seg["rom_jp"])


out = []
for line in src.splitlines():
    if name_re.match(line):
        seg.update(rom_us=None, rom_jp=None, vram_us=None, in_sub=False)
        out.append(line)
        continue
    if subseg_re.match(line):
        seg["in_sub"] = True
        out.append(line)
        continue

    m = bare_re.match(line)
    if m:
        state["sub"] += 1
        state["named"] += 1
        out.append(f"{m.group(1)}- [0x{rewrite(m.group(2))}, {m.group(3)}, "
                   f"{int(m.group(2), 16):X}]{m.group(4) or ''}")
        continue

    m = sub_re.match(line)
    if m:
        state["sub"] += 1
        out.append(f"{m.group(1)}- [0x{rewrite(m.group(2))}{m.group(3)}")
        continue

    m = start_re.search(line)
    if m:
        state["start"] += 1
        val = rewrite(m.group(2))
        if not seg["in_sub"]:
            seg["rom_us"] = int(m.group(2), 16)
            seg["rom_jp"] = int(val, 16)
        out.append(start_re.sub(lambda _m: _m.group(1) + val, line, count=1))
        continue

    m = vram_re.search(line)
    if m:
        v = int(m.group(2), 16)
        if seg["in_sub"] or line.lstrip().startswith("- {"):
            new = remap_vram(v)
            if new is not None:
                state["vram"] += 1
                out.append(vram_re.sub(lambda _m: _m.group(1) + hexfmt(new, _m.group(2)),
                                       line, count=1))
                continue
        if not seg["in_sub"] and seg["vram_us"] is None:
            seg["vram_us"] = v
        out.append(line)
        continue

    out.append(line)


def check_vram_order(text):
    """splat rejects groups whose subsegment vrams are not ascending."""
    seg_name, base, rom_us, in_sub, prev = None, None, None, False, None
    bad = []
    for line in text.splitlines():
        if name_re.match(line):
            seg_name, base, rom_us, prev = line.split("name:", 1)[1].strip(), None, None, None
            in_sub = False
            continue
        if subseg_re.match(line):
            in_sub = True
            continue
        m = re.match(r"^(\s*)- ", line)
        if m and len(m.group(1)) < 4:
            in_sub, prev = False, None
            continue
        if not in_sub:
            m = re.match(r"^\s*vram: 0x([0-9A-Fa-f]+)", line)
            if m:
                base = int(m.group(1), 16)
            m = re.match(r"^\s*start: 0x([0-9A-Fa-f]+)", line)
            if m:
                rom_us = int(m.group(1), 16)
            continue
        m = re.match(r"^\s{4,}- \[0x([0-9A-Fa-f]+)", line)
        if m and base is not None and rom_us is not None:
            v = base + (int(m.group(1), 16) - rom_us)
        else:
            m = vram_re.search(line)
            if not m:
                continue
            v = int(m.group(2), 16)
        if prev is not None and v < prev:
            bad.append(f"  {seg_name}: {v:#x} after {prev:#x}   ({line.strip()[:70]})")
        prev = v
    assert not bad, "vram order violations:\n" + "\n".join(bad[:10])


dst = ROOT / "yamls/jp/rom.yaml"
text = "\n".join(out) + "\n"
dst.write_text(text)
check_vram_order(text)

vals = []
for line in text.splitlines():
    m = sub_re.match(line)
    if m:
        vals.append(int(m.group(2), 16))
        continue
    m = re.search(r"start: 0x([0-9A-Fa-f]+)", line)
    if m:
        vals.append(int(m.group(1), 16))
assert vals == sorted(vals), "output is not monotonic"
assert all(0 <= v <= ROM_SIZE for v in vals), "offset outside the ROM"

print(f"rewrote {state['sub']} subsegment offsets, {state['start']} segment starts, "
      f"{state['vram']} subsegment vrams, {state['named']} offset-named subsegments")
