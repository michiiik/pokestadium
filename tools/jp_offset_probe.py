#!/usr/bin/env python3
"""Locate each US subsegment in the JP ROM by unique raw anchors, then by
address-masked instruction windows.

usage: tools/jp_offset_probe.py [repo_root]
writes: yamls/jp/segment_transfer.csv
"""
import collections
import csv
import pathlib
import statistics
import sys

import yaml

from jpmatch import mask_stream, unique_find, selftest

ROOT = pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else ".")
ANCH = 128
MAX_PER_SEG = 8
MIN_SEG_SIZE = 0x800
MWIN = 256
MMAX_PER_SEG = 6

selftest()

us = (ROOT / "baseroms/us/baserom.z64").read_bytes()
jp = (ROOT / "baseroms/jp/baserom.z64").read_bytes()
jp_masked = mask_stream(jp)

segments = yaml.safe_load((ROOT / "yamls/us/header.yaml").read_text()
                          + (ROOT / "yamls/us/rom.yaml").read_text())["segments"]


def norm(sub):
    if isinstance(sub, dict):
        return (sub.get("start"), sub.get("type", ""), sub.get("name") or "",
                sub.get("subsegments") or [])
    return (sub[0], sub[1] if len(sub) > 1 else "",
            sub[2] if len(sub) > 2 else "", [])


def flatten(seg_name, subs, out):
    entries = [norm(s) for s in subs]
    for i, (off, typ, name, nested) in enumerate(entries):
        if off is None:
            continue
        if nested:
            flatten(seg_name, nested, out)
            continue
        nxt = next((o for o, *_ in entries[i + 1:] if o is not None), None)
        if nxt is None or nxt <= off:
            continue
        out.append((seg_name, off, nxt - off, typ, name))


flat = []
for s in segments:
    off, typ, name, subs = norm(s)
    label = name or typ or "?"
    if subs:
        flatten(label, subs, flat)
    elif off is not None:
        flat.append((label, off, 0, typ, name))

rows = []
for seg_name, off, size, typ, sub_name in flat:
    if size < MIN_SEG_SIZE:
        continue

    tried = hits = 0
    deltas = []
    for k in range(MAX_PER_SEG):
        a = off + (size * (k + 1)) // (MAX_PER_SEG + 1)
        pat = us[a:a + ANCH]
        if len(pat) < ANCH or len(set(pat)) < 48:
            continue
        tried += 1
        at = unique_find(jp, pat, 1)
        if at is None:
            continue
        hits += 1
        deltas.append(at - a)

    m_tried = m_hits = 0
    m_deltas = []
    if not deltas and typ in ("c", "hasm", "lib", "rodata", "rodatabin"):
        for k in range(MMAX_PER_SEG):
            a = off + (size * (k + 1)) // (MMAX_PER_SEG + 1)
            a -= a % 4
            win = us[a:a + MWIN]
            if len(win) < MWIN:
                continue
            m_tried += 1
            at = unique_find(jp_masked, mask_stream(win), 4)
            if at is None:
                continue
            m_hits += 1
            m_deltas.append(at - a)

    delta = int(statistics.median(deltas)) if deltas else None
    m_delta = int(statistics.median(m_deltas)) if m_deltas else None
    rows.append(dict(
        seg=seg_name, name=sub_name, type=typ, us_off=f"0x{off:08X}", us_size=f"0x{size:X}",
        raw=f"{hits}/{tried}", raw_delta="" if delta is None else f"{delta:+#x}",
        raw_jp_off="" if delta is None else f"0x{off + delta:08X}",
        masked=f"{m_hits}/{m_tried}", masked_delta="" if m_delta is None else f"{m_delta:+#x}",
        masked_jp_off="" if m_delta is None else f"0x{off + m_delta:08X}",
        verdict="raw" if delta is not None else ("masked" if m_delta is not None else "UNLOCATED"),
    ))

out = ROOT / "yamls/jp/segment_transfer.csv"
with out.open("w", newline="") as f:
    w = csv.DictWriter(f, fieldnames=list(rows[0]), lineterminator="\n")
    w.writeheader()
    w.writerows(rows)

by_v = collections.Counter(r["verdict"] for r in rows)
print(f"{len(rows)} subsegments >= 0x{MIN_SEG_SIZE:X}")
print("verdict: " + "  ".join(f"{k}={v}" for k, v in sorted(by_v.items())))
for r in rows:
    if r["verdict"] == "UNLOCATED":
        print(f"  unlocated {r['us_off']} {r['us_size']:>8} {r['type']:10s} {r['seg']}/{r['name']}")
