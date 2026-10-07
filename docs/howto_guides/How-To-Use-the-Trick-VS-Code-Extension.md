# Trick Simulation Toolkit for VS Code

**Contents**

* [Purpose](#purpose)<br>
* [Introduction](#introduction)<br>
* [Features](#features)<br>
* [Prerequisite Knowledge](#prerequisite-knowledge)<br>
* [Building the Extension](#building-the-extension)<br>
* [Installing the Extension](#installing-the-extension)<br>
* [Editing S_define/.sm Files](#editing-sdefine-files)<br>
* [C/C++ IntelliSense for Model Code](#cpp-intellisense)<br>
* [Editing input.py and .dr Files](#editing-python-files)<br>
* [Building a Sim from VS Code](#building-a-sim)<br>
* [Running a Sim from VS Code](#running-a-sim)<br>
* [Trick Sims View](#trick-sims-view)<br>
* [Commands](#commands)<br>
* [Settings](#settings)<br>
* [Configuration](#configuration)<br>

---

<a id=purpose></a>
## Purpose
The purpose of this document is to explain how to build, install, and use the Trick
Simulation Toolkit extension for Visual Studio Code.

---

<a id=introduction></a>
## Introduction

The `vscode-trick` extension, included in Trick's source tree under
[`vscode-trick/`](https://github.com/nasa/trick/tree/master/vscode-trick), brings
Trick-aware editing, IntelliSense, and build support to Visual Studio Code, so working
on a sim doesn't mean bouncing between an editor that treats `S_define`/`input.py` as
opaque text and a separate terminal for everything else.

It is not currently published to the VS Code Marketplace, so it's installed from a
`.vsix` package — either built locally or downloaded from this repo's
[Releases page](https://github.com/nasa/trick/releases) (see
[Installing the Extension](#installing-the-extension)).

---

<a id=features></a>
## Features

* **Syntax highlighting and snippets** for `S_define`/`.sm` simulation definition
  files, distinguishing CP-parsed includes (`#include`) from model header includes
  (`##include`).
* **Ctrl+click navigation and unresolved-include diagnostics** on those `#include`/
  `##include` lines, resolved against each sim's `S_overrides.mk` and Trick's own
  system include paths.
* **Ctrl+click / Go to Definition** on sim object types in `S_define`/`.sm` files
  jumps straight to the real `class` definition (the open file, `S_define`, or a
  `.sm` file under the sim root/`TRICK_SFLAGS`), not the C/C++ extension's generated
  `S_source.hh`/`S_source_py.i`. Job target methods still resolve via that extension's
  workspace symbol index.
* **IntelliSense for model `.c`/`.cpp`/`.h` files** (via the Microsoft C/C++
  extension), using each sim's actual `TRICK_CFLAGS`/`TRICK_CXXFLAGS`/
  `TRICK_SFLAGS` instead of guessing, so Go to Definition/Implementation works across
  header/source pairs. Sims are resolved as soon as the workspace loads, except sims
  that live inside a git submodule or other nested clone — those are indexed the
  first time a file inside one is opened, so large multi-package workspaces still
  activate quickly.
* **Python support for `input.py`/`.dr` files**: `.dr` files recognized as Python,
  snippets for common `trick.*` call patterns, a generated `trick.*` stub (via the
  Microsoft Python extension/Pylance) for completions and hover, Ctrl+click
  navigation on `open("...")` targets, and — once a sim has been built at least
  once — completion/hover/diagnostics for the sim's own variables (e.g.
  `ball.state.input.mass`), generated from that sim's `S_sie.resource`. Ctrl+click
  on a sim variable or a `trick.*` name goes to the C++ declaration it was
  generated from, not the stub. Generated for sims not inside a git submodule or
  other nested clone up front, same as the C/C++ IntelliSense above; a nested
  sim's own variables join once a file inside it is opened.
* **A build task for `trick-CP`**, runnable for the sim containing the active file,
  with a problem matcher that sends `trick-ICG` parse errors and compiler
  errors/warnings to the Problems panel.
* **A run task** for the sim containing the active file, launching its built
  executable with a chosen `RUN_*/*.py` input file, offering to build first if it
  hasn't been built yet.
* A **Trick activity bar icon**, with **Sims** and **Connected** views, that discovers
  running sims, watches variables with live values, and runs/freezes/stops a sim —
  all as native trees inside the VS Code window, replacing the separate Sim Sniffer,
  Trick View, and Sim Control Panel Java tools.

Each of these is covered in more detail, with usage examples, in its own section
below.

---

<a id=prerequisite-knowledge></a>
## Prerequisite Knowledge
One should:

* Complete the [Trick Tutorial](https://nasa.github.io/trick/tutorial/Tutorial).
* Have [Node.js](https://nodejs.org/) (18+) and `npm` installed to build the
  extension package.
* Install the
  [C/C++ extension (`ms-vscode.cpptools`)](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
  in VS Code to get the C/C++ IntelliSense integration. Syntax highlighting, snippets,
  and include navigation work without it.
* Install the
  [Python extension (`ms-python.python`)](https://marketplace.visualstudio.com/items?itemName=ms-python.python)
  (with Pylance) to get `trick.*` completions and hover in `input.py`/`.dr` files.
  Full `trick.*` coverage requires a built Trick.

---

<a id=building-the-extension></a>
## Building the Extension

Every [Trick release](https://github.com/nasa/trick/releases) has a prebuilt
`vscode-trick-<version>.vsix` attached as a release asset, built and tested by CI — if
you just want to install the extension, grab that and skip straight to
[Installing the Extension](#installing-the-extension).

To build it yourself instead (for example, to try local changes), from a Trick source
checkout:

```sh
cd vscode-trick
npm install
npm run package
```

This produces `vscode-trick-<version>.vsix` in that directory.

---

<a id=installing-the-extension></a>
## Installing the Extension

From a terminal:

```sh
code --install-extension vscode-trick-<version>.vsix
```

Or from VS Code's UI: open the Extensions view, click the `...` menu in its top
corner, choose **Install from VSIX...**, and select the `.vsix` file.

After installing, reload the window and open an `S_define` file - it should be
auto-detected as "Trick".

---

<a id=editing-sdefine-files></a>
## Editing S_define/.sm Files

Opening an `S_define` or `.sm` file gets you Trick-aware syntax highlighting on top of
embedded C++ highlighting: `##include` (model headers) is colored differently from
`#include` (CP-parsed includes), `%header{ }`/`%{ }` raw code blocks are recognized,
and job declarations (e.g. `C1 {tag} P2 (0.01, "scheduled") obj.method();`) have their
class/phase/cycle-time/job-tag pieces highlighted individually.

**Snippets** (type the prefix, then press Tab/Enter to expand): `simobject`,
`job-default_data`, `job-initialization`, `job-scheduled`, `job-derivative`,
`job-integration`, `job-dynamic_event`, `job-shutdown`, `IntegLoop`, `integrate`,
`collect`, `job_class_order`, `create_connections`, plus `#include`/`##include`/
`%header`/`%{` block skeletons and a `header` snippet for the Trick comment-header
fields (`PURPOSE:`, `LIBRARY DEPENDENCIES:`, ...).

**Ctrl+click** (or Cmd+click on macOS) on an `#include`/`##include` target jumps to
that header, resolved against the owning sim's `TRICK_CFLAGS`/`TRICK_CXXFLAGS`/
`TRICK_SFLAGS` (from its `S_overrides.mk`) and Trick's own system include paths. If a
target can't be resolved anywhere, it's underlined as an **unresolved-include
diagnostic** instead of silently failing.

**Ctrl+click / Go to Definition** also works on sim object types (e.g.
`IHM::SimObject ihm;`). A SimObject class can only be defined in `S_define` itself or
a `.sm` file, so it's resolved directly: the currently open file first, then
`S_define`, then `.sm` files under the sim root and its `TRICK_SFLAGS` include paths
(from `S_overrides.mk`, including anything a makefile it `include`s adds). This finds
the real class definition even when the C/C++ extension's own index would only see
the generated `S_source.hh`/`S_source_py.i` (it doesn't parse `.sm` files at all).

Job target methods (e.g. `ihm.update();`) aren't confined to `S_define`/`.sm`, so
those still use the C/C++ extension's own workspace symbol index — which requires it
to have indexed the relevant header/source files first.

---

<a id=cpp-intellisense></a>
## C/C++ IntelliSense for Model Code

Opening a model `.c`/`.cpp`/`.h` file under a sim gets real IntelliSense (completions,
hover, Go to Definition/Implementation, no false "cannot open source file" squiggles)
because the extension registers as a Custom Configuration Provider for the Microsoft
C/C++ extension: for any file, it walks up to find the owning `SIM_*` directory (the
one containing `S_define`), then resolves that sim's actual `TRICK_CFLAGS`/
`TRICK_CXXFLAGS`/`TRICK_SFLAGS` — by default by actually invoking `make` against a
shipped helper makefile that includes Trick's real `Makefile.common` plus that sim's
`S_overrides.mk`, so `${TRICK_HOME}` and any custom make rules resolve exactly as
`trick-CP` would see them. This means sims with different include paths in the same
workspace each get the right IntelliSense, instead of one guessed-at
`c_cpp_properties.json` for everything.

Use **Trick: Show Resolved Include Paths** (see [Commands](#commands)) to see exactly
what was resolved for the sim containing the active file, and whether it came from
`make` or the regex fallback.

---

<a id=editing-python-files></a>
## Editing input.py and .dr Files

`.dr` files (plain Python pulled into `input.py` via `exec(open(...).read())`) are
recognized as Python, same as `input.py` itself (VS Code already treats `.py` files
as Python; `.dr` needed an explicit association since it isn't a standard Python
extension).

**Snippets** (all prefixed `trick-` so they don't clutter non-Trick Python files):
`trick-realtime` (real-time + frame + itimer setup), `trick-varserver`,
`trick-terminate`, `trick-add_read`, `trick-drgroup` (a `DRAscii`/`DRBinary`/`DRHDF5`
data record group skeleton), `trick-exec-dr` (`exec(open("Modified_data/....dr").read())`),
`trick-simcontrol`, `trick-trickview`, and `trick-integrator`
(`<loop>.getIntegrator(trick.<Runge_Kutta_4|Euler|...>, <state size>)`).

**`trick.*` completions and hover**: Trick's input processor pre-imports `trick`
(along with `os`, `sys`, `struct`, `binascii`) and binds every sim object name
(including `IntegLoop`s, e.g. `armIntegLoop`) into global scope before `input.py`
runs, with no `import` statement anywhere in the input file. Pylance has no way to
know that on its own, so the extension generates, per workspace folder that contains
an `S_define`:

* a `trick.*` stub — scraped from a built Trick's `share/trick/swig/sim_services.py`/
  `shortcuts.py` if present, curated by hand for the ~25 most common calls, and falling
  back to curated-only if Trick hasn't been built yet;
* an `__builtins__.pyi` that re-exports `trick`/`os`/`sys`/`struct`/`binascii` (so
  Pylance keeps full completions/hover for them) and declares every sim object name as
  a builtin, so none of them are flagged as "undefined".

These are written to `.vscode/trick-python/` and `__builtins__.pyi` in each workspace
folder, excluded from git via `.git/info/exclude` (never a tracked `.gitignore`), and
regenerated automatically when a sim's `S_define` changes. Use **Trick: Regenerate
Python Stubs** (see [Commands](#commands)) to force a refresh — most commonly after
building Trick for the first time, since that's when `sim_services.py` becomes
available to scrape for full `trick.*` coverage.

**Sim variable completion and hover** (e.g. `ball.state.input.mass`) work the same
way, but need a built *sim*, not just a built Trick: every `trick-CP`/`make` build
regenerates `<sim>/S_sie.resource`, an XML description of that sim's entire
object/variable tree (classes, members, units, descriptions, enumerations). The
extension parses it and generates one typed Python class per reachable class, so
each sim object in `__builtins__.pyi` points at a real type instead of a bare `Any`
— giving completions, hover text (member units/description), and unknown-attribute
diagnostics at every nesting depth, not just on the top-level object name. This
refreshes automatically a moment after each build (watching `S_sie.resource`
directly, separately from the `S_define` watcher above); an unbuilt sim still gets
a plain, untyped object — no false "undefined" warnings, just no completions past
the top level until it's built. If two sims in the same workspace folder declare
the same object name with different types, that name falls back to `Any` and the
conflict is logged to the "Trick" output channel rather than guessed at.

**Ctrl+click on a sim variable or a `trick.*` name** goes to the C++ declaration it
was generated from, not the generated stub those names actually resolve to under the
hood — the stub just re-declares the name with no body, so landing there isn't useful.
For example, Ctrl+click on `mass` in `ball.state.input.mass` jumps to `double mass ;`
in the model header that declares it, Ctrl+click on `ball` jumps to its declaration in
`S_define`, and Ctrl+click on `exec_set_terminate_time` in `trick.exec_set_terminate_time(...)`
jumps to its prototype in `include/trick/exec_proto.h`. This also works inside `.dr`
files' variable-path strings (e.g. `drg0.add_variable("ball.state.output.position[0]")`),
which Pylance can't resolve at all since to the Python parser that's just a string
literal. Since VS Code can't suppress Pylance's own stub-pointing result, the extension
also writes a one-time `[python]`-scoped `editor.gotoLocation.multipleDefinitions:
"goto"` to the workspace folder's settings (only if nothing there already set a value)
so Ctrl+click jumps straight to the C++ source instead of opening a picker between the
two; the stub is still one click away via **Go to Declaration**/Peek Definition.

**Ctrl+click** on an `open("...")` target (e.g.
`exec(open("Modified_data/Rocket.dr").read())`) jumps to that file, resolved against
the sim root first — because that's where Trick's own working directory is when it
runs a sim, not the `RUN_*` directory `input.py` lives in — then against the file's
own directory as a fallback.

**`import` statements that only resolve via `TRICK_PYTHON_PATH`**: Trick's input
processor (`IPPython.cpp`) puts the sim root, `<simRoot>/Modified_data`,
`$TRICK_HOME/share/trick/pymods`, and each directory in `S_overrides.mk`'s
`TRICK_PYTHON_PATH` onto `sys.path` before running `input.py`. Pylance doesn't know
about any of that on its own, so a real `import` that only resolves through one of
those paths (e.g. `import ParseJson as pj`) shows up as unresolved. The extension
resolves `<simRoot>/Modified_data`, `pymods`, and `TRICK_PYTHON_PATH`'s entries, and
keeps them in `python.analysis.extraPaths` for each workspace folder, added
alongside the generated stub path and refreshed whenever a sim's `S_overrides.mk`
changes; anything you've added to `extraPaths` yourself is left alone. The sim root
itself is deliberately **not** added, even though Trick puts it on `sys.path` too:
unlike those other directories, a sim root can contain build output, every `RUN_*`
directory, and other large generated trees, and Pylance's `extraPaths` indexer tries
to crawl everything under an entry in full (it doesn't honor `.gitignore` the way
normal workspace indexing does) — adding a whole sim root this way can hang Pylance
indexing indefinitely on a large sim. The practical effect is that an import written
relative to the sim root itself (e.g. `from Modified_data.utils.logger import ...`
from a `RUN_*` directory, rather than from `Modified_data` reached via
`TRICK_PYTHON_PATH`) still shows up as unresolved. Because `extraPaths` is one flat
list per workspace folder rather than per sim, two sims in the same folder that both
have a same-named module under `Modified_data` will also collide — Pylance resolves
against whichever sim's path happens to come first.

---

<a id=building-a-sim></a>
## Building a Sim from VS Code

The extension wraps `trick-CP` — the actual build command Trick sims use, which
generates a makefile and execs `make` — as a VS Code task, scoped to whichever sim
contains the active file:

* Run **Tasks: Run Task** from the Command Palette and pick the `trick: Build
  <SIM_name>` entry for the sim you want, or
* Run **Trick: Build Current Sim** (see [Commands](#commands)) to build the sim
  containing the active file directly, or
* Press `Ctrl+Shift+B` (`Cmd+Shift+B` on macOS) while a file inside a sim is focused —
  this is bound to **Trick: Build Current Sim** so the normal VS Code build shortcut
  just works; outside a sim, it falls back to VS Code's regular "Run Build Task"
  behavior.

Both `trick-ICG` (Trick's code generator, which parses your model headers) and the
C++ compiler report errors in the same plain-text Clang/GCC diagnostic format
(`file:line:column: error: message`), since neither tool offers a machine-readable
output mode. The extension's problem matcher understands that format, so build errors
and warnings — from either tool — show up as clickable entries in the Problems panel
(`Ctrl+Shift+M`/`Cmd+Shift+M`) instead of as raw terminal text.

This requires `TRICK_HOME` to be resolvable; see [Configuration](#configuration).

---

<a id=running-a-sim></a>
## Running a Sim from VS Code

Once a sim is built, **Trick: Run Current Sim** (see [Commands](#commands)) launches
its executable directly — equivalent to `cd`-ing into the `SIM_*` directory and
running `./S_main_*.exe RUN_<name>/<input>.py` by hand — in a VS Code task terminal,
with the working directory set to the sim root (same as running it from a shell):

* Press `Ctrl+F5` (`Cmd+F5` on macOS) while a file inside a sim is focused, or click
  the ▶ button that appears in the editor title bar for a `RUN_*/*.py` file, or run
  **Trick: Run Current Sim** from the Command Palette.
* Outside a sim, `Ctrl+F5`/`Cmd+F5` falls back to VS Code's regular "Run Without
  Debugging" behavior, the same way `Ctrl+Shift+B` falls back for build.

**Picking an input file.** A sim can have more than one `RUN_*` directory, and a
`RUN_*` directory can itself hold more than one `.py` file (for example a
`RUN_test/unit_test.py` alongside its `input.py`). The input file is chosen like this:

* If the file you're currently looking at is itself a `RUN_*/*.py` file, that one runs
  immediately, with no prompt.
* Otherwise (for example, you're looking at `S_define` or a model file), a picker
  lists every `RUN_*/*.py` under the sim, with whichever one you ran last sorted to
  the top so pressing Enter repeats it. If there's only one candidate, it runs
  directly without showing the picker.

**Building first.** If the sim hasn't been built yet (no `S_main_*.exe` present), a
prompt offers **Build and Run** — this runs the same build as **Trick: Build Current
Sim**, and only launches the sim if that build succeeds.

A sim launched this way shows up automatically in the [Trick Sims
View](#trick-sims-view) below, like any other running sim, since that view discovers
sims the same way regardless of how they were started.

---

<a id=trick-sims-view></a>
## Trick Sims View

Click the Trick icon in the activity bar to open two stacked views, **Sims** and
**Connected**. Together they replace three separate Java tools — Sim Sniffer, Trick
View, and Sim Control Panel — with trees embedded in the VS Code window rather than
separate pop-up windows. The two views share a resizable divider — drag it to give
more room to whichever one you're using, or collapse either one entirely.

**Discovery.** A running sim's variable server broadcasts its host, port, and run
directory over UDP multicast every couple of seconds, and the **Sims** view picks
those up automatically — a sim should appear within a few seconds of launch, and
disappear a few seconds after it exits (unless it's connected — see below). This is
on by default everywhere except **macOS**, where it's commonly blocked; a firewall or
VPN can also block it elsewhere. If a sim doesn't show up on its own, use **Connect to
Sim (host:port)…** (the **+** button in either view's title bar) instead — the port is
printed to the sim's console at startup, or can be read from
`trick.var_server_get_port()` in its input file.

**Connecting.** Click the plug icon next to a discovered-but-disconnected sim in
**Sims** to connect. Connecting doesn't change what the sim is doing — it just opens a
variable server connection so the view can show live status and run the control
buttons. The moment you connect, the sim moves out of **Sims** and into **Connected**,
and stays there — even if it stops broadcasting, or other sims come and go in
**Sims** — so you don't have to chase it around a re-sorting list. If the sim process
ends while you're connected, it stays in **Connected** showing **Ended** until you
click **Disconnect**.

**Running, freezing, stopping.** In **Connected**, a sim shows its current mode (e.g.
`Frozen`, `Running`) and simulation time, with inline buttons for whichever actions
make sense for that mode: **Run** (sends `exec_run()`), **Freeze** (sends
`exec_freeze()`), and **Stop** (sends `trick.stop()`, after a confirmation, since it
ends the run). A sim that calls `trick.exec_set_freeze_command(True)` in its input
file starts out frozen and waits for **Run** before it does anything — this is where
that button matters most.

**Watching variables.** Click **Add Watch** on a sim in **Connected** to pick a
variable. If the sim's `S_sie.resource` is reachable on your filesystem (true for any
sim running locally), you get a step-by-step picker through its sim objects, drilling
into each one's members — otherwise you can type a path directly. Watched variables
show up as children of the sim in **Connected** (which expands automatically when you
add one), with their live value (and units,
where Trick reports any) updating a few times a second; an unknown or mistyped path
shows `BAD_REF`. Remove a watch with the **x** button next to it. Watch lists are
remembered per sim directory, so they come back if you reconnect later.

You can also right-click a variable reference in `input.py`/`.dr` (e.g.
`ball.state.output.position[0]`) and choose **Watch in Trick Sims View** to add it
without retyping it, as long as you're connected to the sim it belongs to.

---

<a id=commands></a>
## Commands

* **Trick: Refresh Sim Configuration** — clears cached per-sim include paths and
  re-resolves them (use after editing `S_overrides.mk` if it isn't picked up
  automatically).
* **Trick: Show Resolved Include Paths** — opens a JSON view of the include paths,
  defines, and standard resolved for the sim containing the active file, and whether
  they came from `make` or the regex fallback.
* **Trick: Regenerate Python Stubs** — re-scrapes `trick.*` and sim object names and
  rewrites the generated stub files (use after building Trick for the first time, or
  after adding a new sim object, if it isn't picked up automatically).
* **Trick: Build Current Sim** — runs `trick-CP` for the sim containing the active
  file. See [Building a Sim from VS Code](#building-a-sim).
* **Trick: Run Current Sim** — launches the built executable for the sim containing
  the active file, with a chosen input file. See [Running a Sim from VS
  Code](#running-a-sim).
* **Trick: Connect to Sim (host:port)…** — connects directly to a variable server, for
  sims discovery can't see. See [Trick Sims View](#trick-sims-view).
* **Trick: Watch in Trick Sims View** — from an editor's right-click menu, adds the
  variable path under the cursor as a watch on a connected sim.

The rest of the Sims/Connected views' actions (Connect, Run, Freeze, Stop, Add Watch,
Remove Watch, Disconnect) are inline buttons and context-menu items on each sim or
watched variable in the view itself, rather than Command Palette entries.

---

<a id=settings></a>
## Settings

| Setting | Default | Description |
|---|---|---|
| `trick.home` | `""` | Explicit path to `TRICK_HOME`. Leave empty to auto-detect. |
| `trick.useMakeForFlags` | `true` | Resolve flags by invoking `make` against Trick's own makefiles (exact). Disable for a best-effort regex parse of `S_overrides.mk` only. |
| `trick.cppStandard` | `""` | Override the C++ standard reported to the C/C++ extension. Leave empty to infer from `-std=` flags. |
| `trick.python.generateStubs` | `true` | Generate a `trick.*` stub and `__builtins__.pyi` per workspace folder for Pylance. Disable if you don't want files written into the workspace. |

---

<a id=configuration></a>
## Configuration

The extension needs to know `TRICK_HOME` to resolve Trick's own system include paths
and to run `trick-CP`. It is discovered automatically, in this order:

1. the `trick.home` VS Code setting
2. the `TRICK_HOME` environment variable VS Code was launched with
3. walking up from an open file to find `share/trick/makefiles/Makefile.common`
4. a `.../trick/bin` directory found on `PATH`

If none of these apply to your setup (for example, a sim developed outside the Trick
source tree against an installed Trick with VS Code launched from a shell that
doesn't export `TRICK_HOME`), set the `trick.home` setting explicitly.

See the extension's own
[README](https://github.com/nasa/trick/tree/master/vscode-trick#settings) for
implementation details.
