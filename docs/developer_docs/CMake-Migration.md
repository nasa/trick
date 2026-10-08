| [Home](/trick) → [Developer Docs](Developer-Docs-Home) → CMake Migration |
| ------------------------------------------------------------------------ |

# Migrating Trick's build to CMake

Trick's own build (its libraries, `trick-ICG`, the SWIG interface, data
products, and Java tools) is moving from autoconf and hand-written makefiles to
CMake 3.25 or newer. Simulation builds are out of scope. `trick-CP` and the
makefiles under `share/trick/makefiles/` keep working as they do today.

The autoconf build stays in place and keeps working until the CMake build
reaches parity with it. The final phase removes it.

## Decisions

- **Out-of-source builds only.** CMake never writes into the source tree.
  Generated `io_src`, SWIG output, parsers, and resource files are written to
  the build directory.
- **An install tree is `TRICK_HOME`; the source checkout is not.** Developers
  build and stage an install with `cmake --workflow --preset dev`, then point
  `TRICK_HOME` at the staged install (`out/install/dev`). Sims are always
  tested against what actually gets installed.
- **flex and bison >= 3.0 are required.** The `_premade` parser fallback and the
  `make premade` target are removed.
- **Optional dependencies are explicit.** Each optional component has a
  `TRICK_ENABLE_*` option. When the option is `ON` and the dependency is
  missing, configure fails; nothing is auto-detected. Given the same options,
  every machine produces the same Trick.
- **The sim-facing contract does not change.** The installed layout, static
  library names, and variables in `config_user.mk` stay the same, so
  simulations build unchanged against a CMake-built Trick.

## The sim-facing contract

Simulations depend on the following from an installed Trick:

- The layout `bin/`, `include/`, `lib/` (or `lib64/`), `libexec/trick/`,
  `share/trick/`.
- These static archives: `libtrick`, `libtrick_pyip`, `libtrick_mm`,
  `libtrick_comm`, `libtrick_math`, `libtrick_units`,
  `libtrick_connection_handlers`, `libtrick_optimization`,
  `libtrick_var_binary_parser`, and optionally `liber7_utils` and
  `libtrickCivet`.
- The variables in `share/trick/makefiles/config_user.mk`, and the values
  `Makefile.common` derives from them (`TRICK_LIBS`, `TRICK_EXEC_LINK_LIBS`,
  `TRICK_SYSTEM_CXXFLAGS`, and others).
- The output of `trick-config`.
- `share/trick/xml/sim_services_classes.resource` and the SWIG Python modules
  in `share/trick/swig/`.

## Checking parity

`tools/build-parity.sh` records the observable results of a build so the two
build systems can be compared:

```bash
# Autotools baseline, staged to a scratch prefix
./configure && make
make install PREFIX=$PWD/out/parity-baseline/stage
tools/build-parity.sh capture out/parity-baseline/stage out/parity-baseline/<platform>
tools/build-parity.sh sim-test . out/parity-baseline/<platform>

# Candidate build, installed the same way, then:
tools/build-parity.sh capture <candidate-install> out/parity-candidate/<platform>
tools/build-parity.sh compare out/parity-baseline/<platform> out/parity-candidate/<platform>
```

A capture contains:

- the installed file manifest (with executable bits)
- the defined external symbols of each archive
- every `trick-config` option
- the resolved sim-facing make variables
- `config_user.mk`

The install prefix is rewritten to `${TRICK_HOME}`, so captures taken at
different prefixes can be compared directly. Captures contain host-specific
paths such as compiler and dependency locations, so they are not committed.
Regenerate them on each platform you compare.

### Baselines recorded

Baselines were first captured from `master` at `d1ea8cac`:

| Platform                 | Compiler    | Sim-test result                                  |
| ------------------------ | ----------- | ------------------------------------------------ |
| macOS 26 (arm64)         | Apple clang | All 248 jobs pass                                |
| Ubuntu 24.04 (aarch64)   | GCC 13      | `SIM_rti RUN_test` fails; everything else passes |
| Oracle Linux 8 (aarch64) | GCC 8.5     | `SIM_rti RUN_test` fails; everything else passes |

On both Linux captures, the `SIM_rti` failure is in the `char bitfield`
checks. Plain `char` is unsigned on aarch64 Linux, so this is most likely a
Trick bug that appears on ARM Linux, not something caused by the build. A
CMake build must reproduce the same result on these platforms.

These Linux captures are aarch64. `config_Linux.mk` chooses `lib64` only on
x86_64 RHEL-family hosts, so the `lib64` layout needs an x86_64 capture, for
example from CI.

### Known differences in the autotools baseline

The baseline has these problems. The CMake build fixes them on purpose, so
they show up as expected differences in `compare`:

- `trick-config --includedir`, `--cflags`, and `--cxxflags` include
  `-isystem ${TRICK_HOME}/trick_source`, a directory an install tree does not
  have.
- `make install` copies the Maven build directory, about 830 files under
  `libexec/trick/java/build/`. Only the jars are needed.
- `make install` installs the `config_user.mk.in` template.
- `make uninstall` does not remove `libtrick_var_binary_parser.a`.
- The archives depend on each other in a cycle. The unit tests link them inside
  `-Wl,--start-group` on Linux.

## Configuring with CMake

The CMake build currently configures only: it finds dependencies and reports
what it found, but builds nothing yet. Presets in `CMakePresets.json` set the
build and install directories under `out/`:

| Preset | Build type | Tests | Use |
|---|---|---|---|
| `dev` | Debug | on | Working on Trick |
| `release` | Release | off | Installing Trick |
| `ci` | RelWithDebInfo | on | Continuous integration |

```bash
cmake --workflow --preset dev          # configure, build, and test
cmake --preset release -DTRICK_ENABLE_HDF5=ON
```

Put personal presets in `CMakeUserPresets.json`, which git ignores.

Each optional component has an option. When an option is `ON`, its
dependencies are required.

| Option | Default | Replaces |
|---|---|---|
| `TRICK_ENABLE_ER7_UTILS` | `ON` | `--enable-er7utils` |
| `TRICK_ENABLE_JAVA` | `ON` | `--enable-java` |
| `TRICK_JAVA_OFFLINE_DIR` | empty | `--enable-offline`; installs prebuilt jars, so it needs only a Java runtime |
| `TRICK_ENABLE_DATA_PRODUCTS` | `ON` | always built |
| `TRICK_ENABLE_X11_APPS` | `ON` | built when X11 was detected |
| `TRICK_ENABLE_HDF5` | `OFF` | `--with-hdf5`, or detected |
| `TRICK_ENABLE_GSL` | `OFF` | `--with-gsl`, or detected |
| `TRICK_ENABLE_CIVETWEB` | `OFF` | `--with-civetweb`, or detected |
| `TRICK_ENABLE_DOCS` | `OFF` | `make doxygen` |
| `BUILD_TESTING` | `OFF` | `--with-gtest`, or detected |

To use a dependency from a specific location, pass CMake's standard hints
instead of a `--with-<package>=DIR` flag:

- `-D<Package>_ROOT=DIR`, for example `-DUDUNITS2_ROOT=/opt/udunits` or
  `-DHDF5_ROOT=/opt/hdf5`
- `-DClang_DIR=<llvm>/lib/cmake/clang`, in place of `--with-llvm`
- `-DPython_EXECUTABLE=/usr/bin/python2`, in place of `PYTHON_VERSION=2`
- a toolchain file for 32-bit builds (not yet written), in place of
  `--enable-32bit`

Like `./configure`, the CMake build uses the `python3` (or `python`) on `PATH`
before any newer versioned interpreter. It also searches Homebrew's keg-only
LLVM after the default locations. On macOS, it prefers Homebrew's keg-only
Bison over the system's older version. Set `-DBISON_EXECUTABLE=/path/to/bison`
to select a specific Bison installation; it must be version 3.0 or newer.

## Existing bugs found during the migration

These predate the migration. The CMake build reproduces them, so parity checks
still pass, and each should be fixed separately.

- `test/SIM_rti RUN_test` fails its `char bitfield` checks on aarch64 Linux,
  where plain `char` is unsigned.

## Phases

0. Record the baseline and decisions, and remove the old, non-functional CMake
   files.
1. Project skeleton, options, presets, and dependency discovery.
2. Libraries with no code generation (`trick_utils`, `er7_utils`).
3. `trick-ICG`.
4. ICG code generation, parsers, `libtrick`, and `libtrick_mm`.
5. SWIG and `libtrick_pyip`.
6. The install tree, `config_user.mk`, and an exported CMake package.
7. Data products, Java, CivetWeb, and documentation.
8. Unit tests and sim tests through CTest.
9. Switch CI and packaging to CMake, then remove the autotools build.
