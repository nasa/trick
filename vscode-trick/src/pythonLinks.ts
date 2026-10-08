import * as fs from 'fs';
import * as path from 'path';
import * as vscode from 'vscode';
import { SimConfigProvider } from './simConfig';

const OPEN_CALL_RE = /open\(\s*(['"])([^'"]+)\1/g;

interface ParsedOpenCall {
  line: number;
  startCol: number;
  endCol: number;
  target: string;
}

function parseOpenCalls(document: vscode.TextDocument): ParsedOpenCall[] {
  const results: ParsedOpenCall[] = [];
  for (let line = 0; line < document.lineCount; line++) {
    const text = document.lineAt(line).text;
    OPEN_CALL_RE.lastIndex = 0;
    let m: RegExpExecArray | null;
    while ((m = OPEN_CALL_RE.exec(text))) {
      const target = m[2];
      const endCol = m.index + m[0].length - 1;
      results.push({ line, startCol: endCol - target.length, endCol, target });
    }
  }
  return results;
}

/**
 * Trick input files run with the sim root (the directory containing
 * S_define) as the working directory, not the RUN_x directory input.py
 * itself lives in - e.g. SIM_robot/RUN_2DPlanar/input.py's
 * `open("Modified_data/realtime.py")` resolves against SIM_robot/, not
 * SIM_robot/RUN_2DPlanar/. Falls back to resolving relative to the
 * open()-ing file itself, for files like Modified_data/*.dr that chain-load
 * siblings via a relative path.
 */
function resolveOpenTarget(
  target: string,
  documentDir: string,
  simRoot: string | undefined
): string | undefined {
  const candidateDirs = simRoot ? [simRoot, documentDir] : [documentDir];
  for (const dir of candidateDirs) {
    const candidate = path.resolve(dir, target);
    if (fs.existsSync(candidate) && fs.statSync(candidate).isFile()) {
      return candidate;
    }
  }
  return undefined;
}

export class TrickPythonLinkProvider implements vscode.DocumentLinkProvider {
  constructor(private readonly simConfigs: SimConfigProvider) {}

  provideDocumentLinks(document: vscode.TextDocument): vscode.DocumentLink[] {
    const documentDir = path.dirname(document.uri.fsPath);
    const simRoot = this.simConfigs.findSimRoot(document.uri.fsPath);

    const links: vscode.DocumentLink[] = [];
    for (const call of parseOpenCalls(document)) {
      const resolved = resolveOpenTarget(call.target, documentDir, simRoot);
      if (!resolved) {
        continue;
      }
      const range = new vscode.Range(call.line, call.startCol, call.line, call.endCol);
      links.push(new vscode.DocumentLink(range, vscode.Uri.file(resolved)));
    }
    return links;
  }
}
