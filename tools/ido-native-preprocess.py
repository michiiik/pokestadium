#!/usr/bin/env python3
"""Preprocess C with GNU cpp, then compile the result with Windows IDO."""
from __future__ import annotations

import os
import tempfile
import subprocess
import sys
from pathlib import Path


def msys_path(value: str) -> str:
    normalized = value.replace("\\", "/")
    if len(normalized) >= 2 and normalized[1] == ":":
        return "/" + normalized[0].lower() + normalized[2:]
    return normalized


def native_path(value: str) -> str:
    """Convert MSYS-style absolute paths emitted by make to Windows paths."""
    normalized = value.replace("\\", "/")
    if len(normalized) >= 3 and normalized[0] == "/" and normalized[2] == "/" and normalized[1].isalpha():
        return normalized[1].upper() + ":" + normalized[2:]
    return normalized


def native_option(value: str) -> str:
    """Convert paths attached to compiler options such as -I/d/project."""
    for prefix in ("-include", "-imacros", "-I"):
        if value.startswith(prefix) and len(value) > len(prefix):
            return prefix + native_path(value[len(prefix):])
    return native_path(value)


def main() -> int:
    if len(sys.argv) < 4:
        print(f"usage: {sys.argv[0]} CPP IDO_CC [IDO_ARGS...]", file=sys.stderr)
        return 2
    cpp, compiler = sys.argv[1:3]
    if os.name == "nt" and Path(cpp).name.lower() == "cpp.exe":
        mingw_cpp = Path("C:/msys64/mingw64/bin/cpp.exe")
        if mingw_cpp.exists():
            cpp = str(mingw_cpp)
    repo_root = Path(__file__).resolve().parents[1]
    compiler_path = Path(native_path(compiler))
    if not compiler_path.is_absolute():
        compiler_path = repo_root / compiler_path
    # Use the original Windows IDO driver; cc-tmp rejects generated paths.
    compiler_args = sys.argv[3:]
    sources = [arg for arg in compiler_args if not arg.startswith("-") and Path(arg).suffix.lower() in (".c", ".s")]
    if len(sources) != 1:
        print("expected exactly one C or assembly source argument", file=sys.stderr)
        return 2
    source = sources[0]

    source_native = native_path(source)
    source_path = Path(source_native).resolve()
    if "-o" not in compiler_args:
        print("IDO command is missing -o", file=sys.stderr)
        return 2
    output = Path(native_path(compiler_args[compiler_args.index("-o") + 1]))
    if not output.is_absolute():
        output = repo_root / output
    output = output.resolve()
    # asm_processor already expands GLOBAL_ASM sources into generated C.
    # Do not preprocess those files a second time.
    generated_by_asmproc = (
        ".ido-tmp" in source_path.parts
        or (
            source_path.parent.name.startswith("asm_processor")
            and source_path.name.startswith("preprocessed_")
        )
    )
    if generated_by_asmproc:
        preprocessed = source_path.with_suffix(".cpp")
    else:
        preprocessed = output.with_suffix(".s" if Path(source).suffix.lower() == ".s" else ".c")
    preprocessed.parent.mkdir(parents=True, exist_ok=True)
    cpp_args = [cpp, "-P", "-undef", "-D__attribute__(x)=", "-D__sgi"]
    cpp_source_path = source_path
    if generated_by_asmproc:
        # asm-processor has already converted this source to EUC-JP. Windows
        # IDO otherwise re-encodes non-ASCII literals as UTF-8, changing both
        # their bytes and section sizes. Three-digit octal escapes keep the
        # compiler input ASCII while preserving every original EUC-JP byte.
        escaped_path = source_path.with_suffix(".escaped.c")
        escaped = "".join(chr(byte) if byte < 0x80 else f"\\{byte:03o}" for byte in source_path.read_bytes())
        escaped_path.write_text(escaped, encoding="ascii", newline="")
        cpp_source_path = escaped_path
    process_env = os.environ.copy()
    if os.name == "nt":
        process_env["PATH"] = "C:/msys64/mingw64/bin;" + process_env.get("PATH", "")
    index = 0
    while index < len(compiler_args):
        arg = compiler_args[index]
        if arg in ("-I", "-D", "-U", "-include", "-imacros") and index + 1 < len(compiler_args):
            cpp_args.extend([arg, native_path(compiler_args[index + 1])])
            index += 2
            continue
        if arg.startswith(("-I", "-D", "-U", "-include", "-imacros")):
            cpp_args.append(native_option(arg))
        index += 1
    if Path(source).suffix.lower() == ".s":
        cpp_args = [arg for arg in cpp_args if arg != "-D_LANGUAGE_C"]
        cpp_args.extend(["-D_LANGUAGE_ASSEMBLY", "-D_MIPS_SIM=1", "-Dmips"])
    elif not any(arg in ("-D_LANGUAGE_C", "-DLANGUAGE_C") for arg in compiler_args):
        cpp_args.append("-D_LANGUAGE_C")
    cpp_args.extend(["-o", preprocessed.as_posix(), cpp_source_path.as_posix()])
    subprocess.run(cpp_args, check=True, cwd=repo_root, env=process_env)
    if not generated_by_asmproc:
        generated = preprocessed.read_text(encoding="utf-8")
        for builtin, standard in (("__builtin_va_list", "va_list"), ("__builtin_va_start", "va_start"), ("__builtin_va_arg", "va_arg"), ("__builtin_va_end", "va_end")):
            generated = generated.replace(builtin, standard)
        escaped = "".join(
            char
            if ord(char) < 0x80
            else "".join(f"\\{byte:03o}" for byte in char.encode("euc-jp"))
            for char in generated
        )
        preprocessed.write_text(escaped, encoding="ascii", newline="\n")
    compile_args = [str(preprocessed) if arg == source else native_option(arg).replace("\\\\\"", "\"") for arg in compiler_args if not arg.startswith("-DBUILD_VERSION_STRING=")]
    compile_args[compile_args.index("-o") + 1] = str(output)
    env = process_env
    ido_tmp_root = Path(os.environ.get("IDO_TMPDIR", str(repo_root / ".ido-tmp")))
    ido_tmp_root.mkdir(parents=True, exist_ok=True)
    command = subprocess.list2cmdline([str(compiler_path), *compile_args])
    compile_cwd = repo_root if generated_by_asmproc else preprocessed.parent
    # IDO uses fixed scratch filenames. Give every invocation a private directory
    # so parallel make jobs cannot overwrite each other's assembler input.
    with tempfile.TemporaryDirectory(prefix="ido-", dir=ido_tmp_root) as ido_tmp:
        for temp_var in ("TMPDIR", "TEMP", "TMP"):
            env[temp_var] = ido_tmp
        return subprocess.run(command, env=env, cwd=compile_cwd, shell=True).returncode


if __name__ == "__main__":
    raise SystemExit(main())
