#!/usr/bin/env bash
set -euo pipefail
root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
rom="$root/baseroms/us/baserom.z64"
temporary=0
cleanup() {
  status=$?
  [ "$temporary" -eq 0 ] || rm -f "$rom"
  return "$status"
}
trap cleanup EXIT
if [ ! -f "$rom" ]; then
  : "${POKESTADIUM_BASEROM:?Set POKESTADIUM_BASEROM to the retail US ROM path}"
  mkdir -p "$(dirname "$rom")"
  cp "$POKESTADIUM_BASEROM" "$rom"
  temporary=1
fi
[ -f "$root/baseroms/us/checksum.md5" ] || { echo 'missing checksum.md5' >&2; exit 1; }
docker build --platform linux/amd64 --tag "${POKESTADIUM_DECOMP_AGENT_IMAGE:-pokestadium-decomp-agent:latest}" --file "$root/ci/Dockerfile" "$root"
