#!/usr/bin/env bash
# Archive-membership parity test (Part C, Phase 2, step 6 / R4 in
# CMAKE_MIGRATION_PLAN.md): builds Trick's core libraries with both `make`
# and CMake on the same machine and diffs `ar t` member-basename sets for
# every one of the nine archives listed in A3. This is a mandatory gate —
# archive membership drift breaks sim link lines subtly (TRICK_LIBS in
# Makefile.common:70), often without a build-time error.
#
# Usage: test/build_config/compare_archives.sh
# Requires: a working `./configure` toolchain, and everything Phase 1/2's
# CMake detection needs (same dependencies as ./configure).
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$REPO_ROOT"

WORKDIR="$(mktemp -d)"
trap 'rm -rf "$WORKDIR"' EXIT

# Archive name -> path relative to the make build's lib/ dir vs. the cmake
# build tree (paths differ because cmake keeps per-directory build trees;
# see each CMakeLists.txt's ARCHIVE_OUTPUT_NAME/output directory).
ARCHIVES=(
    "libtrick.a:libtrick.a"
    "liber7_utils.a:trick_source/er7_utils/liber7_utils.a"
    "libtrick_mm.a:trick_source/sim_services/MemoryManager/libtrick_mm.a"
    "libtrick_pyip.a:trick_source/trick_swig/libtrick_pyip.a"
    "libtrick_comm.a:trick_source/trick_utils/comm/libtrick_comm.a"
    "libtrick_connection_handlers.a:trick_source/trick_utils/connection_handlers/libtrick_connection_handlers.a"
    "libtrick_math.a:trick_source/trick_utils/math/libtrick_math.a"
    "libtrick_optimization.a:trick_source/trick_utils/optimization/libtrick_optimization.a"
    "libtrick_units.a:trick_source/trick_utils/units/libtrick_units.a"
    "libtrick_var_binary_parser.a:trick_source/trick_utils/var_binary_parser/libtrick_var_binary_parser.a"
)

# ── 1. make build (skipped if lib/ already has a complete build — CI builds
# this once via the existing autotools job and this script re-uses it) ──────
if [ ! -f lib/libtrick.a ] && [ ! -f lib64/libtrick.a ]; then
    ./configure >"$WORKDIR/configure.log" 2>&1
    make -j"$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)" no_dp >"$WORKDIR/make.log" 2>&1 || {
        echo "make failed; see $WORKDIR/make.log" >&2
        exit 1
    }
fi
MAKE_LIB_DIR="lib"
[ -d lib64 ] && MAKE_LIB_DIR="lib64"

# ── 2. cmake build (three-step bootstrap — see cmake/TrickICG.cmake) ───────
CMAKE_BUILD_DIR="$WORKDIR/cmake-build"
cmake -S . -B "$CMAKE_BUILD_DIR" >"$WORKDIR/cmake_configure1.log" 2>&1
cmake --build "$CMAKE_BUILD_DIR" --target trick-ICG trick_io_src_gen -j"$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)" \
    >"$WORKDIR/cmake_build_icg.log" 2>&1
cmake -S . -B "$CMAKE_BUILD_DIR" >"$WORKDIR/cmake_configure2.log" 2>&1
cmake --build "$CMAKE_BUILD_DIR" -j"$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)" >"$WORKDIR/cmake_build.log" 2>&1 || {
    echo "cmake --build failed; see $WORKDIR/cmake_build.log" >&2
    exit 1
}

# ── 3. Compare `ar t` member basenames, normalizing away CMake's
# extension-in-object-name convention (Foo.cpp.o vs make's Foo.o) ──────────
normalize_ar() {
    ar t "$1" | grep -v '^__\.SYMDEF' | sed -E 's/\.(cpp|cc|c)\.o$/.o/' | sort
}

FAIL=0
for pair in "${ARCHIVES[@]}"; do
    make_name="${pair%%:*}"
    cmake_rel="${pair##*:}"
    make_path="$MAKE_LIB_DIR/$make_name"
    cmake_path="$CMAKE_BUILD_DIR/$cmake_rel"

    if [ ! -f "$make_path" ]; then
        echo "MISSING (make): $make_path" >&2
        FAIL=1
        continue
    fi
    if [ ! -f "$cmake_path" ]; then
        echo "MISSING (cmake): $cmake_path" >&2
        FAIL=1
        continue
    fi

    normalize_ar "$make_path" >"$WORKDIR/${make_name}.make.txt"
    normalize_ar "$cmake_path" >"$WORKDIR/${make_name}.cmake.txt"

    if diff -u "$WORKDIR/${make_name}.make.txt" "$WORKDIR/${make_name}.cmake.txt" >"$WORKDIR/${make_name}.diff"; then
        echo "$make_name: OK ($(wc -l <"$WORKDIR/${make_name}.make.txt") members)"
    else
        echo "$make_name: MISMATCH" >&2
        cat "$WORKDIR/${make_name}.diff" >&2
        FAIL=1
    fi
done

if [ "$FAIL" -ne 0 ]; then
    echo "Archive parity: FAILED" >&2
    exit 1
fi
echo "Archive parity: OK (all ${#ARCHIVES[@]} archives match)"
