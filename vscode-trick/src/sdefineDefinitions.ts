import * as vscode from 'vscode';
import { INCLUDE_RE } from './sdefineLinks';

/**
 * Go to Definition for identifiers in S_define/.sm files - sim object types
 * (`IHM::SimObject ihm;`), member variables, and job target methods
 * (`ihm.update()`).
 *
 * S_define bodies are ordinary C++, but they're parsed under the custom
 * `trick-sdefine` language ID rather than `cpp`, so the C/C++ extension never
 * registers its own Definition provider for them. Rather than re-parsing C++
 * ourselves, this delegates to cpptools' existing workspace symbol index -
 * the same Tag Parser database that SimConfigProvider.warmAllSimRoots() feeds
 * via provideBrowseConfiguration, and that already powers "Go to Symbol in
 * Workspace". Unlike Definition providers, workspace symbol providers aren't
 * scoped to the querying document's language, so it can be reused as-is.
 *
 * #include/##include lines are left to TrickIncludeLinkProvider instead.
 */
export class TrickSdefineDefinitionProvider implements vscode.DefinitionProvider {
  async provideDefinition(
    document: vscode.TextDocument,
    position: vscode.Position,
    token: vscode.CancellationToken
  ): Promise<vscode.Definition | undefined> {
    const line = document.lineAt(position.line).text;
    if (INCLUDE_RE.test(line)) {
      return undefined;
    }

    const wordRange = document.getWordRangeAtPosition(position);
    if (!wordRange) {
      return undefined;
    }
    const word = document.getText(wordRange);
    if (!word || /^\d/.test(word)) {
      return undefined;
    }

    let symbols: vscode.SymbolInformation[] | undefined;
    try {
      symbols = await vscode.commands.executeCommand<vscode.SymbolInformation[]>(
        'vscode.executeWorkspaceSymbolProvider',
        word
      );
    } catch {
      return undefined;
    }
    if (token.isCancellationRequested || !symbols?.length) {
      return undefined;
    }

    const matches = symbols.filter((s) => symbolMatchesWord(s.name, word));
    if (matches.length === 0) {
      return undefined;
    }
    return matches.map((s) => new vscode.Location(s.location.uri, s.location.range));
  }
}

// cpptools' workspace symbol names are sometimes qualified ("Class::member")
// or carry a parameter list ("update(double)"); strip both before comparing
// so a plain identifier still matches.
export function symbolMatchesWord(symbolName: string, word: string): boolean {
  const baseName = symbolName.split('(')[0].trim();
  return baseName === word || baseName.endsWith(`::${word}`);
}
