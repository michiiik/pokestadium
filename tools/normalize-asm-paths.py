#!/usr/bin/env python3
"""Stream assembly while making generated .incbin paths portable."""

from __future__ import annotations

import sys
from pathlib import Path


def main() -> int:
    if len(sys.argv) != 2:
        print(f"usage: {sys.argv[0]} FILE", file=sys.stderr)
        return 2

    for line in Path(sys.argv[1]).read_bytes().splitlines(keepends=True):
        if b".incbin" in line:
            line = line.replace(b"\\", b"/")
        sys.stdout.buffer.write(line)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
