#!/usr/bin/env bash
# Build the local public baseline image.
#
# Unlike a plain `docker build`, this stages a persistent, incrementally
# maintained host checkout's own build (asm/, assets/, lib/ultralib/build/,
# build/) into the image instead of compiling inside Docker. A `COPY . /work`
# followed by `make extract && make rom` inside the Dockerfile busts its own
# build cache on every single repo change -- including to the Dockerfile
# itself -- so every previous rebuild paid the full ~20-30 minute bake from
# scratch regardless of how small the actual change was. Make's own
# incremental dependency tracking on a real, persistent checkout does not
# have that problem: a small change (the common case -- a yaml-touching PR
# merging) rebuilds in a couple of minutes, because only what actually
# changed gets recompiled.
#
# Usage:
#   POKESTADIUM_BASEROM=/private/path/baserom.z64 ci/build-baseline.sh
#
# Environment:
#   POKESTADIUM_CI_CHECKOUT   the persistent, incrementally-built host
#                               checkout (default: a fixed path next to this
#                               repo's own sibling checkouts -- see below).
#                               Created and `make setup`/`make venv`'d once
#                               if it doesn't exist yet; never destroyed.
#   POKESTADIUM_BASEROM       a legally-obtained retail ROM dump, needed
#                               only the first time this persistent checkout
#                               is created.
#   POKESTADIUM_DECOMP_AGENT_IMAGE       image tag to build (default:
#                               pokestadium-decomp-agent:latest).

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
ci_checkout="${POKESTADIUM_CI_CHECKOUT:-$HOME/projects/pokestadium-ci-baseline}"
target_sha="$(git -C "$repo_root" rev-parse HEAD)"

echo "build-baseline.sh: target commit $target_sha" >&2
echo "build-baseline.sh: persistent checkout at $ci_checkout" >&2

# Create the persistent checkout once. This never happens in normal
# operation on a runner that already has it -- only on a fresh runner, or
# after a deliberate `rm -rf` recovery.
if [ ! -d "$ci_checkout/.git" ]; then
    echo "build-baseline.sh: persistent checkout does not exist yet, cloning" >&2
    mkdir -p "$(dirname "$ci_checkout")"
    git clone --quiet "$(git -C "$repo_root" remote get-url origin)" "$ci_checkout"

    baserom="$ci_checkout/baseroms/us/baserom.z64"
    source_baserom="${POKESTADIUM_BASEROM:-}"
    if [ -z "$source_baserom" ] || [ ! -f "$source_baserom" ]; then
        echo "build-baseline.sh: first-time setup needs POKESTADIUM_BASEROM (a legally obtained baserom.z64) to seed $ci_checkout" >&2
        exit 1
    fi
    mkdir -p "$(dirname "$baserom")"
    cp "$source_baserom" "$baserom"

    ( cd "$ci_checkout" && make venv && make setup )
fi

if [ ! -f "$ci_checkout/baseroms/us/baserom.z64" ]; then
    echo "build-baseline.sh: $ci_checkout is missing baseroms/us/baserom.z64 -- place a legally obtained retail ROM there once, per this repo's own README" >&2
    exit 1
fi

# Sync the persistent checkout to the exact commit being built. A plain
# `git pull` would merge; this checkout only ever mirrors upstream, so hard
# reset to whatever ref triggered this build.
git -C "$ci_checkout" fetch --quiet origin "$target_sha"
git -C "$ci_checkout" checkout --quiet --detach "$target_sha"

# Re-extract only when yamls/ actually changed since the last extraction --
# splat's output depends on it, nothing else does. A plain `git diff
# --quiet` against the previous commit is not available here (this
# checkout may have jumped several commits, or none, since its last
# build), so compare a content hash of yamls/ against a marker this script
# itself writes after a successful extraction.
yamls_hash="$(cd "$ci_checkout" && find yamls -type f -print0 | sort -z | xargs -0 shasum -a 256 | shasum -a 256 | cut -d' ' -f1)"
marker="$ci_checkout/asm/.build-baseline-yamls-hash"
if [ ! -d "$ci_checkout/asm" ] || [ ! -f "$marker" ] || [ "$(cat "$marker" 2>/dev/null)" != "$yamls_hash" ]; then
    echo "build-baseline.sh: yamls/ changed (or first build) -- re-extracting" >&2
    ( cd "$ci_checkout" && make extract )
    mkdir -p "$ci_checkout/asm"
    echo "$yamls_hash" > "$marker"
else
    echo "build-baseline.sh: yamls/ unchanged since the last extraction, reusing asm/assets" >&2
fi

# Incremental, not forced: only what actually changed (relative to this
# same persistent checkout's own last build) gets recompiled. No CC=
# override -- the Makefile's own target-specific rule already routes
# still-unmatched GLOBAL_ASM files through asm-processor correctly; see
# ci/gate-pr.sh's own comment on why overriding CC here is exactly the bug
# that made the previous in-Docker bake fail.
( cd "$ci_checkout" && make COMPARE=0 -j"$(sysctl -n hw.ncpu 2>/dev/null || nproc)" rom )

# --- Stage a throwaway Docker build context from the persistent checkout ---
staging_dir="$(mktemp -d "${TMPDIR:-/tmp}/pokestadium-baseline-build.XXXXXX")"
cleanup() { rm -rf "$staging_dir"; }
trap cleanup EXIT

for path in src include tools linker_scripts lib yamls Makefile requirements.txt ci; do
    cp -a "$ci_checkout/$path" "$staging_dir/$path"
done
mkdir -p "$staging_dir/prebuilt"
cp -a "$ci_checkout/asm" "$staging_dir/prebuilt/asm"
cp -a "$ci_checkout/assets" "$staging_dir/prebuilt/assets"
cp -a "$ci_checkout/lib/ultralib/build" "$staging_dir/prebuilt/lib-ultralib-build"
cp -a "$ci_checkout/build" "$staging_dir/prebuilt/build"

baserom="$staging_dir/baseroms/us/baserom.z64"
mkdir -p "$(dirname "$baserom")"
cp "$ci_checkout/baseroms/us/baserom.z64" "$baserom"
cp "$ci_checkout/baseroms/us/checksum.md5" "$staging_dir/baseroms/us/checksum.md5"

image="${POKESTADIUM_DECOMP_AGENT_IMAGE:-pokestadium-decomp-agent:latest}"
docker build \
    --platform linux/amd64 \
    --tag "$image" \
    --file "$staging_dir/ci/Dockerfile" \
    "$staging_dir"

echo "build-baseline.sh: built $image from $target_sha (persistent checkout at $ci_checkout)"
