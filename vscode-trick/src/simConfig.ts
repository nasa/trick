import * as cp from 'child_process';
import * as fs from 'fs';
import * as path from 'path';
import * as vscode from 'vscode';

export interface SimConfig {
  simRoot: string;
  source: 'make' | 'regex';
  cIncludes: string[];
  cxxIncludes: string[];
  sIncludes: string[];
  cDefines: string[];
  cxxDefines: string[];
  cxxStandard: string;
  compilerPath?: string;
  cCompilerPath?: string;
  /**
   * A subset of the directories Trick's input processor adds to sys.path
   * before running input.py (see IPPython.cpp): TRICK_HOME's pymods,
   * <simRoot>/Modified_data, and TRICK_PYTHON_PATH (set in S_overrides.mk).
   * Deliberately excludes the sim root itself - unlike these, which are
   * narrow, bounded directories, a sim root can contain build output, every
   * RUN_* directory, and other large generated trees that Pylance's
   * extraPaths-driven indexer (which doesn't honor .gitignore the way normal
   * workspace indexing does) will try to crawl in full, hanging indefinitely
   * on a large sim. Imports written relative to the sim root itself (e.g.
   * `from Modified_data.utils.x import y` from a RUN directory) are not
   * supported as a result - only imports reachable via one of these.
   */
  pythonPaths: string[];
  raw: Record<string, string>;
}

const FLAG_VARS = [
  'TRICK_CFLAGS',
  'TRICK_CXXFLAGS',
  'TRICK_SFLAGS',
  'TRICK_SYSTEM_CFLAGS',
  'TRICK_SYSTEM_CXXFLAGS',
  'TRICK_SYSTEM_SFLAGS',
  'TRICK_CXX',
  'TRICK_CC',
  'TRICK_PYTHON_PATH',
];

export class SimConfigProvider implements vscode.Disposable {
  private readonly cache = new Map<string, SimConfig>();
  private readonly output: vscode.OutputChannel;
  private readonly watcher: vscode.FileSystemWatcher;
  private readonly onDidInvalidateEmitter = new vscode.EventEmitter<string>();
  readonly onDidInvalidate = this.onDidInvalidateEmitter.event;
  private readonly onDidResolveEmitter = new vscode.EventEmitter<string>();
  /** Fires when a sim's config is resolved for the first time (not on cache hits). */
  readonly onDidResolve = this.onDidResolveEmitter.event;

  constructor(output: vscode.OutputChannel) {
    this.output = output;
    this.watcher = vscode.workspace.createFileSystemWatcher(
      '**/{S_overrides.mk,S_define,trickify.mk,S_post.mk}'
    );
    const invalidate = (uri: vscode.Uri) => {
      const simRoot = path.dirname(uri.fsPath);
      if (this.cache.delete(simRoot)) {
        this.onDidInvalidateEmitter.fire(simRoot);
      }
    };
    this.watcher.onDidChange(invalidate);
    this.watcher.onDidCreate((uri) => {
      invalidate(uri);
      // A brand-new S_define means a brand-new sim that was never cached, so
      // `invalidate` above is a no-op for it - warm it explicitly so it joins
      // the cpptools browse path without requiring a file inside it to be
      // opened first. Skipped for sims inside a nested git repo (submodule or
      // nested clone), same as warmPrimarySimRoots, so a `git submodule
      // update` that adds sims doesn't trigger a burst of `make` calls.
      if (path.basename(uri.fsPath) === 'S_define') {
        const simRoot = path.dirname(uri.fsPath);
        const workspaceRoot = vscode.workspace.getWorkspaceFolder(uri)?.uri.fsPath;
        if (!isInNestedRepo(simRoot, workspaceRoot)) {
          void this.getConfig(simRoot).then(() => this.onDidInvalidateEmitter.fire(simRoot));
        }
      }
    });
    this.watcher.onDidDelete(invalidate);
  }

  dispose(): void {
    this.watcher.dispose();
    this.onDidInvalidateEmitter.dispose();
    this.onDidResolveEmitter.dispose();
  }

  /** Walks up from fsPath looking for a directory containing S_define. */
  findSimRoot(fsPath: string): string | undefined {
    let dir = fs.statSync(fsPath, { throwIfNoEntry: false })?.isDirectory()
      ? fsPath
      : path.dirname(fsPath);
    const root = path.parse(dir).root;
    while (true) {
      if (fs.existsSync(path.join(dir, 'S_define'))) {
        return dir;
      }
      if (dir === root) {
        return undefined;
      }
      const parent = path.dirname(dir);
      if (parent === dir) {
        return undefined;
      }
      dir = parent;
    }
  }

  /**
   * Resolves the SimConfig that applies to an arbitrary file: its owning
   * sim (nearest ancestor S_define) if there is one, otherwise a global
   * fallback keyed on TRICK_HOME itself. The fallback covers Trick-shipped
   * files like share/trick/sim_objects/default_trick_sys.sm that live
   * outside any sim directory but still resolve ##include/#include lines
   * against TRICK_HOME's system include paths.
   */
  async getConfigForFile(fsPath: string): Promise<SimConfig | undefined> {
    const simRoot = this.findSimRoot(fsPath);
    if (simRoot) {
      return this.getConfig(simRoot);
    }
    const trickHome = this.resolveTrickHome(path.dirname(fsPath));
    if (trickHome) {
      return this.getConfig(trickHome);
    }
    return undefined;
  }

  invalidate(simRoot: string): void {
    this.cache.delete(simRoot);
  }

  clearAll(): void {
    this.cache.clear();
  }

  getAllCached(): SimConfig[] {
    return [...this.cache.values()];
  }

  /** Sim roots whose config has been resolved (eagerly or lazily) so far. */
  getAllCachedRoots(): string[] {
    return [...this.cache.keys()];
  }

  /**
   * Eagerly resolves and caches every *primary* sim in the workspace - every
   * directory containing an S_define that isn't inside a nested git repo
   * (a submodule, where `.git` is a file, or any other nested clone). cpptools'
   * Tag Parser uses provideBrowseConfiguration's browsePath to resolve
   * cross-file navigation (e.g. jumping from a method declared in a header to
   * its out-of-line definition in a .cpp), and that path is built from
   * getAllCached() - without this warm-up it would stay empty until a file in
   * each sim happened to be opened individually.
   *
   * Sims inside a nested repo are skipped here on purpose: a multi-package
   * workspace (e.g. a top-level repo with a dozen vendored libraries as git
   * submodules, each shipping its own demo/verification sims) can have
   * hundreds of those, and resolving all of them up front - one `make`
   * subprocess per sim - makes activation slow and floods cpptools' browse
   * path with directories from packages nobody's actually working in. Those
   * sims are still fully supported - they're just resolved lazily, the first
   * time a file inside one is opened (via getConfigForFile), same as any sim
   * that isn't warmed. getConfig() firing onDidResolve on that first
   * resolution is what adds them to the browse path at that point.
   */
  async warmPrimarySimRoots(): Promise<void> {
    const sDefineFiles = await vscode.workspace.findFiles(
      '**/S_define',
      '**/{node_modules,.git}/**'
    );
    const roots = new Set(
      sDefineFiles
        .filter((f) => !isInNestedRepo(path.dirname(f.fsPath), vscode.workspace.getWorkspaceFolder(f)?.uri.fsPath))
        .map((f) => path.dirname(f.fsPath))
    );
    const skipped = sDefineFiles.length - roots.size;
    await Promise.all(
      [...roots].map((root) =>
        this.getConfig(root).catch((err) => {
          this.output.appendLine(`[${root}] warm-up failed: ${err}`);
        })
      )
    );
    if (skipped > 0) {
      this.output.appendLine(
        `Warmed ${roots.size} sim configuration(s); ${skipped} sim(s) inside nested git repos will be resolved on first use.`
      );
    } else {
      this.output.appendLine(`Warmed ${roots.size} sim configuration(s) for IntelliSense browsing.`);
    }
  }

  async getConfig(simRoot: string): Promise<SimConfig> {
    const cached = this.cache.get(simRoot);
    if (cached) {
      return cached;
    }
    const config = await this.resolveConfig(simRoot);
    this.cache.set(simRoot, config);
    this.onDidResolveEmitter.fire(simRoot);
    return config;
  }

  private async resolveConfig(simRoot: string): Promise<SimConfig> {
    const settings = vscode.workspace.getConfiguration('trick');
    const useMake = settings.get<boolean>('useMakeForFlags', true);
    const trickHome = this.resolveTrickHome(simRoot);

    if (useMake && trickHome) {
      try {
        return await this.resolveViaMake(simRoot, trickHome);
      } catch (err) {
        this.output.appendLine(
          `[${simRoot}] make-based flag resolution failed, falling back to regex parse: ${err}`
        );
      }
    }
    return this.resolveViaRegex(simRoot, trickHome);
  }

  resolveTrickHome(fromDir: string): string | undefined {
    const settings = vscode.workspace.getConfiguration('trick');
    const configured = settings.get<string>('home', '');
    if (configured) {
      return configured;
    }
    if (process.env.TRICK_HOME) {
      return process.env.TRICK_HOME;
    }
    // Walk up from fromDir looking for share/trick/makefiles/Makefile.common,
    // which covers the common case of developing inside the Trick repo itself
    // (trick_sims/SIM_x) where TRICK_HOME == the repo root.
    let dir = fromDir;
    const root = path.parse(dir).root;
    while (true) {
      if (fs.existsSync(path.join(dir, 'share/trick/makefiles/Makefile.common'))) {
        return dir;
      }
      if (dir === root) {
        break;
      }
      const parent = path.dirname(dir);
      if (parent === dir) {
        break;
      }
      dir = parent;
    }
    return this.resolveTrickHomeFromPathEnv();
  }

  // Last resort: look for a "trick/bin" directory on $PATH (e.g. from an
  // installed Trick's bin/ being added to PATH) and treat its parent as
  // TRICK_HOME. Validated against Makefile.common to avoid false positives
  // from unrelated directories that happen to be named .../trick/bin.
  private resolveTrickHomeFromPathEnv(): string | undefined {
    const pathEnv = process.env.PATH;
    if (!pathEnv) {
      return undefined;
    }
    for (const entry of pathEnv.split(path.delimiter)) {
      if (!entry || path.basename(entry) !== 'bin') {
        continue;
      }
      const candidate = path.dirname(entry);
      if (path.basename(candidate) !== 'trick') {
        continue;
      }
      if (fs.existsSync(path.join(candidate, 'share/trick/makefiles/Makefile.common'))) {
        return candidate;
      }
    }
    return undefined;
  }

  private resolveViaMake(simRoot: string, trickHome: string): Promise<SimConfig> {
    const makefile = path.join(__dirname, '..', 'make', 'trick-vars.mk');
    const args = ['-s', '-C', simRoot, '-f', makefile, ...FLAG_VARS.map((v) => `print-${v}`)];
    this.output.appendLine(`[${simRoot}] make ${args.join(' ')}`);
    return new Promise((resolve, reject) => {
      cp.execFile(
        'make',
        args,
        // PWD is set explicitly because `make -C` changes make's own cwd
        // without updating $PWD, and some sims (e.g. S_overrides.mk's
        // `TRICK_PYTHON_PATH += :${PWD}/Modified_data`) rely on it expanding
        // to the sim root, matching trick-CP's actual invocation.
        { cwd: simRoot, env: { ...process.env, TRICK_HOME: trickHome, PWD: simRoot }, timeout: 10000 },
        (error, stdout, stderr) => {
          if (error) {
            this.output.appendLine(`[${simRoot}] make stderr: ${stderr}`);
            reject(error);
            return;
          }
          const lines = stdout.split('\n');
          const raw: Record<string, string> = {};
          FLAG_VARS.forEach((v, i) => {
            raw[v] = (lines[i] ?? '').trim();
          });
          resolve(this.buildConfig(simRoot, 'make', raw, trickHome));
        }
      );
    });
  }

  private resolveViaRegex(simRoot: string, trickHome: string | undefined): SimConfig {
    const overridesPath = path.join(simRoot, 'S_overrides.mk');
    const raw: Record<string, string> = {
      TRICK_CFLAGS: '',
      TRICK_CXXFLAGS: '',
      TRICK_SFLAGS: '',
      TRICK_SYSTEM_CFLAGS: '',
      TRICK_SYSTEM_CXXFLAGS: '',
      TRICK_SYSTEM_SFLAGS: '',
      TRICK_CXX: '',
      TRICK_CC: '',
      TRICK_PYTHON_PATH: '',
    };

    if (trickHome) {
      raw.TRICK_SYSTEM_SFLAGS = `-I${trickHome}/share/trick -I${trickHome}/share`;
      raw.TRICK_SYSTEM_CXXFLAGS = `-isystem${trickHome}/trick_source -isystem${trickHome}/include -isystem${trickHome}/include/trick/compat`;
      raw.TRICK_SYSTEM_CFLAGS = raw.TRICK_SYSTEM_CXXFLAGS;
    }

    if (fs.existsSync(overridesPath)) {
      const text = fs.readFileSync(overridesPath, 'utf8');
      const values: Record<string, string> = {};
      // Join line-continuations, then scan assignment lines for the three flag vars.
      const joined = text.replace(/\\\r?\n/g, ' ');
      const assignRe = /^\s*(TRICK_C(?:XX)?FLAGS|TRICK_SFLAGS|TRICK_PYTHON_PATH)\s*(\+?=|:=)\s*(.*)$/gm;
      let m: RegExpExecArray | null;
      while ((m = assignRe.exec(joined))) {
        const [, varName, op, rawValue] = m;
        const expanded = this.expandMakeVars(rawValue.trim(), simRoot, trickHome, values);
        if (op === '+=' && values[varName]) {
          values[varName] = `${values[varName]} ${expanded}`;
        } else {
          values[varName] = expanded;
        }
      }
      for (const key of ['TRICK_CFLAGS', 'TRICK_CXXFLAGS', 'TRICK_SFLAGS', 'TRICK_PYTHON_PATH']) {
        if (values[key] !== undefined) {
          raw[key] = values[key];
        }
      }
    }

    return this.buildConfig(simRoot, 'regex', raw, trickHome);
  }

  private expandMakeVars(
    value: string,
    simRoot: string,
    trickHome: string | undefined,
    alreadyParsed: Record<string, string>
  ): string {
    return value.replace(/\$[({]([A-Za-z_][A-Za-z0-9_]*)[)}]/g, (whole, name: string) => {
      if (name === 'TRICK_HOME' && trickHome) {
        return trickHome;
      }
      if (name === 'CURDIR' || name === 'PWD') {
        return simRoot;
      }
      if (alreadyParsed[name] !== undefined) {
        return alreadyParsed[name];
      }
      if (process.env[name] !== undefined) {
        return process.env[name]!;
      }
      return whole; // leave unresolved rather than guessing
    });
  }

  private buildConfig(
    simRoot: string,
    source: 'make' | 'regex',
    raw: Record<string, string>,
    trickHome: string | undefined
  ): SimConfig {
    const extractIncludes = (...flagStrings: string[]): string[] => {
      const dirs: string[] = [];
      const re = /-(?:I|isystem)\s*(\S+)/g;
      for (const flags of flagStrings) {
        let m: RegExpExecArray | null;
        while ((m = re.exec(flags))) {
          dirs.push(m[1]);
        }
      }
      return [...new Set(dirs)]
        .map((d) => (path.isAbsolute(d) ? d : path.resolve(simRoot, d)))
        .filter((d) => fs.existsSync(d));
    };
    const extractDefines = (flags: string): string[] => {
      const defines: string[] = [];
      const re = /-D\s*(\S+)/g;
      let m: RegExpExecArray | null;
      while ((m = re.exec(flags))) {
        defines.push(m[1]);
      }
      return defines;
    };
    const extractStandard = (flags: string): string | undefined => {
      const m = /-std=(c\+\+\d+|c\d+)/.exec(flags);
      return m?.[1];
    };

    const settings = vscode.workspace.getConfiguration('trick');
    const overrideStandard = settings.get<string>('cppStandard', '');

    return {
      simRoot,
      source,
      cIncludes: extractIncludes(raw.TRICK_CFLAGS, raw.TRICK_SYSTEM_CFLAGS),
      cxxIncludes: extractIncludes(raw.TRICK_CXXFLAGS, raw.TRICK_SYSTEM_CXXFLAGS),
      sIncludes: extractIncludes(raw.TRICK_SFLAGS, raw.TRICK_SYSTEM_SFLAGS),
      cDefines: extractDefines(`${raw.TRICK_CFLAGS} ${raw.TRICK_SYSTEM_CFLAGS}`),
      cxxDefines: extractDefines(`${raw.TRICK_CXXFLAGS} ${raw.TRICK_SYSTEM_CXXFLAGS}`),
      cxxStandard:
        overrideStandard ||
        extractStandard(`${raw.TRICK_CXXFLAGS} ${raw.TRICK_SYSTEM_CXXFLAGS}`) ||
        'c++14',
      compilerPath: raw.TRICK_CXX || undefined,
      cCompilerPath: raw.TRICK_CC || undefined,
      pythonPaths: buildPythonPaths(simRoot, trickHome, raw.TRICK_PYTHON_PATH ?? ''),
      raw,
    };
  }
}

/**
 * Builds a narrow subset of the sys.path entries Trick's input processor adds
 * at runtime (see IPPython.cpp): TRICK_HOME's pymods, <simRoot>/Modified_data,
 * then each TRICK_PYTHON_PATH entry. The sim root itself is deliberately left
 * out - see the SimConfig.pythonPaths doc comment for why. Relative
 * TRICK_PYTHON_PATH entries are resolved against simRoot, and entries that
 * don't exist on disk (e.g. an unexpanded `$(DOUG_HOME)` when that env var
 * isn't set) are dropped rather than guessed at.
 */
export function buildPythonPaths(
  simRoot: string,
  trickHome: string | undefined,
  rawPythonPath: string
): string[] {
  const candidates = [
    ...(trickHome ? [path.join(trickHome, 'share', 'trick', 'pymods')] : []),
    path.join(simRoot, 'Modified_data'),
    ...rawPythonPath.split(':').map((p) => p.trim()),
  ];
  const dirs: string[] = [];
  const seen = new Set<string>();
  for (const candidate of candidates) {
    if (!candidate) {
      continue;
    }
    const resolved = path.isAbsolute(candidate) ? candidate : path.resolve(simRoot, candidate);
    if (seen.has(resolved)) {
      continue;
    }
    seen.add(resolved);
    if (fs.existsSync(resolved) && fs.statSync(resolved).isDirectory()) {
      dirs.push(resolved);
    }
  }
  return dirs;
}

/**
 * True if `simRoot` sits inside a nested git repo relative to `workspaceRoot` -
 * a submodule (whose root has a `.git` *file* pointing at the parent repo's
 * `.git/modules/...`) or any other nested clone (`.git` as a directory). The
 * workspace root's own `.git` doesn't count: only directories strictly
 * between it and simRoot are checked. If simRoot isn't under workspaceRoot
 * (or workspaceRoot is unknown), this returns false - treat it as primary.
 */
export function isInNestedRepo(simRoot: string, workspaceRoot: string | undefined): boolean {
  if (!workspaceRoot) {
    return false;
  }
  const relative = path.relative(workspaceRoot, simRoot);
  if (relative.startsWith('..') || path.isAbsolute(relative)) {
    return false;
  }
  let dir = simRoot;
  while (dir !== workspaceRoot) {
    if (fs.existsSync(path.join(dir, '.git'))) {
      return true;
    }
    const parent = path.dirname(dir);
    if (parent === dir) {
      return false;
    }
    dir = parent;
  }
  return false;
}
