import * as cp from 'child_process';

/**
 * Captures the environment produced by running a shell command (an alias, a
 * shell function, or a plain `source some_env.sh`) in the user's own
 * interactive shell - see "Trick: Update Environment from Shell Command" in
 * extension.ts. This exists because VS Code launched from a desktop/file
 * launcher doesn't inherit the user's shell environment, and sourcing an env
 * script in VS Code's own integrated terminal only changes that terminal's
 * shell process, not the extension host's `process.env` - so TRICK_HOME and
 * the TRICK_*FLAGS never reach SimConfigProvider no matter what's typed there.
 *
 * Deliberately has no `import * as vscode` so it can be unit-tested with
 * plain mocha against a real child_process, same as varServerClient.ts.
 */

const START_MARKER = '__TRICK_ENV_START__';
const END_MARKER = '__TRICK_ENV_END__';

/**
 * Builds the script to run inside the user's shell: the user's own command,
 * then (only if it succeeded) a dump of the resulting environment as JSON,
 * bracketed by sentinel lines.
 *
 * The dump is done with the extension host's own Node binary (passed in via
 * the TRICK_VSCODE_NODE env var, with ELECTRON_RUN_AS_NODE=1 scoped to just
 * that one invocation so Electron's bundled binary behaves as plain node
 * without leaking that flag into the user's own command) rather than the
 * `env`/`printenv` command, for two reasons: it comes back as JSON instead of
 * a form that's ambiguous for multi-line values (e.g. an exported bash
 * function, or a value containing a literal newline), and it sidesteps BSD
 * vs. GNU `env` flag differences across platforms.
 *
 * `&&` before the dump means a failing user command (bad alias name, a
 * `source` of a script that errors) reports failure instead of silently
 * capturing an env that was never actually updated.
 */
export function buildCaptureScript(userCommand: string): string {
  const dump = `ELECTRON_RUN_AS_NODE=1 "$TRICK_VSCODE_NODE" -e "process.stdout.write(JSON.stringify(process.env))"`;
  return `${userCommand} && echo ${START_MARKER} && ${dump} && echo ${END_MARKER}`;
}

/**
 * Extracts and parses the JSON environment dump bracketed by the sentinel
 * lines, ignoring everything outside them - rc-file banners, MOTD, nvm/conda
 * init chatter, prompt escape codes, and so on. Returns undefined if the
 * markers aren't both present (e.g. the shell exited before reaching them).
 */
export function parseCapturedEnv(stdout: string): Record<string, string> | undefined {
  const start = stdout.indexOf(START_MARKER);
  const end = stdout.indexOf(END_MARKER);
  if (start === -1 || end === -1 || end < start) {
    return undefined;
  }
  const jsonText = stdout.slice(start + START_MARKER.length, end).trim();
  try {
    const parsed = JSON.parse(jsonText);
    if (parsed && typeof parsed === 'object') {
      return parsed as Record<string, string>;
    }
    return undefined;
  } catch {
    return undefined;
  }
}

// Shell-internal/volatile vars that change on every shell invocation
// regardless of what the user's command actually set up, and would otherwise
// show up as spurious "changes" on every run - or, worse, clobber the
// extension host's own identity (ELECTRON_RUN_AS_NODE) or VS Code's IPC
// wiring (VSCODE_*) if merged into process.env.
const SKIP_EXACT = new Set(['PWD', 'OLDPWD', 'SHLVL', '_', 'ELECTRON_RUN_AS_NODE', 'TRICK_VSCODE_NODE', 'PS1']);
const SKIP_PREFIX = ['VSCODE_', 'TERM', 'PROMPT'];

function shouldSkip(name: string): boolean {
  return SKIP_EXACT.has(name) || SKIP_PREFIX.some((p) => name.startsWith(p));
}

/**
 * The subset of `captured` that's new or different from `current`, excluding
 * shell-internal/volatile vars (see shouldSkip). Only ever adds/overwrites -
 * never reports a deletion - since "the interactive shell didn't have this
 * var" doesn't mean the user wants it unset in the extension host; it may
 * have been set some other way (trick.home, a previous run of this same
 * command, etc).
 */
export function diffEnv(
  current: NodeJS.ProcessEnv,
  captured: Record<string, string>
): Record<string, string> {
  const diff: Record<string, string> = {};
  for (const [name, value] of Object.entries(captured)) {
    if (shouldSkip(name)) {
      continue;
    }
    if (current[name] !== value) {
      diff[name] = value;
    }
  }
  return diff;
}

export interface CaptureShellEnvOptions {
  /** Defaults to $SHELL, falling back to /bin/bash. */
  shell?: string;
  /** Defaults to 20 seconds. */
  timeoutMs?: number;
  cwd?: string;
}

export class ShellEnvCaptureError extends Error {}

/**
 * Runs `userCommand` in an interactive instance of the user's shell (so
 * aliases and shell functions defined in .bashrc/.zshrc actually exist, not
 * just what a plain non-interactive `shell -c` would see), then captures the
 * resulting environment.
 *
 * Uses `spawn` rather than `execFile`, with the child detached into its own
 * process group, specifically so a timeout can SIGKILL that whole group
 * (`process.kill(-pid, ...)`) rather than just the shell itself -
 * `execFile`'s own built-in `timeout` option only ever signals the immediate
 * child, which leaves a long-running grandchild (e.g. a `sleep`, or whatever
 * the sourced script shells out to) running and the pipe open, so the
 * promise would still eventually settle but the orphaned process wouldn't
 * actually be gone.
 *
 * stdin is closed immediately rather than inherited, so a command that
 * unexpectedly waits on input (a stray `read`, a password prompt from
 * something sourced along the way) hits EOF right away instead of hanging
 * until the timeout.
 */
export function captureShellEnv(
  userCommand: string,
  options: CaptureShellEnvOptions = {}
): Promise<Record<string, string>> {
  const shell = options.shell || process.env.SHELL || '/bin/bash';
  const timeoutMs = options.timeoutMs ?? 20000;
  const script = buildCaptureScript(userCommand);

  return new Promise((resolve, reject) => {
    const child = cp.spawn(shell, ['-i', '-c', script], {
      cwd: options.cwd,
      env: { ...process.env, TRICK_VSCODE_NODE: process.execPath },
      detached: true,
      stdio: ['ignore', 'pipe', 'pipe'],
    });

    let stdout = '';
    let stderr = '';
    let timedOut = false;
    let settled = false;

    const timer = setTimeout(() => {
      timedOut = true;
      if (child.pid) {
        try {
          process.kill(-child.pid, 'SIGKILL');
        } catch {
          child.kill('SIGKILL');
        }
      }
    }, timeoutMs);

    const finish = (fn: () => void) => {
      if (settled) {
        return;
      }
      settled = true;
      clearTimeout(timer);
      fn();
    };

    child.stdout.on('data', (chunk: Buffer) => {
      stdout += chunk.toString('utf8');
    });
    child.stderr.on('data', (chunk: Buffer) => {
      stderr += chunk.toString('utf8');
    });
    child.on('error', (err) => {
      finish(() => reject(new ShellEnvCaptureError(`Could not start ${shell}: ${err.message}`)));
    });
    child.on('close', (code) => {
      finish(() => {
        if (timedOut) {
          reject(new ShellEnvCaptureError(`Timed out after ${timeoutMs}ms waiting for the command to finish.`));
          return;
        }
        if (code !== 0) {
          const tail = stderr.trim().split('\n').slice(-5).join('\n');
          reject(
            new ShellEnvCaptureError(tail ? `Command failed:\n${tail}` : `Command failed with exit code ${code}.`)
          );
          return;
        }
        const captured = parseCapturedEnv(stdout);
        if (!captured) {
          reject(
            new ShellEnvCaptureError(
              'Could not read the environment back from the shell (no output markers found).'
            )
          );
          return;
        }
        resolve(captured);
      });
    });
  });
}
