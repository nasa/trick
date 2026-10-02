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

## Requirements

- [C/C++ extension (`ms-vscode.cpptools`)](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
  for the IntelliSense integration. Syntax highlighting, snippets, and include
  navigation work without it.
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

## Commands

- **Trick: Refresh Sim Configuration** — clears cached per-sim include paths and
  re-resolves them (use after editing `S_overrides.mk` if it isn't picked up
  automatically).
- **Trick: Show Resolved Include Paths** — opens a JSON view of the include paths,
  defines, and standard resolved for the sim containing the active file, and whether
  they came from `make` or the regex fallback.

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
npm test          # unit tests for sim config resolution
```

Press F5 in this directory to launch an Extension Development Host for manual testing.
