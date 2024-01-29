package trick.simcontrol;

import java.util.ArrayList;
import java.util.Iterator;

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
public class SimControlApplicationTest extends ApplicationTest {
	private static int numSims = 0;
	private static String socketInfo;

	private WaitForSimControlApplication simcontrol;

	public SimControlApplicationTest() {
		setupExpectedActionInfo();
		simcontrol = null;
	}
		
	@BeforeClass
	public static void setUpBeforeClass() {
		socketInfo = startBasicSim();
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
	public void testDefinedActions() {
		// ARRANGE
		startApplication();
		Iterator<ActionInfo> 	core_iterator 	 = coreActionInfo.iterator(),
							 	support_iterator = supportActionInfo.iterator(),
								misc_iterator	 = miscActionInfo.iterator();

		// ACT & ASSERT
		core_iterator.forEachRemaining(aInfo -> verifyActionInfo(aInfo));
		support_iterator.forEachRemaining(aInfo -> verifyActionInfo(aInfo));
		misc_iterator.forEachRemaining(aInfo -> verifyActionInfo(aInfo));
	}
    
    @Test
    public void testDefinedKeyText() {
		startApplication();
		verifyResourceInfo("fileMenu.text", "&File");
    }
	
	@Test
	/**
	 * Testing that the connect() action functions when a valid host and 
	 * port is given. 
	 */
	public void testConnectAction() {
		// ARRANGE
		String simDir, statusMsg,
			   expDir = getTrickHome() + "/trick_sims/SIM_basic/S_main",
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

	private void startApplication() {
		if(simcontrol == null) {
			// Launch Testing SimControlPanel
			WaitForSimControlApplication.launchAndWait(WaitForSimControlApplication.class);

			// Set up the required variables for testing
			simcontrol = Application.getInstance(WaitForSimControlApplication.class);
			actionContext = simcontrol.actionMap;
			resourceContext = simcontrol.resourceMap;

			// Ensure that everything got set up correctly.
			assumeTrue("SimControlApplicationTest is not ready yet!", simcontrol.isReady());
		} else {
			System.err.println("SimControlApplication is already Running...");
		}
    }

	private void startApplication(String hostPort) {
		if(simcontrol == null) {
			// Launch Testing SimControlPanel
			WaitForSimControlApplication.launchAndWait(WaitForSimControlApplication.class, hostPort);

			// Set up the required variables for testing
			simcontrol = Application.getInstance(WaitForSimControlApplication.class);
			actionContext = simcontrol.actionMap;
			resourceContext = simcontrol.resourceMap;

			// Ensure that everything got set up correctly.
			assumeTrue("SimControlApplicationTest is not ready yet!", simcontrol.isReady());
		} else {
			System.err.println("SimControlApplication is already Running...");
		}
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
}