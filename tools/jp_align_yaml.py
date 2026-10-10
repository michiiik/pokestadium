#!/usr/bin/env python3
"""Snap the JP yaml's rom offsets and vram bases down to word boundaries, then
bump .bss vrams that fall below their group's data so splat keeps them ordered.

usage: tools/jp_align_yaml.py [repo_root]
"""
import pathlib
import re
import sys

import yaml

ROOT = pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else ".")

off_re = re.compile(r"(\[\s*0x)([0-9A-Fa-f]+)")
start_re = re.compile(r"(\bstart:\s*0x)([0-9A-Fa-f]+)")
vram_re = re.compile(r"(\bvram:\s*0x)([0-9A-Fa-f]+)")


def snap(m):
    return f"{m.group(1)}{int(m.group(2), 16) & ~3:X}"


for name in ("rom.yaml", "header.yaml"):
    p = ROOT / "yamls/jp" / name
    out_lines, n_changed = [], 0
    for line in p.read_text().splitlines(True):
        new = off_re.sub(snap, line)
        new = start_re.sub(snap, new)
        new = vram_re.sub(snap, new)
        if new != line:
            n_changed += 1
        out_lines.append(new)
    p.write_text("".join(out_lines))
    print(f"{name}: {n_changed} lines snapped")

txt = (ROOT / "yamls/jp/header.yaml").read_text() + (ROOT / "yamls/jp/rom.yaml").read_text()
seg = yaml.safe_load(txt)["segments"]
bad_align = 0
offs = []
for g in seg:
    if not isinstance(g, dict):
        continue
    gs, gv = g.get("start"), g.get("vram")
    if gs is not None:
        offs.append(gs)
        if gs % 4:
            bad_align += 1
    for s in (g.get("subsegments") or []):
        if isinstance(s, list) and s and isinstance(s[0], int):
            offs.append(s[0])
            if s[0] % 4:
                bad_align += 1
            if len(s) > 1 and s[1] == "c" and gs is not None and gv is not None and (s[0] - gs) % 4:
                bad_align += 1
assert bad_align == 0, "snapping did not fully align the yaml"
assert all(a <= b for a, b in zip(offs, offs[1:])), "offsets not monotone"

bss_re = re.compile(r"(\{\s*vram:\s*0x)([0-9A-Fa-f]+)(\s*,\s*type:\s*\.bss\s*,\s*name:\s*)([^}]+?)(\s*\})")


def fix_group(g, txt):
    gs, gv = g.get("start"), g.get("vram")
    if gs is None or gv is None:
        return txt
    subs = g.get("subsegments") or []
    derived = [gv + (s[0] - gs) for s in subs
               if isinstance(s, list) and len(s) >= 2 and isinstance(s[0], int)]
    if not derived:
        return txt
    nxt = max(derived) + 4
    for s in subs:
        if not (isinstance(s, dict) and s.get("type") == ".bss" and s.get("vram") is not None):
            continue
        if s["vram"] >= nxt:
            continue

        def sub(m, name=s["name"], val=nxt):
            return m.group(1) + f"{val:X}" + m.group(3) + m.group(4) + m.group(5) \
                if m.group(4) == name else m.group(0)

        new = bss_re.sub(sub, txt)
        if new != txt:
            print(f"  {g.get('name')}: .bss {s['name']} vram 0x{s['vram']:X} -> 0x{nxt:X}")
            txt = new
            s["vram"] = nxt
        nxt += 4
    return txt


p = ROOT / "yamls/jp/rom.yaml"
txt = p.read_text()
for g in seg:
    if isinstance(g, dict) and g.get("subsegments"):
        txt = fix_group(g, txt)
p.write_text(txt)
print(f"ok: {len(offs)} offsets word-aligned and monotone")
