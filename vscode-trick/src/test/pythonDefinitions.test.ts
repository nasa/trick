import * as assert from 'assert';
import * as fs from 'fs';
import * as path from 'path';

// Minimal stand-in for the `vscode` module, same pattern as pythonStubs.test.ts:
// pythonDefinitions.ts's pure helpers (the only things exercised here) never
// touch vscode.
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
const {
  parseAccessChain,
  chainTextAt,
  parseClassMap,
  resolveSimChain,
  findDeclarationLine,
  findTopLevelObjectLine,
  findTrickDeclaration,
} = require('../pythonDefinitions');
// eslint-disable-next-line @typescript-eslint/no-var-requires
const { parseSieResource } = require('../sieResource');

const REPO_ROOT = path.resolve(__dirname, '../../..');
const BALL_ROOT = path.join(REPO_ROOT, 'trick_sims/Ball/SIM_ball_L1');
const BALL_SIE = path.join(BALL_ROOT, 'S_sie.resource');
const BALL_CLASS_MAP = path.join(BALL_ROOT, 'build/class_map.cpp');
const BALL_SDEFINE = path.join(BALL_ROOT, 'S_define');
const BALL_STATE_H = path.join(REPO_ROOT, 'trick_sims/Ball/models/ball/L1/include/ball_state.h');
const EXEC_PROTO_H = path.join(REPO_ROOT, 'include/trick/exec_proto.h');
const DRASCII_HH = path.join(REPO_ROOT, 'include/trick/DRAscii.hh');
const DATA_RECORD_GROUP_HH = path.join(REPO_ROOT, 'include/trick/DataRecordGroup.hh');

describe('pythonDefinitions', () => {
  describe('parseAccessChain', () => {
    it('returns the full chain for a plain dotted expression', () => {
      const line = 'ball.state.input.mass = 10.0';
      const word = 'mass';
      const start = line.indexOf(word);
      assert.deepStrictEqual(parseAccessChain(line, start, start + word.length), [
        'ball',
        'state',
        'input',
        'mass',
      ]);
    });

    it('includes an indexed segment in the chain', () => {
      const line = 'ball.state.output.position[0] = 1.0';
      const word = 'position';
      const start = line.indexOf(word);
      assert.deepStrictEqual(parseAccessChain(line, start, start + word.length), [
        'ball',
        'state',
        'output',
        'position',
      ]);
    });

    it('truncates the chain at a clicked middle segment', () => {
      const line = 'ball.state.input.mass = 10.0';
      const word = 'state';
      const start = line.indexOf(word);
      assert.deepStrictEqual(parseAccessChain(line, start, start + word.length), ['ball', 'state']);
    });

    it('resolves a chain written inside a string literal, as in .dr files', () => {
      const line = 'drg0.add_variable("ball.state.output.position[0]", "ball.state.out.position[0]")';
      const word = 'position';
      const start = line.indexOf(word);
      assert.deepStrictEqual(parseAccessChain(line, start, start + word.length), [
        'ball',
        'state',
        'output',
        'position',
      ]);
    });

    it('returns undefined for a bare word with no chain', () => {
      const line = 'trick.exec_set_terminate_time(300.0)';
      const word = 'trick';
      const start = line.indexOf(word);
      assert.deepStrictEqual(parseAccessChain(line, start, start + word.length), ['trick']);
    });
  });

  describe('chainTextAt', () => {
    it('keeps the array index, unlike parseAccessChain', () => {
      const line = 'ball.state.output.position[0] = 1.0';
      const word = 'position';
      const start = line.indexOf(word);
      assert.strictEqual(chainTextAt(line, start), 'ball.state.output.position[0]');
    });

    it('works from any column within the chain, not just its start', () => {
      const line = 'ball.state.output.position[0] = 1.0';
      assert.strictEqual(chainTextAt(line, line.indexOf('output')), 'ball.state.output.position[0]');
    });

    it('strips internal whitespace around dots/brackets', () => {
      const line = 'ball . state .position[ 0 ]';
      assert.strictEqual(chainTextAt(line, 0), 'ball.state.position[0]');
    });

    it('returns undefined outside any chain', () => {
      const line = '# just a comment';
      assert.strictEqual(chainTextAt(line, 0), undefined);
    });
  });

  describe('parseClassMap', () => {
    it('maps a sim-local class to its header, from a real built class_map.cpp', function () {
      if (!fs.existsSync(BALL_CLASS_MAP)) {
        this.skip();
      }
      const map = parseClassMap(fs.readFileSync(BALL_CLASS_MAP, 'utf8'));
      assert.ok(map.get('BSTATE_IN')?.endsWith('ball_state.h'));
    });

    it('maps the sim-object class to the generated S_source.hh', function () {
      if (!fs.existsSync(BALL_CLASS_MAP)) {
        this.skip();
      }
      const map = parseClassMap(fs.readFileSync(BALL_CLASS_MAP, 'utf8'));
      assert.ok(map.get('ballSimObject')?.endsWith('S_source.hh'));
    });
  });

  describe('resolveSimChain', () => {
    it('resolves a nested member chain against a real built Ball sim', function () {
      if (!fs.existsSync(BALL_SIE)) {
        this.skip();
      }
      const model = parseSieResource(fs.readFileSync(BALL_SIE, 'utf8'));
      const target = resolveSimChain(model, ['ball', 'state', 'input', 'mass']);
      assert.deepStrictEqual(target, { className: 'BSTATE_IN', memberName: 'mass' });
    });

    it('resolves a bare top-level object to its class with no member', function () {
      if (!fs.existsSync(BALL_SIE)) {
        this.skip();
      }
      const model = parseSieResource(fs.readFileSync(BALL_SIE, 'utf8'));
      assert.deepStrictEqual(resolveSimChain(model, ['ball']), { className: 'ballSimObject' });
    });

    it('returns undefined for an unknown root', () => {
      const model = { classes: new Map(), enums: new Map(), topLevel: [] };
      assert.strictEqual(resolveSimChain(model, ['nope', 'x']), undefined);
    });
  });

  describe('findDeclarationLine', () => {
    it('finds a member inside a real C-style typedef struct (ball_state.h)', function () {
      if (!fs.existsSync(BALL_STATE_H)) {
        this.skip();
      }
      const text = fs.readFileSync(BALL_STATE_H, 'utf8');
      const match = findDeclarationLine(text, 'BSTATE_IN', 'mass');
      assert.strictEqual(match.matchedMember, true);
      assert.strictEqual(text.split('\n')[match.line].trim().startsWith('double mass'), true);
    });

    it('finds a real class declaration line (no member requested)', function () {
      if (!fs.existsSync(BALL_SDEFINE)) {
        this.skip();
      }
      const text = fs.readFileSync(BALL_SDEFINE, 'utf8');
      const match = findDeclarationLine(text, 'ballSimObject');
      assert.strictEqual(text.split('\n')[match.line].includes('class ballSimObject'), true);
    });

    it('finds a member of a namespaced (mangled) class', () => {
      const text = ['namespace Trick {', '  class Thing {', '    public:', '      int count ;', '  } ;', '}'].join(
        '\n'
      );
      const match = findDeclarationLine(text, 'Trick__Thing', 'count');
      assert.strictEqual(match.matchedMember, true);
      assert.strictEqual(text.split('\n')[match.line].includes('int count'), true);
    });

    it('finds a member inside a synthetic typedef struct', () => {
      const text = ['typedef struct {', '  double speed ;', '} MY_STRUCT ;'].join('\n');
      const match = findDeclarationLine(text, 'MY_STRUCT', 'speed');
      assert.strictEqual(match.matchedMember, true);
      assert.strictEqual(text.split('\n')[match.line].includes('double speed'), true);
    });

    it('falls back to the class line when the member is not in its own region', () => {
      const text = ['class Base {', '  public:', '    int inherited ;', '} ;'].join('\n');
      const match = findDeclarationLine(text, 'Base', 'notThere');
      assert.strictEqual(match.matchedMember, false);
      assert.strictEqual(text.split('\n')[match.line].includes('class Base'), true);
    });

    it('falls back to line 0 when the class itself is not found', () => {
      const match = findDeclarationLine('int x ;', 'Missing', 'y');
      assert.deepStrictEqual(match, { line: 0, matchedMember: false });
    });
  });

  describe('findTopLevelObjectLine', () => {
    it('finds a real sim object declaration in S_define', function () {
      if (!fs.existsSync(BALL_SDEFINE)) {
        this.skip();
      }
      const text = fs.readFileSync(BALL_SDEFINE, 'utf8');
      const line = findTopLevelObjectLine(text, 'ball');
      assert.strictEqual(text.split('\n')[line].trim(), 'ballSimObject ball ;');
    });
  });

  describe('findTrickDeclaration', () => {
    it('finds a real function prototype in exec_proto.h', function () {
      if (!fs.existsSync(EXEC_PROTO_H)) {
        this.skip();
      }
      const files = new Map([[EXEC_PROTO_H, fs.readFileSync(EXEC_PROTO_H, 'utf8')]]);
      const found = findTrickDeclaration('exec_set_terminate_time', files);
      assert.strictEqual(found?.file, EXEC_PROTO_H);
      assert.strictEqual(
        fs.readFileSync(EXEC_PROTO_H, 'utf8').split('\n')[found.line].includes('exec_set_terminate_time'),
        true
      );
    });

    it('prefers the constructor prototype over the class line in DRAscii.hh (since that\'s what `trick.DRAscii(...)` actually calls)', function () {
      if (!fs.existsSync(DRASCII_HH)) {
        this.skip();
      }
      const files = new Map([[DRASCII_HH, fs.readFileSync(DRASCII_HH, 'utf8')]]);
      const found = findTrickDeclaration('DRAscii', files);
      assert.strictEqual(found?.file, DRASCII_HH);
      assert.strictEqual(
        fs.readFileSync(DRASCII_HH, 'utf8').split('\n')[found!.line].includes('DRAscii('),
        true
      );
    });

    it('finds a real enumerator in DataRecordGroup.hh, ignoring a prose mention elsewhere', function () {
      if (!fs.existsSync(DATA_RECORD_GROUP_HH)) {
        this.skip();
      }
      const files = new Map([[DATA_RECORD_GROUP_HH, fs.readFileSync(DATA_RECORD_GROUP_HH, 'utf8')]]);
      const found = findTrickDeclaration('DR_Always', files);
      assert.strictEqual(
        fs.readFileSync(DATA_RECORD_GROUP_HH, 'utf8').split('\n')[found!.line].trim().startsWith('DR_Always'),
        true
      );
    });

    it('returns undefined for a name that only exists in Python', () => {
      const files = new Map([['dummy.hh', 'class Something {} ;']]);
      assert.strictEqual(findTrickDeclaration('var_get', files), undefined);
    });
  });
});
