#!/usr/bin/env python3
"""Post-extract fixups so the JP disassembly reassembles to the baserom bytes.

- `.double` auto-aligns to 8, but guessed doubles can sit at 4 mod 8 in their
  file, so they are written as two words.
- In .text, words spimdisasm could not decode as instructions are written as
  their literal value, since it may emit a symbol or a folded constant instead.

usage: tools/jp_fix_asm.py [asm_dir]
"""
import pathlib
import re
import sys

ASM = pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else "asm/jp")

double = re.compile(r"^(\s*/\* [0-9A-F]+ [0-9A-F]{8} )([0-9A-F]{8})([0-9A-F]{8}) \*/ \.double .*$", re.M)
word = re.compile(r"^(\s*/\* [0-9A-F]+ [0-9A-F]{8} ([0-9A-F]{8}) \*/\s*)\.word\s.*$", re.M)

for p in ASM.rglob("*.s"):
    s = p.read_text()
    new = double.sub(r"\1\2\3 */ .word 0x\2, 0x\3", s)
    if re.search(r"^\.section \.text", new, re.M):
        new = word.sub(r"\1.word 0x\2", new)
    if new != s:
        p.write_bytes(new.encode())
