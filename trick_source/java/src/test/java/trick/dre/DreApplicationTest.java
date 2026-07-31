package trick.dre;

import static org.junit.Assert.*;

import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import org.junit.Test;

/**
 * Tests for DreApplication DR-file format generation and parsing.
 */
public class DreApplicationTest {

    // -----------------------------------------------------------------------
    // buildDrContent — save-format tests
    // -----------------------------------------------------------------------

    @Test
    public void testBuildDrContentNoGlobalBoilerplate() {
        String content = DreApplication.buildDrContent(
                "DRBinary", "myGroup", "DR_Always", "0.1", "False", Collections.emptyList(), "0", "B", "DR_Buffer");

        // Old code emitted these three lines; new code must not.
        assertFalse("Should not contain 'global DR_GROUP_ID'", content.contains("global DR_GROUP_ID"));
        assertFalse("Should not contain 'global drg'", content.contains("global drg"));
        assertFalse("Should not contain 'drg.append('", content.contains("drg.append("));
        assertFalse("Should not contain 'drg[DR_GROUP_ID]'", content.contains("drg[DR_GROUP_ID]"));
    }

    @Test
    public void testBuildDrContentNewDeclarationLine() {
        String content = DreApplication.buildDrContent(
                "DRBinary", "myGroup", "DR_Always", "0.1", "False", Collections.emptyList(), "0", "B", "DR_Buffer");

        assertTrue("Should declare dr_group with DRBinary", content.contains("dr_group = trick.DRBinary(\"myGroup\")"));
    }

    @Test
    public void testBuildDrContentSetCycleUsesNewVariable() {
        String content = DreApplication.buildDrContent(
                "DRBinary", "myGroup", "DR_Always", "0.5", "False", Collections.emptyList(), "0", "B", "DR_Buffer");

        // New format: dr_group.set_cycle(0.5)
        assertTrue("set_cycle should use dr_group prefix", content.contains("dr_group.set_cycle(0.5)"));
        // Old format: drg[DR_GROUP_ID].set_cycle(...) — must not appear
        assertFalse("set_cycle must not use old drg prefix", content.contains("drg[DR_GROUP_ID].set_cycle"));
    }

    @Test
    public void testBuildDrContentThisownLine() {
        String content = DreApplication.buildDrContent(
                "DRBinary", "myGroup", "DR_Always", "0.1", "False", Collections.emptyList(), "0", "B", "DR_Buffer");

        // Old code never emitted thisown; new code must.
        assertTrue("New format must include 'dr_group.thisown = 0'", content.contains("dr_group.thisown = 0"));
    }

    @Test
    public void testBuildDrContentVariables() {
        String content = DreApplication.buildDrContent(
                "DRAscii",
                "logGroup",
                "DR_Always",
                "0.01",
                "False",
                Arrays.asList("ball.obj.state.pos[0]", "ball.obj.state.pos[1]"),
                "0",
                "B",
                "DR_No_Buffer");

        assertTrue(content.contains("dr_group.add_variable(\"ball.obj.state.pos[0]\")"));
        assertTrue(content.contains("dr_group.add_variable(\"ball.obj.state.pos[1]\")"));
    }

    @Test
    public void testBuildDrContentAddDataRecordGroup() {
        String content = DreApplication.buildDrContent(
                "DRBinary", "g", "DR_Always", "0.1", "False", Collections.emptyList(), "1", "GiB", "DR_Ring_Buffer");

        assertTrue(
                "Should call trick.add_data_record_group with correct buffering",
                content.contains("trick.add_data_record_group(dr_group, trick.DR_Ring_Buffer)"));
    }

    @Test
    public void testBuildDrContentFileSizeGiB() {
        String content = DreApplication.buildDrContent(
                "DRBinary", "g", "DR_Always", "0.1", "False", Collections.emptyList(), "2", "GiB", "DR_Buffer");

        assertTrue("GiB should produce multiplier", content.contains("dr_group.set_max_file_size(2 * 1073741824)"));
    }

    @Test
    public void testBuildDrContentFileSizeBytes() {
        String content = DreApplication.buildDrContent(
                "DRBinary", "g", "DR_Always", "0.1", "False", Collections.emptyList(), "512", "B", "DR_Buffer");

        assertTrue("Bytes unit should not produce a multiplier", content.contains("dr_group.set_max_file_size(512)"));
    }

    // -----------------------------------------------------------------------
    // parseDrContent — new-format parsing tests
    // -----------------------------------------------------------------------

    @Test
    public void testParseNewFormatFormat() {
        String content = "dr_group = trick.DRBinary(\"myGroup\")\n" + "dr_group.set_freq(trick.DR_Always)\n"
                + "dr_group.set_cycle(0.5)\n"
                + "dr_group.set_single_prec_only(False)\n"
                + "trick.add_data_record_group(dr_group, trick.DR_Buffer)\n"
                + "dr_group.thisown = 0\n";

        DreApplication.DrFileData d = DreApplication.parseDrContent(content);

        assertEquals("DRBinary", d.format);
        assertEquals("myGroup", d.name);
    }

    @Test
    public void testParseNewFormatCycle() {
        String content = "dr_group = trick.DRBinary(\"g\")\n" + "dr_group.set_cycle(0.25)\n"
                + "trick.add_data_record_group(dr_group, trick.DR_Buffer)\n";

        DreApplication.DrFileData d = DreApplication.parseDrContent(content);
        assertEquals("0.25", d.cycle);
    }

    @Test
    public void testParseNewFormatFrequency() {
        String content = "dr_group = trick.DRAscii(\"g\")\n" + "dr_group.set_freq(trick.DR_Changes)\n"
                + "trick.add_data_record_group(dr_group, trick.DR_Buffer)\n";

        DreApplication.DrFileData d = DreApplication.parseDrContent(content);
        assertEquals("DR_Changes", d.frequency);
    }

    @Test
    public void testParseNewFormatVariables() {
        String content = "dr_group = trick.DRBinary(\"g\")\n" + "dr_group.add_variable(\"ball.pos[0]\")\n"
                + "dr_group.add_variable(\"ball.pos[1]\")\n"
                + "trick.add_data_record_group(dr_group, trick.DR_Buffer)\n";

        DreApplication.DrFileData d = DreApplication.parseDrContent(content);
        assertEquals(2, d.variables.size());
        assertEquals("ball.pos[0]", d.variables.get(0));
        assertEquals("ball.pos[1]", d.variables.get(1));
    }

    @Test
    public void testParseNewFormatBuffering() {
        String content =
                "dr_group = trick.DRBinary(\"g\")\n" + "trick.add_data_record_group(dr_group, trick.DR_No_Buffer)\n";

        DreApplication.DrFileData d = DreApplication.parseDrContent(content);
        assertEquals("DR_No_Buffer", d.buffering);
    }

    @Test
    public void testParseNewFormatSinglePrec() {
        String content = "dr_group = trick.DRBinary(\"g\")\n" + "dr_group.set_single_prec_only(True)\n"
                + "trick.add_data_record_group(dr_group, trick.DR_Buffer)\n";

        DreApplication.DrFileData d = DreApplication.parseDrContent(content);
        assertEquals("True", d.singlePrecOnly);
    }

    // -----------------------------------------------------------------------
    // parseDrContent — old-format backwards-compatibility tests
    // -----------------------------------------------------------------------

    @Test
    public void testParseOldFormatFormat() {
        String content = "global DR_GROUP_ID\n" + "global drg\n"
                + "try:\n"
                + "    if DR_GROUP_ID >= 0:\n"
                + "        DR_GROUP_ID += 1\n"
                + "except NameError:\n"
                + "    DR_GROUP_ID = 0\n"
                + "    drg = []\n\n"
                + "drg.append(trick.DRAscii(\"oldGroup\"))\n"
                + "drg[DR_GROUP_ID].set_freq(trick.DR_Always)\n"
                + "drg[DR_GROUP_ID].set_cycle(0.05)\n"
                + "drg[DR_GROUP_ID].set_single_prec_only(False)\n"
                + "drg[DR_GROUP_ID].add_variable(\"sim.x\")\n"
                + "drg[DR_GROUP_ID].set_max_file_size(1073741824)\n"
                + "trick.add_data_record_group(drg[DR_GROUP_ID], trick.DR_Buffer)\n"
                + "drg[DR_GROUP_ID].enable()\n";

        DreApplication.DrFileData d = DreApplication.parseDrContent(content);

        assertEquals("DRAscii", d.format);
        assertEquals("oldGroup", d.name);
        assertEquals("DR_Always", d.frequency);
        assertEquals("0.05", d.cycle);
        assertEquals("False", d.singlePrecOnly);
        assertEquals(1, d.variables.size());
        assertEquals("sim.x", d.variables.get(0));
        assertEquals("DR_Buffer", d.buffering);
        assertEquals("1073741824", d.maxFileSizeRaw);
    }

    @Test
    public void testParseOldFormatCycleNotExtractedAsNewFormat() {
        // Regression: the old openFile only handled drg[DR_GROUP_ID] lines.
        // parseDrContent must still extract the cycle from old-format files.
        String content = "drg.append(trick.DRBinary(\"g\"))\n" + "drg[DR_GROUP_ID].set_cycle(1.0)\n"
                + "trick.add_data_record_group(drg[DR_GROUP_ID], trick.DR_Buffer)\n";

        DreApplication.DrFileData d = DreApplication.parseDrContent(content);
        assertEquals("1.0", d.cycle);
    }

    // -----------------------------------------------------------------------
    // Round-trip test: buildDrContent → parseDrContent
    // -----------------------------------------------------------------------

    @Test
    public void testRoundTrip() {
        List<String> vars = Arrays.asList("spacecraft.pos[0]", "spacecraft.pos[1]");
        String content = DreApplication.buildDrContent(
                "DRHDF5", "hdfGroup", "DR_Changes_Step", "0.01", "True", vars, "4", "GiB", "DR_Ring_Buffer");

        DreApplication.DrFileData d = DreApplication.parseDrContent(content);

        assertEquals("DRHDF5", d.format);
        assertEquals("hdfGroup", d.name);
        assertEquals("DR_Changes_Step", d.frequency);
        assertEquals("0.01", d.cycle);
        assertEquals("True", d.singlePrecOnly);
        assertEquals(2, d.variables.size());
        assertEquals("spacecraft.pos[0]", d.variables.get(0));
        assertEquals("spacecraft.pos[1]", d.variables.get(1));
        assertEquals("DR_Ring_Buffer", d.buffering);
    }
}
