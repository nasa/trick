import * as fs from 'fs';
import * as path from 'path';
import * as vscode from 'vscode';
import { SimConfigProvider } from './simConfig';

export const TRICK_TASK_TYPE = 'trick';

export interface TrickTaskDefinition extends vscode.TaskDefinition {
  type: typeof TRICK_TASK_TYPE;
  simRoot: string;
  // Only present on a run task (see createTrickRunTask) - a build task has no
  // single input file to record.
  inputFile?: string;
}

// trick-CP (bin/trick-CP) generates a makefile and execs `make`; neither it
// nor trick-ICG offer a machine-readable output mode, so both compiler errors
// and trick-ICG's Clang-based parse diagnostics go to stdout/stderr as plain
// text (see the $trick problem matcher in package.json). Piping through `cat`
// defeats isatty() color-diagnostics detection in g++/clang++/trick-ICG, since
// VS Code's task terminal is itself a pty and would otherwise get ANSI escape
// codes the problem matcher can't parse.
export const TRICK_BUILD_COMMAND = 'trick-CP 2>&1 | cat';

// TRICK_HOME/bin isn't guaranteed to be on PATH just because VS Code can find
// TRICK_HOME (trick.home / auto-detection don't touch the shell's PATH), so
// this is added explicitly rather than assuming the user's shell profile has it.
export function buildTrickEnv(
  trickHome: string | undefined,
  basePath: string | undefined
): { [key: string]: string } | undefined {
  if (!trickHome) {
    return undefined;
  }
  const trickBin = path.join(trickHome, 'bin');
  return { PATH: basePath ? `${trickBin}${path.delimiter}${basePath}` : trickBin };
}

export function createTrickBuildTask(
  scope: vscode.WorkspaceFolder,
  simRoot: string,
  trickHome: string | undefined
): vscode.Task {
  const definition: TrickTaskDefinition = { type: TRICK_TASK_TYPE, simRoot };
  const execution = new vscode.ShellExecution(TRICK_BUILD_COMMAND, {
    cwd: simRoot,
    env: buildTrickEnv(trickHome, process.env.PATH),
  });
  const task = new vscode.Task(
    definition,
    scope,
    `Build ${path.basename(simRoot)}`,
    'trick',
    execution,
    ['$trick']
  );
  task.group = vscode.TaskGroup.Build;
  return task;
}

const SIM_EXECUTABLE_RE = /^S_main_.*\.exe$/;

// TRICK_HOST_CPU (baked into the executable name by trick-CP) encodes the
// build machine's OS version (e.g. S_main_Linux_13.3_x86_64.exe), so it can't
// be guessed - this has to glob the sim root rather than assume a fixed name.
// If more than one exists (e.g. left over from a build on a different
// machine), the most recently built one wins.
export function findSimExecutable(simRoot: string): string | undefined {
  let entries: string[];
  try {
    entries = fs.readdirSync(simRoot);
  } catch {
    return undefined;
  }
  const matches = entries.filter((name) => SIM_EXECUTABLE_RE.test(name));
  if (matches.length <= 1) {
    return matches[0];
  }
  return matches
    .map((name) => ({ name, mtimeMs: fs.statSync(path.join(simRoot, name)).mtimeMs }))
    .sort((a, b) => b.mtimeMs - a.mtimeMs)[0].name;
}

// Every RUN_*/*.py under the sim root, as sim-root-relative paths (always
// forward-slashed, since that's also what the sim's own command line expects
// - see Running-a-Simulation.md). A RUN_ directory commonly has more than one
// .py file (e.g. input.py alongside a unit_test.py), so this can't assume a
// fixed input.py name either.
export function listInputFiles(simRoot: string): string[] {
  let entries: fs.Dirent[];
  try {
    entries = fs.readdirSync(simRoot, { withFileTypes: true });
  } catch {
    return [];
  }
  const files: string[] = [];
  for (const entry of entries) {
    if (!entry.isDirectory() || !entry.name.startsWith('RUN_')) {
      continue;
    }
    let runEntries: string[];
    try {
      runEntries = fs.readdirSync(path.join(simRoot, entry.name));
    } catch {
      continue;
    }
    for (const file of runEntries) {
      if (file.endsWith('.py')) {
        files.push(`${entry.name}/${file}`);
      }
    }
  }
  return files.sort();
}

// If the active file is itself a RUN_*/*.py, running it directly (no picker)
// covers the common flow of editing an input file and running it right away.
export function inputFileForActiveFile(simRoot: string, fsPath: string): string | undefined {
  const rel = path.relative(simRoot, fsPath);
  if (!rel || rel.startsWith('..') || path.isAbsolute(rel)) {
    return undefined;
  }
  const parts = rel.split(path.sep);
  if (parts.length !== 2 || !parts[0].startsWith('RUN_') || !parts[1].endsWith('.py')) {
    return undefined;
  }
  return parts.join('/');
}

// Like buildTrickEnv, but a running sim also needs TRICK_HOME itself in its
// environment (not just TRICK_HOME/bin on PATH): the embedded Python input
// processor reads os.environ['TRICK_HOME'] directly (IPPython.cpp) to find
// share/trick/pymods.
export function buildTrickRunEnv(
  trickHome: string | undefined,
  basePath: string | undefined
): { [key: string]: string } | undefined {
  const base = buildTrickEnv(trickHome, basePath);
  if (!base || !trickHome) {
    return undefined;
  }
  return { ...base, TRICK_HOME: trickHome };
}

export function createTrickRunTask(
  scope: vscode.WorkspaceFolder,
  simRoot: string,
  executable: string,
  inputFile: string,
  trickHome: string | undefined
): vscode.Task {
  const definition: TrickTaskDefinition = { type: TRICK_TASK_TYPE, simRoot, inputFile };
  const execution = new vscode.ShellExecution(`./${executable}`, [inputFile], {
    cwd: simRoot,
    env: buildTrickRunEnv(trickHome, process.env.PATH),
  });
  const task = new vscode.Task(definition, scope, `Run ${path.basename(simRoot)} (${inputFile})`, 'trick', execution);
  task.presentationOptions = { clear: true, focus: true };
  return task;
}

export class TrickTaskProvider implements vscode.TaskProvider {
  constructor(private readonly simConfigs: SimConfigProvider) {}

  async provideTasks(): Promise<vscode.Task[]> {
    const tasks: vscode.Task[] = [];
    for (const folder of vscode.workspace.workspaceFolders ?? []) {
      const sDefineFiles = await vscode.workspace.findFiles(
        new vscode.RelativePattern(folder, '**/S_define'),
        '**/{build,.git}/**'
      );
      for (const file of sDefineFiles) {
        const simRoot = path.dirname(file.fsPath);
        tasks.push(createTrickBuildTask(folder, simRoot, this.simConfigs.resolveTrickHome(simRoot)));
      }
    }
    return tasks;
  }

  resolveTask(task: vscode.Task): vscode.Task | undefined {
    const definition = task.definition as TrickTaskDefinition;
    if (!definition.simRoot || task.scope === undefined || typeof task.scope === 'number') {
      return undefined;
    }
    return createTrickBuildTask(task.scope, definition.simRoot, this.simConfigs.resolveTrickHome(definition.simRoot));
  }
}
