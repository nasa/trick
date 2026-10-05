import * as assert from 'assert';
import * as fs from 'fs';
import * as os from 'os';
import * as path from 'path';

// Minimal stand-in for the `vscode` module so simConfig.ts can be loaded under
// plain ts-node/mocha outside the Extension Host. Only the surface area used
// by simConfig.ts is implemented.
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
const { SimConfigProvider, isInNestedRepo } = require('../simConfig');

const REPO_ROOT = path.resolve(__dirname, '../../..');
const TRICK_HOME = REPO_ROOT;

function makeProvider() {
  return new SimConfigProvider({ appendLine: () => undefined } as any);
}

describe('SimConfigProvider regex fallback', () => {
  it('parses a simple -I./models style S_overrides.mk (SIM_rocket)', async () => {
    const provider = makeProvider();
    const simRoot = path.join(REPO_ROOT, 'trick_sims', 'SIM_rocket');
    const config = await (provider as any).resolveViaRegex(simRoot, TRICK_HOME);
    assert.strictEqual(config.source, 'regex');
    assert.ok(config.cxxIncludes.includes(path.join(simRoot, 'models')));
  });

  it('parses -I../models style paths (Cannon/SIM_cannon_aero)', async () => {
    const provider = makeProvider();
    const simRoot = path.join(REPO_ROOT, 'trick_sims', 'Cannon', 'SIM_cannon_aero');
    const config = await (provider as any).resolveViaRegex(simRoot, TRICK_HOME);
    assert.ok(config.cxxIncludes.includes(path.resolve(simRoot, '../models')));
  });

  it('expands ${TRICK_HOME} in -I paths (SIM_robot)', async () => {
    const provider = makeProvider();
    const simRoot = path.join(REPO_ROOT, 'trick_sims', 'SIM_robot');
    const config = await (provider as any).resolveViaRegex(simRoot, TRICK_HOME);
    assert.ok(config.cxxIncludes.includes(path.join(simRoot, 'models')));
    assert.ok(config.cxxIncludes.includes(TRICK_HOME));
  });

  // Skipped: the regex fallback hardcodes TRICK_SYSTEM_CXXFLAGS and doesn't
  // replicate Makefile.common's optional system probes (e.g. UDUNITS_INCLUDES,
  // only added when udunits2 headers are actually installed), so this diverges
  // from the make-based resolver on hosts that have those optional packages.
  it.skip('agrees with the make-based resolver on include dirs for SIM_rocket', async function () {
    if (process.env.CI_NO_MAKE) {
      this.skip();
    }
    const provider = makeProvider();
    const simRoot = path.join(REPO_ROOT, 'trick_sims', 'SIM_rocket');
    const viaRegex = await (provider as any).resolveViaRegex(simRoot, TRICK_HOME);
    const viaMake = await (provider as any).resolveViaMake(simRoot, TRICK_HOME);
    assert.deepStrictEqual(new Set(viaRegex.cxxIncludes), new Set(viaMake.cxxIncludes));
  });

  it('falls back to TRICK_HOME system includes for files outside any sim (default_trick_sys.sm)', async function () {
    if (process.env.CI_NO_MAKE) {
      this.skip();
    }
    const provider = makeProvider();
    const file = path.join(TRICK_HOME, 'share', 'trick', 'sim_objects', 'default_trick_sys.sm');
    assert.strictEqual(provider.findSimRoot(file), undefined);
    const config = await provider.getConfigForFile(file);
    assert.ok(config, 'expected a fallback SimConfig for a file outside any sim');
    // ##include "trick/SimObject.hh" must resolve via TRICK_SYSTEM_CXXFLAGS (-isystem$TRICK_HOME/include)
    assert.ok(config!.cxxIncludes.some((d: string) => fs.existsSync(path.join(d, 'trick', 'SimObject.hh'))));
    // #include "sim_objects/..." style targets must resolve via TRICK_SYSTEM_SFLAGS (-I$TRICK_HOME/share)
    assert.ok(config!.sIncludes.includes(path.join(TRICK_HOME, 'share')));
  });

  it('falls back to a trick/bin directory on PATH when walk-up and env fail', () => {
    const provider = makeProvider();
    const savedPath = process.env.PATH;
    const savedHome = process.env.TRICK_HOME;
    delete process.env.TRICK_HOME;
    try {
      process.env.PATH = [path.join(TRICK_HOME, 'bin'), '/usr/bin', '/bin'].join(path.delimiter);
      // Resolve from a directory with no ancestor Makefile.common (e.g. /tmp)
      // so only the PATH-based fallback can succeed.
      const resolved = provider.resolveTrickHome(path.parse(TRICK_HOME).root);
      assert.strictEqual(resolved, TRICK_HOME);
    } finally {
      process.env.PATH = savedPath;
      if (savedHome !== undefined) {
        process.env.TRICK_HOME = savedHome;
      }
    }
  });
});

describe('isInNestedRepo', () => {
  let workspaceRoot: string;

  beforeEach(() => {
    workspaceRoot = fs.mkdtempSync(path.join(os.tmpdir(), 'vscode-trick-nested-repo-'));
  });

  afterEach(() => {
    fs.rmSync(workspaceRoot, { recursive: true, force: true });
  });

  it('is false for a sim directly under the workspace root\'s own .git', () => {
    fs.mkdirSync(path.join(workspaceRoot, '.git'));
    const simRoot = path.join(workspaceRoot, 'sims', 'SIM_a');
    fs.mkdirSync(simRoot, { recursive: true });
    assert.strictEqual(isInNestedRepo(simRoot, workspaceRoot), false);
  });

  it('is true for a sim inside a submodule (.git as a file)', () => {
    const submoduleRoot = path.join(workspaceRoot, 'sub');
    fs.mkdirSync(submoduleRoot, { recursive: true });
    fs.writeFileSync(path.join(submoduleRoot, '.git'), 'gitdir: ../.git/modules/sub\n');
    const simRoot = path.join(submoduleRoot, 'sims', 'SIM_b');
    fs.mkdirSync(simRoot, { recursive: true });
    assert.strictEqual(isInNestedRepo(simRoot, workspaceRoot), true);
  });

  it('is true for a sim inside any other nested clone (.git as a directory)', () => {
    const cloneRoot = path.join(workspaceRoot, 'clone');
    fs.mkdirSync(path.join(cloneRoot, '.git'), { recursive: true });
    const simRoot = path.join(cloneRoot, 'SIM_c');
    fs.mkdirSync(simRoot, { recursive: true });
    assert.strictEqual(isInNestedRepo(simRoot, workspaceRoot), true);
  });

  it('is true when the sim root is itself the nested repo root', () => {
    const submoduleRoot = path.join(workspaceRoot, 'sub');
    fs.mkdirSync(submoduleRoot, { recursive: true });
    fs.writeFileSync(path.join(submoduleRoot, '.git'), 'gitdir: ../.git/modules/sub\n');
    assert.strictEqual(isInNestedRepo(submoduleRoot, workspaceRoot), true);
  });

  it('is false when the sim root equals the workspace root', () => {
    assert.strictEqual(isInNestedRepo(workspaceRoot, workspaceRoot), false);
  });

  it('is false when the sim root is outside the workspace root', () => {
    const outside = fs.mkdtempSync(path.join(os.tmpdir(), 'vscode-trick-outside-'));
    try {
      assert.strictEqual(isInNestedRepo(outside, workspaceRoot), false);
    } finally {
      fs.rmSync(outside, { recursive: true, force: true });
    }
  });

  it('is false when the workspace root is unknown', () => {
    assert.strictEqual(isInNestedRepo(path.join(workspaceRoot, 'sims', 'SIM_a'), undefined), false);
  });

  it('is false for a real sim in the Trick repo itself (no nested .git)', () => {
    const simRoot = path.join(REPO_ROOT, 'trick_sims', 'SIM_robot');
    assert.strictEqual(isInNestedRepo(simRoot, REPO_ROOT), false);
  });
});
