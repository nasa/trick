#!/usr/bin/env bash
#
# Universal acceptance test (CMAKE_MIGRATION_PLAN.md A2/Part E rule 4): a sim built
# with trick-CP against a CMake-installed Trick must compile and run. This drives
# the real installed product interface — trick-CP -> Makefile.common -> config_user.mk
# -> trick-ICG/SWIG — against a staged/installed TRICK_HOME, exactly as an end user
# would.
#
# Usage: run_sim_flow.sh <TRICK_HOME> <sim_dir> [run_input]
#   TRICK_HOME  a CMake-installed (or staged) Trick prefix
#   sim_dir     path to a sim directory containing an S_define
#   run_input   input file to run, relative to sim_dir (default RUN_test/input.py)
#
set -euo pipefail

if [ $# -lt 2 ]; then
    echo "usage: $0 <TRICK_HOME> <sim_dir> [run_input]" >&2
    exit 2
fi

export TRICK_HOME="$(cd "$1" && pwd)"
SIM_DIR="$(cd "$2" && pwd)"
RUN_INPUT="${3:-RUN_test/input.py}"

export PATH="${TRICK_HOME}/bin:${PATH}"

echo "=== sim-flow: TRICK_HOME=${TRICK_HOME}"
echo "=== sim-flow: building ${SIM_DIR} with trick-CP"
cd "${SIM_DIR}"
# Clean any stale build so this genuinely exercises ICG/SWIG/compile against the
# installed Trick, not leftover artifacts from another build system.
trick-CP

TRICK_HOST_CPU="$(trick-gte TRICK_HOST_CPU)"
SIM_EXE="S_main_${TRICK_HOST_CPU}.exe"
if [ ! -x "${SIM_DIR}/${SIM_EXE}" ]; then
    echo "sim-flow FAILED: ${SIM_EXE} was not produced" >&2
    exit 1
fi

echo "=== sim-flow: running ${SIM_EXE} ${RUN_INPUT}"
"${SIM_DIR}/${SIM_EXE}" "${RUN_INPUT}"
echo "=== sim-flow: OK"
