import * as fs from 'fs';
import * as path from 'path';
import * as vscode from 'vscode';
import { SimConfigProvider, isInNestedRepo } from './simConfig';
import { parseSieResource, generateSieStub } from './sieResource';

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

export interface SimObjectDecl {
  name: string;
  /** Set when a parsed S_sie.resource gives this object a real class to point at. */
  typeRef?: { moduleName: string; className: string };
}

/**
 * Builds a workspace-root __builtins__.pyi declaring the modules Trick's
 * input processor pre-imports (as real `import x as x` re-exports, so
 * Pylance keeps full completions/hover for e.g. `os.path`, not just `Any`)
 * plus every known sim object name - typed against its generated
 * trick_sie.* class when available (see generateSieStub), otherwise `Any`.
 * Callers are expected to pass at most one decl per name already resolved
 * (e.g. conflicting types across sims collapsed to a plain `Any` decl); if
 * duplicates slip through, the first one wins.
 */
export function generateBuiltinsStub(objects: SimObjectDecl[]): string {
  const byName = new Map<string, SimObjectDecl>();
  for (const obj of objects) {
    if (!byName.has(obj.name)) {
      byName.set(obj.name, obj);
    }
  }

  const imports = new Map<string, string>(); // moduleName -> alias
  const declLines: string[] = [];
  for (const name of [...byName.keys()].sort()) {
    const decl = byName.get(name)!;
    if (!decl.typeRef) {
      declLines.push(`${name}: Any`);
      continue;
    }
    let alias = imports.get(decl.typeRef.moduleName);
    if (!alias) {
      alias = `_sie_${imports.size}`;
      imports.set(decl.typeRef.moduleName, alias);
    }
    declLines.push(`${name}: ${alias}.${decl.typeRef.className}`);
  }

  const lines = [
    '# Auto-generated by vscode-trick. Delete, or disable via the',
    '# trick.python.generateStubs setting, if you do not want this file.',
    'from typing import Any',
    '',
    ...BOOTSTRAP_MODULES.map((name) => `import ${name} as ${name}`),
    ...[...imports.entries()].map(([moduleName, alias]) => `import ${moduleName} as ${alias}`),
    '',
    ...declLines,
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
  private sieRefreshTimer: ReturnType<typeof setTimeout> | undefined;

  constructor(
    private readonly simConfigs: SimConfigProvider,
    private readonly output: vscode.OutputChannel,
    private readonly state: vscode.Memento
  ) {
    this.disposables.push(simConfigs.onDidInvalidate(() => void this.refreshAll()));

    // Sims inside a nested git repo aren't scanned by refreshFolder up front
    // (see the isInNestedRepo filter there), so if the user opens a file
    // inside one, its object names need to join __builtins__.pyi the same
    // way they join cpptools' browse path - see SimConfigProvider.onDidResolve.
    // Debounced for the same reason as TrickCppConfigurationProvider's.
    const scheduleRefresh = () => {
      if (this.sieRefreshTimer) {
        clearTimeout(this.sieRefreshTimer);
      }
      this.sieRefreshTimer = setTimeout(() => void this.refreshAll(), 1000);
    };
    this.disposables.push(simConfigs.onDidResolve(scheduleRefresh));

    // A separate watcher from SimConfigProvider's: S_sie.resource changes on
    // every build (and is rewritten again whenever a variable-server client
    // queries it at runtime), so re-running SimConfigProvider's make-based
    // flag resolution on every change would be wasteful - this only needs to
    // re-parse the (cheap, already-on-disk) XML. Debounced since a build can
    // touch the file more than once in quick succession.
    const sieWatcher = vscode.workspace.createFileSystemWatcher('**/S_sie.resource');
    sieWatcher.onDidChange(scheduleRefresh);
    sieWatcher.onDidCreate(scheduleRefresh);
    sieWatcher.onDidDelete(scheduleRefresh);
    this.disposables.push(sieWatcher);
  }

  dispose(): void {
    if (this.sieRefreshTimer) {
      clearTimeout(this.sieRefreshTimer);
    }
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
    const allSDefineFiles = await vscode.workspace.findFiles(
      new vscode.RelativePattern(folder, '**/S_define'),
      '**/{node_modules,.git}/**'
    );
    // Same scoping as SimConfigProvider.warmPrimarySimRoots: scanning every
    // S_define in a large multi-package repo (hundreds of them, most in
    // submodules nobody touches) is what made this "insanely slow" to begin
    // with, and most of those sims' object names only bloat __builtins__.pyi
    // for sims the user isn't even looking at. Sims already resolved (opened
    // at least once - see the onDidResolve subscription above) are kept even
    // if nested, so their own object names don't disappear once seen.
    const resolvedRoots = new Set(this.simConfigs.getAllCachedRoots());
    const sDefineFiles = allSDefineFiles.filter((f) => {
      const simRoot = path.dirname(f.fsPath);
      return resolvedRoots.has(simRoot) || !isInNestedRepo(simRoot, folderPath);
    });
    const skipped = allSDefineFiles.length - sDefineFiles.length;
    if (skipped > 0) {
      this.output.appendLine(
        `[python-stubs] ${folderPath}: ${sDefineFiles.length} sim(s) scanned, ${skipped} inside nested git repos skipped (will join when opened).`
      );
    }
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
    const typeRefsByName = new Map<string, { moduleName: string; className: string }[]>();
    const pythonPathDirs = new Set<string>();
    let wroteAnySieStub = false;
    for (const sdefineFile of sDefineFiles) {
      const simRoot = path.dirname(sdefineFile.fsPath);
      const text = fs.readFileSync(sdefineFile.fsPath, 'utf8');
      for (const name of parseSimObjectNames(text)) {
        objectNames.add(name);
      }

      // Sim roots here are already resolved (or cheap cache hits) by
      // warmPrimarySimRoots/onDidResolve - see the scoping filter above - so
      // this doesn't reintroduce the per-sim `make` cost that filter exists
      // to avoid, except for the same one-time race at activation.
      try {
        const config = await this.simConfigs.getConfig(simRoot);
        for (const dir of config.pythonPaths) {
          pythonPathDirs.add(dir);
        }
      } catch (err) {
        this.output.appendLine(`[python-stubs] ${simRoot}: failed to resolve TRICK_PYTHON_PATH: ${err}`);
      }

      const siePath = path.join(simRoot, 'S_sie.resource');
      if (!fs.existsSync(siePath)) {
        this.output.appendLine(
          `[python-stubs] ${siePath} not found - ${path.basename(simRoot)} not built; variable completion unavailable until trick-CP runs.`
        );
        continue;
      }
      const model = parseSieResource(fs.readFileSync(siePath, 'utf8'));
      const sieModuleBaseName = this.sieModuleName(folderPath, simRoot);
      const moduleName = `trick_sie.${sieModuleBaseName}`;
      this.writeIfChanged(
        path.join(folderPath, STUB_SUBDIR, 'trick_sie', `${sieModuleBaseName}.pyi`),
        generateSieStub(model)
      );
      wroteAnySieStub = true;
      for (const obj of model.topLevel) {
        objectNames.add(obj.name);
        const refs = typeRefsByName.get(obj.name) ?? [];
        refs.push({ moduleName, className: obj.type });
        typeRefsByName.set(obj.name, refs);
      }
    }
    if (wroteAnySieStub) {
      this.writeIfChanged(path.join(folderPath, STUB_SUBDIR, 'trick_sie', '__init__.pyi'), '');
    }

    const decls: SimObjectDecl[] = [...objectNames].sort().map((name) => {
      const refs = typeRefsByName.get(name);
      if (!refs) {
        return { name };
      }
      const unique = new Map<string, { moduleName: string; className: string }>();
      for (const ref of refs) {
        unique.set(`${ref.moduleName}\u0000${ref.className}`, ref);
      }
      if (unique.size > 1) {
        this.output.appendLine(
          `[python-stubs] "${name}" resolves to different SIE types across sims - declaring as Any: ` +
            [...unique.values()].map((r) => `${r.moduleName}.${r.className}`).join(', ')
        );
        return { name };
      }
      return { name, typeRef: refs[0] };
    });
    const builtinsStub = generateBuiltinsStub(decls);
    this.writeIfChanged(path.join(folderPath, '__builtins__.pyi'), builtinsStub);

    this.excludeFromGit(folderPath);
    await this.ensureExtraPaths(folder, [...pythonPathDirs]);
    await this.ensureGotoSetting(folder);
  }

  // Keyed on the sim's path relative to the workspace folder (not just its
  // basename) so two identically-named SIM_x dirs in different subtrees of
  // the same workspace folder don't collide, then sanitized into something
  // that's both a valid Python module name and a valid filename.
  private sieModuleName(folderPath: string, simRoot: string): string {
    const rel = path.relative(folderPath, simRoot) || path.basename(simRoot);
    return rel.replace(/[^A-Za-z0-9_]/g, '_').replace(/^(\d)/, '_$1');
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

  // Keeps python.analysis.extraPaths in sync with the stub dir and each
  // scoped sim's TRICK_PYTHON_PATH (see SimConfig.pythonPaths) - TRICK_HOME's
  // pymods, <simRoot>/Modified_data, and whatever S_overrides.mk sets - so
  // e.g. `import ParseJson` resolves the same way Trick's input processor
  // resolves it at runtime (IPPython.cpp), not just the generated stub. The
  // sim root itself is deliberately not included here - see the
  // SimConfig.pythonPaths doc comment. Entries this method added last time
  // (tracked in workspace state, keyed per folder) are replaced wholesale on
  // each refresh - e.g. a sim dropped from S_overrides.mk stops being
  // suggested - while anything the user added themselves is left alone.
  private async ensureExtraPaths(folder: vscode.WorkspaceFolder, pythonPaths: string[]): Promise<void> {
    const stateKey = `trick.managedExtraPaths:${folder.uri.fsPath}`;
    const previouslyManaged = new Set(this.state.get<string[]>(stateKey, []));
    const wantedManaged = [STUB_SUBDIR, ...pythonPaths];

    const config = vscode.workspace.getConfiguration('python', folder);
    const current = config.get<string[]>('analysis.extraPaths', []);
    const userEntries = current.filter((p) => !previouslyManaged.has(p));
    const merged = [...new Set([...userEntries, ...wantedManaged])];

    if (merged.length !== current.length || merged.some((p, i) => p !== current[i])) {
      await config.update('analysis.extraPaths', merged, vscode.ConfigurationTarget.WorkspaceFolder);
    }
    await this.state.update(stateKey, wantedManaged);
  }

  // Pylance's own Go to Definition result (pointing at the generated .pyi
  // stub) can't be suppressed - VS Code just merges it with
  // TrickPythonDefinitionProvider's. Setting this to "goto" (instead of the
  // default "peek") makes Ctrl+click jump straight to the first-ordered
  // result instead of opening a picker; TrickPythonDefinitionProvider is
  // registered after Pylance activates specifically so it sorts first. The
  // stub stays reachable via Go to Declaration / Peek Definition. Only set
  // when nothing in this workspace already has an opinion, so a user's own
  // choice is never overwritten.
  private async ensureGotoSetting(folder: vscode.WorkspaceFolder): Promise<void> {
    const config = vscode.workspace.getConfiguration('editor', { uri: folder.uri, languageId: 'python' });
    const inspected = config.inspect<string>('gotoLocation.multipleDefinitions');
    const hasWorkspaceValue =
      inspected?.workspaceFolderLanguageValue !== undefined ||
      inspected?.workspaceLanguageValue !== undefined ||
      inspected?.workspaceFolderValue !== undefined ||
      inspected?.workspaceValue !== undefined;
    if (hasWorkspaceValue) {
      return;
    }
    await config.update(
      'gotoLocation.multipleDefinitions',
      'goto',
      vscode.ConfigurationTarget.WorkspaceFolder,
      true
    );
  }
}
