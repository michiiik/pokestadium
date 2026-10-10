#!/usr/bin/env python3
"""Turn the JP yaml into a build from JP's own disassembly: code becomes asm,
data/rodata are disassembled, and everything else is raw bins in ROM order, so
every byte of the ROM comes from the baserom.

usage: tools/jp_asm_yaml.py [repo_root]
"""
import pathlib
import re
import sys

ROOT = pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else ".")
YAML = ROOT / "yamls/jp/rom.yaml"

TYPES = {"c": "asm", "hasm": "asm", ".data": "data", ".rodata": "rodata", "pad": "bin"}
CLASS = {"asm": 0, "textbin": 0, "data": 1, "databin": 1, "rodata": 2, "rodatabin": 2}
BIN = ["textbin", "databin", "rodatabin"]

ent = re.compile(r"^(\s*- \[0x([0-9A-Fa-f]+),\s*)([.\w]+)(,\s*)(.*)$")
term = re.compile(r"^(\s*)- \[0x([0-9A-Fa-f]+)\]\s*$")
start_of = re.compile(r"^\s*(?:start: |- \[)0x([0-9A-Fa-f]+)")


def next_start(lines, i):
    return next((int(m.group(1), 16) for m in map(start_of.match, lines[i + 1:]) if m), None)


lines = YAML.read_text().split("\n")

# retype entries, cover gaps between segments, drop zero-length entries
out = []
for i, l in enumerate(lines):
    if "ld_align_segment_start" in l or re.match(r"^\s+bss_size:", l) or "bss" in l and l.lstrip().startswith("- {"):
        continue
    m = ent.match(l)
    if m:
        off = int(m.group(2), 16)
        if next_start(lines, i) == off:
            continue
        typ, rest = m.group(3), m.group(5)
        if typ == "lib":
            obj = re.match(r"libultra,\s*(\w+)\s*(?:,\s*\.(\w+))?\s*\](.*)$", rest)
            typ, rest = obj.group(2) or "asm", f"libultra/{obj.group(1)}]{obj.group(3)}"
        else:
            typ = TYPES.get(typ, typ)
            rest = rest.replace("../../src/", "")
        l = f"{m.group(1)}{typ}{m.group(4)}{rest}"
    else:
        t = term.match(l)
        nxt = next_start(lines, i) if t else None
        if nxt is not None and nxt > int(t.group(2), 16):
            off = int(t.group(2), 16)
            l = f"{t.group(1)}- [0x{off:X}, bin, {off:X}]"
    out.append(l)

# keep code groups in text, data, rodata order and drop groups left empty
blocks, cur = [], []
for l in out:
    if re.match(r"^  - ", l) and cur:
        blocks.append(cur)
        cur = []
    cur.append(l)
blocks.append(cur)

out = []
for b in blocks:
    if not (b[0].startswith("  - name:") and any(x.strip().startswith("subsegments:") for x in b)):
        out += b
        continue
    if not any(re.match(r"^\s+- [\[{]", x) for x in b[1:]):
        continue
    cls = 0
    for l in b:
        m = ent.match(l)
        if m:
            t = m.group(3)
            if t == "bin" or CLASS.get(t, cls) < cls:
                t = BIN[cls]
            elif t in CLASS:
                cls = CLASS[t]
            l = f"{m.group(1)}{t}{m.group(4)}{m.group(5)}"
        out.append(l)
        if re.match(r"^\s+type: code", l):
            out.append(re.match(r"^(\s+)", l).group(1) + "align: 1")

# a bin's name is its file name, so repeats would overwrite each other
seen = set()
for i, l in enumerate(out):
    m = ent.match(l)
    if m and m.group(3).endswith("bin"):
        name = re.match(r"[^\]\s,]+", m.group(5)).group(0)
        if name in seen:
            out[i] = f"{m.group(1)}{m.group(3)}{m.group(4)}{name}_{m.group(2).upper()}{m.group(5)[len(name):]}"
            name = f"{name}_{m.group(2).upper()}"
        seen.add(name)

YAML.write_bytes("\n".join(out).encode())
