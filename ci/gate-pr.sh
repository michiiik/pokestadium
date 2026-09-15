#!/usr/bin/env bash
# Run a PR checkout against the isolated retail baseline image.
set -euo pipefail
[ "$#" -ge 1 ] && [ "$#" -le 2 ] || { echo "usage: $0 CHECKOUT [OUT]" >&2; exit 2; }
src="$(cd "$1" && pwd)"; out="${2:-}"
for required in Makefile src include linker_scripts yamls tools lib; do [ -e "$src/$required" ] || { echo "missing $required" >&2; exit 1; }; done
image="${POKESTADIUM_DECOMP_AGENT_IMAGE:-pokestadium-decomp-agent:latest}"
log="$(mktemp "${TMPDIR:-/tmp}/pokestadium-gate.XXXXXX.log")"; trap 'rm -f "$log"' EXIT
args=(run --rm --platform linux/amd64 --network none --cap-drop=ALL --security-opt=no-new-privileges --pids-limit=2048 -v "$src:/src:ro")
if [ -n "$out" ]; then mkdir -p "$out"; args+=(-v "$out:/out:rw"); fi
set +e
docker "${args[@]}" --entrypoint bash "$image" -c '
set -euo pipefail
rm -rf /work/src /work/include /work/tools /work/linker_scripts /work/lib
for path in src include tools lib; do cp -a "/src/$path" "/work/$path"; done
mkdir -p /work/linker_scripts; cp -a /src/linker_scripts/. /work/linker_scripts/
cp -a /src/Makefile /work/Makefile
cmp -s /src/requirements.txt /work/requirements.txt || { echo "requirements changed; rebuild baseline image" >&2; exit 3; }
if ! diff -qr /src/yamls /work/yamls >/dev/null 2>&1; then rm -rf /work/yamls /work/asm /work/assets; cp -a /src/yamls /work/yamls; make extract; fi
if [ ! -f /work/build/pokestadium-us.map ]; then make CC=tools/ido/linux/7.1/cc COMPARE=0 -j2 rom || true; fi
if [ -d /out ]; then python3 /src/ci/update_coverage.py --repo-root /src --output /out/COVERAGE.md; fi
jobs="${POKESTADIUM_DECOMP_JOBS:-$(nproc)}"; case "$jobs" in ""|*[!0-9]*|0) exit 2;; esac
make -B COMPARE=0 -j"$jobs" rom
md5sum -c baseroms/us/checksum.md5
' >"$log" 2>&1
status=$?; set -e
cat "$log"; [ -z "$out" ] || cp "$log" "$out/gate.log"
if [ "$status" -ne 0 ] || ! grep -q 'build/pokestadium-us\.z64: OK' "$log"; then exit "${status:-1}"; fi
echo 'gate-pr.sh: PASS -- build/pokestadium-us.z64: OK' >&2
