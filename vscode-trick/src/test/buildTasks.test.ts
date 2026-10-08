import * as assert from 'assert';
import * as fs from 'fs';
import * as os from 'os';
import * as path from 'path';

const vscodeStub = {
  ShellExecution: class {
    public command?: string;
    public args?: unknown[];
    public commandLine?: string;
    public options?: unknown;
    constructor(a: string, b?: unknown[] | unknown, c?: unknown) {
      if (Array.isArray(b)) {
        this.command = a;
        this.args = b;
        this.options = c;
      } else {
        this.commandLine = a;
        this.options = b;
      }
    }
  },
  Task: class {
    group?: unknown;
    presentationOptions?: unknown;
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
const {
  buildTrickEnv,
  buildTrickRunEnv,
  createTrickBuildTask,
  createTrickRunTask,
  findSimExecutable,
  inputFileForActiveFile,
  listInputFiles,
  TRICK_BUILD_COMMAND,
} = require('../buildTasks');

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

  describe('buildTrickRunEnv', () => {
    it('adds TRICK_HOME on top of what buildTrickEnv sets', () => {
      const env = buildTrickRunEnv('/opt/trick', '/usr/bin');
      assert.deepStrictEqual(env, { PATH: `/opt/trick/bin${path.delimiter}/usr/bin`, TRICK_HOME: '/opt/trick' });
    });

    it('returns undefined when TRICK_HOME could not be resolved', () => {
      assert.strictEqual(buildTrickRunEnv(undefined, '/usr/bin'), undefined);
    });
  });

  describe('createTrickRunTask', () => {
    it('builds a ShellExecution running the given executable and input file in the sim root', () => {
      const folder = { name: 'ws', uri: { fsPath: '/ws' } };
      const task = createTrickRunTask(
        folder,
        '/ws/trick_sims/SIM_robot',
        'S_main_Linux_13.3_x86_64.exe',
        'RUN_test/input.py',
        '/opt/trick'
      );

      assert.strictEqual(task.execution.command, './S_main_Linux_13.3_x86_64.exe');
      assert.deepStrictEqual(task.execution.args, ['RUN_test/input.py']);
      assert.strictEqual(task.execution.options.cwd, '/ws/trick_sims/SIM_robot');
      assert.strictEqual(task.execution.options.env.TRICK_HOME, '/opt/trick');
      assert.deepStrictEqual(task.definition, {
        type: 'trick',
        simRoot: '/ws/trick_sims/SIM_robot',
        inputFile: 'RUN_test/input.py',
      });
      assert.strictEqual(task.problemMatchers, undefined);
      assert.strictEqual(task.group, undefined);
    });
  });

  describe('findSimExecutable, listInputFiles, inputFileForActiveFile', () => {
    let simRoot: string;

    beforeEach(() => {
      simRoot = fs.mkdtempSync(path.join(os.tmpdir(), 'trick-run-test-'));
    });

    afterEach(() => {
      fs.rmSync(simRoot, { recursive: true, force: true });
    });

    it('findSimExecutable returns undefined when nothing has been built', () => {
      assert.strictEqual(findSimExecutable(simRoot), undefined);
    });

    it('findSimExecutable finds the single S_main_*.exe', () => {
      fs.writeFileSync(path.join(simRoot, 'S_main_Linux_13.3_x86_64.exe'), '');
      assert.strictEqual(findSimExecutable(simRoot), 'S_main_Linux_13.3_x86_64.exe');
    });

    it('findSimExecutable picks the most recently built one when more than one exists', () => {
      const older = path.join(simRoot, 'S_main_Linux_12.0_x86_64.exe');
      const newer = path.join(simRoot, 'S_main_Linux_13.3_x86_64.exe');
      fs.writeFileSync(older, '');
      const past = new Date(Date.now() - 60000);
      fs.utimesSync(older, past, past);
      fs.writeFileSync(newer, '');
      assert.strictEqual(findSimExecutable(simRoot), 'S_main_Linux_13.3_x86_64.exe');
    });

    it('listInputFiles finds RUN_*/*.py across multiple run directories, ignoring non-RUN_ dirs and stray files', () => {
      fs.mkdirSync(path.join(simRoot, 'RUN_test'));
      fs.writeFileSync(path.join(simRoot, 'RUN_test', 'input.py'), '');
      fs.writeFileSync(path.join(simRoot, 'RUN_test', 'unit_test.py'), '');
      fs.mkdirSync(path.join(simRoot, 'RUN_graphics'));
      fs.writeFileSync(path.join(simRoot, 'RUN_graphics', 'input.py'), '');
      fs.mkdirSync(path.join(simRoot, 'build'));
      fs.writeFileSync(path.join(simRoot, 'build', 'input.py'), '');
      fs.writeFileSync(path.join(simRoot, 'stray.py'), '');

      assert.deepStrictEqual(listInputFiles(simRoot), [
        'RUN_graphics/input.py',
        'RUN_test/input.py',
        'RUN_test/unit_test.py',
      ]);
    });

    it('listInputFiles returns an empty array when there are no RUN_ directories', () => {
      assert.deepStrictEqual(listInputFiles(simRoot), []);
    });

    it('inputFileForActiveFile matches a file inside a RUN_ directory', () => {
      const fsPath = path.join(simRoot, 'RUN_test', 'input.py');
      assert.strictEqual(inputFileForActiveFile(simRoot, fsPath), 'RUN_test/input.py');
    });

    it('inputFileForActiveFile returns undefined for a file outside any RUN_ directory', () => {
      assert.strictEqual(inputFileForActiveFile(simRoot, path.join(simRoot, 'S_define')), undefined);
    });

    it('inputFileForActiveFile returns undefined for a file nested below a RUN_ directory', () => {
      assert.strictEqual(
        inputFileForActiveFile(simRoot, path.join(simRoot, 'RUN_test', 'sub', 'input.py')),
        undefined
      );
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
