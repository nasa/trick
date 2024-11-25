package trick.tv;

import java.io.File;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.nio.file.Files;
import java.nio.file.StandardCopyOption;
import java.util.Arrays;

import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assumptions.assumeThat;
import static org.assertj.swing.launcher.ApplicationLauncher.application;
import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;

import trick.common.ApplicationTest;
import trick.common.TestUtils;
import trick.tv.fixtures.TVFixture;
import static trick.tv.fixtures.TVFixture.CASE_SENSITIVE_ON;
import static trick.tv.fixtures.TVFixture.GREEDY_SEARCH_ON;
import static trick.tv.fixtures.TVFixture.REG_EXPRESSION_ON;

/**
 * 
 * Test TVApplication life cycle.
 * 
 * @author hchen
 *
 */
public class TVApplicationTest extends ApplicationTest {
	private static final int PORT = getOpenPort();
	private static final String SIM_NAME = "SIM_test_dr",
								GUI_TEST_INPUT = TestUtils.getTrickHome()
													+ "/trick_source/java"
													+ "/src/test/resources/RUN_gui_test/input.py";
	private static Process sim_process;

	private TVFixture tv_fix;
	private MockTVApplication tv_app;

    @BeforeClass
    public static void onSetUpBeforeClass() {
		assumeThat(TestUtils.compileTestSim(SIM_NAME))
			.withFailMessage("Simulation \"%s\" Did Not Compile\n", SIM_NAME)
			.isTrue();
		
		sim_process = TestUtils.runTestSim(SIM_NAME, GUI_TEST_INPUT);
		assumeThat(sim_process)
			.withFailMessage("Simulation \"%s\" Did Not Start\n", SIM_NAME)
			.isNotNull();
    }

	@AfterClass
	public static void onCleanUpAfterClass() {
		if(sim_process != null && sim_process.isAlive()) {
			sim_process.destroy();
		}
	}

    @Override
    protected void onSetUp() {        
        application(MockTVApplication.class)
			.withArgs("--host", "localhost",
					  "--port", "" + PORT)
			.start();
			
		sleep(2500);

		tv_app = MockTVApplication.getInstance(MockTVApplication.class);
		tv_fix = new TVFixture(robot(), tv_app);
    }

	@Test
	public void testGeneric() {
		tv_fix.selectVar("drx.drt.uintB.var1");
		tv_fix.setSearchOptions(CASE_SENSITIVE_ON | REG_EXPRESSION_ON | GREEDY_SEARCH_ON);
		tv_fix.enterQuery("var1\n");
		sleep(500);

		System.out.println("SEARCH RESULTS");;
		String[] results = tv_fix.getSearchResults();
		for (String string : results) {
			System.out.println(string);
			tv_fix.selectVar(string);
			sleep(250);
		}

		System.out.println("\nSEL VARS");
		String[][] table = tv_fix.getSelectedVars();
		for (String[] strings : table) {
			for (String string : strings) {
				System.out.print(string + "\t");
			}
			System.out.println();
		}

		sleep(5000);
	}

    @Test
    public void testSelectVars() {
        // ARRANGE
        final String[][] SEL_VARS = { {	"drx.drt.a",			"97",	"1", "Decimal" },
									  {	"drx.drt.b",			"98",	"1", "Decimal" },
									  {	"drx.drt.c",			"-1234","1", "Decimal" },
                                      {	"drx.drt.uintB.var1",	"128",	"1", "Decimal" },
									  {	"drx.drt.intB.var1",	"63",	"1", "Decimal" },
                                      {	"drx.drt.ucharB.var1",	"2",	"1", "Decimal" },
									  {	"drx.drt.charB.var1",	"1",	"1", "Decimal" },
                                      {	"drx.drt.ushortB.var1",	"4",	"1", "Decimal" },
									  {	"drx.drt.shortB.var1",	"127",	"1", "Decimal" },
                                      {	"drx.drt.mixB.var1",	"3",	"1", "Decimal" } };
        String[][] res_vars;

        // ACT
        for(int i = 0; i < SEL_VARS.length; i++){
            tv_fix.selectVar(SEL_VARS[i][0]);
            sleep(250);
        }

        res_vars = tv_fix.getSelectedVars();

        // ASSERT
        assumeThat(res_vars.length)
			.withFailMessage("Unexpected number of rows.")
			.isEqualTo(SEL_VARS.length);

		for (int i = 0; i < res_vars.length; i++) {
			assumeThat(res_vars[i].length)
				.withFailMessage("Unexpected number of columns.")
				.isEqualTo(SEL_VARS[i].length);

			for (int j = 0; j < res_vars[0].length; j++) {
				assertThat(res_vars[i][j])
					.isEqualTo(SEL_VARS[i][j]);
			}
		}
    }

    @Test
    public void testSearchVars() {
        // ARRANGE
        final String[] SEARCH_VARS = {  "drx.drt.uintB.var1"  , "drx.drt.intB.var1",
                                        "drx.drt.ucharB.var1" , "drx.drt.charB.var1",
                                        "drx.drt.ushortB.var1", "drx.drt.shortB.var1",
                                        "drx.drt.mixB.var1"  };
        String[] found_vars;
        assumeThat(tv_fix.getSearchResults()).isNull();

        // ACT
        tv_fix.enterQuery("var1\n");
        sleep(50);

        found_vars = tv_fix.getSearchResults();

        // ASSERT
        assertThat(found_vars.length)
            .withFailMessage("Unexpected number of selected variables.")
            .isEqualTo(SEARCH_VARS.length);

        for(int i = 0; i < SEARCH_VARS.length; i++)
            assertThat(found_vars[i]).isEqualTo(SEARCH_VARS[i]);
    }

	@Test 
	public void testEditTable() {
		// ARRANGE
		final String[][] SEL_VARS = { {	"drx.drt.a",			"88",	"1", "Decimal" },
									  {	"drx.drt.b",			"88",	"1", "Decimal" },
									  {	"drx.drt.c",			"88",	"1", "Decimal" },
                                      {	"drx.drt.uintB.var1",	"28",	"1", "Decimal" },
									  {	"drx.drt.intB.var1",	"28",	"1", "Decimal" },
                                      {	"drx.drt.ucharB.var1",	"3",	"1", "Decimal" },
									  {	"drx.drt.charB.var1",	"-1",	"1", "Decimal" },
                                      {	"drx.drt.ushortB.var1",	"14",	"1", "Decimal" },
									  {	"drx.drt.shortB.var1",	"78",	"1", "Decimal" },
                                      {	"drx.drt.mixB.var1",	"1",	"1", "Decimal" } };

		String[][] res_vars;
		String[] inital_values = new String[SEL_VARS.length];

		// ACT
        for(int i = 0; i < SEL_VARS.length; i++){
            tv_fix.selectVar(SEL_VARS[i][0]);
            sleep(250);
        }

        for(int i = 0; i < SEL_VARS.length; i++){
			inital_values[i] = tv_fix.getCellValue(i, 1);
			tv_fix.editVariableTable(i, 1, SEL_VARS[i][1] + "\n");
			sleep(500);
        }

		res_vars = tv_fix.getSelectedVars();
		
		// ASSERT
        assertThat2DArraysAreEqual(res_vars, SEL_VARS);

		// CLEANUP
		for(int i = 0; i < inital_values.length; i++) {
			tv_fix.editVariableTable(i, 1, inital_values[i]);
			sleep(500);
		}
	}

    @Test
    public void testSave() {
        // ARRANGE
		final File EXPECTED_OUTPUT = new File("src/test/java/trick/tv/resources/tv_save_test.tv"),
				   SAVED_OUTPUT    = new File("test_results.tv"),
				   FAILED_TEST     = new File("failed_test.tv");
        final String[] SEL_VARS = { "drx.drt.a",	 "drx.drt.b",	  "drx.drt.c",
									"drx.drt.uintB.var1",     "drx.drt.intB.var1",
									"drx.drt.ucharB.var1",   "drx.drt.charB.var1",
									"drx.drt.ushortB.var1", "drx.drt.shortB.var1",
									"drx.drt.mixB.var1" };
		boolean file_written, file_expected;

		SAVED_OUTPUT.deleteOnExit();

		assumeThat(EXPECTED_OUTPUT).isNotNull();
		assumeThat(EXPECTED_OUTPUT.exists()).isTrue();

        // ACT
        for(int i = 0; i < SEL_VARS.length; i++){
            tv_fix.selectVar(SEL_VARS[i]);
            sleep(250);
        }

        file_written = tv_fix.saveMenuItem(SAVED_OUTPUT);

        // ASSERT
		file_expected = sameFileContent(SAVED_OUTPUT, EXPECTED_OUTPUT);

        assertThat(file_written)
			.withFailMessage("Save operation failed. File not written\n")
			.isTrue();

        if (!file_expected) {
            try{
                Files.copy( SAVED_OUTPUT.toPath(), 
                            FAILED_TEST.toPath(), 
                            StandardCopyOption.REPLACE_EXISTING );
            } catch(Exception IGNORED) {}
        }

		assertThat(file_expected)
			.withFailMessage("Generated File doesn't match Benchmark. Failed .tv file is in java directory\n")
			.isTrue();
    }

    @Test
    public void testLoadVariables() {
        // ARRANGE
		final File SAVED_FILE = new File("src/test/java/trick/tv/resources/tv_load_test.tv");
        final String[][] SEL_VARS = { {	"drx.drt.a",			"97",	"1", "Decimal" },
									  {	"drx.drt.b",			"98",	"1", "Decimal" },
									  {	"drx.drt.c",			"-1234","1", "Decimal" },
                                      {	"drx.drt.uintB.var1",	"128",	"1", "Decimal" },
									  {	"drx.drt.intB.var1",	"63",	"1", "Decimal" },
                                      {	"drx.drt.ucharB.var1",	"2",	"1", "Decimal" },
									  {	"drx.drt.charB.var1",	"1",	"1", "Decimal" },
                                      {	"drx.drt.ushortB.var1",	"4",	"1", "Decimal" },
									  {	"drx.drt.shortB.var1",	"127",	"1", "Decimal" },
                                      {	"drx.drt.mixB.var1",	"3",	"1", "Decimal" } };

		String[][] res_vars;

        // ACT
		tv_fix.openMenuItem(SAVED_FILE);

		sleep(500);

		res_vars = tv_fix.getSelectedVars();

        // ASSERT
        assertThat2DArraysAreEqual(res_vars, SEL_VARS);
    }

    @Test
    public void testLoadVariables_SetValues() {
        // ARRANGE
		final File SAVED_FILE = new File("src/test/java/trick/tv/resources/tv_load_test.tv"),
				   ORIGINAL_VALS_FILE = new File("src/test/java/trick/tv/resources/tv_save_test.tv");
		final String[][] SEL_VARS = { {	"drx.drt.a",			"88",	"1", "Decimal" },
									  {	"drx.drt.b",			"88",	"1", "Decimal" },
									  {	"drx.drt.c",			"88",	"1", "Decimal" },
                                      {	"drx.drt.uintB.var1",	"28",	"1", "Decimal" },
									  {	"drx.drt.intB.var1",	"28",	"1", "Decimal" },
                                      {	"drx.drt.ucharB.var1",	"3",	"1", "Decimal" },
									  {	"drx.drt.charB.var1",	"-1",	"1", "Decimal" },
                                      {	"drx.drt.ushortB.var1",	"14",	"1", "Decimal" },
									  {	"drx.drt.shortB.var1",	"78",	"1", "Decimal" },
                                      {	"drx.drt.mixB.var1",	"1",	"1", "Decimal" } };

		String[][] res_vars;

        // ACT
		tv_fix.openSetMenuItem(SAVED_FILE);
		sleep(500);

		res_vars = tv_fix.getSelectedVars();

        // ASSERT
        assertThat2DArraysAreEqual(res_vars, SEL_VARS);

		// CLEANUP
		tv_fix.openSetMenuItem(ORIGINAL_VALS_FILE);
		sleep(500);
    }

    @Test
    public void testSetValues() {
        // ARRANGE
		final File SAVED_FILE = new File("src/test/java/trick/tv/resources/tv_load_test.tv"),
				   ORIGINAL_VALS_FILE = new File("src/test/java/trick/tv/resources/tv_save_test.tv");
		final String[][] SEL_VARS = { {	"drx.drt.a",			"88",	"1", "Decimal" },
									  {	"drx.drt.b",			"88",	"1", "Decimal" },
									  {	"drx.drt.c",			"88",	"1", "Decimal" },
                                      {	"drx.drt.uintB.var1",	"28",	"1", "Decimal" },
									  {	"drx.drt.intB.var1",	"28",	"1", "Decimal" },
                                      {	"drx.drt.ucharB.var1",	"3",	"1", "Decimal" },
									  {	"drx.drt.charB.var1",	"-1",	"1", "Decimal" },
                                      {	"drx.drt.ushortB.var1",	"14",	"1", "Decimal" },
									  {	"drx.drt.shortB.var1",	"78",	"1", "Decimal" },
                                      {	"drx.drt.mixB.var1",	"1",	"1", "Decimal" } };

		String[][] res_vars1, res_vars2;

        // ACT
		tv_fix.setValMenuItem(SAVED_FILE);
		sleep(500);

		res_vars1 = tv_fix.getSelectedVars();

        for(int i = 0; i < SEL_VARS.length; i++){
            tv_fix.selectVar(SEL_VARS[i][0]);
            sleep(250);
        }

		res_vars2 = tv_fix.getSelectedVars();

        // ASSERT
		assertThat(res_vars1.length)
			.withFailMessage("Variable Table has unexpected Entries: %s\n", Arrays.deepToString(res_vars1))
			.isEqualTo(0);
			
        assertThat2DArraysAreEqual(res_vars2, SEL_VARS);

		// CLEANUP
		tv_fix.setValMenuItem(ORIGINAL_VALS_FILE);
		sleep(500);
    }

	@Test 
	public void testMonitorToggle() {
		// ARRANGE 
		final String[] EXP_VAR_MONITOR   = { "drx.drt.a", "97", "1", "Decimal" },
					   EXP_VAR_NO_MONITOR = { "drx.drt.a", "<Unknown>", "1", "Decimal" };
		String[] var_no_monitor, var_monitor;

		// ACT 
		tv_fix.clickMonitorToggleMenuItem();
		tv_fix.selectVar(EXP_VAR_MONITOR[0]);
		sleep(250);

		var_no_monitor = tv_fix.getSelectedVars()[0];
		tv_fix.clickMonitorToggleMenuItem();
		sleep(500);

		var_monitor = tv_fix.getSelectedVars()[0];

		// ASSERT
		assertThat(var_monitor.length).isEqualTo(EXP_VAR_MONITOR.length);
		assertThat(var_no_monitor.length).isEqualTo(EXP_VAR_NO_MONITOR.length);

		for(int i = 0; i < EXP_VAR_MONITOR.length; i++) {
			assertThat(var_no_monitor[i]).isEqualTo(EXP_VAR_NO_MONITOR[i]);
			assertThat(var_monitor[i]).isEqualTo(EXP_VAR_MONITOR[i]);
		}

	}

	public static int getOpenPort() {
		String port = "39595";
		File port_info;
		ServerSocket temp = null;
		PrintWriter info_writer = null;
		
		try {
			temp = new ServerSocket(0);
			port_info = new File("src/test/resources/RUN_gui_test/port.info");
			info_writer = new PrintWriter(port_info, "UTF-8");

			port = "" + temp.getLocalPort();
			info_writer.write(port);
		} catch(IOException e) {
			System.err.println("Free Port Not Found");
		} finally {
			try {
				if(temp != null)
					temp.close();
				if(info_writer != null)
					info_writer.close();
			} catch(IOException e) {
				System.err.println("Issue with Cleanup.");
			}
		}

		return Integer.parseInt(port);
	}
}