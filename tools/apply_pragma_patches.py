#!/usr/bin/env python3
"""Apply guarded post-compile pragmas to an object produced from C."""

from pathlib import Path
import argparse
import sys


TOOLS_DIR = Path(__file__).resolve().parent / "asm-processor"
sys.path.insert(0, str(TOOLS_DIR))
import asm_processor  # noqa: E402


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("objfile")
    parser.add_argument("source")
    args = parser.parse_args()
    asm_processor.apply_rewrite_function_stack_offsets(
        args.objfile,
        args.source,
        "utf-8",
    )


if __name__ == "__main__":
    main()
