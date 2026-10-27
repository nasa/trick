import * as assert from 'assert';
import * as cp from 'child_process';
import * as fs from 'fs';
import * as path from 'path';

// Minimal stand-in for the `vscode` module, same approach as simConfig.test.ts:
// pythonStubs.ts's module-level code never touches vscode (only inside class
// methods, which these tests don't exercise), so this just has to satisfy
// `require('vscode')` without throwing.
const vscodeStub = {
  workspace: {
    getConfiguration: () => ({
      get: (_key: string, def?: unknown) => def,
    }),
    createFileSystemWatcher: () => ({
      onDidChange: () => undefined,
      onDidCreate: () => undefined,
      onDidDelete: () => undefined,
      dispose: () => undefined,
    }),
  },
  EventEmitter: class {
    event = () => ({ dispose: () => undefined });
    fire(): void {}
    dispose(): void {}
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
const { generateTrickStub, parseSimObjectNames, generateBuiltinsStub } = require('../pythonStubs');

const REPO_ROOT = path.resolve(__dirname, '../../..');
const CURATED_PATH = path.join(__dirname, '..', '..', 'python', 'trick-curated.pyi');
const CURATED_TEXT = fs.readFileSync(CURATED_PATH, 'utf8');
const SIM_SERVICES_PATH = path.join(REPO_ROOT, 'share/trick/swig/sim_services.py');
const SHORTCUTS_PATH = path.join(REPO_ROOT, 'share/trick/swig/shortcuts.py');

function assertValidPython(source: string): void {
  const result = cp.spawnSync('python3', ['-c', 'import ast,sys; ast.parse(sys.stdin.read())'], {
    input: source,
    encoding: 'utf8',
  });
  assert.strictEqual(
    result.status,
    0,
    `generated stub is not valid Python:\n${result.stderr}\n---\n${source}`
  );
}

describe('pythonStubs', () => {
  describe('generateTrickStub', () => {
    it('falls back to curated-only when sim_services.py is unavailable', () => {
      const stub = generateTrickStub(undefined, undefined, CURATED_TEXT);
      assert.ok(stub.includes('def exec_set_terminate_time('));
      assert.ok(stub.includes('class DRAscii(DataRecordGroup):'));
      assert.ok(stub.includes('def __getattr__(name: str) -> Any: ...'));
      assertValidPython(stub);
    });

    it('scrapes functions, classes, and constants from a built sim_services.py', function () {
      if (!fs.existsSync(SIM_SERVICES_PATH)) {
        this.skip();
      }
      const simServicesText = fs.readFileSync(SIM_SERVICES_PATH, 'utf8');
      const shortcutsText = fs.existsSync(SHORTCUTS_PATH)
        ? fs.readFileSync(SHORTCUTS_PATH, 'utf8')
        : undefined;
      const stub = generateTrickStub(simServicesText, shortcutsText, CURATED_TEXT);

      // Scraped from sim_services.py, not curated.
      assert.ok(stub.includes('def exec_set_thread_enabled('));
      assert.ok(/\bDR_Always: int\b/.test(stub));
      // Curated signature wins over the generic scraped one.
      assert.ok(stub.includes('def exec_set_terminate_time(time_value: float) -> None:'));
      assert.ok(!/def exec_set_terminate_time\(time_value\) -> Any: \.\.\./.test(stub));
      // DR_Always/DR_Buffer are declared by the curated file too - must not be duplicated.
      assert.strictEqual((stub.match(/^DR_Always: int$/gm) ?? []).length, 1);

      if (shortcutsText) {
        // shortcuts.py's add_read is superseded by the curated real signature.
        assert.ok(stub.includes('def add_read(time: float, code: str) -> int:'));
        assert.ok(!/def add_read\(\*args/.test(stub));
        // var_get is only in shortcuts.py, not curated - scraped generically.
        assert.ok(stub.includes('def var_get(name) -> Any: ...'));
      }

      assertValidPython(stub);
    });
  });

  describe('parseSimObjectNames', () => {
    it('finds the sim object and its IntegLoop in SIM_rocket/S_define', () => {
      const text = fs.readFileSync(path.join(REPO_ROOT, 'trick_sims/SIM_rocket/S_define'), 'utf8');
      assert.deepStrictEqual(parseSimObjectNames(text), ['dyn', 'dyn_integloop']);
    });

    it('finds the sim object and its IntegLoop in Cannon/SIM_cannon_aero/S_define', () => {
      const text = fs.readFileSync(
        path.join(REPO_ROOT, 'trick_sims/Cannon/SIM_cannon_aero/S_define'),
        'utf8'
      );
      assert.deepStrictEqual(parseSimObjectNames(text), ['dyn', 'dyn_integloop']);
    });

    it('finds an IntegLoop whose integrand trails the cycle-time parens, as in SIM_robot/S_define', () => {
      const text = fs.readFileSync(path.join(REPO_ROOT, 'trick_sims/SIM_robot/S_define'), 'utf8');
      assert.ok(text.includes('IntegLoop armIntegLoop(0.050) Manip2D;'));
      assert.ok(parseSimObjectNames(text).includes('armIntegLoop'));
    });

    it('captures IntegLoop names but ignores job_class_order and class bodies', () => {
      const text = [
        'class Foo {',
        '  public:',
        '    int bar;',
        '};',
        '',
        'IHM::SimObject ihm;',
        'IntegLoop dyn_integloop (0.01) dyn;',
        'job_class_order {',
        '  "derivative";',
        '};',
        'void create_connections() {',
        '  int x;',
        '}',
      ].join('\n');
      assert.deepStrictEqual(parseSimObjectNames(text), ['ihm', 'dyn_integloop']);
    });
  });

  describe('generateBuiltinsStub', () => {
    it('re-exports the modules Trick pre-imports, plus each sim object name, deduplicated and sorted', () => {
      const stub = generateBuiltinsStub([{ name: 'dyn' }, { name: 'ihm' }, { name: 'dyn' }]);
      // trick/os/sys/struct/binascii are bound by IPPython's bootstrap before
      // input.py runs - re-exported (not `: Any`) so Pylance keeps real
      // completions/hover for e.g. os.path.isfile without an explicit import.
      assert.ok(stub.includes('import trick as trick'));
      assert.ok(stub.includes('import os as os'));
      assert.ok(stub.includes('import sys as sys'));
      assert.ok(stub.includes('import struct as struct'));
      assert.ok(stub.includes('import binascii as binascii'));
      assert.ok(stub.includes('dyn: Any'));
      assert.ok(stub.includes('ihm: Any'));
      assert.strictEqual((stub.match(/^dyn: Any$/gm) ?? []).length, 1);
      assertValidPython(stub);
    });

    it('declares a typed sim object with an import of its generated trick_sie module', () => {
      const stub = generateBuiltinsStub([
        { name: 'ball', typeRef: { moduleName: 'trick_sie.SIM_ball_L1', className: 'ballSimObject' } },
        { name: 'untyped' },
      ]);
      assert.ok(stub.includes('import trick_sie.SIM_ball_L1 as _sie_0'));
      assert.ok(stub.includes('ball: _sie_0.ballSimObject'));
      assert.ok(stub.includes('untyped: Any'));
      assertValidPython(stub);
    });
  });
});
