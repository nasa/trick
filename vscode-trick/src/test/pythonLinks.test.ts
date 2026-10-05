import * as assert from 'assert';
import * as fs from 'fs';
import * as os from 'os';
import * as path from 'path';

// Minimal stand-in for the `vscode` module, same approach as the other test
// files: pythonLinks.ts only touches vscode's value exports (Range,
// DocumentLink, Uri.file), never anything requiring the real extension host.
const vscodeStub = {
  Range: class {
    constructor(
      public startLine: number,
      public startCol: number,
      public endLine: number,
      public endCol: number
    ) {}
  },
  DocumentLink: class {
    constructor(
      public range: unknown,
      public target: unknown
    ) {}
  },
  Uri: {
    file: (fsPath: string) => ({ fsPath }),
  },
};

const Module = require('module');
const originalLoad = Module._load;
Module._load = function (request: string, ...rest: unknown[]) {
  if (request === 'vscode') {
    return vscodeStub;
  }
  return originalLoad.call(this, request, ...rest);
};

// eslint-disable-next-line @typescript-eslint/no-var-requires
const { TrickPythonLinkProvider } = require('../pythonLinks');

function makeDocument(lines: string[], fsPath: string) {
  return {
    uri: { fsPath },
    lineCount: lines.length,
    lineAt: (i: number) => ({ text: lines[i] }),
  };
}

describe('pythonLinks', () => {
  it('resolves an open() target relative to the sim root, not the RUN directory', () => {
    const tmpRoot = fs.mkdtempSync(path.join(os.tmpdir(), 'trick-python-links-'));
    try {
      const simRoot = path.join(tmpRoot, 'SIM_robot');
      const runDir = path.join(simRoot, 'RUN_2DPlanar');
      const modifiedDataDir = path.join(simRoot, 'Modified_data');
      fs.mkdirSync(runDir, { recursive: true });
      fs.mkdirSync(modifiedDataDir, { recursive: true });
      fs.writeFileSync(path.join(simRoot, 'S_define'), '');
      fs.writeFileSync(path.join(modifiedDataDir, 'realtime.py'), '');

      const document = makeDocument(
        ['exec(open("./Modified_data/realtime.py").read())'],
        path.join(runDir, 'input.py')
      );
      const provider = new TrickPythonLinkProvider({ findSimRoot: () => simRoot });
      const links = provider.provideDocumentLinks(document);

      assert.strictEqual(links.length, 1);
      assert.strictEqual(links[0].target.fsPath, path.join(modifiedDataDir, 'realtime.py'));
    } finally {
      fs.rmSync(tmpRoot, { recursive: true, force: true });
    }
  });

  it('skips open() calls whose target cannot be resolved anywhere', () => {
    const tmpRoot = fs.mkdtempSync(path.join(os.tmpdir(), 'trick-python-links-'));
    try {
      const document = makeDocument(['open("does_not_exist.dr")'], path.join(tmpRoot, 'input.py'));
      const provider = new TrickPythonLinkProvider({ findSimRoot: () => undefined });
      assert.strictEqual(provider.provideDocumentLinks(document).length, 0);
    } finally {
      fs.rmSync(tmpRoot, { recursive: true, force: true });
    }
  });
});
