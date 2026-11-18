import * as path from 'path';
import * as vscode from 'vscode';
import { SimConfigProvider } from './simConfig';
import { TrickCppConfigurationProvider } from './cpptoolsProvider';
import { TrickIncludeLinkProvider, TrickIncludeDiagnostics } from './sdefineLinks';
import { TrickSdefineDefinitionProvider } from './sdefineDefinitions';
import { TrickPythonDefinitionProvider } from './pythonDefinitions';
import { PythonStubManager } from './pythonStubs';
import { TrickPythonLinkProvider } from './pythonLinks';
import {
  TRICK_TASK_TYPE,
  TrickTaskProvider,
  createTrickBuildTask,
  createTrickRunTask,
  findSimExecutable,
  inputFileForActiveFile,
  listInputFiles,
} from './buildTasks';
import { SimDiscovery } from './simDiscovery';
import { ConnectedSimsTreeProvider, TrickSimsTreeProvider, registerSimCommands } from './simTree';
import { ShellEnvCaptureError, captureShellEnv, diffEnv } from './shellEnv';
import { TrickCommandsTreeProvider } from './commandsTree';

// Finds the sim's built executable, or - if it hasn't been built yet - offers
// to build it first and only proceeds to run if that build succeeds.
async function findBuiltExecutableOrOfferBuild(
  simRoot: string,
  folder: vscode.WorkspaceFolder,
  trickHome: string | undefined
): Promise<string | undefined> {
  const existing = findSimExecutable(simRoot);
  if (existing) {
    return existing;
  }
  const choice = await vscode.window.showWarningMessage(
    `${path.basename(simRoot)} hasn't been built yet.`,
    'Build and Run'
  );
  if (choice !== 'Build and Run') {
    return undefined;
  }
  const buildTask = createTrickBuildTask(folder, simRoot, trickHome);
  const execution = await vscode.tasks.executeTask(buildTask);
  const exitCode = await new Promise<number | undefined>((resolve) => {
    const subscription = vscode.tasks.onDidEndTaskProcess((e) => {
      if (e.execution === execution) {
        subscription.dispose();
        resolve(e.exitCode);
      }
    });
  });
  if (exitCode !== 0) {
    vscode.window.showErrorMessage(`Build failed; not running ${path.basename(simRoot)}.`);
    return undefined;
  }
  return findSimExecutable(simRoot);
}

// If the workspace has at least one sim and none of them can resolve
// TRICK_HOME, nudges the user toward "Trick: Update Environment from Shell
// Command" rather than leaving them to discover on their own that IntelliSense
// and builds are silently falling back to guesses - the common cause being VS
// Code launched from a desktop/file launcher with no TRICK_HOME in its own
// process.env, with the environment only ever sourced afterward in a terminal
// tab (which doesn't reach the extension host - see shellEnv.ts). Runs once
// per activation, not on every file open.
async function nudgeIfTrickHomeUnresolved(simConfigs: SimConfigProvider): Promise<void> {
  const sDefineFiles = await vscode.workspace.findFiles('**/S_define', '**/{node_modules,.git}/**');
  if (sDefineFiles.length === 0) {
    return;
  }
  const anyResolved = sDefineFiles.some(
    (f) => simConfigs.resolveTrickHome(path.dirname(f.fsPath)) !== undefined
  );
  if (anyResolved) {
    return;
  }
  const choice = await vscode.window.showWarningMessage(
    "Trick: TRICK_HOME couldn't be found. If you set up your environment in a terminal, run it here instead.",
    'Update Environment…',
    'Open Settings'
  );
  if (choice === 'Update Environment…') {
    await vscode.commands.executeCommand('trick.updateEnvironmentFromShell');
  } else if (choice === 'Open Settings') {
    await vscode.commands.executeCommand('workbench.action.openSettings', 'trick.home');
  }
}

export async function activate(context: vscode.ExtensionContext): Promise<void> {
  const output = vscode.window.createOutputChannel('Trick');
  context.subscriptions.push(output);

  const simConfigs = new SimConfigProvider(output);
  context.subscriptions.push(simConfigs);

  const cppProvider = new TrickCppConfigurationProvider(simConfigs, output);
  context.subscriptions.push(cppProvider);
  await cppProvider.activate();

  // Warm every *primary* sim's config up front (sims not inside a nested git
  // repo - see warmPrimarySimRoots) so cpptools' Tag Parser has a complete
  // browse path from the start for those, letting it resolve cross-file
  // navigation (e.g. header method declaration -> .cpp implementation)
  // without first requiring every file to be opened manually. Sims inside a
  // nested repo join the browse path lazily instead, via onDidResolve.
  void simConfigs.warmPrimarySimRoots().then(() => nudgeIfTrickHomeUnresolved(simConfigs));

  const pythonStubs = new PythonStubManager(simConfigs, output, context.workspaceState);
  context.subscriptions.push(pythonStubs);
  void pythonStubs.refreshAll();

  context.subscriptions.push(
    vscode.commands.registerCommand('trick.regeneratePythonStubs', () => {
      void pythonStubs.refreshAll().then(() => {
        output.appendLine('Regenerated Python stubs for trick.* and sim object builtins.');
        output.show(true);
      });
    })
  );

  const linkProvider = new TrickIncludeLinkProvider(simConfigs);
  context.subscriptions.push(
    vscode.languages.registerDocumentLinkProvider({ language: 'trick-sdefine' }, linkProvider)
  );

  const pythonLinkProvider = new TrickPythonLinkProvider(simConfigs);
  context.subscriptions.push(
    vscode.languages.registerDocumentLinkProvider({ language: 'python' }, pythonLinkProvider)
  );

  // When both providers return a result for the same symbol, same-selector-score
  // providers tie-break by registration time, newest first (VS Code's
  // languageFeatureRegistry._compareByScoreAndTime). Awaiting Pylance's own
  // activate() isn't enough to guarantee ours registers last: activate() can
  // resolve before Pylance's language server has finished starting, and
  // Pylance only registers its DefinitionProvider once that client connects -
  // which can happen well after a document is opened (its LSP server is a
  // separate process that can take several seconds to spin up, especially on
  // a cold start). Re-registering just once per document open isn't enough
  // either: if Pylance's registration lands after that one re-registration,
  // Pylance stays "newest" - and therefore wins every Ctrl+click - until some
  // other Python document happens to be opened (e.g. the very stub file
  // Pylance's own result just navigated to, which is what made the bug look
  // like only ever the *second* click onward resolved correctly). So instead
  // of a single re-register, re-assert ourselves repeatedly for a few seconds
  // after each open, which re-wins the tie-break once Pylance does show up
  // without needing a "Pylance is ready" signal (none is publicly exposed).
  const definitionProvider = new TrickPythonDefinitionProvider(simConfigs);
  let pythonDefinitionRegistration: vscode.Disposable | undefined;
  const refreshPythonDefinitionProvider = () => {
    pythonDefinitionRegistration?.dispose();
    pythonDefinitionRegistration = vscode.languages.registerDefinitionProvider(
      { language: 'python', scheme: 'file' },
      definitionProvider
    );
  };
  let outrunPylanceTimers: ReturnType<typeof setTimeout>[] = [];
  const outrunPylanceRegistration = () => {
    outrunPylanceTimers.forEach(clearTimeout);
    outrunPylanceTimers = [300, 1000, 3000, 8000].map((delay) =>
      setTimeout(refreshPythonDefinitionProvider, delay)
    );
  };
  refreshPythonDefinitionProvider();
  outrunPylanceRegistration();
  context.subscriptions.push(
    {
      dispose: () => {
        pythonDefinitionRegistration?.dispose();
        outrunPylanceTimers.forEach(clearTimeout);
      },
    },
    vscode.workspace.onDidOpenTextDocument((document) => {
      if (document.languageId === 'python') {
        refreshPythonDefinitionProvider();
        outrunPylanceRegistration();
      }
    })
  );

  const sdefineDefinitionProvider = new TrickSdefineDefinitionProvider(simConfigs);
  context.subscriptions.push(
    sdefineDefinitionProvider,
    vscode.languages.registerDefinitionProvider({ language: 'trick-sdefine' }, sdefineDefinitionProvider)
  );

  const diagnostics = new TrickIncludeDiagnostics(simConfigs);
  context.subscriptions.push(diagnostics);

  const refreshDiagnostics = (document: vscode.TextDocument) => {
    if (document.languageId === 'trick-sdefine') {
      void diagnostics.refresh(document);
    }
  };
  context.subscriptions.push(
    vscode.workspace.onDidOpenTextDocument(refreshDiagnostics),
    vscode.workspace.onDidSaveTextDocument(refreshDiagnostics),
    vscode.workspace.onDidChangeTextDocument((e) => refreshDiagnostics(e.document)),
    vscode.workspace.onDidCloseTextDocument((doc) => diagnostics.clear(doc)),
    simConfigs.onDidInvalidate(() => {
      for (const doc of vscode.workspace.textDocuments) {
        refreshDiagnostics(doc);
      }
    })
  );
  vscode.workspace.textDocuments.forEach(refreshDiagnostics);

  const statusBar = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Left, 100);
  context.subscriptions.push(statusBar);
  const updateStatusBar = async (editor: vscode.TextEditor | undefined) => {
    if (!editor) {
      statusBar.hide();
      return;
    }
    const simRoot = simConfigs.findSimRoot(editor.document.uri.fsPath);
    void vscode.commands.executeCommand('setContext', 'trick.inSimRoot', !!simRoot);
    if (!simRoot) {
      statusBar.hide();
      return;
    }
    const config = await simConfigs.getConfig(simRoot);
    statusBar.text = `Trick: ${simRoot.split('/').pop()} (${config.source})`;
    statusBar.tooltip = `Sim root: ${simRoot}\nFlags resolved via: ${config.source}`;
    statusBar.show();
  };

  // Commands sidebar section: quick-access list of every Trick: command,
  // plus a TRICK_HOME header row (see commandsTree.ts for why it's a row
  // rather than a tooltip on the "Trick" title text itself). Refreshed on
  // the same signals as the status bar above, since both report on the same
  // underlying TRICK_HOME/sim-config resolution.
  const commandsTreeProvider = new TrickCommandsTreeProvider(simConfigs, context.extension.packageJSON);
  context.subscriptions.push({ dispose: () => commandsTreeProvider.dispose() });
  const commandsTreeView = vscode.window.createTreeView('trickCommands', { treeDataProvider: commandsTreeProvider });
  context.subscriptions.push(commandsTreeView);
  context.subscriptions.push(
    vscode.commands.registerCommand('trick.commands.refreshView', () => commandsTreeProvider.refresh())
  );

  context.subscriptions.push(
    vscode.window.onDidChangeActiveTextEditor((editor) => {
      void updateStatusBar(editor);
      commandsTreeProvider.refresh();
    }),
    simConfigs.onDidInvalidate(() => {
      void updateStatusBar(vscode.window.activeTextEditor);
      commandsTreeProvider.refresh();
    }),
    vscode.workspace.onDidChangeConfiguration((e) => {
      if (e.affectsConfiguration('trick.home')) {
        commandsTreeProvider.refresh();
      }
    })
  );
  void updateStatusBar(vscode.window.activeTextEditor);

  context.subscriptions.push(
    vscode.commands.registerCommand('trick.refreshSimConfiguration', () => {
      simConfigs.invalidateAll();
      vscode.workspace.textDocuments.forEach(refreshDiagnostics);
      void updateStatusBar(vscode.window.activeTextEditor);
      commandsTreeProvider.refresh();
      void simConfigs.warmPrimarySimRoots();
      output.appendLine('Cleared all cached sim configurations.');
      output.show(true);
    })
  );

  context.subscriptions.push(
    vscode.commands.registerCommand('trick.showResolvedIncludePaths', async () => {
      const editor = vscode.window.activeTextEditor;
      if (!editor) {
        vscode.window.showWarningMessage('Open a file inside a Trick sim directory first.');
        return;
      }
      const simRoot = simConfigs.findSimRoot(editor.document.uri.fsPath);
      if (!simRoot) {
        vscode.window.showWarningMessage('No S_define found in any parent directory of the active file.');
        return;
      }
      const config = await simConfigs.getConfig(simRoot);
      const doc = await vscode.workspace.openTextDocument({
        language: 'json',
        content: JSON.stringify(config, null, 2),
      });
      await vscode.window.showTextDocument(doc, { preview: true });
    })
  );

  const LAST_ENV_COMMAND_KEY = 'trick.env.lastCommand';
  // Values are only worth logging for vars whose contents are inherently
  // non-secret (TRICK_* flags/paths, PATH itself) - everything else from an
  // arbitrary sourced script could be a credential, token, etc, so only its
  // name is ever written to the output channel or shown in a message.
  const safeToLogValue = (name: string) => name.startsWith('TRICK_') || name === 'PATH';

  context.subscriptions.push(
    vscode.commands.registerCommand('trick.updateEnvironmentFromShell', async () => {
      const lastCommand = context.workspaceState.get<string>(LAST_ENV_COMMAND_KEY, '');
      const userCommand = await vscode.window.showInputBox({
        title: 'Trick: Update Environment from Shell Command',
        prompt:
          'Shell command that sets up your Trick environment (an alias, a shell function, or `source ...`)',
        placeHolder: 'source /path/to/trick_env.sh',
        value: lastCommand,
        ignoreFocusOut: true,
      });
      if (!userCommand) {
        return;
      }
      void context.workspaceState.update(LAST_ENV_COMMAND_KEY, userCommand);

      const cwd = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath;
      let diff: Record<string, string>;
      try {
        diff = await vscode.window.withProgress(
          {
            location: vscode.ProgressLocation.Notification,
            title: 'Trick: running shell command to capture environment…',
            cancellable: false,
          },
          async () => {
            const captured = await captureShellEnv(userCommand, { cwd });
            return diffEnv(process.env, captured);
          }
        );
      } catch (err) {
        const message = err instanceof ShellEnvCaptureError ? err.message : String(err);
        output.appendLine(`[update-environment] ${userCommand}\n${message}`);
        const choice = await vscode.window.showErrorMessage(
          `Could not capture the environment from that command: ${message}`,
          'Show Output'
        );
        if (choice === 'Show Output') {
          output.show(true);
        }
        return;
      }

      const changedNames = Object.keys(diff);
      if (changedNames.length === 0) {
        vscode.window.showInformationMessage('Ran the command, but no new or changed environment variables came back.');
        return;
      }

      Object.assign(process.env, diff);
      output.appendLine(
        `[update-environment] ${userCommand}\nUpdated: ${changedNames
          .map((name) => (safeToLogValue(name) ? `${name}=${diff[name]}` : name))
          .join(', ')}`
      );

      simConfigs.invalidateAll();
      vscode.workspace.textDocuments.forEach(refreshDiagnostics);
      void updateStatusBar(vscode.window.activeTextEditor);
      commandsTreeProvider.refresh();
      void simConfigs.warmPrimarySimRoots();

      const homeNote = vscode.workspace.getConfiguration('trick').get<string>('home', '')
        ? ' (the trick.home setting still takes precedence over TRICK_HOME for resolving TRICK_HOME itself)'
        : '';
      const summary = diff.TRICK_HOME ? `TRICK_HOME = ${diff.TRICK_HOME}` : `${changedNames.length} variable(s)`;
      vscode.window.showInformationMessage(`Trick: updated environment. ${summary}.${homeNote}`);
    })
  );

  context.subscriptions.push(
    vscode.tasks.registerTaskProvider(TRICK_TASK_TYPE, new TrickTaskProvider(simConfigs))
  );

  // Trick activity bar: discovery (replacing Sim Sniffer), live variable
  // watch (replacing Trick View), and run/freeze/stop (replacing Sim Control
  // Panel), across two native TreeViews - "Sims" (available, from discovery)
  // and "Connected" - rather than separate windows. Splitting connected sims
  // into their own view means a sim you've connected to doesn't move around
  // as other sims come and go from the available list.
  const discovery = new SimDiscovery((message) => output.appendLine(message));
  context.subscriptions.push({ dispose: () => discovery.dispose() });

  const simsTreeProvider = new TrickSimsTreeProvider(discovery, context.workspaceState, output);
  context.subscriptions.push({ dispose: () => simsTreeProvider.dispose() });
  discovery.on('changed', () => simsTreeProvider.refreshFromDiscovery());

  const connectedSimsTreeProvider = new ConnectedSimsTreeProvider(simsTreeProvider);

  const simsTreeView = vscode.window.createTreeView('trickSims', { treeDataProvider: simsTreeProvider });
  context.subscriptions.push(simsTreeView);
  const connectedSimsTreeView = vscode.window.createTreeView('trickSimsConnected', {
    treeDataProvider: connectedSimsTreeProvider,
  });
  context.subscriptions.push(connectedSimsTreeView);

  // Multicast discovery opens a socket, so it's only started once a view is
  // actually visible, rather than unconditionally on activation.
  if (simsTreeView.visible || connectedSimsTreeView.visible) {
    discovery.start();
  }
  context.subscriptions.push(
    simsTreeView.onDidChangeVisibility((e) => {
      if (e.visible) {
        discovery.start();
      }
    }),
    connectedSimsTreeView.onDidChangeVisibility((e) => {
      if (e.visible) {
        discovery.start();
      }
    })
  );

  registerSimCommands(context, simsTreeProvider, connectedSimsTreeView);

  context.subscriptions.push(
    vscode.commands.registerCommand('trick.buildCurrentSim', async () => {
      const editor = vscode.window.activeTextEditor;
      if (!editor) {
        vscode.window.showWarningMessage('Open a file inside a Trick sim directory first.');
        return;
      }
      const simRoot = simConfigs.findSimRoot(editor.document.uri.fsPath);
      if (!simRoot) {
        vscode.window.showWarningMessage('No S_define found in any parent directory of the active file.');
        return;
      }
      const folder =
        vscode.workspace.getWorkspaceFolder(editor.document.uri) ?? vscode.workspace.workspaceFolders?.[0];
      if (!folder) {
        vscode.window.showWarningMessage('No workspace folder open.');
        return;
      }
      const task = createTrickBuildTask(folder, simRoot, simConfigs.resolveTrickHome(simRoot));
      await vscode.tasks.executeTask(task);
    })
  );

  context.subscriptions.push(
    vscode.commands.registerCommand('trick.runCurrentSim', async () => {
      const editor = vscode.window.activeTextEditor;
      if (!editor) {
        vscode.window.showWarningMessage('Open a file inside a Trick sim directory first.');
        return;
      }
      const simRoot = simConfigs.findSimRoot(editor.document.uri.fsPath);
      if (!simRoot) {
        vscode.window.showWarningMessage('No S_define found in any parent directory of the active file.');
        return;
      }
      const folder =
        vscode.workspace.getWorkspaceFolder(editor.document.uri) ?? vscode.workspace.workspaceFolders?.[0];
      if (!folder) {
        vscode.window.showWarningMessage('No workspace folder open.');
        return;
      }

      const lastRunKey = `trick.run.lastInput.${simRoot}`;
      let inputFile = inputFileForActiveFile(simRoot, editor.document.uri.fsPath);
      if (!inputFile) {
        const candidates = listInputFiles(simRoot);
        if (candidates.length === 0) {
          vscode.window.showWarningMessage(`No RUN_*/*.py input files found under ${path.basename(simRoot)}.`);
          return;
        }
        if (candidates.length === 1) {
          [inputFile] = candidates;
        } else {
          const lastRun = context.workspaceState.get<string>(lastRunKey);
          const items = candidates
            .slice()
            .sort((a, b) => (a === lastRun ? -1 : b === lastRun ? 1 : a.localeCompare(b)))
            .map((file) => ({ label: file, description: file === lastRun ? 'last run' : undefined }));
          const picked = await vscode.window.showQuickPick(items, { placeHolder: 'Choose an input file to run' });
          if (!picked) {
            return;
          }
          inputFile = picked.label;
        }
      }
      void context.workspaceState.update(lastRunKey, inputFile);

      const trickHome = simConfigs.resolveTrickHome(simRoot);
      const executable = await findBuiltExecutableOrOfferBuild(simRoot, folder, trickHome);
      if (!executable) {
        return;
      }

      const task = createTrickRunTask(folder, simRoot, executable, inputFile, trickHome);
      await vscode.tasks.executeTask(task);
    })
  );
}

export function deactivate(): void {
  // Disposables registered via context.subscriptions handle cleanup.
}
