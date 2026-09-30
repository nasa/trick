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
- **flex and bison are required.** The `_premade` parser fallback and the
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

Until the CMake build installs, compare its libraries with the Make build's
in the same checkout:

```bash
./configure && make
cmake --workflow --preset parity
tools/build-parity.sh check-cmake lib out/build/parity out/parity-check
tools/build-parity.sh check-icg bin/trick-ICG out/build/parity out/icg-check
```

`check-cmake` compares the archives the CMake build produces so far, which are
listed in `CMAKE_ARCHIVES` in the script. Add each archive there as it is
migrated. It leaves out the Make build's ICG-generated `io_*` members until the
CMake build runs ICG.

`check-icg` runs both builds' `trick-ICG` on `include/trick/files_to_ICG.hh`
with the flags the Make build uses, and compares everything they generate: the
`io_*.cpp` files, the class and enum maps, and the XML class resource. `-o`
redirects only the `io_*.cpp` files. ICG still writes its maps into
`trick_source/sim_services/include/io_src` and appends to
`share/trick/xml/sim_services_classes.resource`, so `check-icg` starts each run
from the same state, collects what it wrote, and restores the originals. It
fails if anything else in the tree changed.

CI runs both checks on every Linux and macOS job.

The `parity` preset leaves the build type empty. The Make build passes no
optimization or debug flags, and an optimized build inlines functions whose
symbols the Make build's archives contain, so it cannot be compared.

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

The CMake build is incomplete. It builds the libraries listed under
[Library layout](#library-layout) but does not install anything yet. Presets in
`CMakePresets.json` set the build and install directories under `out/`:

| Preset | Build type | Tests | Use |
|---|---|---|---|
| `dev` | Debug | on | Working on Trick |
| `release` | Release | off | Installing Trick |
| `ci` | RelWithDebInfo | on | Continuous integration, after the migration |
| `parity` | none | on | Comparing with the Make build during the migration |

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
LLVM after the default locations.

## Library layout

`cmake/TrickLibraries.cmake` defines two targets every library links:

- `trick_headers` (`Trick::headers`): the include paths `include/` and
  `include/trick/compat`, C++17, and the definitions Trick's headers test:
  `TRICK_VER`, `TRICK_MINOR`, `USE_ER7_UTILS_INTEGRATORS`, `_HAVE_GSL`, and
  `USE_CIVETWEB`. Simulations receive the same set through
  `TRICK_SYSTEM_CXXFLAGS`.
- `trick_build_options`: settings used only to compile Trick itself, currently
  `-fexceptions` for C. It is never exported.

`trick_add_library()` creates a library linked to both. Each archive
simulations link is a `STATIC` library with the same name, and has a
`Trick::` alias for use by other targets and, later, other projects:

| Target | Archive | Alias |
|---|---|---|
| `trick_comm` | `libtrick_comm.a` | `Trick::comm` |
| `trick_connection_handlers` | `libtrick_connection_handlers.a` | `Trick::connection_handlers` |
| `trick_math` | `libtrick_math.a` | `Trick::math` |
| `trick_optimization` | `libtrick_optimization.a` | `Trick::optimization` |
| `trick_units` | `libtrick_units.a` | `Trick::units` |
| `trick_var_binary_parser` | `libtrick_var_binary_parser.a` | `Trick::var_binary_parser` |
| `er7_utils` | `liber7_utils.a` | `Trick::er7_utils` |

Directories whose code the Make build puts into `libtrick.a`
(`compareFloatingPoint`, `interpolator`, `shm`, `trick_adt`, `unicode`) are
`OBJECT` libraries. `libtrick` includes them when it is added.

Some dependencies point from these libraries back into libraries that don't
exist yet. They are added along with those libraries:

- `libtrick_math` calls `message_publish()` in `libtrick`.
- er7_utils' adapter (`trick/integration`) derives from `Trick::Integrator` and
  uses the memory manager.
- ICG generates code for er7_utils headers into er7_utils' own directories. It
  is compiled into `liber7_utils.a` along with ICG's other output.

## trick-ICG

`trick_source/codegen/Interface_Code_Gen/CMakeLists.txt` builds `trick-ICG`.
The values the makefile passes as `-D` flags come from CMake's LLVM package:

| Definition | Value |
|---|---|
| `LIBCLANG_MAJOR`, `LIBCLANG_MINOR`, `LIBCLANG_PATCHLEVEL` | `LLVM_VERSION_MAJOR`, `_MINOR`, `_PATCH` |
| `LLVM_HOME` | `LLVM_INSTALL_PREFIX` |
| `TRICK_VERSION` | the full version from `trick_ver.txt` |
| `TRICK_GCC_VERSION` | the C++ compiler's version, when it is GCC |

ICG links Clang's `clangFrontend`, `clangParse`, `clangSema`, `clangLex`,
`clangAST`, and `clangBasic` targets. Clang's CMake package records the
dependencies among them, so the link order problems that `configure` works
around do not arise. If a distribution installs only the combined library, ICG
links `clang-cpp` instead.

ICG also calls LLVM directly, so it links LLVM itself: the `LLVM` shared
library where LLVM is built as one (`LLVM_LINK_LLVM_DYLIB`), otherwise the
`Support` and, from LLVM 16, `TargetParser` components. Relying on Clang's
targets to bring LLVM in fails on Enterprise Linux: LLVM reaches the link only
as a dependency of a shared library, and GNU ld will not use it to resolve
ICG's own references.

## Existing bugs found during the migration

These predate the migration. The CMake build reproduces them, so parity checks
still pass, and each should be fixed separately.

- `test/SIM_rti RUN_test` fails its `char bitfield` checks on aarch64 Linux,
  where plain `char` is unsigned.
- `MulticastGroup.cpp` has a non-void function that does not return a value
  (`-Wreturn-type`).
- On Linux, ICG looks for Clang's builtin headers in
  `<LLVM_HOME>/lib/clang/<major>.<minor>.<patch>/include`. LLVM 16 and later
  install them in `lib/clang/<major>/include`, so ICG silently skips the
  directory.
- Without `EXTERNAL_BUILD`, ICG writes enum entries for the XML class resource
  to `share/trick/xml/include/sim_services_classes.resource`, a directory that
  does not exist, so they are dropped. Classes go to
  `share/trick/xml/sim_services_classes.resource`.
- ICG appends to `sim_services_classes.resource` and nothing truncates it, so
  any run that regenerates Trick's `io_src` adds a second copy of every class.

## Phases

0. Record the baseline and decisions, and remove the old, non-functional CMake
   files.
1. Project skeleton, options, presets, and dependency discovery.
2. Libraries with no code generation (`trick_utils`, `er7_utils`).
3. `trick-ICG`.
4. ICG code generation, parsers, `libtrick`, and `libtrick_mm`.
5. SWIG and `libtrick_pyip`.
6. The install tree, `config_user.mk`, and an exported CMake package.
7. Data products, Java, CivetWeb, and documentation. The Make build compiles
   data products without `-std`, so they use the compiler's default C++
   dialect (gnu++14 with GCC 8.5, gnu++17 with GCC 11 and later). Match it for
   parity, or decide to move them to C++17.
8. Unit tests and sim tests through CTest.
9. Switch CI and packaging to CMake, then remove the autotools build.
