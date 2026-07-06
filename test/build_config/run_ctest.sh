#!/usr/bin/env bash
# Phase 3 install + CTest gate (CMAKE_MIGRATION_PLAN.md Part C, Phase 3): does the
# full three-step CMake build (see cmake/TrickICG.cmake), stages a runnable
# TRICK_HOME with `cmake --build ... --target stage`, then runs CTest. The unit
# tests build mini-sims through the real trick-CP flow against the stage, which
# doubles as install/interface validation; the sim-flow tests are the universal
# acceptance test (a real sim built and run by trick-CP from the installed prefix).
#
# Usage: test/build_config/run_ctest.sh [ctest-label-regex]   (default: unit|sims)
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$REPO_ROOT"

LABELS="${1:-unit|sims}"
BUILD_DIR="${TRICK_CMAKE_BUILD_DIR:-$REPO_ROOT/build}"
NPROC="$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)"

# Three-step io_src bootstrap (a nonexistent-at-first-configure generated list
# does not wire itself into CMAKE_CONFIGURE_DEPENDS — see cmake/TrickICG.cmake).
cmake -S . -B "$BUILD_DIR"
cmake --build "$BUILD_DIR" --target trick-ICG trick_io_src_gen -j"$NPROC"
cmake -S . -B "$BUILD_DIR"
cmake --build "$BUILD_DIR" -j"$NPROC"

# Stage a runnable TRICK_HOME (build/stage) — the fixture in cmake/TrickTest.cmake
# also does this, but doing it explicitly here surfaces install errors directly.
cmake --build "$BUILD_DIR" --target stage -j"$NPROC"

ctest --test-dir "$BUILD_DIR" --output-on-failure -L "$LABELS"
