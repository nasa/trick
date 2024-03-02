package trick.simcontrol;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.Scanner;
import java.beans.Transient;
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

import trick.common.ActionInfo;
import trick.common.ApplicationTest;

/**
 * Test SimControlApplication life cycle.
 * 
 * @author hchen
 * @intern mrockwell2
 *
 */
public class SimControlApplicationStartupTest extends ApplicationTest {
	private static int numSims = 0;
	private static String socketInfo, basicSimDir;

	private WaitForSimControlApplication simcontrol;

	public SimControlApplicationStartupTest() {
		setupExpectedActionInfo();
		simcontrol = null;
	}
		
	@BeforeClass
	public static void setUpBeforeClass() {
		socketInfo = startBasicSim();
		basicSimDir = getTrickHome() + "/trick_sims/SIM_basic";
		numSims++;
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
		WaitForSimControlApplication.updateConnectionInfo(null, -1);
		if(simcontrol != null)	endApplication();
	}

	@After
	public void tearDown() {
		endApplication();
	}

	@Override
	protected void getCoreActionInfo() {
		coreActionInfo.add(new ActionInfo("connect", "Connect", "Connect to the specified server at specified port"));
		coreActionInfo.add(new ActionInfo("startSim", "Start", "Start the simulation"));
		coreActionInfo.add(new ActionInfo("freezeSim", "Freeze", "Freeze/Stop the simulation"));
		coreActionInfo.add(new ActionInfo("freezeAt", "Freeze At...", "Freeze At"));
		coreActionInfo.add(new ActionInfo("freezeIn", "Freeze In...", "Freeze In"));
		coreActionInfo.add(new ActionInfo("stepSim", "Step", "Step through sim initialization"));
		coreActionInfo.add(new ActionInfo("throttle", "Throttle...", "Throttle"));
		coreActionInfo.add(new ActionInfo("realtime", "RealTime On", "Realtime on/off"));
		coreActionInfo.add(new ActionInfo("shutdownSim", "Shutdown", "Shutdown Simulation"));
	}

	@Override
	protected void getSupportActionInfo() {
		supportActionInfo.add(new ActionInfo("checkpointObjects", "Checkpoint Objects...", "Checkpoint the specified objects"));
		supportActionInfo.add(new ActionInfo("dumpChkpntASCII", "Dump Chkpnt", "Dump ASCII Checkpoint"));
		supportActionInfo.add(new ActionInfo("loadChkpnt", "Load Chkpnt", "Load ASCII Checkpoint"));
		supportActionInfo.add(new ActionInfo("recordingSim", "Data Rec On", "Data Recording On/Off"));
		supportActionInfo.add(new ActionInfo("lite", "Lite", "Lite/Full"));
	}
		
	@Override
	protected void getMiscActionInfo() {
		miscActionInfo.add(new ActionInfo("showStatusFont", "Font...", "Show Status Pane Font"));
		miscActionInfo.add(new ActionInfo("saveStatusMsgs", "Save Status Msgs...", "Save Status Messages"));
		miscActionInfo.add(new ActionInfo("clearStatusMsgs", "Clear Status Msgs...", "Clear Status Messages"));

		miscActionInfo.add(new ActionInfo("startTV", "Start Trick View", "Start Trick View (TV)"));
		miscActionInfo.add(new ActionInfo("startMTV", "Start Event/Malfunction Trick View", "Start Event/Malfunction Trick View (MTV)"));
	}
	
	@Test
	public void testDefinitions() {
		// ARRANGE
		startApplication();
		Iterator<ActionInfo> 	core_iterator 	 = coreActionInfo.iterator(),
							 	support_iterator = supportActionInfo.iterator(),
								misc_iterator	 = miscActionInfo.iterator();

		// ACT & ASSERT
		core_iterator.forEachRemaining(aInfo -> verifyActionInfo(aInfo));
		support_iterator.forEachRemaining(aInfo -> verifyActionInfo(aInfo));
		misc_iterator.forEachRemaining(aInfo -> verifyActionInfo(aInfo));
		
		verifyResourceInfo("fileMenu.text", "&File");
	}

	@Test
	/**
	 * Testing that the connect() action functions when a valid host and 
	 * port is given. 
	 */
	public void testConnection() {
		// ARRANGE
		String simDir, statusMsg,
			   expDir = basicSimDir + "/S_main",
			   expStatus = "";

		startApplication();
		simcontrol.clearStatusMsgs();
		simcontrol.editRunningSimList(socketInfo);
			   	   
		// ACT
		simcontrol.connect();
		simDir = simcontrol.getRunningSimInfo();
		statusMsg = simcontrol.getStatusMessages();

		// ASSERT
		assertTrue("SimControlPanel did not connect!", simDir.startsWith(expDir));
		assertTrue("Unexpected Error Message: \n\t" + statusMsg, statusMsg.isEmpty());
	}

	@Test
	public void testConnectionFailures() {
		// ARRANGE
		startApplication();
		cleanupPanel();

		ConnectionInfo_FormattingError();
		cleanupPanel();
		
		PortNumber_FormattingError();
		cleanupPanel();
	}

	/**
	 * Test the error handling when the connect() action is used with 
	 * incorrectly formatted information.
	 */
	public void ConnectionInfo_FormattingError() {
		// ARRANGE
		String statusMsg, 
			   expStatus = "Can't connect! Please provide valid host name and port number separated by : or whitespace!",
			   badInfo = socketInfo.replace(' ', '/');

		simcontrol.editRunningSimList(badInfo);

		// ACT
		simcontrol.connect();
		statusMsg = simcontrol.getStatusMessages();

		// ASSERT
		assertTrue("Unexpected Error Message: \n\t" + statusMsg, statusMsg.indexOf(expStatus) != -1);
	}

	/**
	 * Test the error handling when the connect() action is used with an 
	 * invalid port number.
	 */
	public void PortNumber_FormattingError() {
		// ARRANGE
		String statusMsg, 
			   expStatus = "is not a valid port number!",
			   badInfo = socketInfo + "A";

		simcontrol.editRunningSimList(badInfo);

		// ACT
		simcontrol.connect();
		statusMsg = simcontrol.getStatusMessages();

		// ASSERT
		assertTrue("Unexpected Error Message: \n\t" + statusMsg, statusMsg.indexOf(expStatus) != -1);
	}

	private void startApplication() {
		if(simcontrol == null) {
			// Launch Testing SimControlPanel
			WaitForSimControlApplication.launchAndWait(WaitForSimControlApplication.class);
			handleAppSetup();
		} else {
			System.err.println("SimControlApplication is already Running...");
		}
    }
	
	private void startApplication(boolean startConnected) {
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

	private void handleAppSetup() {
		// Set up the required variables for testing
		simcontrol = Application.getInstance(WaitForSimControlApplication.class);
		actionContext = simcontrol.actionMap;
		resourceContext = simcontrol.resourceMap;

		// Ensure that everything got set up correctly.
		assumeTrue("SimControlApplicationTest is not ready yet!", simcontrol.isReady());
	}

	private void endApplication() {
		if(simcontrol != null) {
			// Exit SimControlApplication
			simcontrol.exit();
			simcontrol.removeExitListener(simcontrol.exitListener);

			// Verify and clean up
			assumeTrue(simcontrol.isEnded);
			simcontrol = null;
		} else {
			System.err.println("There is no instance of SimControlApplication to stop...");
		}
	}

	private void cleanupPanel() {
		if(simcontrol != null) {
			simcontrol.clearStatusMsgs();
			assumeTrue("Unexpected Status Message!", simcontrol.getStatusMessages().isEmpty());
		}
	}
}