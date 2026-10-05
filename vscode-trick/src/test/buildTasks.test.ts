import * as assert from 'assert';
import * as path from 'path';

const vscodeStub = {
  ShellExecution: class {
    constructor(
      public commandLine: string,
      public options?: unknown
    ) {}
  },
  Task: class {
    group?: unknown;
    constructor(
      public definition: unknown,
      public scope: unknown,
      public name: string,
      public source: string,
      public execution: unknown,
      public problemMatchers?: string[]
    ) {}
  },
  TaskGroup: { Build: 'build' },
  RelativePattern: class {
    constructor(
      public base: unknown,
      public pattern: string
    ) {}
  },
  workspace: {
    workspaceFolders: [] as unknown[],
    findFiles: async () => [],
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
const { buildTrickEnv, createTrickBuildTask, TRICK_BUILD_COMMAND } = require('../buildTasks');

describe('buildTasks', () => {
  describe('buildTrickEnv', () => {
    it('prepends TRICK_HOME/bin to the existing PATH', () => {
      const env = buildTrickEnv('/opt/trick', '/usr/bin:/bin');
      assert.deepStrictEqual(env, { PATH: `/opt/trick/bin${path.delimiter}/usr/bin:/bin` });
    });

    it('returns undefined when TRICK_HOME could not be resolved, leaving PATH untouched', () => {
      assert.strictEqual(buildTrickEnv(undefined, '/usr/bin'), undefined);
    });
  });

  describe('createTrickBuildTask', () => {
    it('builds a ShellExecution running trick-CP in the sim root, with the $trick problem matcher', () => {
      const folder = { name: 'ws', uri: { fsPath: '/ws' } };
      const task = createTrickBuildTask(folder, '/ws/trick_sims/SIM_robot', '/opt/trick');

      assert.strictEqual(task.execution.commandLine, TRICK_BUILD_COMMAND);
      assert.strictEqual(task.execution.options.cwd, '/ws/trick_sims/SIM_robot');
      assert.deepStrictEqual(task.definition, { type: 'trick', simRoot: '/ws/trick_sims/SIM_robot' });
      assert.deepStrictEqual(task.problemMatchers, ['$trick']);
      assert.strictEqual(task.group, 'build');
    });
  });
});

describe('the $trick problem matcher pattern (package.json)', () => {
  const packageJson = require('../../package.json');
  const matcher = packageJson.contributes.problemMatchers.find((m: { name: string }) => m.name === 'trick');
  const regexp = new RegExp(matcher.pattern.regexp);

  it('matches a trick-ICG Clang-style diagnostic', () => {
    const line = '/sims/SIM_robot/models/src/Manip2D.cc:42:10: error: expected \';\' after class';
    const m = regexp.exec(line);
    assert.ok(m);
    assert.strictEqual(m![matcher.pattern.file], '/sims/SIM_robot/models/src/Manip2D.cc');
    assert.strictEqual(m![matcher.pattern.line], '42');
    assert.strictEqual(m![matcher.pattern.column], '10');
    assert.strictEqual(m![matcher.pattern.severity], 'error');
    assert.strictEqual(m![matcher.pattern.message], "expected ';' after class");
  });

  it('matches a plain compiler error passed through unfiltered by make', () => {
    const line = "rocket/src/Rocket.cpp:42:10: error: 'foo' was not declared in this scope";
    const m = regexp.exec(line);
    assert.ok(m);
    assert.strictEqual(m![matcher.pattern.file], 'rocket/src/Rocket.cpp');
    assert.strictEqual(m![matcher.pattern.severity], 'error');
  });

  it('does not match an unrelated line of build output', () => {
    assert.strictEqual(regexp.exec('Generating object file list ...'), null);
  });
});
