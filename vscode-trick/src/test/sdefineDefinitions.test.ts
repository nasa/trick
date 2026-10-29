import * as assert from 'assert';
import * as fs from 'fs';
import * as path from 'path';

// Minimal stand-in for the `vscode` module, same pattern as pythonDefinitions.test.ts:
// the pure helpers exercised here never touch vscode.
const vscodeStub = {};

const Module = require('module');
const originalLoad = Module._load;
Module._load = function (request: string, ...rest: unknown[]) {
  if (request === 'vscode') {
    return vscodeStub;
  }
  return originalLoad.call(this, request, ...rest);
};

// eslint-disable-next-line @typescript-eslint/no-var-requires
const { findClassDeclLine, symbolMatchesWord } = require('../sdefineDefinitions');

const REPO_ROOT = path.resolve(__dirname, '../../..');
const BALL_SDEFINE = path.join(REPO_ROOT, 'trick_sims/Ball/SIM_ball_L1/S_define');

describe('findClassDeclLine', () => {
  it('finds a sim object class defined inline in S_define (Ball)', () => {
    const text = fs.readFileSync(BALL_SDEFINE, 'utf8');
    const line = findClassDeclLine(text, 'ballSimObject');
    assert.notStrictEqual(line, undefined);
    assert.ok(text.split('\n')[line as number].includes('class ballSimObject'));
  });

  it('finds a class defined in a .sm-style file', () => {
    const text = '// comment\nclass FooSimObject : public Trick::SimObject {\npublic:\n  int x;\n};\n';
    assert.strictEqual(findClassDeclLine(text, 'FooSimObject'), 1);
  });

  it('ignores a forward declaration (no body)', () => {
    const text = 'class FooSimObject;\n';
    assert.strictEqual(findClassDeclLine(text, 'FooSimObject'), undefined);
  });

  it('returns undefined when the class is not present', () => {
    const text = 'class SomethingElse {\n};\n';
    assert.strictEqual(findClassDeclLine(text, 'Missing'), undefined);
  });

  it('matches struct as well as class', () => {
    const text = 'struct FooState {\n  double x;\n};\n';
    assert.strictEqual(findClassDeclLine(text, 'FooState'), 0);
  });
});

describe('symbolMatchesWord', () => {
  it('matches a plain identifier', () => {
    assert.ok(symbolMatchesWord('update', 'update'));
  });

  it('matches a qualified name against its base', () => {
    assert.ok(symbolMatchesWord('IHM::update', 'update'));
  });

  it('matches a name with a parameter list', () => {
    assert.ok(symbolMatchesWord('update(double)', 'update'));
  });

  it('rejects an unrelated name', () => {
    assert.strictEqual(symbolMatchesWord('otherMethod', 'update'), false);
  });
});
