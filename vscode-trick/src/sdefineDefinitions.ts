import * as fs from 'fs';
import * as path from 'path';
import * as vscode from 'vscode';
import { INCLUDE_RE } from './sdefineLinks';
import { SimConfigProvider } from './simConfig';

/**
 * Go to Definition for identifiers in S_define/.sm files - sim object types
 * (`IHM::SimObject ihm;`), member variables, and job target methods
 * (`ihm.update()`).
 *
 * A SimObject class can only be defined in two kinds of file: the sim's own
 * `S_define`, or a `.sm` file reachable from it - either under the sim root
 * itself, or under one of the directories `TRICK_SFLAGS` resolves to (from
 * that sim's `S_overrides.mk`, same as `SimConfig.sIncludes`). So a sim
 * object type name is resolved with a direct, bounded text search over that
 * small set of files first: the current document, then `S_define`, then
 * `.sm` files under the sim root and `sIncludes`. This is both faster and
 * more correct than asking cpptools - its Tag Parser only indexes real C/C++
 * extensions, so `.sm` files (not one of them) are invisible to it, and any
 * match it does return for a sim-object class name comes from the generated
 * `S_source.hh`/`S_source_py.i` (CP's ICG output), not the file the user
 * actually wrote the class in.
 *
 * Everything else (job target methods, member variables) isn't confined to
 * S_define/.sm the same way - those still fall back to cpptools' workspace
 * symbol index, the same Tag Parser database that SimConfigProvider feeds via
 * provideBrowseConfiguration (warmed up front for sims not inside a nested
 * git repo; others join lazily on first use - see SimConfigProvider.onDidResolve).
 *
 * #include/##include lines are left to TrickIncludeLinkProvider instead.
 */
export class TrickSdefineDefinitionProvider implements vscode.DefinitionProvider, vscode.Disposable {
  private readonly smFileCache = new Map<string, string[]>();
  private readonly invalidateSub: vscode.Disposable;

  constructor(private readonly simConfigs: SimConfigProvider) {
    this.invalidateSub = this.simConfigs.onDidInvalidate((simRoot) => this.smFileCache.delete(simRoot));
  }

  dispose(): void {
    this.invalidateSub.dispose();
  }

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

    const classMatch = await this.findSimObjectClass(document, word);
    if (classMatch) {
      return classMatch;
    }
    if (token.isCancellationRequested) {
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

  /**
   * Looks for `className` as a real `class`/`struct` definition: first in the
   * document already open (so a class authored inline in S_define, as e.g.
   * trick_sims/SIM_robot does, resolves with no file search at all), then in
   * the sim's own S_define, then in `.sm` files under the sim root and
   * `sIncludes`. Returns undefined - not a guess - when none of those files
   * define it, so the caller can fall back to cpptools for identifiers this
   * can't be (job target methods, member variables, ...).
   */
  private async findSimObjectClass(
    document: vscode.TextDocument,
    className: string
  ): Promise<vscode.Location | undefined> {
    const ownLine = findClassDeclLine(document.getText(), className);
    if (ownLine !== undefined) {
      return new vscode.Location(document.uri, new vscode.Position(ownLine, 0));
    }

    const simRoot = this.simConfigs.findSimRoot(document.uri.fsPath);
    if (!simRoot) {
      return undefined;
    }

    const sDefinePath = path.join(simRoot, 'S_define');
    if (path.resolve(sDefinePath) !== path.resolve(document.uri.fsPath)) {
      const line = readAndFindClassDeclLine(sDefinePath, className);
      if (line !== undefined) {
        return new vscode.Location(vscode.Uri.file(sDefinePath), new vscode.Position(line, 0));
      }
    }

    let sIncludes: string[] = [];
    try {
      sIncludes = (await this.simConfigs.getConfig(simRoot)).sIncludes;
    } catch {
      // fall through with just the sim root itself
    }

    for (const file of await this.getSmFiles(simRoot, [simRoot, ...sIncludes])) {
      if (path.resolve(file) === path.resolve(document.uri.fsPath)) {
        continue; // already checked above
      }
      const line = readAndFindClassDeclLine(file, className);
      if (line !== undefined) {
        return new vscode.Location(vscode.Uri.file(file), new vscode.Position(line, 0));
      }
    }
    return undefined;
  }

  private async getSmFiles(cacheKey: string, dirs: string[]): Promise<string[]> {
    const cached = this.smFileCache.get(cacheKey);
    if (cached) {
      return cached;
    }
    const files: string[] = [];
    for (const dir of dirs) {
      await walkForSmFiles(dir, files);
    }
    this.smFileCache.set(cacheKey, files);
    return files;
  }
}

const SKIP_DIRS = new Set(['.git', 'node_modules', 'build']);

async function walkForSmFiles(dir: string, out: string[]): Promise<void> {
  let entries: fs.Dirent[];
  try {
    entries = await fs.promises.readdir(dir, { withFileTypes: true });
  } catch {
    return;
  }
  for (const entry of entries) {
    if (SKIP_DIRS.has(entry.name)) {
      continue;
    }
    const full = path.join(dir, entry.name);
    if (entry.isDirectory()) {
      await walkForSmFiles(full, out);
    } else if (entry.isFile() && entry.name.endsWith('.sm')) {
      out.push(full);
    }
  }
}

function readAndFindClassDeclLine(file: string, className: string): number | undefined {
  let text: string;
  try {
    text = fs.readFileSync(file, 'utf8');
  } catch {
    return undefined;
  }
  return findClassDeclLine(text, className);
}

/** Finds `class Name ... {` / `struct Name ... {` - not a forward declaration - in text. */
export function findClassDeclLine(text: string, name: string): number | undefined {
  const re = new RegExp(`\\b(?:class|struct)\\s+${escapeRegExp(name)}\\b[^{;]*\\{`);
  const m = re.exec(text);
  if (!m) {
    return undefined;
  }
  let line = 0;
  for (let i = 0; i < m.index; i++) {
    if (text.charCodeAt(i) === 10 /* \n */) {
      line++;
    }
  }
  return line;
}

function escapeRegExp(text: string): string {
  return text.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
}

// cpptools' workspace symbol names are sometimes qualified ("Class::member")
// or carry a parameter list ("update(double)"); strip both before comparing
// so a plain identifier still matches.
export function symbolMatchesWord(symbolName: string, word: string): boolean {
  const baseName = symbolName.split('(')[0].trim();
  return baseName === word || baseName.endsWith(`::${word}`);
}
