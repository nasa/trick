import * as dgram from 'dgram';
import { EventEmitter } from 'events';

/**
 * Discovers running sims via Trick's variable server broadcast, replacing the
 * Java Sim Sniffer tool. A sim's variable server broadcasts a UDP line to both
 * `224.3.14.15` and `239.3.14.15` on port 9265 roughly every 2 seconds, as
 * long as `trick.var_server_set_vdieonbroadcast` wasn't used to disable it
 * (see VariableServerListenThread.cpp:215-219, 283-286). Broadcasting is on
 * by default, except on macOS, where multicast is commonly blocked - manual
 * host:port connect is the fallback for that case.
 *
 * Packet shape (tab-separated, one line, no trailing fields guaranteed):
 *   host  port  user  pid  simDir  S_main  inputFile  version  tag  port  vsEnabled  execMode
 * Older Trick versions only send the first 9 fields - vsEnabled/execMode
 * default to "enabled"/unknown when absent, since those sims predate the
 * fields this view would otherwise use to disable Run/Freeze/Stop.
 */

export interface SimAnnouncement {
  /** Stable identity for this sim process - a relaunch gets a new key. */
  key: string;
  host: string;
  port: number;
  user: string;
  pid: number;
  simDir: string;
  sMain: string;
  inputFile: string;
  version: string;
  tag: string;
  vsEnabled: boolean;
  /** 0 Init, 1 Freeze, 4 Step, 5 Run, 6 Exit - undefined if the packet didn't say. */
  execMode: number | undefined;
}

export function parseBroadcast(line: string): SimAnnouncement | undefined {
  const fields = line.trim().split('\t');
  if (fields.length < 9) {
    return undefined;
  }
  const [host, portStr, user, pidStr, simDir, sMain, inputFile, version, tag, , vsEnabledStr, execModeStr] = fields;
  const port = parseInt(portStr, 10);
  const pid = parseInt(pidStr, 10);
  if (!host || Number.isNaN(port) || Number.isNaN(pid) || !simDir) {
    return undefined;
  }
  return {
    key: `${host}:${port}:${pid}`,
    host,
    port,
    user,
    pid,
    simDir,
    sMain,
    inputFile,
    version,
    tag,
    vsEnabled: vsEnabledStr === undefined ? true : vsEnabledStr === '1',
    execMode: execModeStr === undefined ? undefined : parseInt(execModeStr, 10),
  };
}

const MULTICAST_GROUPS = ['224.3.14.15', '239.3.14.15'];
const DISCOVERY_PORT = 9265;
const EXPIRY_MS = 5000;
const SWEEP_MS = 1000;

interface TrackedSim {
  announcement: SimAnnouncement;
  lastSeen: number;
}

/**
 * Listens for sim broadcasts and emits `changed` whenever the set of live
 * sims, or any sim's exec mode, changes. Does not open a socket until
 * `start()` is called - the tree view calls that lazily, the first time it
 * becomes visible, so sims that never open the view pay no discovery cost.
 */
export class SimDiscovery extends EventEmitter {
  private socket: dgram.Socket | undefined;
  private sweepTimer: ReturnType<typeof setInterval> | undefined;
  private readonly sims = new Map<string, TrackedSim>();

  constructor(private readonly log: (message: string) => void = () => undefined) {
    super();
  }

  start(): void {
    if (this.socket) {
      return;
    }
    const socket = dgram.createSocket({ type: 'udp4', reuseAddr: true });
    socket.on('error', (err) => {
      this.log(`Sim discovery disabled: ${err.message}`);
      this.stop();
    });
    socket.on('message', (msg) => this.handleMessage(msg.toString('utf8')));
    socket.bind(DISCOVERY_PORT, () => {
      for (const group of MULTICAST_GROUPS) {
        try {
          socket.addMembership(group);
        } catch (err) {
          this.log(`Could not join multicast group ${group}: ${(err as Error).message}`);
        }
      }
    });
    this.socket = socket;
    this.sweepTimer = setInterval(() => this.sweepExpired(), SWEEP_MS);
  }

  stop(): void {
    this.socket?.close();
    this.socket = undefined;
    if (this.sweepTimer) {
      clearInterval(this.sweepTimer);
      this.sweepTimer = undefined;
    }
  }

  dispose(): void {
    this.stop();
    this.removeAllListeners();
  }

  list(): SimAnnouncement[] {
    return [...this.sims.values()].map((t) => t.announcement);
  }

  private handleMessage(line: string): void {
    const announcement = parseBroadcast(line);
    if (!announcement) {
      return;
    }
    const existing = this.sims.get(announcement.key);
    const changed = !existing || existing.announcement.execMode !== announcement.execMode;
    this.sims.set(announcement.key, { announcement, lastSeen: Date.now() });
    if (changed) {
      this.emit('changed');
    }
  }

  private sweepExpired(): void {
    const now = Date.now();
    let changed = false;
    for (const [key, tracked] of this.sims) {
      if (now - tracked.lastSeen > EXPIRY_MS) {
        this.sims.delete(key);
        changed = true;
      }
    }
    if (changed) {
      this.emit('changed');
    }
  }
}
