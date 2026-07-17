#!/usr/bin/env bash
# Golden-file parity test (Part C, Phase 1, step 6 / D4 in
# CMAKE_MIGRATION_PLAN.md): runs both ./configure and cmake against the same
# tree and diffs the two generated config_user.mk files after normalizing
# away differences that are cosmetic or otherwise known-harmless to
# Makefile.common consumers. Any other difference fails the test.
#
# Usage: test/build_config/compare_config_user.sh [extra cmake -D args...]
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$REPO_ROOT"

CONFIG_USER_MK="share/trick/makefiles/config_user.mk"
WORKDIR="$(mktemp -d)"

# ── 1. Run autoconf's ./configure, preserving any pre-existing config_user.mk ──
# configure writes into the source tree; save/restore so this script is
# non-destructive to a developer's existing build state.
HAD_ORIGINAL=0
if [ -f "$CONFIG_USER_MK" ]; then
    HAD_ORIGINAL=1
    cp "$CONFIG_USER_MK" "$WORKDIR/original_config_user.mk"
fi

restore_original() {
    if [ "$HAD_ORIGINAL" -eq 1 ]; then
        cp "$WORKDIR/original_config_user.mk" "$CONFIG_USER_MK"
    else
        rm -f "$CONFIG_USER_MK"
    fi
}
trap 'restore_original; rm -rf "$WORKDIR"' EXIT

./configure >"$WORKDIR/configure.log" 2>&1 || {
    echo "./configure failed; see $WORKDIR/configure.log" >&2
    cat "$WORKDIR/configure.log" >&2
    exit 1
}
cp "$CONFIG_USER_MK" "$WORKDIR/autoconf_config_user.mk"
restore_original

# ── 2. Run cmake into a scratch build dir ──────────────────────────────────
CMAKE_BUILD_DIR="$WORKDIR/cmake-build"
cmake -S . -B "$CMAKE_BUILD_DIR" "$@" >"$WORKDIR/cmake.log" 2>&1 || {
    echo "cmake configure failed; see $WORKDIR/cmake.log" >&2
    cat "$WORKDIR/cmake.log" >&2
    exit 1
}
CMAKE_GENERATED="$CMAKE_BUILD_DIR/share/trick/makefiles/config_user.mk"
if [ ! -f "$CMAKE_GENERATED" ]; then
    echo "cmake did not produce $CMAKE_GENERATED" >&2
    exit 1
fi
cp "$CMAKE_GENERATED" "$WORKDIR/cmake_config_user.mk"

# ── 3. Normalize both files ─────────────────────────────────────────────────
# Accepted deltas:
#  - TRICK_GCC_VERSION: autoconf uses "$CC -dumpfullversion -dumpversion";
#    CMake uses CMAKE_CXX_COMPILER_VERSION. Both resolve to the compiler's
#    dotted version triplet on every GCC we support; tolerate a trailing
#    ".0" CMake sometimes omits (e.g. "11.4" vs "11.4.0").
#  - X_LIB_DIR: confirmed in CI (not just hypothetical dev machines) on the
#    macOS runner, which has more than one valid X11 provider installed
#    (Homebrew's + Xquartz's /usr/X11); autoconf's AC_PATH_X and CMake's
#    FindX11 module each independently pick a different, equally valid
#    installation. Both resolve to a linkable libX11, so the value itself
#    (not just its presence/absence) is ignored here rather than replicating
#    AC_PATH_X's legacy search-order heuristic in CMake. On single-X11-
#    provider platforms this value should now match exactly (see
#    cmake/TrickPrograms.cmake's IMPLICIT_LINK_DIRECTORIES check) — this
#    normalization is a safety net for the multi-provider case, not a mask
#    for a real bug.
normalize() {
    local infile="$1"
    local outfile="$2"
    sed -E \
        -e 's/[[:space:]]+$//' \
        -e '/^[[:space:]]*#/d' \
        -e '/^[[:space:]]*$/d' \
        -e 's/^(TRICK_GCC_VERSION[[:space:]]*=[[:space:]]*[0-9]+\.[0-9]+)$/\1.0/' \
        -e 's/^X_LIB_DIR[[:space:]]*=.*/X_LIB_DIR = <ignored>/' \
        "$infile" | sort >"$outfile"
}

normalize "$WORKDIR/autoconf_config_user.mk" "$WORKDIR/autoconf_normalized.mk"
normalize "$WORKDIR/cmake_config_user.mk" "$WORKDIR/cmake_normalized.mk"

# ── 4. Diff ──────────────────────────────────────────────────────────────
if diff -u "$WORKDIR/autoconf_normalized.mk" "$WORKDIR/cmake_normalized.mk"; then
    echo "config_user.mk parity: OK"
    exit 0
else
    echo "config_user.mk parity: FAILED (see diff above)" >&2
    echo "autoconf: $WORKDIR/autoconf_config_user.mk" >&2
    echo "cmake:    $WORKDIR/cmake_config_user.mk" >&2
    exit 1
fi
