# Changelog

## [25.1.1] - 2026-09-04

### Changed

- Support LLVM 23 ([#2163](https://github.com/nasa/trick/pull/2163)) (Brendan Fattig)
- Support SWIG 4.5.0 ([#2184](https://github.com/nasa/trick/pull/2184)) (Brendan Fattig)
- Make the `Integrator` state and derivative input/output hooks virtual so derived integrators can override them ([#2151](https://github.com/nasa/trick/pull/2151)) (Hong Chen)
- Warn when the interval timer is automatically disabled because the software frame is shorter than its 10 ms minimum ([#2155](https://github.com/nasa/trick/pull/2155)) (Hong Chen)
- Write unsigned character arrays as numeric checkpoint values rather than string literals ([#2171](https://github.com/nasa/trick/pull/2171)) (Hong Chen)

### Added

- Add ASCII, binary, and HDF5 data recording for supported `std::vector`, `std::array`, and `std::deque` values ([#2154](https://github.com/nasa/trick/pull/2154)) (Hong Chen)
- Add `VariableServer.get_stl_size()` to the Python client for querying supported STL-container bounds ([#2161](https://github.com/nasa/trick/pull/2161)) (Hong Chen)
- Add `TMM_get_checkpoint_restore_state()` and corresponding C++ memory-manager accessors for determining whether a checkpoint restore succeeded ([#2167](https://github.com/nasa/trick/pull/2167)) (Hong Chen)
- Add rubber-band zooming to `trick-jperf`, with controls to step back through prior views or reset the zoom ([#2172](https://github.com/nasa/trick/pull/2172)) (Sean Harmeyer)

### Removed

- Remove the legacy `sim_services/DataTypes` subsystem and its compatibility headers ([#2175](https://github.com/nasa/trick/pull/2175)) (Brendan Fattig)

### Fixed

- Correct the cannon simulation's impact calculations and update the related tutorials ([#2191](https://github.com/nasa/trick/pull/2191)) (Brendan Fattig)
- Select `libLTO` from the configured LLVM installation to prevent mismatched-library failures in macOS ICG builds ([#2194](https://github.com/nasa/trick/pull/2194)) (Hong Chen)
- Keep simulations discoverable when the VariableServer is disabled and make Java clients fail promptly instead of hanging on rejected connections ([#2133](https://github.com/nasa/trick/pull/2133)) (Brendan Fattig)
- Prevent simulation shutdown from hanging when another thread holds the Python GIL ([#2173](https://github.com/nasa/trick/pull/2173)) (Brendan Fattig)
- Generate all required `init_attr` calls, including for namespaced types, when simulation-service I/O metadata generation is disabled ([#2166](https://github.com/nasa/trick/pull/2166)) (Alex Lin, Hong Chen)
- Preserve the input Python object passed to `attach_units()` and return a modified copy ([#2180](https://github.com/nasa/trick/pull/2180)) (Hong Chen)
- Load MTV events whose condition or action list index is greater than zero ([#2141](https://github.com/nasa/trick/pull/2141)) (Brendan Fattig)
- Shut down the input processor cleanly for JIT input files that have no saved Python state ([#2148](https://github.com/nasa/trick/pull/2148)) (Hong Chen)
- Compensate for delayed `SIGALRM` delivery so the interval timer wakes at the requested time on macOS ([#2146](https://github.com/nasa/trick/pull/2146)) (Hong Chen)
- Avoid duplicate generated `cast_as` functions when the same structured type is declared in multiple headers ([#2149](https://github.com/nasa/trick/pull/2149)) (Hong Chen)
- Recognize desugared libstdc++ `std::string` types when generating I/O metadata for STL-container elements ([#2161](https://github.com/nasa/trick/pull/2161)) (Hong Chen)
- Close TrickOps job standard-input and log-file handles after completion or termination ([#2192](https://github.com/nasa/trick/pull/2192)) (Hong Chen)
- Set Clang language defaults with Clang 15 and newer ([#2164](https://github.com/nasa/trick/pull/2164)) (Will MacCormack)
- Document GoogleTest as optional rather than a required installation dependency ([#2137](https://github.com/nasa/trick/pull/2137)) (Brendan Fattig)

[25.1.1]: https://github.com/nasa/trick/releases/tag/25.1.1
