import * as vscode from 'vscode';
import { SimConfigProvider } from './simConfig';
import { TrickCppConfigurationProvider } from './cpptoolsProvider';
import { TrickIncludeLinkProvider, TrickIncludeDiagnostics } from './sdefineLinks';

export async function activate(context: vscode.ExtensionContext): Promise<void> {
  const output = vscode.window.createOutputChannel('Trick');
  context.subscriptions.push(output);

  const simConfigs = new SimConfigProvider(output);
  context.subscriptions.push(simConfigs);

  const cppProvider = new TrickCppConfigurationProvider(simConfigs, output);
  context.subscriptions.push(cppProvider);
  await cppProvider.activate();

  // Warm every sim's config up front so cpptools' Tag Parser has a complete
  // browse path from the start, letting it resolve cross-file navigation
  // (e.g. header method declaration -> .cpp implementation) without first
  // requiring every file to be opened manually.
  void simConfigs.warmAllSimRoots().then(() => {
    cppProvider.notifyBrowseConfigurationChanged();
    output.appendLine(`Warmed ${simConfigs.getAllCached().length} sim configuration(s) for IntelliSense browsing.`);
  });

  const linkProvider = new TrickIncludeLinkProvider(simConfigs);
  context.subscriptions.push(
    vscode.languages.registerDocumentLinkProvider({ language: 'trick-sdefine' }, linkProvider)
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
}

export function deactivate(): void {
  // Disposables registered via context.subscriptions handle cleanup.
}
