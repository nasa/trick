import * as vscode from 'vscode';
import { SimConfigProvider, TrickHomeSource } from './simConfig';

/**
 * Backs the Trick activity bar's "Commands" section: a quick-access list of
 * every `Trick:` command-palette command, so they don't have to be typed out
 * via Ctrl+Shift+P, plus a header row showing TRICK_HOME.
 *
 * The header row exists because VS Code has no API for an extension to put a
 * tooltip on the view *container's* own title text (the "Trick" label at the
 * very top of the sidebar, next to the activity bar icon) - this is the
 * closest equivalent: a non-clickable-for-info row, always visible at the top
 * of this section, whose tooltip explains where TRICK_HOME came from.
 */

interface HomeNode {
  kind: 'home';
}

interface CommandNode {
  kind: 'command';
  id: string;
  title: string;
  icon?: string;
}

export type CommandsTreeNode = HomeNode | CommandNode;

interface PackageJsonCommand {
  command: string;
  title: string;
  icon?: string;
}

/** Every contributed command whose title starts with "Trick:", in package.json declaration order - so a new command shows up here automatically with no second list to keep in sync. Internal commands (e.g. the ones backing the Sims/Connected views' inline buttons) are deliberately titled without that prefix for exactly this reason. */
function discoverCommands(packageJSON: { contributes?: { commands?: PackageJsonCommand[] } }): CommandNode[] {
  const contributed = packageJSON.contributes?.commands ?? [];
  return contributed
    .filter((c) => c.title.startsWith('Trick:'))
    .map((c) => ({
      kind: 'command',
      id: c.command,
      title: c.title,
      icon: c.icon?.replace(/^\$\(|\)$/g, ''),
    }));
}

/**
 * What each command actually does, for the tooltip in the Commands section -
 * package.json's command contributions have no description field (just
 * command/title/icon/category), so this is the one place that text lives.
 * Keep these in sync with the longer explanations in README.md's Commands
 * section and the how-to guide if a command's behavior changes.
 */
const COMMAND_DESCRIPTIONS: Record<string, string> = {
  'trick.refreshSimConfiguration':
    'Clears cached per-sim include paths and re-resolves them. Use after editing S_overrides.mk if it isn\'t picked up automatically.',
  'trick.showResolvedIncludePaths':
    'Opens a JSON view of the include paths, defines, and C++ standard resolved for the sim containing the active file, and whether they came from make or the regex fallback.',
  'trick.updateEnvironmentFromShell':
    'Runs a shell command you give it (an alias, a shell function, or `source some_env.sh`) in your interactive shell, then merges the resulting environment into the extension. Use this when TRICK_HOME/TRICK_*FLAGS were only ever set up in a terminal after VS Code launched.',
  'trick.regeneratePythonStubs':
    'Re-scrapes trick.* and sim object names and rewrites the generated Python stub files. Use after building Trick for the first time, or after adding a new sim object, if it isn\'t picked up automatically.',
  'trick.buildCurrentSim':
    'Runs trick-CP for the sim containing the active file, reporting errors/warnings in the Problems panel. Bound to Ctrl+Shift+B (Cmd+Shift+B on macOS) while a file inside a sim is focused.',
  'trick.runCurrentSim':
    "Launches the built executable for the sim containing the active file, with a chosen RUN_*/*.py input file - offering to build first if it hasn't been built yet. Bound to Ctrl+F5 (Cmd+F5 on macOS) while a file inside a sim is focused.",
  'trick.sims.connectManual':
    "Connects to a variable server directly, for sims discovery can't see (multicast blocked, or a different host). Also available as the + button in the Sims and Connected views.",
  'trick.sims.watchFromEditor':
    'Adds the variable path under the cursor (right-click in input.py/.dr) as a watch on a connected sim, keeping its array index if there is one.',
};

function sourceDescription(source: TrickHomeSource, fromDir: string): string {
  switch (source) {
    case 'setting':
      return 'the `trick.home` setting';
    case 'env':
      return 'the `TRICK_HOME` environment variable';
    case 'walk-up':
      return `walking up from \`${fromDir}\` to find \`share/trick/makefiles/Makefile.common\``;
    case 'path':
      return 'a `trick/bin` directory on PATH';
  }
}

export class TrickCommandsTreeProvider implements vscode.TreeDataProvider<CommandsTreeNode> {
  private readonly changeEmitter = new vscode.EventEmitter<CommandsTreeNode[] | undefined>();
  readonly onDidChangeTreeData = this.changeEmitter.event;

  // Stable object identity for the header row across refreshes - VS Code's
  // TreeView matches refreshed elements by identity, same reasoning as
  // simTree.ts's SimTreeNode/WatchTreeNode.
  private readonly homeNode: HomeNode = { kind: 'home' };
  private readonly commandNodes: CommandNode[];

  constructor(
    private readonly simConfigs: SimConfigProvider,
    packageJSON: { contributes?: { commands?: PackageJsonCommand[] } }
  ) {
    this.commandNodes = discoverCommands(packageJSON);
  }

  dispose(): void {
    this.changeEmitter.dispose();
  }

  /** Call whenever TRICK_HOME resolution might have changed (active editor, sim config invalidation, trick.home setting, after Update Environment). */
  refresh(): void {
    this.changeEmitter.fire([this.homeNode]);
  }

  getTreeItem(node: CommandsTreeNode): vscode.TreeItem {
    return node.kind === 'home' ? this.homeTreeItem() : this.commandTreeItem(node);
  }

  getChildren(node?: CommandsTreeNode): CommandsTreeNode[] {
    return node ? [] : [this.homeNode, ...this.commandNodes];
  }

  private commandTreeItem(node: CommandNode): vscode.TreeItem {
    const label = node.title.replace(/^Trick:\s*/, '');
    const item = new vscode.TreeItem(label, vscode.TreeItemCollapsibleState.None);
    item.id = `command:${node.id}`;
    item.contextValue = 'trickCommand';
    item.command = { command: node.id, title: node.title };
    const description = COMMAND_DESCRIPTIONS[node.id];
    item.tooltip = description ? new vscode.MarkdownString(`**${node.title}**\n\n${description}`) : node.title;
    if (node.icon) {
      item.iconPath = new vscode.ThemeIcon(node.icon);
    }
    return item;
  }

  /** Directory used to resolve TRICK_HOME: the active editor's owning sim root, else the first workspace folder - the same starting point Build/Run would use for that file. */
  private referenceDir(): { dir: string; simName?: string } | undefined {
    const activePath = vscode.window.activeTextEditor?.document.uri.fsPath;
    if (activePath) {
      const simRoot = this.simConfigs.findSimRoot(activePath);
      if (simRoot) {
        return { dir: simRoot, simName: simRoot.split('/').pop() };
      }
    }
    const folder = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath;
    return folder ? { dir: folder } : undefined;
  }

  private homeTreeItem(): vscode.TreeItem {
    const item = new vscode.TreeItem('TRICK_HOME', vscode.TreeItemCollapsibleState.None);
    item.id = 'trickHome';
    item.contextValue = 'trickHome';

    const reference = this.referenceDir();
    const resolved = reference ? this.simConfigs.resolveTrickHomeWithSource(reference.dir) : undefined;

    if (!resolved) {
      item.description = 'not found';
      item.iconPath = new vscode.ThemeIcon('warning');
      item.tooltip = new vscode.MarkdownString(
        "**TRICK_HOME** couldn't be resolved.\n\nIf your environment is only set up in a terminal " +
          '(common when VS Code is launched from a desktop/file launcher), run **Trick: Update ' +
          'Environment from Shell Command…**. Otherwise, set the `trick.home` setting.'
      );
      item.command = { command: 'trick.updateEnvironmentFromShell', title: 'Update Environment' };
      return item;
    }

    item.description = resolved.path;
    item.iconPath = new vscode.ThemeIcon('home');
    const simNote = reference?.simName ? ` (for ${reference.simName})` : '';
    item.tooltip = new vscode.MarkdownString(
      `**TRICK_HOME**: \`${resolved.path}\`\n\nResolved via ${sourceDescription(resolved.source, reference!.dir)}${simNote}.`
    );
    return item;
  }
}
