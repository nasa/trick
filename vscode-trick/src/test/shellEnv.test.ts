import * as assert from 'assert';
import {
  ShellEnvCaptureError,
  buildCaptureScript,
  captureShellEnv,
  diffEnv,
  parseCapturedEnv,
} from '../shellEnv';

describe('parseCapturedEnv', () => {
  it('parses a clean JSON dump between the markers', () => {
    const stdout = '__TRICK_ENV_START__{"FOO":"bar"}__TRICK_ENV_END__';
    assert.deepStrictEqual(parseCapturedEnv(stdout), { FOO: 'bar' });
  });

  it('ignores banner noise before and after the markers', () => {
    const stdout = [
      'Welcome to bash!',
      '** message of the day **',
      '__TRICK_ENV_START__',
      '{"TRICK_HOME":"/opt/trick"}',
      '__TRICK_ENV_END__',
      'logout',
    ].join('\n');
    assert.deepStrictEqual(parseCapturedEnv(stdout), { TRICK_HOME: '/opt/trick' });
  });

  it('returns undefined when a marker is missing', () => {
    assert.strictEqual(parseCapturedEnv('__TRICK_ENV_START__{"FOO":"bar"}'), undefined);
    assert.strictEqual(parseCapturedEnv('{"FOO":"bar"}__TRICK_ENV_END__'), undefined);
    assert.strictEqual(parseCapturedEnv('no markers here'), undefined);
  });

  it('returns undefined for malformed JSON between the markers', () => {
    const stdout = '__TRICK_ENV_START__not json__TRICK_ENV_END__';
    assert.strictEqual(parseCapturedEnv(stdout), undefined);
  });

  it('handles a value containing a literal newline (multi-line exported var)', () => {
    const stdout = '__TRICK_ENV_START__{"FOO":"line1\\nline2"}__TRICK_ENV_END__';
    assert.deepStrictEqual(parseCapturedEnv(stdout), { FOO: 'line1\nline2' });
  });
});

describe('buildCaptureScript', () => {
  it('chains the user command before the dump with &&, and brackets it with markers', () => {
    const script = buildCaptureScript('source trick_env.sh');
    assert.ok(script.startsWith('source trick_env.sh && echo __TRICK_ENV_START__'));
    assert.ok(script.includes('TRICK_VSCODE_NODE'));
    assert.ok(script.includes('ELECTRON_RUN_AS_NODE=1'));
    assert.ok(script.trimEnd().endsWith('__TRICK_ENV_END__'));
  });
});

describe('diffEnv', () => {
  it('reports only added/changed vars', () => {
    const current = { FOO: '1', BAR: '2' };
    const captured = { FOO: '1', BAR: '3', BAZ: '4' };
    assert.deepStrictEqual(diffEnv(current, captured), { BAR: '3', BAZ: '4' });
  });

  it('skips shell-internal and volatile vars', () => {
    const current = {};
    const captured = {
      PWD: '/tmp',
      OLDPWD: '/',
      SHLVL: '1',
      _: '/bin/bash',
      ELECTRON_RUN_AS_NODE: '1',
      TRICK_VSCODE_NODE: '/usr/bin/node',
      PS1: '$ ',
      VSCODE_IPC_HOOK: 'x',
      TERM: 'xterm',
      PROMPT_COMMAND: 'x',
      TRICK_HOME: '/opt/trick',
    };
    assert.deepStrictEqual(diffEnv(current, captured), { TRICK_HOME: '/opt/trick' });
  });

  it('never reports a deletion for a var missing from captured', () => {
    const current = { KEEP_ME: 'yes' };
    const captured = { OTHER: 'value' };
    assert.deepStrictEqual(diffEnv(current, captured), { OTHER: 'value' });
  });
});

describe('captureShellEnv (integration, /bin/bash)', () => {
  const shell = '/bin/bash';

  it('picks up a variable exported by an inline shell function', async () => {
    const result = await captureShellEnv('f() { export TRICK_HOME=/opt/trick; }; f', { shell });
    assert.strictEqual(result.TRICK_HOME, '/opt/trick');
  });

  it('picks up a variable exported by an inline alias', async () => {
    // bash only expands an alias in a command parsed *after* the alias
    // definition was read - on the same semicolon-joined line, it isn't
    // expanded yet, so the definition and its use need separate lines here
    // (both still arrive as one userCommand string).
    const command = ["shopt -s expand_aliases", "alias trickenv='export TRICK_HOME=/opt/trick2'", 'trickenv'].join(
      '\n'
    );
    const result = await captureShellEnv(command, { shell });
    assert.strictEqual(result.TRICK_HOME, '/opt/trick2');
  });

  it('rejects when the user command fails', async () => {
    await assert.rejects(
      captureShellEnv('exit 1', { shell }),
      (err: unknown) => err instanceof ShellEnvCaptureError
    );
  });

  it('does not hang when the command tries to read stdin', async () => {
    const result = await captureShellEnv('read -r unused_var; export TRICK_READ_RESULT=done', { shell, timeoutMs: 5000 });
    assert.strictEqual(result.TRICK_READ_RESULT, 'done');
  });

  it('rejects on timeout rather than hanging forever', async () => {
    await assert.rejects(
      captureShellEnv('sleep 5', { shell, timeoutMs: 300 }),
      (err: unknown) => err instanceof ShellEnvCaptureError
    );
  }).timeout(5000);
});
