package trick.tv;

import java.io.File;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.ServerSocket;

import static org.assertj.core.api.Assumptions.assumeThat;
import static org.assertj.swing.launcher.ApplicationLauncher.application;
import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;

import trick.common.ApplicationTest;
import trick.common.TestUtils;
import trick.tv.fixtures.TVFixture;

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
		tv_fix.getErrorPopupFixture()
			  .okButton().click();
    }

	@Test
	public void testGeneric() {
		// tv_fix.selectVar("drx.drt.uintB.var1");

		sleep(5000);
		
		// tv_app.connectSimulation();
		if(tv_app.isConnected())	System.out.println("CONNECTED");
		else System.out.println("NOT CONNECTED");
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