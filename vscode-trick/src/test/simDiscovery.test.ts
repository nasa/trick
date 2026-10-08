import * as assert from 'assert';
import { parseBroadcast } from '../simDiscovery';

describe('parseBroadcast', () => {
  it('parses a full 12-field broadcast line', () => {
    const line = [
      'myhost', '48385', 'alice', '12345', '/sims/SIM_cannon_analytic',
      'S_main_Linux.exe', 'RUN_test/input.py', '19.6.0', 'tag', '48385', '1', '5',
    ].join('\t');
    const a = parseBroadcast(line);
    assert.ok(a);
    assert.strictEqual(a!.host, 'myhost');
    assert.strictEqual(a!.port, 48385);
    assert.strictEqual(a!.user, 'alice');
    assert.strictEqual(a!.pid, 12345);
    assert.strictEqual(a!.simDir, '/sims/SIM_cannon_analytic');
    assert.strictEqual(a!.sMain, 'S_main_Linux.exe');
    assert.strictEqual(a!.inputFile, 'RUN_test/input.py');
    assert.strictEqual(a!.version, '19.6.0');
    assert.strictEqual(a!.vsEnabled, true);
    assert.strictEqual(a!.execMode, 5);
    assert.strictEqual(a!.key, 'myhost:48385:12345');
  });

  it('tolerates an older 9-field broadcast line (no vsEnabled/execMode)', () => {
    const line = [
      'myhost', '48385', 'alice', '12345', '/sims/SIM_cannon_analytic',
      'S_main_Linux.exe', 'RUN_test/input.py', '17.0.0', 'tag',
    ].join('\t');
    const a = parseBroadcast(line);
    assert.ok(a);
    assert.strictEqual(a!.vsEnabled, true);
    assert.strictEqual(a!.execMode, undefined);
  });

  it('treats vsEnabled "0" as disabled', () => {
    const line = [
      'myhost', '48385', 'alice', '12345', '/sims/SIM_x',
      'S_main_Linux.exe', 'RUN_test/input.py', '19.6.0', 'tag', '48385', '0', '1',
    ].join('\t');
    const a = parseBroadcast(line);
    assert.strictEqual(a!.vsEnabled, false);
  });

  it('returns undefined for a malformed line', () => {
    assert.strictEqual(parseBroadcast('not a real broadcast'), undefined);
    assert.strictEqual(parseBroadcast(''), undefined);
    assert.strictEqual(parseBroadcast('host\tnotaport\tuser\t1\tdir\tmain\tinput\tver\ttag'), undefined);
  });
});
