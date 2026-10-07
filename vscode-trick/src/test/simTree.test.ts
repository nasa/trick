import * as assert from 'assert';
import * as net from 'net';

const vscodeStub: any = {
  EventEmitter: class {
    private listeners: Array<(e: unknown) => void> = [];
    event = (cb: (e: unknown) => void) => {
      this.listeners.push(cb);
      return { dispose() {} };
    };
    fire(e: unknown) {
      for (const cb of this.listeners) {
        cb(e);
      }
    }
    dispose() {}
  },
  TreeItemCollapsibleState: { None: 0, Collapsed: 1, Expanded: 2 },
  TreeItem: class {
    label: string;
    collapsibleState: number;
    id?: string;
    contextValue?: string;
    description?: string;
    tooltip?: string;
    iconPath?: unknown;
    constructor(label: string, collapsibleState?: number) {
      this.label = label;
      this.collapsibleState = collapsibleState ?? 0;
    }
  },
  ThemeIcon: class {
    constructor(public id: string) {}
  },
  window: {
    showErrorMessage: () => undefined,
    showWarningMessage: () => undefined,
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
const { TrickSimsTreeProvider, ConnectedSimsTreeProvider } = require('../simTree');

function makeAnnouncement(key: string, simDir: string) {
  const [host, portStr] = key.split(':');
  return {
    key,
    host,
    port: parseInt(portStr, 10),
    user: 'u',
    pid: 1,
    simDir,
    sMain: 'S_main',
    inputFile: 'RUN_test/input.py',
    version: '19',
    tag: '',
    vsEnabled: true,
    execMode: 1,
  };
}

class FakeDiscovery {
  private announcements: ReturnType<typeof makeAnnouncement>[] = [];
  list() {
    return this.announcements;
  }
  set(list: ReturnType<typeof makeAnnouncement>[]) {
    this.announcements = list;
  }
}

function makeWorkspaceState() {
  const store = new Map<string, unknown>();
  return {
    get: (key: string, def: unknown) => (store.has(key) ? store.get(key) : def),
    update: async (key: string, value: unknown) => {
      store.set(key, value);
    },
  };
}

function makeOutput() {
  return { appendLine: () => undefined };
}

/**
 * Minimal stand-in for Trick's variable server - same shape as the helper in
 * varServerClient.test.ts - just enough to get VarServerClient.connect()'s
 * var_exists handshake to succeed.
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

describe('TrickSimsTreeProvider / ConnectedSimsTreeProvider panes', () => {
  it('keeps disconnected sims in the available view, alphabetized, and nothing in Connected', () => {
    const discovery = new FakeDiscovery();
    const provider = new TrickSimsTreeProvider(discovery, makeWorkspaceState(), makeOutput());
    const connected = new ConnectedSimsTreeProvider(provider);

    discovery.set([
      makeAnnouncement('127.0.0.1:1001', '/sims/SIM_b'),
      makeAnnouncement('127.0.0.1:1002', '/sims/SIM_a'),
    ]);
    provider.refreshFromDiscovery();

    assert.strictEqual(provider.getChildren().length, 2);
    assert.strictEqual(connected.getChildren().length, 0);
    const labels = provider.getChildren().map((n: unknown) => provider.getTreeItem(n).label);
    assert.deepStrictEqual(labels, ['SIM_a', 'SIM_b']);

    provider.dispose();
  });

  it('moves a sim into Connected on connect, keeps it there (as "ended") after it stops broadcasting, and Disconnect removes it from both', async function () {
    this.timeout(5000);
    const { server, port, socket } = await startFakeVarServer();
    const discovery = new FakeDiscovery();
    const id = `127.0.0.1:${port}`;
    discovery.set([makeAnnouncement(id, '/sims/SIM_cannon'), makeAnnouncement('127.0.0.1:9999', '/sims/SIM_other')]);

    const provider = new TrickSimsTreeProvider(discovery, makeWorkspaceState(), makeOutput());
    const connected = new ConnectedSimsTreeProvider(provider);
    provider.refreshFromDiscovery();

    assert.strictEqual(provider.getChildren().length, 2);
    assert.strictEqual(connected.getChildren().length, 0);

    provider.connect(id);
    await new Promise((r) => setTimeout(r, 100));

    assert.strictEqual(connected.getChildren().length, 1, 'connecting sim should move to Connected immediately');
    assert.strictEqual(
      provider.getChildren().length,
      1,
      'available list should drop to the one remaining disconnected sim'
    );

    // Sim stops broadcasting while still connected - discovery no longer
    // lists it, but it must not vanish from Connected.
    discovery.set([makeAnnouncement('127.0.0.1:9999', '/sims/SIM_other')]);
    provider.refreshFromDiscovery();
    assert.strictEqual(connected.getChildren().length, 1, 'still-connected sim must survive leaving discovery');

    // The sim process itself ends.
    socket().destroy();
    await new Promise((r) => setTimeout(r, 100));
    const [connectedNode] = connected.getChildren();
    assert.strictEqual(connected.getTreeItem(connectedNode).contextValue, 'sim.ended');

    provider.disconnect(id);
    assert.strictEqual(connected.getChildren().length, 0, 'Disconnect removes an ended, no-longer-broadcasting sim');

    provider.dispose();
    server.close();
  });

  it('disconnecting a still-broadcasting sim sticks, even once the socket\'s own delayed close event arrives', async function () {
    this.timeout(5000);
    const { server, port } = await startFakeVarServer();
    const discovery = new FakeDiscovery();
    const id = `127.0.0.1:${port}`;
    discovery.set([makeAnnouncement(id, '/sims/SIM_cannon')]);

    const provider = new TrickSimsTreeProvider(discovery, makeWorkspaceState(), makeOutput());
    const connected = new ConnectedSimsTreeProvider(provider);
    provider.refreshFromDiscovery();

    provider.connect(id);
    await new Promise((r) => setTimeout(r, 100));
    assert.strictEqual(connected.getChildren().length, 1, 'sim should be connected');

    // disconnect() closes the client synchronously, but the underlying
    // socket's own 'close' event (and this client's resulting 'closed'
    // emit) lands on a later tick. That stale event must not resurrect the
    // sim into Connected as "ended" after the user has already disconnected it.
    provider.disconnect(id);
    assert.strictEqual(connected.getChildren().length, 0, 'disconnect should take effect immediately');
    assert.strictEqual(provider.getChildren().length, 1, 'still-broadcasting sim returns to the available list');

    await new Promise((r) => setTimeout(r, 150));
    assert.strictEqual(connected.getChildren().length, 0, 'the delayed closed event must not undo the disconnect');
    assert.strictEqual(provider.getChildren().length, 1, 'sim must still be in the available list, not re-added to Connected');

    provider.dispose();
    server.close();
  });

  it('a second Connect click while the first is still connecting is a no-op, not a second racing client', async function () {
    this.timeout(5000);
    const { server, port } = await startFakeVarServer();
    const discovery = new FakeDiscovery();
    const id = `127.0.0.1:${port}`;
    discovery.set([makeAnnouncement(id, '/sims/SIM_cannon')]);

    const provider = new TrickSimsTreeProvider(discovery, makeWorkspaceState(), makeOutput());
    const connected = new ConnectedSimsTreeProvider(provider);
    provider.refreshFromDiscovery();

    provider.connect(id);
    provider.connect(id);
    provider.connect(id);
    await new Promise((r) => setTimeout(r, 100));

    assert.strictEqual(connected.getChildren().length, 1, 'repeated clicks must not duplicate the connected sim');

    provider.dispose();
    server.close();
  });

  it('steady value cycles only redraw changed watch rows, never the sim row and its buttons', async function () {
    this.timeout(5000);
    const { server, port } = await startFakeVarServer();
    const discovery = new FakeDiscovery();
    const id = `127.0.0.1:${port}`;
    discovery.set([makeAnnouncement(id, '/sims/SIM_cannon')]);

    const provider = new TrickSimsTreeProvider(discovery, makeWorkspaceState(), makeOutput());
    const connected = new ConnectedSimsTreeProvider(provider);
    provider.refreshFromDiscovery();
    provider.connect(id);
    await new Promise((r) => setTimeout(r, 100));
    provider.addWatch(id, 'dyn.cannon.pos[0]');

    const client = provider.getEntry(id).client;
    client.emit('status', { mode: 1, time: 0 });
    client.emit('values', new Map([['dyn.cannon.pos[0]', { value: '1.0', units: 'm' }]]));
    await new Promise((r) => setTimeout(r, 400));

    const events: unknown[] = [];
    provider.onDidChangeTreeData((e: unknown) => events.push(e));

    // Frozen sim: same mode and same value, cycle after cycle.
    for (let i = 0; i < 3; i++) {
      client.emit('status', { mode: 1, time: 0 });
      client.emit('values', new Map([['dyn.cannon.pos[0]', { value: '1.0', units: 'm' }]]));
    }
    await new Promise((r) => setTimeout(r, 400));
    assert.deepStrictEqual(events, [], 'unchanged status/values must not redraw anything');

    client.emit('values', new Map([['dyn.cannon.pos[0]', { value: '2.0', units: 'm' }]]));
    await new Promise((r) => setTimeout(r, 400));
    assert.strictEqual(events.length, 1);
    const [watchNode] = connected.getChildren(provider.simNode(id));
    assert.deepStrictEqual(events[0], [watchNode], 'only the changed watch row is redrawn, by the same node object');
    assert.strictEqual(connected.getTreeItem(watchNode).description, '2.0 {m}');

    client.emit('status', { mode: 5, time: 1 });
    assert.strictEqual(events[1], undefined, 'a mode change redraws the whole tree immediately');
    assert.strictEqual(events.length, 2);

    provider.dispose();
    server.close();
  });

  it('shows watch children only from the Connected provider, and preserves connect order independent of discovery re-sorting', async function () {
    this.timeout(5000);
    const a = await startFakeVarServer();
    const b = await startFakeVarServer();
    const discovery = new FakeDiscovery();
    const idA = `127.0.0.1:${a.port}`;
    const idB = `127.0.0.1:${b.port}`;
    discovery.set([makeAnnouncement(idA, '/sims/SIM_z')]);

    const provider = new TrickSimsTreeProvider(discovery, makeWorkspaceState(), makeOutput());
    const connected = new ConnectedSimsTreeProvider(provider);
    provider.refreshFromDiscovery();
    provider.connect(idA);
    await new Promise((r) => setTimeout(r, 100));

    const [simNode] = connected.getChildren();
    assert.strictEqual(provider.getChildren(simNode).length, 0, 'available provider never shows watch children');

    provider.addWatch(idA, 'dyn.cannon.impactTime');
    await new Promise((r) => setTimeout(r, 100));

    const watchChildren = connected.getChildren(simNode);
    assert.strictEqual(watchChildren.length, 1);
    assert.strictEqual(connected.getTreeItem(watchChildren[0]).label, 'dyn.cannon.impactTime');

    // SIM_a sorts before SIM_z alphabetically, but connecting it second must
    // not reorder Connected - only the available (disconnected) list sorts.
    discovery.set([makeAnnouncement(idA, '/sims/SIM_z'), makeAnnouncement(idB, '/sims/SIM_a')]);
    provider.refreshFromDiscovery();
    provider.connect(idB);
    await new Promise((r) => setTimeout(r, 100));

    const order = connected.getChildren().map((n: { id: string }) => n.id);
    assert.deepStrictEqual(order, [idA, idB]);

    a.server.close();
    b.server.close();
    provider.dispose();
  });
});
