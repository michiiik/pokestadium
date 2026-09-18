#!/usr/bin/env python3
"""Run IDO and apply narrowly verified function-word scheduling repairs."""

from __future__ import annotations

import os
import platform
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path


PRAGMA = re.compile(
    r"^[ \t]*#pragma\s+SWAP_FUNCTION_WORDS\s*\(\s*"
    r"([A-Za-z_]\w*)\s*,\s*(0x[0-9A-Fa-f]+|[0-9]+)\s*,\s*"
    r"(0x[0-9A-Fa-f]+|[0-9]+)\s*,\s*(0x[0-9A-Fa-f]+|[0-9]+)\s*\)"
    r"[ \t]*$",
    re.MULTILINE,
)


def tool(candidates: list[str]) -> str:
    for candidate in candidates:
        resolved = shutil.which(candidate)
        if resolved:
            return resolved
    raise RuntimeError(f"missing required tool: {' or '.join(candidates)}")


def real_cc() -> Path:
    system = platform.system()
    if system == "Darwin":
        os_name = "macos"
    elif system == "Linux":
        os_name = "linux"
    else:
        raise RuntimeError(f"unsupported host OS: {system}")
    return Path(__file__).resolve().parent / "ido" / os_name / "7.1" / "cc"


def symbol_value(nm: str, object_path: str, function: str) -> int:
    output = subprocess.check_output(
        [nm, "-S", "--defined-only", object_path], text=True
    )
    for line in output.splitlines():
        fields = line.split()
        if len(fields) >= 4 and fields[-1] == function:
            return int(fields[0], 16)
    raise RuntimeError(f"{function}: symbol not found in {object_path}")


def apply_swaps(object_path: str, source_path: str) -> None:
    source = Path(source_path).read_text(encoding="latin1")
    patches = [
        (name, int(offset, 0), int(first, 0), int(second, 0))
        for name, offset, first, second in PRAGMA.findall(source)
    ]
    if not patches:
        return

    nm = tool(["mips-linux-gnu-nm", "mips64-elf-nm", "mips-elf-nm"])
    objcopy = tool(["mips-linux-gnu-objcopy", "mips64-elf-objcopy", "mips-elf-objcopy"])

    with tempfile.TemporaryDirectory() as temp_dir:
        text_path = Path(temp_dir) / "text.bin"
        subprocess.run(
            [objcopy, "--dump-section", f".text={text_path}", object_path],
            check=True,
        )
        text = bytearray(text_path.read_bytes())
        for function, offset, expected_first, expected_second in patches:
            start = symbol_value(nm, object_path, function) + offset
            if start < 0 or start + 8 > len(text):
                raise RuntimeError(f"{function}: swap offset 0x{offset:X} is outside .text")
            first = int.from_bytes(text[start : start + 4], "big")
            second = int.from_bytes(text[start + 4 : start + 8], "big")
            if (first, second) != (expected_first, expected_second):
                raise RuntimeError(
                    f"{function}: expected {expected_first:08X}/{expected_second:08X}, "
                    f"found {first:08X}/{second:08X}"
                )
            text[start : start + 4] = expected_second.to_bytes(4, "big")
            text[start + 4 : start + 8] = expected_first.to_bytes(4, "big")
        text_path.write_bytes(text)
        subprocess.run(
            [objcopy, "--update-section", f".text={text_path}", object_path],
            check=True,
        )


def main() -> int:
    args = sys.argv[1:]
    result = subprocess.run([str(real_cc()), *args])
    if result.returncode != 0 or "-c" not in args or "-o" not in args or not args:
        return result.returncode
    try:
        output_path = args[args.index("-o") + 1]
        apply_swaps(output_path, args[-1])
    except (OSError, RuntimeError, subprocess.CalledProcessError) as exc:
        print(f"cc_wrapper.py: {exc}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
