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
#  - PYTHON_LIB: configure's "tr '\r\n' ' '" leaves a trailing space that
#    autoconf keeps and CMake's string(STRIP) removes.
#  - TRICK_GCC_VERSION: autoconf uses "$CC -dumpfullversion -dumpversion";
#    CMake uses CMAKE_CXX_COMPILER_VERSION. Both resolve to the compiler's
#    dotted version triplet on every GCC we support; tolerate a trailing
#    ".0" CMake sometimes omits (e.g. "11.4" vs "11.4.0").
#
# Known NOT-normalized, environment-specific delta (not a bug): on a
# machine with more than one X11 provider installed (e.g. a dev Mac with
# both Homebrew's libx11 formula AND the legacy /usr/X11 selector),
# autoconf's AC_PATH_X and CMake's FindX11 module can each pick a
# different, independently valid installation, producing a different
# X_LIB_DIR value. Every CI image here installs exactly one X11 provider,
# so this does not appear in CI; it is not worth replicating AC_PATH_X's
# legacy search heuristic in CMake to chase it on developer machines.
normalize() {
    local infile="$1"
    local outfile="$2"
    sed -E \
        -e 's/[[:space:]]+$//' \
        -e '/^[[:space:]]*#/d' \
        -e '/^[[:space:]]*$/d' \
        -e 's/^(PYTHON_LIB[[:space:]]*=.*[^[:space:]])[[:space:]]+$/\1/' \
        -e 's/^(TRICK_GCC_VERSION[[:space:]]*=[[:space:]]*[0-9]+\.[0-9]+)$/\1.0/' \
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
