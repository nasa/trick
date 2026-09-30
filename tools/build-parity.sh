#!/usr/bin/env bash
#
# Capture and compare the observable outputs of a Trick build, so a new build
# system can be checked against the autotools/Make build it replaces.
#
# Usage:
#   tools/build-parity.sh capture <trick_install_dir> <out_dir>
#   tools/build-parity.sh archives <build_dir> <out_dir>
#   tools/build-parity.sh check-cmake <make_lib_dir> <cmake_build_dir> <out_dir>
#   tools/build-parity.sh check-icg <make_icg> <cmake_build_dir> <out_dir>
#   tools/build-parity.sh sim-test <trick_source_dir> <out_dir>
#   tools/build-parity.sh compare <baseline_dir> <candidate_dir>
#
# capture   Records, for an installed Trick tree:
#             manifest.txt        every installed path, with type (f, x, l)
#             symbols/<lib>.txt   defined external symbols of each static archive
#             trick-config.txt    output of each trick-config option
#             make-vars.txt       resolved values of the sim-facing make variables
#             config_user.mk      the installed configuration file, verbatim
#           Absolute occurrences of the install dir are rewritten to
#           ${TRICK_HOME} so captures from different prefixes compare equal.
#
# archives  Records symbols/<lib>.txt, as capture does, for every lib*.a found
#           under a build directory. Use it to check libraries before an
#           install tree exists; compare its output with a capture's symbols/.
#
# check-cmake
#           Compares the archives the CMake build produces so far
#           (CMAKE_ARCHIVES below) with the same archives from a Make build in
#           the same tree. Both builds must use the same compiler and no
#           optimization; configure CMake with the parity preset. Exits
#           non-zero on any difference. Used by CI during the migration.
#
# check-icg Runs the Make-built trick-ICG and the one under a CMake build
#           directory on include/trick/files_to_ICG.hh, the way the Make build
#           does, and compares the code they generate. Run it from the top of a
#           configured source tree. Exits non-zero on any difference.
#
# sim-test  Runs trickops against test_sims.yml and records one line per job
#           (OK / FAIL / NOT RUN) in sim-results.txt.
#
# compare   Diffs every file two captures have in common and reports files
#           present in only one of them. Exits non-zero on any difference.

set -euo pipefail

die() { echo "build-parity: $*" >&2; exit 2; }

# Variables sims and trick-config read from Makefile.common / config_user.mk.
MAKE_VARS=(
    TRICK_HOST_TYPE TRICK_HOST_CPU TRICK_VERSION TRICK_MAJOR TRICK_MINOR
    TRICK_LIB_DIR LIBEXEC TRICK_INCLUDES TRICK_VERSIONS
    TRICK_CC TRICK_CXX TRICK_LD
    TRICK_SYSTEM_CFLAGS TRICK_SYSTEM_CXXFLAGS TRICK_SYSTEM_SFLAGS
    TRICK_SYSTEM_LDFLAGS TRICK_SYSTEM_ICG_EXCLUDE TRICK_EXCLUDE
    TRICK_LIBS TRICK_EXEC_LINK_LIBS TRICK_TEST_FLAGS
    HDF5_LIB LD_PARTIAL LD_WHOLE_ARCHIVE LD_NO_WHOLE_ARCHIVE
    SHARED_LIB_OPT RPATH PLATFORM_LIBS
)

# Archives the CMake build produces so far. Add each archive as the migration
# builds it.
CMAKE_ARCHIVES=(
    liber7_utils
    libtrick_comm
    libtrick_connection_handlers
    libtrick_math
    libtrick_optimization
    libtrick_units
    libtrick_var_binary_parser
)
# Archive members the CMake build does not produce yet: ICG's generated io_*
# code. Remove once the CMake build runs ICG.
CMAKE_EXCLUDE_MEMBERS='^io_'

# Defined external symbols of one archive. Member names are omitted on
# purpose: object file names differ between build systems. If
# PARITY_EXCLUDE_MEMBERS is set, symbols from members whose name matches that
# regular expression are left out.
archive_symbols() {
    nm -g -P "$1" 2>/dev/null \
        | awk -v exclude="${PARITY_EXCLUDE_MEMBERS:-}" '
            # Member headers: "lib.a[member.o]:" or "member.o:".
            NF == 1 && /:$/ {
                member = $1
                sub(/^.*\[/, "", member)
                sub(/\]?:$/, "", member)
                skip = (exclude != "" && member ~ exclude)
                next
            }
            !skip && NF >= 2 && $2 != "U" && $2 != "w" && $2 != "v" { print $2, $1 }' \
        | LC_ALL=C sort -u
}

normalize() {
    # Rewrite the install prefix; sed delimiter chosen to be absent from paths.
    sed -e "s|$1|\${TRICK_HOME}|g"
}

capture() {
    local home out
    home=$(cd "$1" && pwd -P)
    out=$2
    [[ -x $home/bin/trick-config ]] || die "$home does not look like a Trick install (no bin/trick-config)"
    rm -rf "$out"
    mkdir -p "$out/symbols"

    # Manifest: f = regular file, x = executable file, l = symlink.
    (
        cd "$home"
        find . -mindepth 1 \( -type f -o -type l \) | LC_ALL=C sort | while IFS= read -r p; do
            if [[ -L $p ]]; then t=l; elif [[ -x $p ]]; then t=x; else t=f; fi
            printf '%s\t%s\n' "$t" "${p#./}"
        done
    ) > "$out/manifest.txt"

    local lib
    while IFS= read -r lib; do
        archive_symbols "$lib" > "$out/symbols/$(basename "$lib" .a).txt"
    done < <(find "$home" -name '*.a' -path '*/lib*/*' -not -path '*/java/*' | LC_ALL=C sort)

    # trick-config and make variables must run with the captured tree as
    # TRICK_HOME, not whatever the caller's environment points at.
    local opt
    for opt in version prefix includedir libdir cflags cxxflags ldflags libs; do
        printf -- '--%s\t%s\n' "$opt" \
            "$(env -u TRICK_HOME "$home/bin/trick-config" --$opt 2>&1 | tr '\n' ' ' | sed 's/ *$//')"
    done | normalize "$home" > "$out/trick-config.txt"

    local var
    for var in "${MAKE_VARS[@]}"; do
        TRICK_HOME=$home make -s -f "$home/share/trick/makefiles/Makefile.trickconfig" "print-$var" 2>&1
    done | normalize "$home" > "$out/make-vars.txt"

    normalize "$home" < "$home/share/trick/makefiles/config_user.mk" > "$out/config_user.mk"

    {
        echo "captured: $(date -u +%Y-%m-%dT%H:%M:%SZ)"
        echo "host: $(uname -srm)"
        echo "commit: $(git -C "$(dirname "$0")" rev-parse HEAD 2>/dev/null || echo unknown)"
        echo "trick_home: $home"
    } > "$out/capture-info.txt"

    echo "build-parity: captured $home -> $out"
}

archives() {
    local dir=$1 out=$2 lib
    [[ -d $dir ]] || die "$dir is not a directory"
    rm -rf "$out/symbols"
    mkdir -p "$out/symbols"
    while IFS= read -r lib; do
        archive_symbols "$lib" > "$out/symbols/$(basename "$lib" .a).txt"
    done < <(find "$dir" -name 'lib*.a' | LC_ALL=C sort)
    echo "build-parity: recorded $(find "$out/symbols" -type f | wc -l | tr -d ' ') archives from $dir -> $out/symbols"
}

check_cmake() {
    local make_dir=$1 cmake_dir=$2 out=$3 name side dir lib rc=0
    rm -rf "$out"
    for name in "${CMAKE_ARCHIVES[@]}"; do
        for side in make cmake; do
            if [[ $side == make ]]; then dir=$make_dir; else dir=$cmake_dir; fi
            lib=$(find "$dir" -name "$name.a" | head -n 1)
            if [[ -z $lib ]]; then
                echo "build-parity: $name.a not found under $dir"
                rc=1
                continue
            fi
            mkdir -p "$out/$side"
            PARITY_EXCLUDE_MEMBERS=$CMAKE_EXCLUDE_MEMBERS archive_symbols "$lib" > "$out/$side/$name.txt"
        done
    done
    compare "$out/make" "$out/cmake" || rc=1
    return $rc
}

check_icg() {
    local make_icg=$1 cmake_dir=$2 out=$3 cmake_icg flags side exe
    cmake_icg=$(find "$cmake_dir" -name trick-ICG -type f | head -n 1)
    [[ -x $make_icg ]] || die "$make_icg is not an executable"
    [[ -n $cmake_icg ]] || die "no trick-ICG under $cmake_dir"
    [[ -f include/trick/files_to_ICG.hh ]] || die "run check-icg from the top of the Trick source tree"
    export TRICK_HOME=$PWD
    # The Make build's flags for ICG'ing Trick itself: TRICK_CXXFLAGS plus
    # TRICK_SYSTEM_CXXFLAGS, with -isystem changed to -I so ICG doesn't skip
    # Trick's headers.
    flags=$(make -s -f share/trick/makefiles/Makefile.trickconfig print-TRICK_SYSTEM_CXXFLAGS)
    flags=${flags#*=}
    flags=${flags//-isystem/-I}
    rm -rf "$out"
    for side in make cmake; do
        if [[ $side == make ]]; then exe=$make_icg; else exe=$cmake_icg; fi
        mkdir -p "$out/$side"
        # shellcheck disable=SC2086 # flags is a list of arguments
        "$exe" -sim_services -m -o "$out/$side" -std=c++17 $flags include/trick/files_to_ICG.hh \
            > "$out/$side.log" 2>&1 || { cat "$out/$side.log"; die "$exe failed"; }
    done
    if diff -r "$out/make" "$out/cmake"; then
        echo "build-parity: trick-ICG output identical ($(find "$out/make" -type f | wc -l | tr -d ' ') files)"
    else
        return 1
    fi
}

sim_test() {
    local src out
    src=$(cd "$1" && pwd -P)
    out=$2
    mkdir -p "$out"
    local status=0
    (cd "$src" && CI=1 make sim_test) > "$out/sim-test.log" 2>&1 || status=$?
    # Strip colors; keep trickops' per-job status lines and its summary counts.
    sed -e $'s/\x1b\\[[0-9;]*m//g' "$out/sim-test.log" \
        | grep -E ' (succeeded|failed|timed out|was not run)$' \
        | grep -v '^|' \
        | sed -E 's/^ +//; s/ +/ /g' \
        | LC_ALL=C sort > "$out/sim-results.txt" || true
    echo "build-parity: sim_test exit status $status; $(grep -c ' succeeded$' "$out/sim-results.txt" || true) succeeded," \
         "$(grep -vc ' succeeded$' "$out/sim-results.txt" || true) other -> $out/sim-results.txt"
}

compare() {
    local a=$1 b=$2 rc=0 f
    [[ -d $a && -d $b ]] || die "compare needs two capture directories"
    while IFS= read -r f; do
        if [[ ! -e $a/$f ]]; then echo "only in candidate: $f"; rc=1
        elif [[ ! -e $b/$f ]]; then echo "only in baseline: $f"; rc=1
        elif ! diff -u --label "baseline/$f" --label "candidate/$f" "$a/$f" "$b/$f"; then rc=1
        fi
    done < <( (cd "$a" && find . -type f; cd "$OLDPWD" && cd "$b" && find . -type f) \
                | sed 's|^\./||' | grep -vxE 'capture-info.txt|sim-test.log' | LC_ALL=C sort -u)
    [[ $rc -eq 0 ]] && echo "build-parity: no differences"
    return $rc
}

case $1 in
    check-cmake) [[ $# -eq 4 ]] || die "usage: $0 check-cmake <make_lib_dir> <cmake_build_dir> <out_dir>" ;;
    check-icg)   [[ $# -eq 4 ]] || die "usage: $0 check-icg <make_icg> <cmake_build_dir> <out_dir>" ;;
    *)           [[ $# -eq 3 ]] || die "usage: $0 {capture|archives|sim-test|compare} <dir> <dir>" ;;
esac
case $1 in
    capture)  capture "$2" "$3" ;;
    archives) archives "$2" "$3" ;;
    check-cmake) check_cmake "$2" "$3" "$4" ;;
    check-icg)   check_icg "$2" "$3" "$4" ;;
    sim-test) sim_test "$2" "$3" ;;
    compare)  compare "$2" "$3" ;;
    *)        die "unknown command: $1" ;;
esac
