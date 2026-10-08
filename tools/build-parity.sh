#!/usr/bin/env bash
#
# Capture and compare the observable outputs of a Trick build, so a new build
# system can be checked against the autotools/Make build it replaces.
#
# Usage:
#   tools/build-parity.sh capture <trick_install_dir> <out_dir>
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
# sim-test  Runs trickops against test_sims.yml. Records one line per job in
#           sim-results.txt and make's exit status in sim-test-status.txt, so
#           compare sees a failed run. Exits non-zero if make failed or no job
#           results were found; the captured files are written either way.
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

# Defined external symbols of one archive. Member names are omitted on
# purpose: object file names differ between build systems.
archive_symbols() {
    nm -g -P "$1" 2>/dev/null \
        | awk 'NF >= 2 && $2 != "U" && $2 != "w" && $2 != "v" { print $2, $1 }' \
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
    echo "$status" > "$out/sim-test-status.txt"
    echo "build-parity: sim_test exit status $status; $(grep -c ' succeeded$' "$out/sim-results.txt" || true) succeeded," \
         "$(grep -vc ' succeeded$' "$out/sim-results.txt" || true) other -> $out/sim-results.txt"
    if [[ ! -s $out/sim-results.txt ]]; then
        echo "build-parity: no job results in $out/sim-test.log" >&2
        return 1
    fi
    return "$status"
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

[[ $# -eq 3 ]] || die "usage: $0 {capture|sim-test|compare} <dir> <dir>"
case $1 in
    capture)  capture "$2" "$3" ;;
    sim-test) sim_test "$2" "$3" ;;
    compare)  compare "$2" "$3" ;;
    *)        die "unknown command: $1" ;;
esac
