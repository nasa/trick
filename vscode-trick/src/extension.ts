import * as vscode from 'vscode';
import { SimConfigProvider } from './simConfig';
import { TrickCppConfigurationProvider } from './cpptoolsProvider';
import { TrickIncludeLinkProvider, TrickIncludeDiagnostics } from './sdefineLinks';
import { TrickSdefineDefinitionProvider } from './sdefineDefinitions';
import { TrickPythonDefinitionProvider } from './pythonDefinitions';
import { PythonStubManager } from './pythonStubs';
import { TrickPythonLinkProvider } from './pythonLinks';
import { TRICK_TASK_TYPE, TrickTaskProvider, createTrickBuildTask } from './buildTasks';

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
  void simConfigs.warmPrimarySimRoots();

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
  context.subscriptions.push(
    vscode.window.onDidChangeActiveTextEditor(updateStatusBar),
    simConfigs.onDidInvalidate(() => updateStatusBar(vscode.window.activeTextEditor))
  );
  void updateStatusBar(vscode.window.activeTextEditor);

  context.subscriptions.push(
    vscode.commands.registerCommand('trick.refreshSimConfiguration', () => {
      simConfigs.clearAll();
      vscode.workspace.textDocuments.forEach(refreshDiagnostics);
      void updateStatusBar(vscode.window.activeTextEditor);
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

  context.subscriptions.push(
    vscode.tasks.registerTaskProvider(TRICK_TASK_TYPE, new TrickTaskProvider(simConfigs))
  );

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
}

export function deactivate(): void {
  // Disposables registered via context.subscriptions handle cleanup.
}
