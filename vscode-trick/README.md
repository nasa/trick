# Trick Simulation Toolkit for VS Code

Editor support for [Trick](https://github.com/nasa/trick)'s `S_define`/`.sm` simulation
definition files, plus per-sim IntelliSense for the C/C++ model code they reference.

## Features

- **Syntax highlighting** for `S_define` and `.sm` files: model header includes
  (`##include`) vs. CP-parsed includes (`#include`), `%header{ }` / `%{ }` raw code
  blocks, job declarations (`C1 {tag} P2 (0.01, "scheduled") obj.method();`), and the
  Trick header-comment fields (`PURPOSE:`, `LIBRARY DEPENDENCIES:`, ...).
- **Snippets** for sim objects, jobs (by job class), `IntegLoop`, `integrate`,
  `collect`, `job_class_order`, and `create_connections`.
- **Ctrl+click navigation** on `#include`/`##include` lines in `S_define`/`.sm` files,
  resolved against the owning sim's `TRICK_CFLAGS`/`TRICK_CXXFLAGS`/`TRICK_SFLAGS`
  (from that sim's `S_overrides.mk`) as well as Trick's own system include paths.
- **Unresolved-include diagnostics** when a target can't be found in any of the above.
- **Ctrl+click / Go to Definition** on sim object types and job target methods in
  `S_define`/`.sm` files (e.g. `IHM::SimObject ihm;` or `ihm.update()`), resolved via
  the C/C++ extension's own workspace symbol index.
- **IntelliSense for model `.c`/`.cpp`/`.h` files** via a Custom Configuration Provider
  for the Microsoft C/C++ extension, so each sim's own include paths are used instead
  of guessing — no more false "cannot open source file" squiggles, and Go to
  Definition/Implementation works across header/source pairs.
- **Python support for `input.py`/`.dr` files**: `.dr` files (plain Python, pulled in
  via `exec(open(...).read())`) are recognized as Python; snippets for common
  `trick.*` call patterns (real-time setup, data record groups, `add_read`, Sim
  Control Panel/Trick View, ...); and a generated `trick.*` stub (scraped from a
  built Trick's `sim_services.py`/`shortcuts.py`, curated for the most common calls)
  plus a generated `__builtins__.pyi` re-exporting `trick`/`os`/`sys`/`struct`/
  `binascii` (pre-imported by Trick's input processor before input.py runs) and
  declaring each sim's object names (including `IntegLoop`s, e.g. `armIntegLoop`),
  so the Python extension's IntelliSense (Pylance) offers completions/hover for
  `trick.*` and stops flagging `trick`/sim objects/those modules as undefined.
- **Ctrl+click navigation** on `open("...")` targets in `input.py`/`.dr` files (e.g.
  `exec(open("Modified_data/Rocket.dr").read())`), resolved against the sim root
  (where Trick's own working directory is) first, then the file's own directory.
- **Build task** (`trick-CP`) for each sim, discoverable via **Tasks: Run Task** or the
  **Trick: Build Current Sim** command, with a problem matcher that sends `trick-ICG`
  parse errors and compiler errors/warnings to the Problems panel (both are plain Clang/
  GCC-style diagnostics, since neither `trick-CP` nor `trick-ICG` offer a machine-readable
  output mode).

## Requirements

- [C/C++ extension (`ms-vscode.cpptools`)](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
  for the C/C++ IntelliSense integration. Syntax highlighting, snippets, and include
  navigation work without it.
- [Python extension (`ms-python.python`)](https://marketplace.visualstudio.com/items?itemName=ms-python.python)
  with Pylance for `trick.*` completions/hover in `input.py`/`.dr` files. `.dr` file
  association and snippets work without it. Full `trick.*` coverage (beyond the ~25
  curated calls) requires a built Trick, since the stub is scraped from
  `share/trick/swig/sim_services.py`.
- `TRICK_HOME` discoverable one of these ways (checked in order):
  1. the `trick.home` setting
  2. the `TRICK_HOME` environment variable VS Code was launched with
  3. auto-detected by walking up from an open file to find
     `share/trick/makefiles/Makefile.common`
  4. auto-detected from a `.../trick/bin` directory on `PATH`

## Settings

| Setting | Default | Description |
|---|---|---|
| `trick.home` | `""` | Explicit path to `TRICK_HOME`. Leave empty to auto-detect. |
| `trick.useMakeForFlags` | `true` | Resolve flags by invoking `make` against Trick's own makefiles (exact). Disable for a best-effort regex parse of `S_overrides.mk` only. |
| `trick.cppStandard` | `""` | Override the C++ standard reported to the C/C++ extension. Leave empty to infer from `-std=` flags. |
| `trick.python.generateStubs` | `true` | Generate a `trick.*` stub and `__builtins__.pyi` per workspace folder for Pylance. Disable if you don't want files written into the workspace. |

## Commands

- **Trick: Refresh Sim Configuration** — clears cached per-sim include paths and
  re-resolves them (use after editing `S_overrides.mk` if it isn't picked up
  automatically).
- **Trick: Show Resolved Include Paths** — opens a JSON view of the include paths,
  defines, and standard resolved for the sim containing the active file, and whether
  they came from `make` or the regex fallback.
- **Trick: Regenerate Python Stubs** — re-scrapes `trick.*` and sim object names and
  rewrites the generated stub files (use after building Trick for the first time, or
  after adding a new sim object, if it isn't picked up automatically).
- **Trick: Build Current Sim** — runs `trick-CP` for the sim containing the active file
  (equivalent to `cd` into that `SIM_*` directory and running `trick-CP` by hand), with
  errors/warnings reported to the Problems panel. Requires `TRICK_HOME` to be resolvable
  (see Requirements above). Bound to `Ctrl+Shift+B` (`Cmd+Shift+B` on macOS) while a file
  inside a sim is focused, so it doesn't require the Command Palette; outside a sim, that
  shortcut falls back to VS Code's normal "Run Build Task" behavior.

## Installing

This extension isn't currently published to the VS Code Marketplace. Install it from
a `.vsix` package instead:

```sh
cd vscode-trick
npm install
npm run package          # produces trick-vscode-trick-<version>.vsix
code --install-extension trick-vscode-trick-<version>.vsix
```

Or, from VS Code's UI: Extensions view → `...` menu → **Install from VSIX...**, and
pick the `.vsix` file.

## Building from source

```sh
npm install
npm run compile   # type-check + bundle to dist/extension.js
npm test          # unit tests for sim config resolution, Python stubs/links, and build tasks
```

Press F5 in this directory to launch an Extension Development Host for manual testing.
