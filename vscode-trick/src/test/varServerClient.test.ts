import * as assert from 'assert';
import * as net from 'net';
import {
  LineBuffer,
  ParsedValue,
  SimStatus,
  VarServerClient,
  parseValue,
  parseVarServerLine,
} from '../varServerClient';

describe('parseVarServerLine', () => {
  it('splits type and fields on tab', () => {
    assert.deepStrictEqual(parseVarServerLine('0\t5\t100\t0.01\n'), { type: 0, fields: ['5', '100', '0.01'] });
  });

  it('handles a reply with no value fields', () => {
    assert.deepStrictEqual(parseVarServerLine('1\t1\n'), { type: 1, fields: ['1'] });
  });

  it('returns undefined for an empty line', () => {
    assert.strictEqual(parseVarServerLine(''), undefined);
    assert.strictEqual(parseVarServerLine('\n'), undefined);
  });

  it('returns undefined when the type field is not a number', () => {
    assert.strictEqual(parseVarServerLine('oops\t1\n'), undefined);
  });
});

describe('parseValue', () => {
  it('splits a value with units', () => {
    const v = parseValue('1.5 {m}');
    assert.deepStrictEqual(v, { value: '1.5', units: 'm' });
  });

  it('leaves a unitless value alone', () => {
    assert.deepStrictEqual(parseValue('5'), { value: '5' });
  });

  it('passes BAD_REF through as the value', () => {
    assert.deepStrictEqual(parseValue('BAD_REF'), { value: 'BAD_REF' });
  });

  it('handles an empty field', () => {
    assert.deepStrictEqual(parseValue(''), { value: '' });
  });

  it('handles units containing special characters (e.g. --)', () => {
    assert.deepStrictEqual(parseValue('0 {--}'), { value: '0', units: '--' });
  });
});

describe('LineBuffer', () => {
  it('yields nothing until a newline arrives', () => {
    const buf = new LineBuffer();
    assert.deepStrictEqual(buf.push('0\t5\t100'), []);
  });

  it('yields a complete line split across chunks', () => {
    const buf = new LineBuffer();
    assert.deepStrictEqual(buf.push('0\t5\t100'), []);
    assert.deepStrictEqual(buf.push('\t0.01\n'), ['0\t5\t100\t0.01']);
  });

  it('yields multiple lines delivered in one chunk', () => {
    const buf = new LineBuffer();
    assert.deepStrictEqual(buf.push('1\t1\n0\t5\t100\t0.01\n'), ['1\t1', '0\t5\t100\t0.01']);
  });

  it('strips a trailing carriage return', () => {
    const buf = new LineBuffer();
    assert.deepStrictEqual(buf.push('1\t1\r\n'), ['1\t1']);
  });
});

/**
 * A minimal stand-in for Trick's variable server: answers the `var_exists`
 * handshake, and otherwise just records what was sent so the test can push
 * reply lines on its own schedule, independent of the client's own writes.
 */
function startFakeVarServer(): Promise<{ server: net.Server; port: number; socket: () => net.Socket }> {
  return new Promise((resolve) => {
    let serverSocket: net.Socket | undefined;
    const server = net.createServer((socket) => {
      serverSocket = socket;
      let pending = '';
      socket.on('data', (data) => {
        pending += data.toString('utf8');
        let idx: number;
        while ((idx = pending.indexOf('\n')) !== -1) {
          const line = pending.slice(0, idx);
          pending = pending.slice(idx + 1);
          if (line.includes('var_exists')) {
            socket.write('1\t1\n');
          }
        }
      });
    });
    server.listen(0, '127.0.0.1', () => {
      const port = (server.address() as net.AddressInfo).port;
      resolve({ server, port, socket: () => serverSocket! });
    });
  });
}

describe('VarServerClient end-to-end (fake server)', () => {
  it('connects, parses status/values, and drops mismatched-count messages', async function () {
    this.timeout(5000);
    const { server, port, socket } = await startFakeVarServer();
    const client = new VarServerClient('127.0.0.1', port);
    try {
      await client.connect();
      client.setWatches(['ball.position']);

      const statuses: SimStatus[] = [];
      const valueSnapshots: Map<string, ParsedValue>[] = [];
      client.on('status', (s: SimStatus) => statuses.push(s));
      client.on('values', (v: Map<string, ParsedValue>) => valueSnapshots.push(v));

      // trick_sys.sched.mode=5, time_tics=100, time_tic_value=100 (-> t=1s), ball.position=1.5 {m}
      socket().write('0\t5\t100\t100\t1.5 {m}\n');
      await new Promise((r) => setTimeout(r, 100));

      assert.strictEqual(statuses.length, 1);
      assert.deepStrictEqual(statuses[0], { mode: 5, time: 1 });
      assert.strictEqual(valueSnapshots.length, 1);
      assert.deepStrictEqual(valueSnapshots[0].get('ball.position'), { value: '1.5', units: 'm' });

      // Missing the watch value field - count doesn't match the registered
      // list, so this must be dropped rather than mis-attributed.
      socket().write('0\t5\t100\t100\n');
      await new Promise((r) => setTimeout(r, 100));

      assert.strictEqual(statuses.length, 1, 'mismatched-count message must not fire a new status event');
      assert.strictEqual(valueSnapshots.length, 1, 'mismatched-count message must not fire a new values event');
    } finally {
      client.close();
      server.close();
    }
  });

  it('rejects connect() when the variable server reports it is disabled', async function () {
    this.timeout(5000);
    const server = net.createServer((socket) => {
      let pending = '';
      socket.on('data', (data) => {
        pending += data.toString('utf8');
        let idx: number;
        while ((idx = pending.indexOf('\n')) !== -1) {
          const line = pending.slice(0, idx);
          pending = pending.slice(idx + 1);
          if (line.includes('var_exists')) {
            socket.write('1\t0\n');
          }
        }
      });
    });
    await new Promise<void>((resolve) => server.listen(0, '127.0.0.1', resolve));
    const port = (server.address() as net.AddressInfo).port;
    const client = new VarServerClient('127.0.0.1', port);
    const closedEvents: unknown[] = [];
    client.on('closed', () => closedEvents.push(undefined));
    try {
      await assert.rejects(() => client.connect());
      // The socket does close after a failed connect (Node always fires
      // 'close' after 'error'), but that must not surface as this client's
      // own 'closed' event - connect()'s rejection already told the caller,
      // and a caller that treats 'closed' as "the sim went away" (like the
      // Trick Sims view) would otherwise undo its own "connect failed"
      // handling.
      await new Promise((r) => setTimeout(r, 50));
      assert.strictEqual(closedEvents.length, 0);
    } finally {
      server.close();
    }
  });

  it('emits closed when a successfully connected socket later disconnects', async function () {
    this.timeout(5000);
    const { server, port, socket } = await startFakeVarServer();
    const client = new VarServerClient('127.0.0.1', port);
    const closedEvents: unknown[] = [];
    client.on('closed', () => closedEvents.push(undefined));
    try {
      await client.connect();
      socket().destroy();
      await new Promise((r) => setTimeout(r, 50));
      assert.strictEqual(closedEvents.length, 1);
    } finally {
      server.close();
    }
  });
});
