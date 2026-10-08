import * as assert from 'assert';
import * as cp from 'child_process';
import * as fs from 'fs';
import * as path from 'path';
import { parseSieResource, generateSieStub } from '../sieResource';

const REPO_ROOT = path.resolve(__dirname, '../../..');
const BALL_SIE = path.join(REPO_ROOT, 'trick_sims/Ball/SIM_ball_L1/S_sie.resource');
const ROBOT_SIE = path.join(REPO_ROOT, 'trick_sims/SIM_robot/S_sie.resource');

function assertValidPython(source: string): void {
  const result = cp.spawnSync('python3', ['-c', 'import ast,sys; ast.parse(sys.stdin.read())'], {
    input: source,
    encoding: 'utf8',
  });
  assert.strictEqual(
    result.status,
    0,
    `generated stub is not valid Python:\n${result.stderr}\n---\n${source}`
  );
}

describe('sieResource', () => {
  describe('parseSieResource', () => {
    it('parses top_level_object -> class -> nested class -> array member from a real built Ball sim', function () {
      if (!fs.existsSync(BALL_SIE)) {
        this.skip();
      }
      const model = parseSieResource(fs.readFileSync(BALL_SIE, 'utf8'));

      const ball = model.topLevel.find((o) => o.name === 'ball');
      assert.ok(ball, 'expected a "ball" top_level_object');
      assert.strictEqual(ball!.type, 'ballSimObject');

      const ballClass = model.classes.get('ballSimObject');
      assert.ok(ballClass);
      assert.ok(ballClass!.some((m) => m.name === 'state' && m.type === 'BSTATE'));

      const bstateIn = model.classes.get('BSTATE_IN');
      assert.ok(bstateIn);
      const position = bstateIn!.find((m) => m.name === 'position');
      assert.deepStrictEqual(position?.dims, [2]);
      assert.strictEqual(position?.units, 'm');
      assert.strictEqual(position?.description, 'X(horizontal),Y(vertical) position');
    });

    it('parses enumerations and enum-typed members from SIM_robot', function () {
      if (!fs.existsSync(ROBOT_SIE)) {
        this.skip();
      }
      const model = parseSieResource(fs.readFileSync(ROBOT_SIE, 'utf8'));
      const controlFrame = model.enums.get('ControlFrame');
      assert.ok(controlFrame);
      assert.deepStrictEqual(controlFrame, [
        { label: 'Task', value: '0' },
        { label: 'EE', value: '1' },
      ]);

      const manipControl = model.classes.get('ManipControl');
      assert.ok(manipControl?.some((m) => m.name === 'manualFrame' && m.type === 'ControlFrame'));
    });

    it('does not truncate a member at an unescaped ">" inside an STL type or a description', () => {
      // Trick's XML writer escapes &, ", and < but not > (confirmed against
      // real S_sie.resource output), so both STL template types and
      // free-text descriptions routinely contain a literal, unescaped ">"
      // inside a quoted attribute value - a naive "stop at the next >"
      // scanner would truncate the tag right there.
      const xml = `<sie>
  <class name="Holder">
    <member
      name="items"
      type="std__vector&lt;Foo *>"
      io_attributes="15"
      units="--"
      description="count > 0, see task->base">
    </member>
  </class>
</sie>`;
      const model = parseSieResource(xml);
      const members = model.classes.get('Holder');
      assert.strictEqual(members?.length, 1);
      assert.strictEqual(members![0].type, 'std__vector<Foo *>');
      assert.strictEqual(members![0].description, 'count > 0, see task->base');
    });
  });

  describe('generateSieStub', () => {
    const xml = `<sie>
  <class name="Unused"><member name="x" type="int" units="--"></member></class>
  <class name="Foo"><member name="n" type="int" units="--" description="a count"></member></class>
  <enumeration name="MyEnum"><pair label ="A" value="0"/></enumeration>
  <class name="Holder">
    <member name="items" type="std__vector&lt;Foo *>" units="--"></member>
    <member name="flag" type="MyEnum" units="--"></member>
    <member name="class" type="int" units="--"></member>
    <member name="position" type="double" units="m"><dimension>2</dimension></member>
  </class>
  <top_level_object name="h" type="Holder"></top_level_object>
</sie>`;

    it('unwraps vector<T>, maps enums to int, skips keyword-named members, wraps fixed arrays, and omits unreachable classes', () => {
      const model = parseSieResource(xml);
      const stub = generateSieStub(model);

      assert.ok(stub.includes('class Holder:'));
      assert.ok(stub.includes('items: TrickArray[Foo]'));
      assert.ok(stub.includes('flag: int'));
      assert.ok(stub.includes('position: TrickArray[Any]'));
      assert.ok(!/^\s+class: /m.test(stub), 'the Python-keyword member "class" must be skipped');
      assert.ok(!stub.includes('class Unused:'), 'classes unreachable from any top_level_object are omitted');
      assertValidPython(stub);
    });

    it('includes a units/description docstring under each member, with the original SIE type (not the Python type)', () => {
      const model = parseSieResource(xml);
      const stub = generateSieStub(model);
      // "n" is a primitive `int` member, deliberately typed `Any` in Python
      // (see pyType) - but the docstring should still show its real SIE type.
      assert.ok(stub.includes('n: Any'));
      assert.ok(stub.includes("'int - a count'"));
    });

    it('marks an empty class body with "..."', () => {
      const emptyXml =
        '<sie><class name="Empty"></class><top_level_object name="e" type="Empty"></top_level_object></sie>';
      const model = parseSieResource(emptyXml);
      const stub = generateSieStub(model);
      assert.match(stub, /class Empty:\n\s+\.\.\.\s*\n/);
      assertValidPython(stub);
    });

    it('safely escapes a description containing quote characters (a real description reads: Run directory name "RUN_<unique_tag>")', () => {
      const quoteXml = `<sie>
  <class name="RunDir">
    <member
      name="dirName"
      type="std__string"
      units="--"
      description="Run directory name &quot;RUN_&lt;unique_tag>&quot;">
    </member>
  </class>
  <top_level_object name="r" type="RunDir"></top_level_object>
</sie>`;
      const model = parseSieResource(quoteXml);
      const stub = generateSieStub(model);
      assertValidPython(stub);
    });
  });
});
