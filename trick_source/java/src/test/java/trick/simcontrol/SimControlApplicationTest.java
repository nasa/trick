package trick.simcontrol;

import java.awt.GraphicsEnvironment;
import java.awt.event.KeyEvent;
import java.beans.Transient;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.PrintStream;
import java.util.Arrays;
import java.util.Scanner;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.jdesktop.application.Application;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.assertTrue;
import static org.junit.Assume.assumeTrue;
import static org.junit.Assume.assumeNotNull;

import trick.common.ApplicationTest;
import trick.common.SimulationInterface;
import trick.common.utils.VariableServerConnection;
import trick.common.ActionInfo;

/**
 * 
 * Test SimControlApplication life cycle.
 * 
 * @author hchen
 * @intern mrockwell2
 *
 */
public class SimControlApplicationTest extends ApplicationTest {
	private final static String SIM_DIR = "/test/SIM_gui_testing";
	private final ByteArrayOutputStream errContent = new ByteArrayOutputStream();
	private final PrintStream originalErr = System.err;

	private final int MODE_FREEZE = 1, 
					     MODE_RUN = 5;

	private static SimControlApplication simControl;
	private static VariableServerConnection varserv;
	private static Process simProc;
	private static String host;
	private static int port;
		
	@BeforeClass
	public static void setUpBeforeClass() {
		String fullDir = getTrickHome() + SIM_DIR;
		simControl = null;

		SimulationInterface.cleanSim(fullDir);
		SimulationInterface.compileSim(fullDir);
		simProc = SimulationInterface.startSim(fullDir, "RUN_test/input.py");

		sleep(1000);
		
		try {
			Scanner infoReader = new Scanner(new File(fullDir + "/socket_info"));
			String line = infoReader.nextLine();

			host = line.split(":")[0];
			port = Integer.parseInt(line.split(":")[1]);

			varserv = new VariableServerConnection(host, port);
		} catch (Exception e) {
			System.err.println(e.getMessage());
			host = "";
			port = -1;
			varserv = null;
		}
		
		assumeTrue("Did not connect to the variable server", varserv != null);
		assumeTrue("Did not find the host name.", !host.isEmpty());
		assumeTrue("Did not find the port number", port >= 0);
	}

	@AfterClass
	public static void tearDownAfterClass() {
		if(simProc != null && simProc.isAlive())
			simProc.destroy();
	}

	@After
	public void cleanup() {
		simControl = null;
	}

	@Override
	protected void getCoreActionInfo() { }

	@Override
	protected void getSupportActionInfo() { }
		
	@Override
	protected void getMiscActionInfo() { }

	@Test
	public void testStartSim() throws IOException {
		if (GraphicsEnvironment.isHeadless()) {
			headlessStartSim();
		} else {
			guiStartSim();
		}
	}

	@Test
	public void testFreezeSim() throws IOException {
		if (GraphicsEnvironment.isHeadless()) {
			headlessFreezeSim();
		} else {
			guiFreezeSim();
		}
	}
	
	@Test
	public void testConnectionFail() {		
		// ARRANGE
		String expOutput = "Invalid TCP/IP port number \"0\"", output;

		String badH = "localhost";
		int badP = 0;
		simControl = new HeadlessSimControlApplication();
		System.setErr(new PrintStream(errContent));

		// ACT
		HeadlessSimControlApplication.setHostPort(badH, badP);
		simControl.connect();
		sleep(500);

		// ASSERT
		output = errContent.toString();
		assertTrue("Did not recieve the expected error message: \n"
					+ output, output.indexOf(expOutput) >= 0);

		// CLEAN UP
		System.setErr(originalErr);
	}

	//--------------------//
	//   GUI Only Tests   //
	//--------------------//

	@Test
	/**
	 * Testing that the freezeAtSim() action functions properly. 
	 */
	public void testFreezeAtTimeSimulation() {
		if (GraphicsEnvironment.isHeadless()) {
			String testName = "testFreezeAtTimeSimulation";
			System.out.println("Testing Headlessly. Skipping '" + testName + "'");
			return;
		}

		// ARRANGE
		startApplication(true);
		assumeTrue("Sim Control Panel didn't start...", simControl != null);

		WaitForSimControlApplication simcontrol = (WaitForSimControlApplication) simControl;

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
		if (GraphicsEnvironment.isHeadless()) {
			String testName = "testFreezeInTimeSimulation";
			System.out.println("Testing Headlessly. Skipping '" + testName + "'");
			return;
		}

		// ARRANGE
		startApplication(true);
		assumeTrue("Sim Control Panel didn't start...", simControl != null);

		WaitForSimControlApplication simcontrol = (WaitForSimControlApplication) simControl;
		
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
		if (GraphicsEnvironment.isHeadless()) {
			String testName = "testRestartSimulation";
			System.out.println("Testing Headlessly. Skipping '" + testName + "'");
			return;
		}

		// ARRANGE
		String statusMsg;
		String[] statusLines;
		Matcher line1, line2, line3;
		Pattern freezeOffPatt = Pattern.compile("\\|.*\\| Freeze OFF\\.\\n?"),
				freezeOnPatt  = Pattern.compile("\\|.*\\| Freeze ON\\. Simulation time holding at \\d+\\.\\d+ seconds\\.\\n?");

		int counter = 0;

		startApplication(true);
		assumeTrue("Sim Control Panel didn't start...", simControl != null);

		WaitForSimControlApplication simcontrol = (WaitForSimControlApplication) simControl;

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
		if (GraphicsEnvironment.isHeadless()) {
			String testName = "testDumpCheckpointSimulation";
			System.out.println("Testing Headlessly. Skipping '" + testName + "'");
			return;
		}

		// ARRANGE
		String expMsg = "Dumped ASCII Checkpoint ", actualMsg, errMsg, 
			   fileName, filePath = getTrickHome() + SIM_DIR + "/RUN_test/";
		File checkpointFile;
		int nameIndex;

		startApplication(true);
		assumeTrue("Sim Control Panel didn't start...", simControl != null);

		WaitForSimControlApplication simcontrol = (WaitForSimControlApplication) simControl;

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
		
		// CLEANUP
		checkpointFile.delete();
	}
	
	@Test
	/**
	 * Testing that the loadChkpnt() action functions properly. 
	 */
	public void testLoadCheckpointSimulation() {
		if (GraphicsEnvironment.isHeadless()) {
			String testName = "testLoadCheckpointSimulation";
			System.out.println("Testing Headlessly. Skipping '" + testName + "'");
			return;
		}

		// ARRANGE
		String actualMsgs[], fileName = "TESTING_CHECKPOINT_LOAD",
			   expMsgs[] = {"Load checkpoint file " + getTrickHome() + SIM_DIR + "/RUN_test/TESTING_CHECKPOINT_LOAD.", 
							"Finished loading checkpoint file.  Calling restart jobs.", 
							"|8.700000| restart variable server message port = " + port};
		
		int counter = 0;

		startApplication(true);
		assumeTrue("Sim Control Panel didn't start...", simControl != null);

		WaitForSimControlApplication simcontrol = (WaitForSimControlApplication) simControl;

		// ACT
		simcontrol.controller.delayedTypeString(fileName + "\n", 500);
		simcontrol.loadChkpnt();
		sleep(5000);
		actualMsgs = simcontrol.getStatusMessages().split("\n");

		assumeTrue("Unexpected Status Messages:\n" + Arrays.toString(actualMsgs), actualMsgs.length >= 3);
		
		// ASSERT
		assertTrue("Unexpected Status Message:\n" + actualMsgs[0], actualMsgs[0].endsWith(expMsgs[0]));
		assertTrue("Unexpected Status Message:\n" + actualMsgs[1], actualMsgs[1].endsWith(expMsgs[1]));
		assertTrue("Unexpected Status Message:\n" + actualMsgs[2], actualMsgs[2].endsWith(expMsgs[2]));
		
		
	}

	@Test
	/**
	 * Testing that data recording toggles off and on correctly. 
	 */
	public void testDataRecordingToggle() {
		if (GraphicsEnvironment.isHeadless()) {
			String testName = "testDataRecordingToggle";
			System.out.println("Testing Headlessly. Skipping '" + testName + "'");
			return;
		}

		// ARRANGE
		final boolean expectedOrder[] = {true, false, true};
		boolean actualOrder[] = new boolean[3];

		startApplication(true);
		assumeTrue("Sim Control Panel didn't start...", simControl != null);

		WaitForSimControlApplication simcontrol = (WaitForSimControlApplication) simControl;

		// ACT
		for(int i = 0; i < actualOrder.length; i++) {
			actualOrder[i] = simcontrol.isDataRecOn();
			simcontrol.toggleDataRecButton();
			sleep(1000);
		}

		// ASSERT
		assertTrue("Data Recording was not ON at the start of the Sim", 
					expectedOrder[0] == actualOrder[0]);
		assertTrue("Data Recording didn't toggle OFF correctly", 
					expectedOrder[1] == actualOrder[1]);
		assertTrue("Data Recording didn't toggle back ON correctly", 
					expectedOrder[2] == actualOrder[2]);
	}

	@Test
	/**
	 * Testing that data recording toggles off and on correctly. 
	 */
	public void testRealTimeToggle() {
		if (GraphicsEnvironment.isHeadless()) {
			String testName = "testRealTimeToggle";
			System.out.println("Testing Headlessly. Skipping '" + testName + "'");
			return;
		}

		// ARRANGE
		final boolean expectedOrder[] = {true, false, true};
		boolean actualOrder[] = new boolean[3];

		startApplication(true);
		assumeTrue("Sim Control Panel didn't start...", simControl != null);

		WaitForSimControlApplication simcontrol = (WaitForSimControlApplication) simControl;

		// ACT
		actualOrder[0] = simcontrol.isRealTimeOn();  // Expected TRUE
		actualOrder[1] = toggleRealTime(simcontrol); // Expected FALSE
		actualOrder[2] = toggleRealTime(simcontrol); // Expected TRUE

		// ASSERT
		assertTrue("Real Time was not ON at the start of the Sim", 
					expectedOrder[0] == actualOrder[0]);
		assertTrue("Real Time didn't toggle OFF correctly", 
					expectedOrder[1] == actualOrder[1]);
		assertTrue("Real Time didn't toggle back ON correctly", 
					expectedOrder[2] == actualOrder[2]);
	}

	// Hybrid Test helper methods

	private boolean toggleRealTime(WaitForSimControlApplication app) {
		app.toggleRealTimeButton();
		sleep(1000);
		return app.isRealTimeOn();
	}

	private void headlessStartSim() throws IOException {
		// ARRANGE
		String out; 
		int mode = 1, count = 0;
		
		simControl = new HeadlessSimControlApplication(host, port);
		sleep(1000);
		
		simControl.connect();
		sleep(1000);

		// ACT
		varserv.put("trick.var_send_once(\"trick_sys.sched.mode\")");
		mode = Integer.parseInt(varserv.get().split("\t")[1]);
		assumeTrue("Sim Mode was not MODE_FREEZE at test start", mode == MODE_FREEZE);

		simControl.startSim();
		do {
			count ++;
			varserv.put("trick.var_send_once(\"trick_sys.sched.mode\")");
			mode = Integer.parseInt(varserv.get().split("\t")[1]);
		} while (mode != 5 && count < 100000);
		
		// ASSERT
		assertTrue("Sim Mode is not MODE_RUN(5)\nMODE_ID=" + mode, mode == MODE_RUN);

		// CLEAN UP
		varserv.put("trick.exec_freeze()\n");
	}

	private void guiStartSim() throws IOException {
		// ARRANGE
		String statusMsg, expStatus = "Freeze OFF";
		int counter = 0;

		startApplication(true);
		assumeTrue("Sim Control Panel didn't start...", simControl != null);

		WaitForSimControlApplication guiControl = (WaitForSimControlApplication) simControl;

		// ACT
		varserv.put("trick.var_send_once(\"trick_sys.sched.mode\")");
		int mode = Integer.parseInt(varserv.get().split("\t")[1]);
		assumeTrue("Sim Mode was not MODE_FREEZE at test start", mode == MODE_FREEZE);

		guiControl.clearStatusMsgs();
		guiControl.startSim();

		do {
			counter++;
			sleep(500);
			statusMsg = guiControl.getStatusMessages();
		} while(statusMsg.isEmpty() && counter < 5);

		guiControl.freezeSim();

		// ASSERT
		assertTrue("Simulation did not start!\n" + statusMsg, statusMsg.indexOf(expStatus) != -1);
	}

	private void headlessFreezeSim() throws IOException {
		// ARRANGE
		String out; 
		int mode = 1, count = 0;

		simControl = new HeadlessSimControlApplication(host, port);
		sleep(1000);
		
		simControl.connect();
		sleep(1000);
		
		simControl.startSim();
		sleep(1000);

		varserv.put("trick.var_send_once(\"trick_sys.sched.mode\")");
		mode = Integer.parseInt(varserv.get().split("\t")[1]);
		assumeTrue("Sim Mode was not MODE_RUN at test start", mode == MODE_RUN);

		// ACT
		simControl.freezeSim();
		do {
			count ++;
			varserv.put("trick.var_send_once(\"trick_sys.sched.mode\")");
			mode = Integer.parseInt(varserv.get().split("\t")[1]);
		} while (mode != 1 && count < 100000);
		
		// ASSERT
		assertTrue("Sim Mode is not MODE_FREEZE (1)\nMODE_ID=" + mode, mode == MODE_FREEZE);
	}

	private void guiFreezeSim() throws IOException {
		// ARRANGE
		String statusMsg, expStatus = "Freeze ON";
		int counter = 0;

		startApplication(true);
		assumeTrue("Sim Control Panel didn't start...", simControl != null);

		WaitForSimControlApplication guiControl = (WaitForSimControlApplication) simControl;

		// ACT
		guiControl.startSim();
		guiControl.freezeSim();

		do {
			counter++;
			sleep(500);
			statusMsg = guiControl.getStatusMessages();
		} while(statusMsg.indexOf(expStatus) < 0 && counter < 5);

		// ASSERT
		assertTrue("Simulation did not freeze!\n" + statusMsg, statusMsg.indexOf(expStatus) != -1);
	}

	private static void startApplication(boolean startConnected) {
		if(simControl == null) {
			if(startConnected)  // Launch Testing SimControlPanel with the provided connection info
				WaitForSimControlApplication.launchAndWait(WaitForSimControlApplication.class, host, port);
			else  // Launch Testing SimControlPanel
				WaitForSimControlApplication.launchAndWait(WaitForSimControlApplication.class);

			handleAppSetup();
		} else {
			System.err.println("SimControlApplication is already Running...");
		}
    }

	private static void handleAppSetup() {
		// Set up the required variables for testing
		simControl = Application.getInstance(WaitForSimControlApplication.class);

		// Ensure that everything got set up correctly.
		assumeTrue("SimControlApplicationTest is not ready yet!", simControl.isReady());
	}
}