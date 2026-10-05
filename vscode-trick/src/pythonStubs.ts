import * as fs from 'fs';
import * as path from 'path';
import * as vscode from 'vscode';
import { SimConfigProvider } from './simConfig';

const TOP_LEVEL_DEF_RE = /^def\s+(\w+)\(([^)]*)\):\s*$/;
const TOP_LEVEL_CLASS_RE = /^class\s+(\w+)\(([^)]*)\):\s*$/;
const TOP_LEVEL_CONST_RE = /^([A-Z][A-Z0-9_]*)\s*=\s*_sim_services\.\1\s*$/;
const CLASS_METHOD_RE = /^ {4}def\s+(\w+)\(([^)]*)\):\s*$/;
const CLASS_PROPERTY_RE = /^ {8}(\w+)\s*=\s*_swig_property\(/;
const ALIAS_RE = /^\s*(\w+)\s*=\s*(?:trick\.[\w.]+|top\.cvar[\w.]*)\s*$/;

interface ParsedClass {
  name: string;
  bases: string[];
  methods: { name: string; argsText: string }[];
  properties: string[];
}

interface ParsedSwigModule {
  functions: Map<string, string>;
  classes: Map<string, ParsedClass>;
  constants: Set<string>;
}

/**
 * Parses the shape of a SWIG-3-generated Python wrapper (e.g.
 * share/trick/swig/sim_services.py): top-level `def name(args):` functions,
 * top-level `NAME = _sim_services.NAME` constants, and `class X(Base):`
 * blocks whose 4-space-indented `def method(self, args):` lines and
 * 8-space-indented `name = _swig_property(...)` lines become methods/attrs.
 * Everything else (dunder plumbing, __swig_setmethods__ bookkeeping,
 * function/method bodies) is ignored - only signatures are needed for a
 * .pyi stub.
 */
function parseSwigModule(text: string): ParsedSwigModule {
  const functions = new Map<string, string>();
  const classes = new Map<string, ParsedClass>();
  const constants = new Set<string>();
  let currentClass: ParsedClass | undefined;

  for (const rawLine of text.split('\n')) {
    const line = rawLine.replace(/\r$/, '');
    const isTopLevel = line.trim() !== '' && !/^\s/.test(line);

    if (isTopLevel) {
      const classMatch = TOP_LEVEL_CLASS_RE.exec(line);
      if (classMatch) {
        currentClass = {
          name: classMatch[1],
          bases: classMatch[2]
            .split(',')
            .map((b) => b.trim())
            .filter((b) => /^\w+$/.test(b)),
          methods: [],
          properties: [],
        };
        classes.set(currentClass.name, currentClass);
        continue;
      }
      currentClass = undefined;

      const defMatch = TOP_LEVEL_DEF_RE.exec(line);
      if (defMatch) {
        functions.set(defMatch[1], defMatch[2]);
        continue;
      }

      const constMatch = TOP_LEVEL_CONST_RE.exec(line);
      if (constMatch) {
        constants.add(constMatch[1]);
      }
      continue;
    }

    if (!currentClass) {
      continue;
    }
    const methodMatch = CLASS_METHOD_RE.exec(line);
    if (methodMatch) {
      currentClass.methods.push({ name: methodMatch[1], argsText: methodMatch[2] });
      continue;
    }
    const propMatch = CLASS_PROPERTY_RE.exec(line);
    if (propMatch) {
      currentClass.properties.push(propMatch[1]);
    }
  }

  return { functions, classes, constants };
}

/**
 * Parses share/trick/swig/shortcuts.py's style of binding short names to
 * `trick.*`/`top.cvar...` targets (e.g. `add_read = trick.ippython_add_read`,
 * `stop = top.cvar.trick_sys.sched.stop`), plus any plain top-level `def`s it
 * declares (e.g. `var_get`). These shortcuts are merged into the `trick`
 * module's own namespace at runtime, so they belong in the same stub.
 */
function parseShortcuts(text: string): { aliases: string[]; functions: Map<string, string> } {
  const aliases = new Set<string>();
  const functions = new Map<string, string>();
  for (const rawLine of text.split('\n')) {
    const line = rawLine.replace(/\r$/, '');
    const trimmed = line.trim();
    if (!trimmed || trimmed.startsWith('#')) {
      continue;
    }
    if (!/^\s/.test(line)) {
      const defMatch = TOP_LEVEL_DEF_RE.exec(line);
      if (defMatch) {
        functions.set(defMatch[1], defMatch[2]);
        continue;
      }
    }
    const aliasMatch = ALIAS_RE.exec(line);
    if (aliasMatch) {
      aliases.add(aliasMatch[1]);
    }
  }
  return { aliases: [...aliases], functions };
}

function extractCuratedNames(curatedText: string): Set<string> {
  const names = new Set<string>();
  for (const line of curatedText.split('\n')) {
    const defOrClass = /^(?:def|class)\s+(\w+)/.exec(line);
    if (defOrClass) {
      names.add(defOrClass[1]);
      continue;
    }
    const annotated = /^([A-Za-z_]\w*)\s*:/.exec(line);
    if (annotated) {
      names.add(annotated[1]);
    }
  }
  return names;
}

/**
 * Builds a `trick` module .pyi stub: curated signatures for the common calls
 * (always win on name conflicts), plus every function/class/constant scraped
 * from a built Trick's `sim_services.py` and `shortcuts.py` that the curated
 * file doesn't already cover. If `simServicesText` is undefined (Trick
 * hasn't been built), only the curated signatures are emitted.
 */
export function generateTrickStub(
  simServicesText: string | undefined,
  shortcutsText: string | undefined,
  curatedText: string
): string {
  const curatedNames = extractCuratedNames(curatedText);
  const lines: string[] = [
    '# Auto-generated by vscode-trick. Do not edit by hand - run',
    '# "Trick: Regenerate Python Stubs" instead.',
    'from typing import Any',
    '',
  ];

  if (simServicesText) {
    const parsed = parseSwigModule(simServicesText);

    for (const name of [...parsed.constants].sort()) {
      if (curatedNames.has(name)) {
        continue;
      }
      lines.push(`${name}: int`);
    }
    lines.push('');

    for (const [name, argsText] of parsed.functions) {
      if (curatedNames.has(name)) {
        continue;
      }
      lines.push(`def ${name}(${argsText}) -> Any: ...`);
    }
    lines.push('');

    for (const cls of parsed.classes.values()) {
      if (curatedNames.has(cls.name)) {
        continue;
      }
      const bases = cls.bases.filter((b) => parsed.classes.has(b));
      lines.push(`class ${cls.name}${bases.length ? `(${bases.join(', ')})` : ''}:`);
      for (const prop of cls.properties) {
        lines.push(`    ${prop}: Any`);
      }
      for (const method of cls.methods) {
        lines.push(`    def ${method.name}(${method.argsText}) -> Any: ...`);
      }
      if (cls.methods.length === 0 && cls.properties.length === 0) {
        lines.push('    ...');
      }
      lines.push('');
    }
  } else {
    lines.push(
      '# share/trick/swig/sim_services.py was not found (Trick not built yet) -',
      '# only the curated signatures below are available.',
      ''
    );
  }

  if (shortcutsText) {
    const { aliases, functions } = parseShortcuts(shortcutsText);
    for (const [name, argsText] of functions) {
      if (curatedNames.has(name)) {
        continue;
      }
      lines.push(`def ${name}(${argsText}) -> Any: ...`);
    }
    for (const name of aliases) {
      if (curatedNames.has(name)) {
        continue;
      }
      lines.push(`def ${name}(*args: Any, **kwargs: Any) -> Any: ...`);
    }
    lines.push('');
  }

  lines.push(curatedText.trim(), '');
  lines.push('def __getattr__(name: str) -> Any: ...', '');

  return lines.join('\n');
}

const SKIP_DECL_KEYWORDS = new Set([
  'class',
  'struct',
  'integrate',
  'collect',
  'job_class_order',
  'create_connections',
  'IntegLoop',
  'void',
  'typedef',
  'return',
  'public',
  'private',
  'protected',
  'namespace',
  'using',
]);
const SDEFINE_DECL_RE = /^\s*([A-Za-z_]\w*(?:::\w+)*)\s+(\w+)\s*(?:\([^;]*\))?\s*;\s*$/;
// IntegLoop's declaration shape has a trailing integrand after the cycle-time
// parameter list (`IntegLoop armIntegLoop(0.050) Manip2D;`), which doesn't
// fit SDEFINE_DECL_RE's "ends right after the parens" shape - but the loop
// itself (e.g. `armIntegLoop.getIntegrator(...)`) is exactly the kind of
// object input.py calls directly, so it needs its own declaration pattern.
const INTEGLOOP_DECL_RE = /^\s*IntegLoop\s+(\w+)\s*\([^;]*\)\s+[\w.]+\s*;\s*$/;

/**
 * Extracts top-level sim object names declared in an S_define/.sm file
 * (e.g. `ModelRocketSimObject dyn;` -> "dyn"), so they can be declared as
 * builtins for input.py/.dr files where Trick injects them at runtime.
 * Comments and %header{ %}/%{ %} raw-code blocks are stripped first so
 * nothing inside them is mistaken for a declaration. A simple brace-depth
 * counter skips declarations nested inside class bodies or function bodies
 * (e.g. create_connections()'s locals), since only sim objects declared at
 * S_define's top level are externally reachable from input.py.
 */
export function parseSimObjectNames(sdefineText: string): string[] {
  const text = sdefineText
    .replace(/\/\*[\s\S]*?\*\//g, '')
    .replace(/\/\/.*$/gm, '')
    .replace(/%header\{[\s\S]*?%\}/g, '')
    .replace(/%\{[\s\S]*?%\}/g, '');

  const names = new Set<string>();
  let depth = 0;
  for (const line of text.split('\n')) {
    if (depth === 0) {
      const m = SDEFINE_DECL_RE.exec(line);
      if (m) {
        const [, typeToken, nameToken] = m;
        if (!SKIP_DECL_KEYWORDS.has(typeToken)) {
          names.add(nameToken);
        }
      } else {
        const integLoopMatch = INTEGLOOP_DECL_RE.exec(line);
        if (integLoopMatch) {
          names.add(integLoopMatch[1]);
        }
      }
    }
    for (const ch of line) {
      if (ch === '{') {
        depth++;
      } else if (ch === '}') {
        depth = Math.max(0, depth - 1);
      }
    }
  }
  return [...names];
}

// Trick's IPPython::init() runs this exact bootstrap (see
// trick_source/sim_services/InputProcessor/IPPython.cpp) before executing
// input.py, so these names are already bound in global scope - with no
// `import` statement in the input file itself - by the time it runs.
const BOOTSTRAP_MODULES = ['trick', 'os', 'sys', 'struct', 'binascii'];

/**
 * Builds a workspace-root __builtins__.pyi declaring the modules Trick's
 * input processor pre-imports (as real `import x as x` re-exports, so
 * Pylance keeps full completions/hover for e.g. `os.path`, not just `Any`)
 * plus every known sim object name (declared `Any`, since there's no stub
 * for sim-specific C++ SimObject types).
 */
export function generateBuiltinsStub(objectNames: string[]): string {
  const lines = [
    '# Auto-generated by vscode-trick. Delete, or disable via the',
    '# trick.python.generateStubs setting, if you do not want this file.',
    'from typing import Any',
    '',
    ...BOOTSTRAP_MODULES.map((name) => `import ${name} as ${name}`),
    '',
    ...[...new Set(objectNames)].sort().map((name) => `${name}: Any`),
    '',
  ];
  return lines.join('\n');
}

const STUB_SUBDIR = path.join('.vscode', 'trick-python');

/**
 * Keeps each workspace folder's generated `trick` stub and __builtins__.pyi
 * up to date so Pylance can offer completions/hover for trick.* calls and
 * stop flagging `trick`/sim-object names as undefined in input.py and .dr
 * files. Regenerates on sim config invalidation (S_define add/change/etc,
 * same signal SimConfigProvider already watches).
 */
export class PythonStubManager implements vscode.Disposable {
  private readonly disposables: vscode.Disposable[] = [];

  constructor(
    private readonly simConfigs: SimConfigProvider,
    private readonly output: vscode.OutputChannel
  ) {
    this.disposables.push(simConfigs.onDidInvalidate(() => void this.refreshAll()));
  }

  dispose(): void {
    this.disposables.forEach((d) => d.dispose());
  }

  async refreshAll(): Promise<void> {
    if (!vscode.workspace.getConfiguration('trick').get<boolean>('python.generateStubs', true)) {
      return;
    }
    for (const folder of vscode.workspace.workspaceFolders ?? []) {
      try {
        await this.refreshFolder(folder);
      } catch (err) {
        this.output.appendLine(`[python-stubs] ${folder.uri.fsPath}: ${err}`);
      }
    }
  }

  private async refreshFolder(folder: vscode.WorkspaceFolder): Promise<void> {
    const folderPath = folder.uri.fsPath;
    const sDefineFiles = await vscode.workspace.findFiles(
      new vscode.RelativePattern(folder, '**/S_define'),
      '**/{node_modules,.git}/**'
    );
    if (sDefineFiles.length === 0) {
      return;
    }

    const curatedPath = path.join(__dirname, '..', 'python', 'trick-curated.pyi');
    const curatedText = fs.readFileSync(curatedPath, 'utf8');

    const trickHome = this.simConfigs.resolveTrickHome(folderPath);
    let simServicesText: string | undefined;
    let shortcutsText: string | undefined;
    if (trickHome) {
      const simServicesPath = path.join(trickHome, 'share/trick/swig/sim_services.py');
      const shortcutsPath = path.join(trickHome, 'share/trick/swig/shortcuts.py');
      if (fs.existsSync(simServicesPath)) {
        simServicesText = fs.readFileSync(simServicesPath, 'utf8');
      } else {
        this.output.appendLine(
          `[python-stubs] ${simServicesPath} not found - Trick not built; using curated stubs only.`
        );
      }
      if (fs.existsSync(shortcutsPath)) {
        shortcutsText = fs.readFileSync(shortcutsPath, 'utf8');
      }
    }

    const trickStub = generateTrickStub(simServicesText, shortcutsText, curatedText);
    this.writeIfChanged(path.join(folderPath, STUB_SUBDIR, 'trick', '__init__.pyi'), trickStub);

    const objectNames = new Set<string>();
    for (const sdefineFile of sDefineFiles) {
      const text = fs.readFileSync(sdefineFile.fsPath, 'utf8');
      for (const name of parseSimObjectNames(text)) {
        objectNames.add(name);
      }
    }
    const builtinsStub = generateBuiltinsStub([...objectNames]);
    this.writeIfChanged(path.join(folderPath, '__builtins__.pyi'), builtinsStub);

    this.excludeFromGit(folderPath);
    await this.ensureExtraPath(folder);
  }

  private writeIfChanged(filePath: string, content: string): void {
    fs.mkdirSync(path.dirname(filePath), { recursive: true });
    const existing = fs.existsSync(filePath) ? fs.readFileSync(filePath, 'utf8') : undefined;
    if (existing === content) {
      return;
    }
    fs.writeFileSync(filePath, content, 'utf8');
  }

  // Keeps the generated files out of `git status` without touching a
  // tracked .gitignore (this is local, machine-specific generated output).
  private excludeFromGit(folderPath: string): void {
    const gitDir = path.join(folderPath, '.git');
    if (!fs.existsSync(gitDir)) {
      return;
    }
    const excludePath = path.join(gitDir, 'info', 'exclude');
    const wantedLines = ['/.vscode/trick-python/', '/__builtins__.pyi'];
    const existing = fs.existsSync(excludePath) ? fs.readFileSync(excludePath, 'utf8') : '';
    const existingLines = new Set(existing.split('\n').map((l) => l.trim()));
    const toAdd = wantedLines.filter((l) => !existingLines.has(l));
    if (toAdd.length === 0) {
      return;
    }
    const prefix = existing.length > 0 && !existing.endsWith('\n') ? '\n' : '';
    fs.mkdirSync(path.dirname(excludePath), { recursive: true });
    fs.writeFileSync(excludePath, existing + prefix + toAdd.join('\n') + '\n', 'utf8');
  }

  private async ensureExtraPath(folder: vscode.WorkspaceFolder): Promise<void> {
    const config = vscode.workspace.getConfiguration('python', folder);
    const extraPaths = config.get<string[]>('analysis.extraPaths', []);
    if (extraPaths.includes(STUB_SUBDIR)) {
      return;
    }
    await config.update(
      'analysis.extraPaths',
      [...extraPaths, STUB_SUBDIR],
      vscode.ConfigurationTarget.WorkspaceFolder
    );
  }
}
