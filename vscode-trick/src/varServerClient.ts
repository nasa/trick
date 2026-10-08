import * as net from 'net';
import { EventEmitter } from 'events';

/**
 * A client for Trick's variable server TCP protocol (see
 * docs/documentation/simulation_capabilities/Variable-Server.md and the
 * reference implementation at share/trick/pymods/trick/variable_server.py),
 * used by the "Trick Sims" tree view to watch variables and control a sim's
 * run state - replacing the Java Trick View and Sim Control Panel tools.
 *
 * Deliberately has no `import * as vscode` so it can be unit-tested with
 * plain mocha/ts-node, including against a real `net.createServer` standing
 * in for the variable server.
 *
 * Commands are plain Python text lines, terminated with `\n` (the variable
 * server is itself a Python interpreter fed over the socket). Replies are
 * lines of the form `<type>\t<v1>\t<v2>...\n`; a value with units is
 * formatted as `value {units}`; a reference to a nonexistent variable comes
 * back as the literal string `BAD_REF`. A single reply can be split across
 * TCP packets (and conversely, more than one reply can land in one packet),
 * so input must be buffered until a newline is seen rather than trusting
 * socket 'data' event boundaries.
 */

export const MSG_TYPE_VARLIST = 0;
export const MSG_TYPE_VAR_EXISTS = 1;

export interface VarServerLine {
  type: number;
  fields: string[];
}

/** Splits one `<type>\t<v1>\t<v2>...` reply line into its type and value fields. */
export function parseVarServerLine(line: string): VarServerLine | undefined {
  const trimmed = line.replace(/\r?\n$/, '');
  if (trimmed.length === 0) {
    return undefined;
  }
  const parts = trimmed.split('\t');
  const type = parseInt(parts[0], 10);
  if (Number.isNaN(type)) {
    return undefined;
  }
  return { type, fields: parts.slice(1) };
}

export interface ParsedValue {
  value: string;
  units?: string;
}

const UNITS_RE = /^(.*) \{(.*)\}$/;

/** Parses one value field, e.g. `1.5 {m}` -> `{value:"1.5", units:"m"}`, or `BAD_REF` -> `{value:"BAD_REF"}`. */
export function parseValue(field: string): ParsedValue {
  const m = UNITS_RE.exec(field);
  if (m) {
    return { value: m[1], units: m[2] };
  }
  return { value: field };
}

/** Buffers arbitrary chunks and yields complete, newline-terminated lines. */
export class LineBuffer {
  private pending = '';

  /** Feeds a chunk and returns any complete lines it completed (newlines stripped). */
  push(chunk: string): string[] {
    this.pending += chunk;
    const lines: string[] = [];
    let idx: number;
    while ((idx = this.pending.indexOf('\n')) !== -1) {
      lines.push(this.pending.slice(0, idx).replace(/\r$/, ''));
      this.pending = this.pending.slice(idx + 1);
    }
    return lines;
  }
}

export interface SimStatus {
  /** 0 Init, 1 Freeze, 4 Step, 5 Run, 6 Exit - see include/trick/sim_mode.h. */
  mode: number;
  /** Seconds, derived from trick_sys.sched.time_tics / time_tic_value. */
  time: number;
}

const STATUS_VARS = ['trick_sys.sched.mode', 'trick_sys.sched.time_tics', 'trick_sys.sched.time_tic_value'];
const HANDSHAKE_TIMEOUT_MS = 2000;
const VAR_CYCLE_SECONDS = 0.25;

/**
 * One connection to a running sim's variable server. Connects, confirms the
 * variable server is actually enabled (a TCP connect can succeed even when
 * it's disabled - the Java Sim Control Panel does the same `var_exists`
 * handshake check for this reason), then keeps a registered variable list in
 * sync with `setWatches`.
 */
export class VarServerClient extends EventEmitter {
  private socket: net.Socket | undefined;
  private readonly lineBuffer = new LineBuffer();
  private watches: string[] = [];
  private timeTicValue = 1;
  private closed = false;
  private connected = false;

  constructor(
    private readonly host: string,
    private readonly port: number
  ) {
    super();
  }

  connect(): Promise<void> {
    return new Promise((resolve, reject) => {
      const socket = new net.Socket();
      this.socket = socket;
      let settled = false;

      const fail = (err: Error) => {
        if (settled) {
          return;
        }
        settled = true;
        socket.destroy();
        reject(err);
      };

      const timeout = setTimeout(() => fail(new Error('Variable server handshake timed out')), HANDSHAKE_TIMEOUT_MS);

      socket.once('error', fail);
      socket.on('close', () => {
        this.closed = true;
        // Node always fires 'close' after a failed connect/'error', same as
        // after a later disconnect - only treat it as "the sim went away"
        // once the handshake actually completed, otherwise connect()'s own
        // rejection already told the caller, and re-emitting 'closed' here
        // would stomp the caller's "disconnected" state back to "ended"
        // (and with it, the Connect button) after every failed attempt.
        if (this.connected) {
          this.emit('closed');
        }
      });

      socket.connect(this.port, this.host, () => {
        socket.write('trick.var_set_client_tag("vscode-trick")\n');
        socket.write('trick.var_ascii()\n');
        socket.write('trick.var_exists("trick_sys.sched.mode")\n');
      });

      const onHandshake = (data: Buffer) => {
        for (const line of this.lineBuffer.push(data.toString('utf8'))) {
          const parsed = parseVarServerLine(line);
          if (parsed?.type === MSG_TYPE_VAR_EXISTS) {
            clearTimeout(timeout);
            socket.off('data', onHandshake);
            socket.on('data', (d) => this.handleData(d));
            if (parsed.fields[0] !== '1') {
              fail(new Error('Variable server is disabled for this sim'));
              return;
            }
            settled = true;
            this.connected = true;
            void this.registerVariables().then(resolve, reject);
          }
        }
      };
      socket.on('data', onHandshake);
    });
  }

  private async registerVariables(): Promise<void> {
    const socket = this.socket;
    if (!socket) {
      return;
    }
    socket.write('trick.var_pause()\n');
    socket.write('trick.var_clear()\n');
    for (const name of [...STATUS_VARS, ...this.watches]) {
      socket.write(`trick.var_add("${name}")\n`);
    }
    socket.write(`trick.var_cycle(${VAR_CYCLE_SECONDS})\n`);
    socket.write('trick.var_unpause()\n');
  }

  /** Replaces the watched variable list (status vars are always included first). */
  setWatches(names: string[]): void {
    this.watches = [...names];
    void this.registerVariables();
  }

  /** Asks the variable server whether a path is valid before adding it as a watch. */
  varExists(name: string): Promise<boolean> {
    return new Promise((resolve, reject) => {
      const socket = this.socket;
      if (!socket || this.closed) {
        reject(new Error('Not connected'));
        return;
      }
      const timeout = setTimeout(() => {
        this.off('var_exists_reply', onReply);
        reject(new Error('var_exists timed out'));
      }, HANDSHAKE_TIMEOUT_MS);
      const onReply = (exists: boolean) => {
        clearTimeout(timeout);
        resolve(exists);
      };
      this.once('var_exists_reply', onReply);
      socket.write(`trick.var_exists("${name}")\n`);
    });
  }

  send(command: string): void {
    this.socket?.write(`${command}\n`);
  }

  run(): void {
    this.send('trick.exec_run()');
  }

  freeze(): void {
    this.send('trick.exec_freeze()');
  }

  stop(): void {
    this.send('trick.stop()');
  }

  close(): void {
    this.send('trick.var_exit()');
    this.socket?.end();
    this.socket?.destroy();
  }

  private handleData(data: Buffer): void {
    for (const line of this.lineBuffer.push(data.toString('utf8'))) {
      const parsed = parseVarServerLine(line);
      if (!parsed) {
        continue;
      }
      if (parsed.type === MSG_TYPE_VAR_EXISTS) {
        this.emit('var_exists_reply', parsed.fields[0] === '1');
        continue;
      }
      if (parsed.type !== MSG_TYPE_VARLIST) {
        continue;
      }
      const expected = STATUS_VARS.length + this.watches.length;
      if (parsed.fields.length !== expected) {
        // A reply already in flight from before the last re-register - the
        // count won't line up with the current watch list, so it has to be
        // dropped rather than mis-attributed to the wrong names (mirrors
        // variable_server.py's reference client).
        continue;
      }
      const [modeField, timeTicsField, timeTicValueField, ...watchFields] = parsed.fields;
      const mode = parseInt(parseValue(modeField).value, 10);
      const timeTics = parseFloat(parseValue(timeTicsField).value);
      this.timeTicValue = parseFloat(parseValue(timeTicValueField).value) || this.timeTicValue;
      const status: SimStatus = { mode, time: timeTics / this.timeTicValue };
      this.emit('status', status);

      const values = new Map<string, ParsedValue>();
      this.watches.forEach((name, i) => values.set(name, parseValue(watchFields[i])));
      this.emit('values', values);
    }
  }
}
