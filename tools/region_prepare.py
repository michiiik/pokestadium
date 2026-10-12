#!/usr/bin/env python3
"""Scaffold a region or revision that is built from its own disassembly.

usage: tools/region_prepare.py REGION            scaffold + yaml/symbol pipeline
       tools/region_prepare.py REGION --build    extract and build (Linux container)

REGION is a key of ci/regions.json. The retail ROM goes in
baseroms/REGION/baserom.z64 (never committed). The first form writes
baseroms/REGION/checksum.md5, yamls/REGION/ and linker_scripts/REGION/ by
running the jp_* tools with REGION set. --build then extracts the split, links
the asm-only ROM, resolves undefined symbols from the link log, and checks the
result against the retail checksum.
"""
import hashlib
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path.cwd()
REGIONS = {k: v for k, v in json.loads((ROOT / "ci/regions.json").read_text()).items() if not k.startswith("_")}
PIPELINE = ("jp_offset_probe", "jp_remap_yaml", "jp_align_yaml", "jp_transfer_symbols", "jp_asm_yaml")


def sh(*cmd, env=None, check=True):
    r = subprocess.run(cmd, cwd=ROOT, env={**os.environ, **(env or {})}, capture_output=True, text=True)
    if check and r.returncode:
        sys.exit(f"{' '.join(cmd)} failed:\n{(r.stdout + r.stderr)[-2500:]}")
    return r


def scaffold(region):
    rom = ROOT / f"baseroms/{region}/baserom.z64"
    if not rom.exists():
        sys.exit(f"missing {rom} (retail dump, not committed)")
    data = rom.read_bytes()
    md5 = hashlib.md5(data).hexdigest()
    if md5 != REGIONS[region]["md5"]:
        sys.exit(f"{rom} md5 {md5} != registry {REGIONS[region]['md5']}")
    (rom.parent / "checksum.md5").write_text(f"{md5}  build/pokestadium-{region}.z64")
    ydir, ldir = ROOT / "yamls" / region, ROOT / "linker_scripts" / region
    ydir.mkdir(parents=True, exist_ok=True)
    ldir.mkdir(parents=True, exist_ok=True)
    head = (ROOT / "yamls/jp/header.yaml").read_text()
    head = head.replace("/jp/", f"/{region}/").replace("asm/jp", f"asm/{region}").replace("assets/jp", f"assets/{region}")
    head = re.sub(r"^name: .*$", f"name: Pokemon Stadium ({REGIONS[region]['name']})", head, flags=re.M)
    head = re.sub(r"^sha1: .*$", f"sha1: {hashlib.sha1(data).hexdigest()}", head, flags=re.M)
    (ydir / "header.yaml").write_text(head)
    shutil.copy(ROOT / "linker_scripts/us/hardware_regs.ld", ldir / "hardware_regs.ld")
    for f in ("unused_syms.ld", "undefined_syms.ld", "c_files.ld"):
        (ldir / f).touch()
    (ldir / "auto").mkdir(exist_ok=True)


def pipeline(region):
    scaffold(region)
    for tool in PIPELINE:
        print(f"{region}: {tool}", flush=True)
        sh(sys.executable, f"tools/{tool}.py", env={"REGION": region})


def build(region):
    env = {"REGION": region}
    sh("make", f"VERSION={region}", "extract", f"PYTHON={sys.executable}")
    log = ""
    for attempt in range(4):
        r = sh("make", f"VERSION={region}", "rom", "-k", f"PYTHON={sys.executable}", check=False)
        log = r.stdout + r.stderr
        if "undefined reference" not in log:
            break
        lf = Path(tempfile.mkdtemp()) / "link.log"
        lf.write_text(log)
        sh(sys.executable, "tools/jp_undefined_syms.py", str(lf), env=env)
        for f in (f"build/pokestadium-{region}.elf", f"build/linker_scripts/{region}/undefined_syms.ld"):
            Path(ROOT / f).unlink(missing_ok=True)
    ok = f"pokestadium-{region}.z64: OK" in log
    print(f"{region}: {'MATCH' if ok else 'NO MATCH'}")
    if not ok:
        print(log[-2000:])
    return 0 if ok else 1


def main():
    if len(sys.argv) < 2 or sys.argv[1] not in REGIONS:
        sys.exit(__doc__)
    region = sys.argv[1]
    if region in ("us",):
        sys.exit("us is the reference build")
    return build(region) if "--build" in sys.argv else (pipeline(region) or 0)


if __name__ == "__main__":
    sys.exit(main())
