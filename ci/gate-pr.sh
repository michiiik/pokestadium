#!/usr/bin/env bash
# Validate a public Stadium 1 PR in a disposable, network-isolated container.

set -euo pipefail

usage() {
    echo "usage: $(basename "$0") <checkout-dir> [output-dir]" >&2
    exit 2
}

[ "$#" -ge 1 ] && [ "$#" -le 2 ] || usage

checkout_dir="$1"
output_dir="${2:-}"
[ -d "$checkout_dir" ] || { echo "gate-pr.sh: missing checkout: $checkout_dir" >&2; exit 1; }
checkout_dir="$(cd "$checkout_dir" && pwd)"

for required in Makefile src include linker_scripts yamls tools lib; do
    [ -e "$checkout_dir/$required" ] || {
        echo "gate-pr.sh: missing public checkout path: $required" >&2
        exit 1
    }
done

image="${POKESTADIUM_DECOMP_AGENT_IMAGE:-pokestadium-decomp-agent:latest}"
log_file="$(mktemp "${TMPDIR:-/tmp}/pokestadium-gate.XXXXXX.log")"
cleanup() { rm -f "$log_file"; }
trap cleanup EXIT

docker_args=(
    run --rm
    --platform linux/amd64
    --network none
    --cap-drop=ALL
    --security-opt=no-new-privileges
    --pids-limit=2048
    -v "$checkout_dir:/src:ro"
)
if [ -d "$checkout_dir/.git" ]; then
    docker_args+=( -v "$checkout_dir/.git:/work/.git:ro" )
fi
if [ -n "$output_dir" ]; then
    mkdir -p "$output_dir"
    docker_args+=( -v "$output_dir:/out:rw" )
fi

set +e
docker "${docker_args[@]}" --entrypoint bash "$image" -c '
set -euo pipefail
git config --global --add safe.directory /work

if ! cmp -s /src/requirements.txt /work/requirements.txt; then
    echo "gate-pr.sh: requirements.txt changed; rebuild the baseline image" >&2
    exit 3
fi

rm -rf /work/src /work/include /work/tools /work/linker_scripts /work/lib /work/yamls
cp -a /src/src /work/src
cp -a /src/include /work/include
cp -a /src/tools /work/tools
cp -a /src/linker_scripts /work/linker_scripts
cp -a /src/lib /work/lib
cp -a /src/yamls /work/yamls
cp -a /src/Makefile /work/Makefile

# Rebuild ignored asm/assets from the exact PR YAML and source namespace.
make extract

if [ -d /out ]; then
    python3 /src/ci/update_coverage.py --repo-root /src --output /out/COVERAGE.md
fi

make cc-check RUN_CC_CHECK=1
make COMPARE=0 -j2 rom
md5sum -c baseroms/us/checksum.md5
' >"$log_file" 2>&1
status=$?
set -e

cat "$log_file"
if [ -n "$output_dir" ]; then
    cp "$log_file" "$output_dir/gate.log"
fi
echo "gate-pr.sh: container exit status $status" >&2

if [ "$status" -ne 0 ]; then
    echo "gate-pr.sh: FAIL -- extraction, syntax check, build, or checksum failed" >&2
    exit "$status"
fi

grep -q '^build/pokestadium-us\.z64: OK$' "$log_file" || {
    echo "gate-pr.sh: FAIL -- expected checksum acceptance line was absent" >&2
    exit 1
}
echo "gate-pr.sh: PASS -- build/pokestadium-us.z64: OK" >&2
