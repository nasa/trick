package trick.simcontrol;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.Scanner;
import java.awt.event.KeyEvent;
import java.io.File;

import org.jdesktop.application.Application;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.FixMethodOrder;
import org.junit.runners.MethodSorters;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.assertEquals;
import static org.junit.Assume.assumeTrue;
import static org.junit.Assume.assumeNotNull;

import trick.common.ActionInfo;
import trick.common.ApplicationTest;

/**
 * Test SimControlApplication life cycle.
 * 
 * @author hchen
 * @intern mrockwell2
 *
 */
public class SimControlApplicationUsageTest extends ApplicationTest {
	private static int numSims = 0;
	private static String socketInfo;

	private static WaitForSimControlApplication simcontrol;

	public SimControlApplicationUsageTest() {
	}
		
	@BeforeClass
	public static void setUpBeforeClass() {
		String statusMsg, simDir, 
			   expDir = getTrickHome() + "/trick_sims/SIM_basic/S_main";
		socketInfo = startBasicSim();
		numSims++;
		
		startApplication(true);

		simDir = simcontrol.getRunningSimInfo();
		statusMsg = simcontrol.getStatusMessages();
		assumeTrue("SimControlPanel did not connect!", simDir.startsWith(expDir));
		assumeTrue("Unexpected Error Message: \n\t" + statusMsg, statusMsg.isEmpty());
	}

	@AfterClass
	public static void tearDownAfterClass() {
		while(numSims > 0 && runningSims.size() > 0) {
			Process sim = runningSims.pop();
			if (sim.isAlive()) {
				sim.destroy();				
			}
			numSims--;
		}
	}

	@Before
	public void setUp() {
		assumeNotNull(simcontrol);
	}

	@After
	public void tearDown() {
		assumeNotNull(simcontrol);
		simcontrol.freezeSim();
		simcontrol.clearStatusMsgs();
		simcontrol.sleep(1000);
	}

	@Override
	protected void getCoreActionInfo() {}

	@Override
	protected void getSupportActionInfo() {}
		
	@Override
	protected void getMiscActionInfo() {}
	
	@Test
	/**
	 * Testing that the startSim() action functions properly. 
	 */
	public void testStartSimulation() {
		// ARRANGE
		String statusMsg, expStatus = "Freeze OFF";
		int counter = 0;

		// ACT
		simcontrol.startSim();

		do {
			counter++;
			simcontrol.sleep(500);
			statusMsg = simcontrol.getStatusMessages();
		} while(statusMsg.isEmpty() && counter < 5);

		simcontrol.freezeSim();

		// ASSERT
		assertTrue("Simulation did not start!", statusMsg.indexOf(expStatus) != -1);
		
	}
	
	@Test
	/**
	 * Testing that the freezeSim() action functions properly. 
	 */
	public void testFreezeSimulation() {
		// ARRANGE
		String statusMsg, expStatus = "Freeze ON";
		int counter = 0;

		// ACT
		simcontrol.startSim();
		simcontrol.freezeSim();

		do {
			counter++;
			simcontrol.sleep(500);
			statusMsg = simcontrol.getStatusMessages();
		} while(statusMsg.isEmpty() && counter < 5);

		// ASSERT
		assertTrue("Simulation did not freeze!", statusMsg.indexOf(expStatus) != -1);
		
	}
	
	@Test
	/**
	 * Testing that the freezeSim() action functions properly. 
	 */
	public void testFreezeAtTimeSimulation() {
		// ARRANGE
		double targetTime = simcontrol.getExecTime() + 3.0;
		String statusMsg, expStatus = "Freeze ON",
			   expTimeStamp = String.format("%.6f", targetTime);
		Matcher line1, line2;
		Pattern freezeOffPatt = Pattern.compile("\\|.*\\| Freeze OFF\\.\\n?"),
				freezeOnPatt  = Pattern.compile("\\|.*\\| Freeze ON\\. Simulation time holding at "
												 + expTimeStamp + " seconds\\.\\n?");

		// ACT
		simcontrol.freezeAt();
		simcontrol.sleep(1000);
		simcontrol.controller.typeString("" + targetTime);
		simcontrol.controller.keyTap(KeyEvent.VK_ENTER);

		simcontrol.startSim();
		simcontrol.sleep(2500);

		statusMsg = simcontrol.getStatusMessages();

		// ASSERT
		// assertTrue("Simulation did not freeze!", statusMsg.indexOf(expStatus) != -1);
		
	}
	
	@Test
	/**
	 * Testing that the startSim() action functions properly after the 
	 * freezeSim() action. 
	 */
	public void testRestartSimulation() {
		// ARRANGE
		String statusMsg;
		String[] statusLines;
		Matcher line1, line2, line3;
		Pattern freezeOffPatt = Pattern.compile("\\|.*\\| Freeze OFF\\.\\n?"),
				freezeOnPatt  = Pattern.compile("\\|.*\\| Freeze ON\\. Simulation time holding at \\d+\\.\\d+ seconds\\.\\n?");

		int counter = 0;

		// ACT
		simcontrol.startSim();
		simcontrol.sleep(1000);

		simcontrol.freezeSim();
		simcontrol.sleep(1000);

		simcontrol.startSim();

		do {
			counter++;
			simcontrol.sleep(500);
			statusMsg = simcontrol.getStatusMessages();
			statusLines = statusMsg.split("\n");
		} while(statusLines.length >= 3 && counter < 6);

		assumeTrue("Status Message Pane does not have the expected number of entries!", statusLines.length >= 3);
		line1 = freezeOffPatt.matcher(statusLines[0]);
		line2 = freezeOnPatt.matcher(statusLines[1]);
		line3 = freezeOffPatt.matcher(statusLines[2]);

		// ASSERT
		assertTrue("Simulation didn't start correctly!", line1.find());
		assertTrue("Simulation didn't freeze correctly!", line2.find());
		assertTrue("Simulation didn't resume correctly!", line3.find());
	}
	
	private static void startApplication(boolean startConnected) {
		if(simcontrol == null) {
			if(startConnected)  // Launch Testing SimControlPanel with the provided connection info
				WaitForSimControlApplication.launchAndWait(WaitForSimControlApplication.class, socketInfo.replace(" ", ","));
			else  // Launch Testing SimControlPanel
				WaitForSimControlApplication.launchAndWait(WaitForSimControlApplication.class);

			handleAppSetup();
		} else {
			System.err.println("SimControlApplication is already Running...");
		}
    }

	private static void handleAppSetup() {
		// Set up the required variables for testing
		simcontrol = Application.getInstance(WaitForSimControlApplication.class);

		// Ensure that everything got set up correctly.
		assumeTrue("SimControlApplicationTest is not ready yet!", simcontrol.isReady());
	}
}