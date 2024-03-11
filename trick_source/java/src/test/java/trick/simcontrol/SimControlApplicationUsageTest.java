package trick.simcontrol;

import java.util.Arrays;
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
	private static String socketInfo, basicSimDir;

	private static WaitForSimControlApplication simcontrol;

	public SimControlApplicationUsageTest() {
	}
		
	@BeforeClass
	public static void setUpBeforeClass() {
		String statusMsg, simDir, 
			   expDir = getTrickHome() + "/trick_sims/SIM_basic/S_main";
		socketInfo = startBasicSim();
		basicSimDir = getTrickHome() + "/trick_sims/SIM_basic";
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
		simcontrol.clearStatusMsgs();
		assumeTrue(simcontrol.getStatusMessages().isEmpty());
	}

	@After
	public void tearDown() {
		assumeNotNull(simcontrol);
		simcontrol.clearStatusMsgs();
		sleep(1000);
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
		simcontrol.clearStatusMsgs();
		simcontrol.startSim();

		do {
			counter++;
			sleep(500);
			statusMsg = simcontrol.getStatusMessages();
		} while(statusMsg.isEmpty() && counter < 5);

		simcontrol.freezeSim();

		// ASSERT
		assertTrue("Simulation did not start!\n" + statusMsg, statusMsg.indexOf(expStatus) != -1);
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
			sleep(500);
			statusMsg = simcontrol.getStatusMessages();
		} while(statusMsg.indexOf(expStatus) < 0 && counter < 5);

		// ASSERT
		assertTrue("Simulation did not freeze!\n" + statusMsg, statusMsg.indexOf(expStatus) != -1);
		
	}

	@Test
	/**
	 * Testing that the freezeAtSim() action functions properly. 
	 */
	public void testFreezeAtTimeSimulation() {
		// ARRANGE
		double targetTime = simcontrol.getExecTime() + 3.0;
		String statusMsg[], expStatus = "Freeze ON",
			   expTimeStamp = String.format("%.6f", targetTime);
		Matcher line1, line2;
		Pattern freezeOffPatt = Pattern.compile("\\|.*\\| Freeze OFF\\.\\n?"),
				freezeOnPatt  = Pattern.compile("\\|.*\\| Freeze ON\\. Simulation time holding at "
												 + expTimeStamp + " seconds\\.\\n?");

		// ACT
		simcontrol.controller.delayedTypeString((targetTime + "\n"), 500);
		simcontrol.freezeAt();

		simcontrol.startSim();
		simcontrol.sleep(4000);

		statusMsg = simcontrol.getStatusMessages().split("\n");
		line1 = freezeOffPatt.matcher(statusMsg[0]);
		line2 =  freezeOnPatt.matcher(statusMsg[1]);

		// ASSERT
		assumeTrue("Unexpected number of messages", statusMsg.length == 2);
		assumeTrue("Simulation did not start!", line1.find());
		assertTrue("Simulation didn't freeze at " + expTimeStamp + "\n" + statusMsg[1],
				   line2.find());
		
	}

	@Test
	/**
	 * Testing that the freezeInSim() action functions properly. 
	 */
	public void testFreezeInTimeSimulation() {
		// ARRANGE
		double targetTime = simcontrol.getExecTime() + 3.0;
		String statusMsg[], expStatus = "Freeze ON",
			   expTimeStamp = String.format("%.6f", targetTime);
		Matcher line1, line2;
		Pattern freezeOffPatt = Pattern.compile("\\|.*\\| Freeze OFF\\.\\n?"),
				freezeOnPatt  = Pattern.compile("\\|.*\\| Freeze ON\\. Simulation time holding at "
												 + expTimeStamp + " seconds\\.\\n?");

		// ACT
		simcontrol.controller.delayedTypeString("3.0\n", 500);
		simcontrol.freezeIn();

		simcontrol.startSim();
		simcontrol.sleep(4000);

		statusMsg = simcontrol.getStatusMessages().split("\n");
		line1 = freezeOffPatt.matcher(statusMsg[0]);
		line2 =  freezeOnPatt.matcher(statusMsg[1]);

		// ASSERT
		assumeTrue("Unexpected number of messages", statusMsg.length == 2);
		assumeTrue("Simulation did not start!", line1.find());
		assertTrue("Simulation didn't freeze at " + expTimeStamp + "\n" + statusMsg[1],
				   line2.find());
		
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
		sleep(1000);

		simcontrol.freezeSim();
		sleep(1000);

		simcontrol.startSim();

		do {
			counter++;
			sleep(500);
			statusMsg = simcontrol.getStatusMessages();
			statusLines = statusMsg.split("\n");
		} while(statusLines.length < 3 && counter < 6);

		assumeTrue("Status Message Pane does not have the expected number of entries!", statusLines.length >= 3);
		line1 = freezeOffPatt.matcher(statusLines[0]);
		line2 = freezeOnPatt.matcher(statusLines[1]);
		line3 = freezeOffPatt.matcher(statusLines[2]);

		simcontrol.freezeSim();

		// ASSERT
		assertTrue( "Simulation didn't start correctly:\n" + statusLines[0], line1.find());
		assertTrue("Simulation didn't freeze correctly:\n" + statusLines[1], line2.find());
		assertTrue("Simulation didn't resume correctly:\n" + statusLines[2], line3.find());
	}
	
	@Test
	/**
	 * Testing that the dumpChkpntASCII() action functions properly. 
	 */
	public void testDumpCheckpointSimulation() {
		// ARRANGE
		String expMsg = "Dumped ASCII Checkpoint ", actualMsg, errMsg, 
			   fileName, filePath = basicSimDir + "/RUN_test/";
		File checkpointFile;
		int nameIndex;

		// ACT
		simcontrol.controller.delayedKeyTap(KeyEvent.VK_ENTER, 500);
		simcontrol.controller.delayedKeyTap(KeyEvent.VK_ENTER, 750);
		simcontrol.dumpChkpntASCII();
		sleep(500);

		actualMsg = simcontrol.getStatusMessages();
		nameIndex = actualMsg.indexOf(expMsg);
		assumeTrue("Dumped Checkpoint Message Not Found", nameIndex >= 0);

		nameIndex += expMsg.length();
		fileName = actualMsg.substring(nameIndex, actualMsg.length() - 2);
		checkpointFile = new File(filePath + fileName );
		
		// ASSERT
		errMsg = String.format("'%s' Checkpoint File Not Found At '%s'\n", fileName, filePath);
		assumeNotNull(errMsg, checkpointFile);
		assertTrue(errMsg, checkpointFile.exists());
		
	}
	
	@Test
	/**
	 * Testing that the loadChkpnt() action functions properly. 
	 */
	public void testLoadCheckpointSimulation() {
		// ARRANGE
		String actualMsgs[], fileName = "TESTING_CHECKPOINT_LOAD",
			   expMsgs[] = {"Load checkpoint file " + basicSimDir + "/RUN_test/TESTING_CHECKPOINT_LOAD.", 
							"Finished loading checkpoint file.  Calling restart jobs.", 
							"|8.700000| restart variable server message port = " + socketInfo.split(" ")[1]};
		
		int counter = 0;

		// ACT
		simcontrol.controller.delayedTypeString(fileName + "\n", 500);
		simcontrol.loadChkpnt();
		sleep(2500);
		actualMsgs = simcontrol.getStatusMessages().split("\n");

		assumeTrue("Unexpected Status Messages:\n" + Arrays.toString(actualMsgs), actualMsgs.length >= 3);
		
		// ASSERT
		assertTrue("Unexpected Status Message:\n" + actualMsgs[0], actualMsgs[0].endsWith(expMsgs[0]));
		assertTrue("Unexpected Status Message:\n" + actualMsgs[1], actualMsgs[1].endsWith(expMsgs[1]));
		assertTrue("Unexpected Status Message:\n" + actualMsgs[2], actualMsgs[2].endsWith(expMsgs[2]));
		
		
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