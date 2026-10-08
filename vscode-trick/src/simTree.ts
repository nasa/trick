import * as fs from 'fs';
import * as path from 'path';
import * as vscode from 'vscode';
import { SimAnnouncement, SimDiscovery } from './simDiscovery';
import { ParsedValue, SimStatus, VarServerClient } from './varServerClient';
import { SieModel, parseSieResource, unwrapContainerElementType } from './sieResource';
import { chainTextAt } from './pythonDefinitions';

/**
 * Backs the Trick activity bar's two TreeViews, replacing the Java Sim
 * Sniffer (discovery), Trick View (live watch), and Sim Control Panel
 * (run/freeze/stop) tools. See the Tier 2 plan for the protocol research
 * this is built against.
 *
 * `TrickSimsTreeProvider` owns all the state (discovered/connected sims,
 * their clients, their watches) and is itself the provider for the
 * "Sims" (available) view. `ConnectedSimsTreeProvider` is a thin
 * read-only wrapper over the same state for the "Connected" view, so a
 * sim is only ever in one view at a time and doesn't have to be chased
 * around the available list as other sims come and go on the network.
 */

type SimState = 'disconnected' | 'connecting' | 'frozen' | 'running' | 'ended';

interface SimEntry {
  id: string;
  // VS Code's TreeView matches refreshed elements by object identity, so
  // nodes must be the same objects every time getChildren hands them out.
  node: SimTreeNode;
  watchNodes: Map<string, WatchTreeNode>;
  announcement?: SimAnnouncement;
  manualHost?: string;
  manualPort?: number;
  client?: VarServerClient;
  status?: SimStatus;
  values: Map<string, ParsedValue>;
  watches: string[];
  state: SimState;
}

export interface SimTreeNode {
  kind: 'sim';
  id: string;
}

export interface WatchTreeNode {
  kind: 'watch';
  simId: string;
  name: string;
}

export type TrickTreeNode = SimTreeNode | WatchTreeNode;

type Pane = 'available' | 'connected';

const REFRESH_COALESCE_MS = 250;

function watchDescription(value: ParsedValue | undefined): string {
  if (!value) {
    return '…';
  }
  if (value.value === 'BAD_REF') {
    return 'BAD_REF ⚠';
  }
  return value.units ? `${value.value} {${value.units}}` : value.value;
}

function simLabel(entry: SimEntry): string {
  if (entry.announcement) {
    return path.basename(entry.announcement.simDir);
  }
  return `${entry.manualHost}:${entry.manualPort}`;
}

function modeLabel(mode: number): string {
  switch (mode) {
    case 0:
      return 'Initializing';
    case 1:
      return 'Frozen';
    case 4:
      return 'Step';
    case 5:
      return 'Running';
    case 6:
      return 'Ended';
    default:
      return `Mode ${mode}`;
  }
}

function stateForMode(mode: number): SimState {
  if (mode === 5) {
    return 'running';
  }
  if (mode === 6) {
    return 'ended';
  }
  // Init/Freeze/Step all sit still until exec_run() is sent, so Run is the
  // relevant button for each of them - see Executive_loop.cpp's freeze_command.
  return 'frozen';
}

export class TrickSimsTreeProvider implements vscode.TreeDataProvider<TrickTreeNode> {
  private readonly changeEmitter = new vscode.EventEmitter<TrickTreeNode[] | undefined>();
  readonly onDidChangeTreeData = this.changeEmitter.event;
  private pendingFull = false;
  private readonly pendingNodes = new Set<TrickTreeNode>();

  private readonly entries = new Map<string, SimEntry>();
  // Ids in the order they were connected, so the Connected pane doesn't
  // reorder a sim out from under you the way the alphabetized available
  // list would.
  private readonly connectOrder: string[] = [];
  private refreshTimer: ReturnType<typeof setTimeout> | undefined;
  private manualCounter = 0;

  constructor(
    private readonly discovery: SimDiscovery,
    private readonly workspaceState: vscode.Memento,
    private readonly output: vscode.OutputChannel
  ) {}

  dispose(): void {
    if (this.refreshTimer) {
      clearTimeout(this.refreshTimer);
    }
    for (const entry of this.entries.values()) {
      entry.client?.close();
    }
    this.changeEmitter.dispose();
  }

  // --- TreeDataProvider ---------------------------------------------------

  getTreeItem(node: TrickTreeNode): vscode.TreeItem {
    return this.treeItemFor(node, 'available');
  }

  getChildren(node?: TrickTreeNode): TrickTreeNode[] {
    return this.childrenFor('available', node);
  }

  /** Shared by both views - see ConnectedSimsTreeProvider below. */
  treeItemFor(node: TrickTreeNode, pane: Pane): vscode.TreeItem {
    if (node.kind === 'watch') {
      return this.watchTreeItem(node);
    }
    return this.simTreeItem(node, pane);
  }

  /** Shared by both views - see ConnectedSimsTreeProvider below. */
  childrenFor(pane: Pane, node?: TrickTreeNode): TrickTreeNode[] {
    if (!node) {
      return this.rootsFor(pane);
    }
    // Watches are only ever shown nested under a sim in the Connected pane -
    // the available list is just names to connect to, not a thing to drill
    // into, and a disconnected sim can't report values for its saved watches
    // anyway.
    if (node.kind !== 'sim' || pane !== 'connected') {
      return [];
    }
    const entry = this.entries.get(node.id);
    if (!entry) {
      return [];
    }
    return entry.watches.map((name) => this.watchNode(entry, name));
  }

  simNode(id: string): SimTreeNode | undefined {
    return this.entries.get(id)?.node;
  }

  private watchNode(entry: SimEntry, name: string): WatchTreeNode {
    let node = entry.watchNodes.get(name);
    if (!node) {
      node = { kind: 'watch', simId: entry.id, name };
      entry.watchNodes.set(name, node);
    }
    return node;
  }

  private rootsFor(pane: Pane): SimTreeNode[] {
    const ids = [...this.entries.keys()].filter(
      (id) => (this.entries.get(id)!.state !== 'disconnected') === (pane === 'connected')
    );
    if (pane === 'connected') {
      // connectOrder may still mention ids that moved back to 'available' or
      // were deleted - filter rather than assume it's been kept in sync.
      return this.connectOrder.filter((id) => ids.includes(id)).map((id) => this.entries.get(id)!.node);
    }
    return ids
      .sort((a, b) => simLabel(this.entries.get(a)!).localeCompare(simLabel(this.entries.get(b)!)))
      .map((id) => this.entries.get(id)!.node);
  }

  private simTreeItem(node: SimTreeNode, pane: Pane): vscode.TreeItem {
    const entry = this.entries.get(node.id);
    if (!entry) {
      return new vscode.TreeItem('(gone)');
    }
    const hasVisibleWatches = pane === 'connected' && entry.watches.length > 0;
    const item = new vscode.TreeItem(
      simLabel(entry),
      hasVisibleWatches ? vscode.TreeItemCollapsibleState.Expanded : vscode.TreeItemCollapsibleState.None
    );
    item.id = `sim:${entry.id}`;
    item.contextValue = `sim.${entry.state}`;
    item.description = this.simDescription(entry);
    item.tooltip = this.simTooltip(entry);
    item.iconPath = this.simIcon(entry);
    return item;
  }

  private simDescription(entry: SimEntry): string {
    const parts: string[] = [];
    if (entry.announcement) {
      parts.push(path.basename(path.dirname(entry.announcement.inputFile || '') || entry.announcement.simDir));
    }
    if (entry.state === 'connecting') {
      parts.push('Connecting…');
    } else if (entry.state === 'disconnected') {
      const lastKnownMode = entry.announcement?.execMode;
      parts.push(lastKnownMode !== undefined ? `${modeLabel(lastKnownMode)} (not connected)` : 'Not connected');
    } else if (entry.state === 'ended') {
      parts.push('Ended');
    } else if (entry.status) {
      parts.push(modeLabel(entry.status.mode));
    }
    return parts.filter(Boolean).join(' · ');
  }

  private simTooltip(entry: SimEntry): string {
    const a = entry.announcement;
    if (!a) {
      return `${entry.manualHost}:${entry.manualPort}`;
    }
    return [
      `${a.simDir}`,
      `Input: ${a.inputFile}`,
      `Host: ${a.host}  User: ${a.user}  PID: ${a.pid}`,
      `Variable server port: ${a.port}`,
      `Trick version: ${a.version}`,
    ].join('\n');
  }

  private simIcon(entry: SimEntry): vscode.ThemeIcon {
    switch (entry.state) {
      case 'running':
        return new vscode.ThemeIcon('debug-start');
      case 'frozen':
        return new vscode.ThemeIcon('debug-pause');
      case 'ended':
        return new vscode.ThemeIcon('circle-slash');
      case 'connecting':
        return new vscode.ThemeIcon('sync~spin');
      default:
        return new vscode.ThemeIcon('debug-disconnect');
    }
  }

  private watchTreeItem(node: WatchTreeNode): vscode.TreeItem {
    const entry = this.entries.get(node.simId);
    const item = new vscode.TreeItem(node.name, vscode.TreeItemCollapsibleState.None);
    item.id = `sim:${node.simId}:watch:${node.name}`;
    item.contextValue = 'watch';
    if (!entry?.client) {
      item.description = '(disconnected)';
      return item;
    }
    item.description = watchDescription(entry.values.get(node.name));
    return item;
  }

  // --- Refresh coalescing ---------------------------------------------------

  // Structural/state changes (connect, disconnect, mode change, watch list
  // edits, discovery) redraw everything. Live value updates only redraw the
  // watch rows whose text actually changed: re-rendering every row on every
  // 250ms value cycle replaces the sim row's inline buttons mid-click, which
  // is what made Connect/Disconnect/Freeze need several clicks.
  private scheduleFullRefresh(): void {
    this.pendingFull = true;
    this.armRefreshTimer();
  }

  private scheduleNodeRefresh(nodes: TrickTreeNode[]): void {
    nodes.forEach((n) => this.pendingNodes.add(n));
    this.armRefreshTimer();
  }

  private armRefreshTimer(): void {
    if (this.refreshTimer) {
      return;
    }
    this.refreshTimer = setTimeout(() => this.flushRefresh(), REFRESH_COALESCE_MS);
  }

  /** Fires any pending refresh now, so user-initiated actions show up without the coalescing delay. */
  private flushRefresh(): void {
    if (this.refreshTimer) {
      clearTimeout(this.refreshTimer);
      this.refreshTimer = undefined;
    }
    if (this.pendingFull) {
      this.changeEmitter.fire(undefined);
    } else if (this.pendingNodes.size > 0) {
      this.changeEmitter.fire([...this.pendingNodes]);
    }
    this.pendingFull = false;
    this.pendingNodes.clear();
  }

  private refreshNow(): void {
    this.pendingFull = true;
    this.flushRefresh();
  }

  // --- Discovery ------------------------------------------------------------

  refreshFromDiscovery(): void {
    const live = new Map(this.discovery.list().map((a) => [a.key, a]));

    for (const [key, announcement] of live) {
      const entry = this.entries.get(key);
      if (!entry) {
        this.entries.set(key, {
          id: key,
          node: { kind: 'sim', id: key },
          watchNodes: new Map(),
          announcement,
          values: new Map(),
          watches: this.loadWatches(announcement.simDir),
          state: 'disconnected',
        });
        continue;
      }
      entry.announcement = announcement;
      // Discovery only ever informs the *disconnected* description (see
      // simDescription) - actual state (and therefore which control buttons
      // show up) only ever comes from this extension's own client connection,
      // never from the broadcast, since sending exec_run()/freeze()/stop()
      // requires a live socket regardless of what the last broadcast said.
    }

    for (const [id, entry] of this.entries) {
      if (entry.announcement && !live.has(id) && entry.state === 'disconnected') {
        // Only ever shown in the available list and now gone from there too -
        // nothing useful left to show. A sim that's connecting/connected/ended
        // stays in the Connected pane even after it stops broadcasting, so it
        // doesn't disappear out from under you; see disconnect() for when
        // those get cleaned up.
        this.entries.delete(id);
      }
    }

    // Discovery only fires `changed` on an actual append/mode-change/expiry
    // (see SimDiscovery), so there's always something worth a redraw here;
    // the tree is small (a handful of sims at most), so a full refresh is
    // simpler than figuring out exactly which disconnected nodes' descriptions
    // moved and is still cheap.
    this.scheduleFullRefresh();
  }

  // --- Watch persistence ------------------------------------------------------

  private watchStorageKey(entry: Pick<SimEntry, 'announcement' | 'manualHost' | 'manualPort'>): string {
    const simDir = entry.announcement?.simDir;
    return `trick.sims.watches.${simDir ?? `${entry.manualHost}:${entry.manualPort}`}`;
  }

  private loadWatches(simDir: string): string[] {
    return this.workspaceState.get<string[]>(`trick.sims.watches.${simDir}`, []);
  }

  private saveWatches(entry: SimEntry): void {
    void this.workspaceState.update(this.watchStorageKey(entry), entry.watches);
  }

  // --- Connection lifecycle ---------------------------------------------------

  connectManual(host: string, port: number): void {
    const existing = [...this.entries.values()].find(
      (e) => e.manualHost === host && e.manualPort === port
    );
    if (existing) {
      void this.connectEntry(existing);
      return;
    }
    const id = `manual:${host}:${port}:${this.manualCounter++}`;
    const entry: SimEntry = {
      id,
      node: { kind: 'sim', id },
      watchNodes: new Map(),
      manualHost: host,
      manualPort: port,
      values: new Map(),
      watches: this.loadWatches(`${host}:${port}`),
      state: 'disconnected',
    };
    this.entries.set(id, entry);
    this.scheduleFullRefresh();
    void this.connectEntry(entry);
  }

  connect(id: string): void {
    const entry = this.entries.get(id);
    if (entry) {
      void this.connectEntry(entry);
    }
  }

  private async connectEntry(entry: SimEntry): Promise<void> {
    if (entry.state !== 'disconnected') {
      // Already connecting/connected - a repeated click on the Connect
      // button before the (debounced) refresh has hidden it would otherwise
      // spin up a second client racing the first one for the same entry.
      return;
    }
    const host = entry.announcement?.host ?? entry.manualHost;
    const port = entry.announcement?.port ?? entry.manualPort;
    if (!host || !port) {
      vscode.window.showErrorMessage(`Cannot connect to ${simLabel(entry)}: no host/port recorded for it.`);
      return;
    }
    entry.state = 'connecting';
    if (!this.connectOrder.includes(entry.id)) {
      this.connectOrder.push(entry.id);
    }
    this.refreshNow();

    const client = new VarServerClient(host, port);
    entry.client = client;
    // Every handler below checks entry.client === client first, so a client
    // superseded or explicitly closed (see disconnect()) can't clobber state
    // that a newer client - or a manual disconnect - already set.
    client.on('status', (status: SimStatus) => {
      if (entry.client !== client) {
        return;
      }
      const previousMode = entry.status?.mode;
      const previousState = entry.state;
      entry.status = status;
      entry.state = stateForMode(status.mode);
      // Status arrives every value cycle; the sim row only shows mode, so
      // only a mode change is worth redrawing it (and its buttons) for.
      if (entry.state !== previousState || status.mode !== previousMode) {
        this.refreshNow();
      }
    });
    client.on('values', (values: Map<string, ParsedValue>) => {
      if (entry.client !== client) {
        return;
      }
      const changed = entry.watches.filter(
        (name) => watchDescription(entry.values.get(name)) !== watchDescription(values.get(name))
      );
      entry.values = values;
      if (changed.length > 0) {
        this.scheduleNodeRefresh(changed.map((name) => this.watchNode(entry, name)));
      }
    });
    client.on('closed', () => {
      if (entry.client !== client) {
        return;
      }
      entry.client = undefined;
      entry.state = 'ended';
      this.refreshNow();
    });

    try {
      await client.connect();
      if (entry.client !== client) {
        return;
      }
      if (entry.watches.length > 0) {
        client.setWatches(entry.watches);
      }
    } catch (err) {
      this.output.appendLine(`Connect to ${host}:${port} failed: ${(err as Error).message}`);
      vscode.window.showErrorMessage(`Could not connect to the variable server at ${host}:${port}: ${(err as Error).message}`);
      if (entry.client === client) {
        entry.client = undefined;
      }
      entry.state = 'disconnected';
      this.removeFromConnectOrder(entry.id);
      this.refreshNow();
    }
  }

  private removeFromConnectOrder(id: string): void {
    const idx = this.connectOrder.indexOf(id);
    if (idx !== -1) {
      this.connectOrder.splice(idx, 1);
    }
  }

  disconnect(id: string): void {
    const entry = this.entries.get(id);
    if (!entry) {
      return;
    }
    entry.client?.close();
    entry.client = undefined;
    entry.status = undefined;
    entry.values = new Map();
    this.removeFromConnectOrder(id);
    if (this.discovery.list().some((a) => a.key === id)) {
      // Still broadcasting - drop back to the available list rather than
      // disappearing, since discovery will keep this entry alive anyway.
      entry.state = 'disconnected';
    } else {
      // Not (or no longer) visible through discovery - manual connections
      // and sims that stopped broadcasting while connected both have nothing
      // useful left to show once disconnected, so drop the node entirely
      // rather than leaving a dead entry.
      this.entries.delete(id);
    }
    this.refreshNow();
  }

  // --- Sim control --------------------------------------------------------

  run(id: string): void {
    this.entries.get(id)?.client?.run();
  }

  freeze(id: string): void {
    this.entries.get(id)?.client?.freeze();
  }

  stop(id: string): void {
    this.entries.get(id)?.client?.stop();
  }

  // --- Watches --------------------------------------------------------------

  getEntry(id: string): Readonly<SimEntry> | undefined {
    return this.entries.get(id);
  }

  connectedEntries(): { id: string; label: string }[] {
    return [...this.entries.values()]
      .filter((e) => e.client)
      .map((e) => ({ id: e.id, label: simLabel(e) }));
  }

  async checkVarExists(id: string, name: string): Promise<boolean> {
    const client = this.entries.get(id)?.client;
    if (!client) {
      return false;
    }
    try {
      return await client.varExists(name);
    } catch {
      return false;
    }
  }

  addWatch(id: string, name: string): void {
    const entry = this.entries.get(id);
    if (!entry || entry.watches.includes(name)) {
      return;
    }
    entry.watches = [...entry.watches, name];
    this.saveWatches(entry);
    entry.client?.setWatches(entry.watches);
    this.refreshNow();
  }

  removeWatch(simId: string, name: string): void {
    const entry = this.entries.get(simId);
    if (!entry) {
      return;
    }
    entry.watches = entry.watches.filter((w) => w !== name);
    entry.watchNodes.delete(name);
    this.saveWatches(entry);
    entry.client?.setWatches(entry.watches);
    this.refreshNow();
  }

  /** Reads S_sie.resource for a sim, if it's reachable on the local filesystem. */
  readSieModel(id: string): SieModel | undefined {
    const simDir = this.entries.get(id)?.announcement?.simDir;
    if (!simDir) {
      return undefined;
    }
    const siePath = path.join(simDir, 'S_sie.resource');
    try {
      return parseSieResource(fs.readFileSync(siePath, 'utf8'));
    } catch {
      return undefined;
    }
  }
}

/**
 * TreeDataProvider for the "Connected" view. All state lives in
 * TrickSimsTreeProvider - this only asks for the connected-pane roots and
 * shares its refresh event, so both views redraw together whenever a sim
 * connects, disconnects, or its status/values change.
 */
export class ConnectedSimsTreeProvider implements vscode.TreeDataProvider<TrickTreeNode> {
  readonly onDidChangeTreeData: vscode.Event<TrickTreeNode[] | undefined>;

  constructor(private readonly model: TrickSimsTreeProvider) {
    this.onDidChangeTreeData = model.onDidChangeTreeData;
  }

  getTreeItem(node: TrickTreeNode): vscode.TreeItem {
    return this.model.treeItemFor(node, 'connected');
  }

  getChildren(node?: TrickTreeNode): TrickTreeNode[] {
    return this.model.childrenFor('connected', node);
  }

  // Required for TreeView.reveal(), which is how a sim gets auto-expanded
  // after adding a watch.
  getParent(node: TrickTreeNode): TrickTreeNode | undefined {
    return node.kind === 'watch' ? this.model.simNode(node.simId) : undefined;
  }
}

/** Child member names of a SIE class, with container types unwrapped to their element class. */
export function sieMemberChoices(
  model: SieModel,
  className: string
): { name: string; detail: string; nextClass: string | undefined }[] {
  const members = model.classes.get(className) ?? [];
  return members.map((m) => {
    const elementType = unwrapContainerElementType(m.type);
    const target = (elementType ?? m.type).replace(/\*+$/, '').trim();
    const nextClass = model.classes.has(target) ? target : undefined;
    const units = m.units && m.units !== '--' ? ` [${m.units}]` : '';
    return { name: m.name, detail: `${m.type}${units}`, nextClass };
  });
}

interface WatchPickItem extends vscode.QuickPickItem {
  action: 'manual' | 'member' | 'stop';
  memberName?: string;
  nextClass?: string;
}

/**
 * Drills into a sim's S_sie.resource one level at a time (top-level object,
 * then member, then member-of-member, ...) so the user can build a watch path
 * without typing it from memory, stopping at any level since not every
 * watchable path bottoms out at a primitive (e.g. watching a whole small
 * struct's scalar fields individually isn't required - var_exists will reject
 * an invalid stop point anyway). Falls back to a plain InputBox when the sim's
 * S_sie.resource isn't reachable on the local filesystem (e.g. a sim
 * discovered on a different host).
 */
async function promptForWatchPath(provider: TrickSimsTreeProvider, simId: string): Promise<string | undefined> {
  const model = provider.readSieModel(simId);
  if (!model) {
    return vscode.window.showInputBox({
      prompt: 'Variable path to watch',
      placeHolder: 'ball.state.output.position[0]',
    });
  }

  const topItems: WatchPickItem[] = [
    { label: '$(edit) Enter a path manually...', action: 'manual' },
    ...model.topLevel.map((o) => ({ label: o.name, description: o.type, action: 'member' as const, memberName: o.name, nextClass: o.type })),
  ];
  const first = await vscode.window.showQuickPick(topItems, { placeHolder: 'Select a top-level sim object' });
  if (!first) {
    return undefined;
  }
  if (first.action === 'manual') {
    return vscode.window.showInputBox({ prompt: 'Variable path to watch' });
  }

  let currentPath = first.memberName!;
  let currentClass = first.nextClass;

  while (currentClass && model.classes.has(currentClass)) {
    const members = sieMemberChoices(model, currentClass);
    if (members.length === 0) {
      break;
    }
    const items: WatchPickItem[] = [
      { label: `$(check) Watch "${currentPath}"`, action: 'stop' },
      ...members.map((m) => ({ label: m.name, description: m.detail, action: 'member' as const, memberName: m.name, nextClass: m.nextClass })),
    ];
    const picked = await vscode.window.showQuickPick(items, { placeHolder: currentPath });
    if (!picked) {
      return undefined;
    }
    if (picked.action === 'stop') {
      break;
    }
    currentPath = `${currentPath}.${picked.memberName}`;
    currentClass = picked.nextClass;
  }
  return currentPath;
}

export function registerSimCommands(
  context: vscode.ExtensionContext,
  provider: TrickSimsTreeProvider,
  connectedView: vscode.TreeView<TrickTreeNode>
): void {
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  const cmd = (id: string, handler: (...args: any[]) => unknown) =>
    context.subscriptions.push(vscode.commands.registerCommand(id, handler));

  // Adding a watch flips the sim's collapsibleState from None to Expanded
  // (see simTreeItem), but the TreeView doesn't re-apply collapsibleState
  // itself for an already-rendered item - without this it stays collapsed
  // until the user clicks the arrow themselves. reveal({expand: true}) forces
  // it open against the item's current (now-expanded) state.
  const expandAfterWatch = (simId: string) => {
    const node = provider.simNode(simId);
    if (node) {
      void connectedView.reveal(node, { expand: true, select: false, focus: false });
    }
  };

  cmd('trick.sims.connectManual', async () => {
    const input = await vscode.window.showInputBox({
      prompt: 'Connect to a variable server at host:port',
      placeHolder: 'localhost:12345',
      validateInput: (v) => (/^.+:\d+$/.test(v.trim()) ? undefined : 'Enter host:port'),
    });
    if (!input) {
      return;
    }
    const idx = input.lastIndexOf(':');
    const host = input.slice(0, idx).trim();
    const port = parseInt(input.slice(idx + 1).trim(), 10);
    provider.connectManual(host, port);
  });

  cmd('trick.sims.connect', (node: SimTreeNode) => provider.connect(node.id));
  cmd('trick.sims.disconnect', (node: SimTreeNode) => provider.disconnect(node.id));
  cmd('trick.sims.run', (node: SimTreeNode) => provider.run(node.id));
  cmd('trick.sims.freeze', (node: SimTreeNode) => provider.freeze(node.id));
  cmd('trick.sims.stop', async (node: SimTreeNode) => {
    const confirm = await vscode.window.showWarningMessage(
      'Stop this sim? This ends the run.',
      { modal: true },
      'Stop'
    );
    if (confirm === 'Stop') {
      provider.stop(node.id);
    }
  });

  cmd('trick.sims.addWatch', async (node: SimTreeNode) => {
    const entry = provider.getEntry(node.id);
    if (!entry?.client) {
      vscode.window.showWarningMessage('Connect to this sim first.');
      return;
    }
    const varPath = await promptForWatchPath(provider, node.id);
    if (!varPath) {
      return;
    }
    const exists = await provider.checkVarExists(node.id, varPath);
    if (!exists) {
      vscode.window.showErrorMessage(`"${varPath}" isn't a valid variable on this sim.`);
      return;
    }
    provider.addWatch(node.id, varPath);
    expandAfterWatch(node.id);
  });

  cmd('trick.sims.removeWatch', (node: WatchTreeNode) => provider.removeWatch(node.simId, node.name));

  cmd('trick.sims.watchFromEditor', async () => {
    const editor = vscode.window.activeTextEditor;
    if (!editor) {
      return;
    }
    const lineText = editor.document.lineAt(editor.selection.active.line).text;
    const chain = chainTextAt(lineText, editor.selection.active.character);
    if (!chain) {
      vscode.window.showWarningMessage('No variable path found under the cursor.');
      return;
    }
    const connected = provider.connectedEntries();
    if (connected.length === 0) {
      vscode.window.showWarningMessage('Connect to a sim in the Trick Sims view first.');
      return;
    }
    let simId = connected[0].id;
    if (connected.length > 1) {
      const picked = await vscode.window.showQuickPick(
        connected.map((c) => ({ label: c.label, id: c.id })),
        { placeHolder: `Watch "${chain}" on which sim?` }
      );
      if (!picked) {
        return;
      }
      simId = picked.id;
    }
    const exists = await provider.checkVarExists(simId, chain);
    if (!exists) {
      vscode.window.showErrorMessage(`"${chain}" isn't a valid variable on that sim.`);
      return;
    }
    provider.addWatch(simId, chain);
    expandAfterWatch(simId);
  });
}
