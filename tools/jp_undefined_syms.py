#!/usr/bin/env python3
"""Write linker_scripts/jp/undefined_syms.ld from the undefined references in
a failed JP link log. splat names a reference after the address it encodes, so
every name resolves exactly: symbol_addrs value, spimdisasm known symbol, or the
address in the name.

usage: tools/jp_undefined_syms.py build_log [repo_root]
"""
import pathlib
import re
import sys

from spimdisasm.common.SymbolsSegment import SymbolsSegment

LOG = pathlib.Path(sys.argv[1])
ROOT = pathlib.Path(sys.argv[2] if len(sys.argv) > 2 else ".")
OUT = ROOT / "linker_scripts/jp/undefined_syms.ld"

known = {name: addr for addr, name in SymbolsSegment.N64HardwareRegs.items()}
known.update({v[0]: addr for addr, v in SymbolsSegment.N64LibultraSyms.items()})
for f in sorted((ROOT / "linker_scripts/jp").glob("symbol_addrs*.txt")):
    for line in f.read_text().splitlines():
        m = re.match(r"^\s*([\w.]+)\s*=\s*0x([0-9A-Fa-f]+)", line)
        if m:
            known.setdefault(m.group(1), int(m.group(2), 16))

have = {}
if OUT.exists():
    for line in OUT.read_text().splitlines():
        m = re.match(r"^([\w.]+) = 0x([0-9A-F]+);", line)
        if m:
            have[m.group(1)] = int(m.group(2), 16)

names = set(re.findall(r"undefined reference to .([\w.]+)'", LOG.read_text(errors="replace")))
missing = []
for n in names:
    m = re.fullmatch(r"(?:\.L|[A-Za-z]+_)([0-9A-Fa-f]{4,8})", n)
    if n in known:
        have[n] = known[n]
    elif m:
        have[n] = int(m.group(1), 16)
    else:
        missing.append(n)

OUT.write_text("".join(f"{n} = 0x{v:08X};\n" for n, v in sorted(have.items())))
print(f"{len(have)} symbols, unresolved: {sorted(missing)}")
sys.exit(1 if missing else 0)
