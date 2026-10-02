# Trick Simulation Toolkit for VS Code

**Contents**

* [Purpose](#purpose)<br>
* [Introduction](#introduction)<br>
* [Prerequisite Knowledge](#prerequisite-knowledge)<br>
* [Building the Extension](#building-the-extension)<br>
* [Installing the Extension](#installing-the-extension)<br>
* [Configuration](#configuration)<br>

---

<a id=purpose></a>
## Purpose
The purpose of this document is to explain how to build and install the Trick
Simulation Toolkit extension for Visual Studio Code.

---

<a id=introduction></a>
## Introduction

The `vscode-trick` extension, included in Trick's source tree under
[`vscode-trick/`](https://github.com/nasa/trick/tree/master/vscode-trick), adds:

* Syntax highlighting and snippets for `S_define`/`.sm` simulation definition files,
  distinguishing CP-parsed includes (`#include`) from model header includes
  (`##include`).
* Ctrl+click navigation and unresolved-include diagnostics on those `#include`/
  `##include` lines, resolved against each sim's `S_overrides.mk` and Trick's own
  system include paths.
* IntelliSense for model `.c`/`.cpp`/`.h` files (via the Microsoft C/C++ extension),
  using each sim's actual `TRICK_CFLAGS`/`TRICK_CXXFLAGS`/`TRICK_SFLAGS` instead of
  guessing, so Go to Definition/Implementation works across header/source pairs.
* Python support for `input.py`/`.dr` files: `.dr` files recognized as Python,
  snippets for common `trick.*` call patterns, and a generated `trick.*` stub (via
  the Microsoft Python extension/Pylance) for completions, hover, and to stop
  flagging `trick`/sim objects as undefined.

It is not currently published to the VS Code Marketplace, so it's installed from a
locally-built `.vsix` package.

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

From a Trick source checkout:

```sh
cd vscode-trick
npm install
npm run package
```

This produces `trick-vscode-trick-<version>.vsix` in that directory.

---

<a id=installing-the-extension></a>
## Installing the Extension

From a terminal:

```sh
code --install-extension trick-vscode-trick-<version>.vsix
```

Or from VS Code's UI: open the Extensions view, click the `...` menu in its top
corner, choose **Install from VSIX...**, and select the `.vsix` file.

After installing, reload the window and open an `S_define` file - it should be
auto-detected as "Trick".

---

<a id=configuration></a>
## Configuration

The extension needs to know `TRICK_HOME` to resolve Trick's own system include
paths. It is discovered automatically, in this order:

1. the `trick.home` VS Code setting
2. the `TRICK_HOME` environment variable VS Code was launched with
3. walking up from an open file to find `share/trick/makefiles/Makefile.common`
4. a `.../trick/bin` directory found on `PATH`

If none of these apply to your setup (for example, a sim developed outside the Trick
source tree against an installed Trick with VS Code launched from a shell that
doesn't export `TRICK_HOME`), set the `trick.home` setting explicitly.

See the extension's own
[README](https://github.com/nasa/trick/tree/master/vscode-trick#settings) for the
full list of settings and commands.
