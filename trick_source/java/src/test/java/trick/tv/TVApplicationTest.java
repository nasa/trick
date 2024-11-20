package trick.tv;

import java.io.File;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.nio.file.Files;
import java.nio.file.StandardCopyOption;

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
    public void testSave() {
        // ARRANGE
		final File EXPECTED_OUTPUT = new File("src/test/java/trick/tv/resources/tv_save_test.tv"),
				   SAVED_OUTPUT = new File("test_results.tv"),
				   FAILED_TEST = new File("failed_test.tv");
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
		final File SAVED_FILE = new File("src/test/java/trick/tv/resources/tv_save_test.tv");
        final String[][] SEL_VARS = { {	"drx.drt.a",			"8",	"1", "Decimal" },
									  {	"drx.drt.b",			"8",	"1", "Decimal" },
									  {	"drx.drt.c",			"8",	"1", "Decimal" },
                                      {	"drx.drt.uintB.var1",	"28",	"1", "Decimal" },
									  {	"drx.drt.intB.var1",	"28",	"1", "Decimal" },
                                      {	"drx.drt.ucharB.var1",	"3",	"1", "Decimal" },
									  {	"drx.drt.charB.var1",	"-1",	"1", "Decimal" },
                                      {	"drx.drt.ushortB.var1",	"14",	"1", "Decimal" },
									  {	"drx.drt.shortB.var1",	"78",	"1", "Decimal" },
                                      {	"drx.drt.mixB.var1",	"3",	"1", "Decimal" } };

		String[][] res_vars;

        // ACT
        for(int i = 0; i < SEL_VARS.length; i++){
            tv_fix.selectVar(SEL_VARS[i][0]);
            sleep(250);
        }

        for(int i = 0; i < SEL_VARS.length; i++){
			tv_fix.editVariableTable(i, 1, SEL_VARS[i][1] + "\n");
			sleep(500);
        }

		tv_fix.openMenuItem(SAVED_FILE);

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