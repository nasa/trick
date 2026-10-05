import * as path from 'path';
import * as vscode from 'vscode';
import { SimConfigProvider } from './simConfig';

export const TRICK_TASK_TYPE = 'trick';

export interface TrickTaskDefinition extends vscode.TaskDefinition {
  type: typeof TRICK_TASK_TYPE;
  simRoot: string;
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
