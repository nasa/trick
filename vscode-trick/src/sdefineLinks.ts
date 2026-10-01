import * as fs from 'fs';
import * as path from 'path';
import * as vscode from 'vscode';
import { SimConfig, SimConfigProvider } from './simConfig';

const INCLUDE_RE = /^(\s*)(##?)include\s+"([^"]+)"/;

interface ParsedInclude {
  line: number;
  startCol: number;
  endCol: number;
  hashes: '#' | '##';
  target: string;
}

function parseIncludes(document: vscode.TextDocument): ParsedInclude[] {
  const results: ParsedInclude[] = [];
  for (let line = 0; line < document.lineCount; line++) {
    const text = document.lineAt(line).text;
    const m = INCLUDE_RE.exec(text);
    if (!m) {
      continue;
    }
    const [, , hashes, target] = m;
    const quoteStart = text.indexOf('"' + target + '"');
    results.push({
      line,
      startCol: quoteStart + 1,
      endCol: quoteStart + 1 + target.length,
      hashes: hashes as '#' | '##',
      target,
    });
  }
  return results;
}

function resolveTarget(
  target: string,
  documentDir: string,
  hashes: '#' | '##',
  config: SimConfig | undefined
): string | undefined {
  const candidateDirs: string[] = [documentDir];
  if (config) {
    candidateDirs.push(...(hashes === '#' ? config.sIncludes : config.cxxIncludes));
  }
  for (const dir of candidateDirs) {
    const candidate = path.resolve(dir, target);
    if (fs.existsSync(candidate) && fs.statSync(candidate).isFile()) {
      return candidate;
    }
  }
  return undefined;
}

export class TrickIncludeLinkProvider implements vscode.DocumentLinkProvider {
  constructor(private readonly simConfigs: SimConfigProvider) {}

  async provideDocumentLinks(document: vscode.TextDocument): Promise<vscode.DocumentLink[]> {
    const documentDir = path.dirname(document.uri.fsPath);
    const config = await this.simConfigs.getConfigForFile(document.uri.fsPath);

    const links: vscode.DocumentLink[] = [];
    for (const inc of parseIncludes(document)) {
      const resolved = resolveTarget(inc.target, documentDir, inc.hashes, config);
      if (!resolved) {
        continue;
      }
      const range = new vscode.Range(inc.line, inc.startCol, inc.line, inc.endCol);
      links.push(new vscode.DocumentLink(range, vscode.Uri.file(resolved)));
    }
    return links;
  }
}

export class TrickIncludeDiagnostics implements vscode.Disposable {
  private readonly collection: vscode.DiagnosticCollection;

  constructor(private readonly simConfigs: SimConfigProvider) {
    this.collection = vscode.languages.createDiagnosticCollection('trick');
  }

  dispose(): void {
    this.collection.dispose();
  }

  async refresh(document: vscode.TextDocument): Promise<void> {
    if (document.languageId !== 'trick-sdefine') {
      return;
    }
    const documentDir = path.dirname(document.uri.fsPath);
    const config = await this.simConfigs.getConfigForFile(document.uri.fsPath);

    const diagnostics: vscode.Diagnostic[] = [];
    for (const inc of parseIncludes(document)) {
      const resolved = resolveTarget(inc.target, documentDir, inc.hashes, config);
      if (resolved) {
        continue;
      }
      const range = new vscode.Range(inc.line, inc.startCol, inc.line, inc.endCol);
      const flagVar = inc.hashes === '#' ? 'TRICK_SFLAGS' : 'TRICK_CFLAGS/TRICK_CXXFLAGS';
      const diagnostic = new vscode.Diagnostic(
        range,
        `Cannot resolve ${inc.hashes}include "${inc.target}" - not found relative to this file or in ${flagVar} search paths from S_overrides.mk.`,
        vscode.DiagnosticSeverity.Warning
      );
      diagnostic.source = 'trick';
      diagnostics.push(diagnostic);
    }
    this.collection.set(document.uri, diagnostics);
  }

  clear(document: vscode.TextDocument): void {
    this.collection.delete(document.uri);
  }
}
