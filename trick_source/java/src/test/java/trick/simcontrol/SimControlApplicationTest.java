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

import trick.common.ActionInfo;
import trick.common.ApplicationTest;

/**
 * 
 * Test SimControlApplication life cycle.
 * 
 * @author hchen
 * @intern mrockwell2
 *
 */
@FixMethodOrder(MethodSorters.NAME_ASCENDING)
public class SimControlApplicationTest extends ApplicationTest {
	private static int numSims = 0;
	private static String socketInfo;

	private WaitForSimControlApplication app;
		
	@BeforeClass
	public static void setUpBeforeClass() {
		socketInfo = startBasicSim();
		numSims++;
		WaitForSimControlApplication.launchAndWait(WaitForSimControlApplication.class);
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
		app = application();
		setupExpectedActionInfo();

		actionContext = app.actionMap;
		resourceContext = app.resourceMap;
	}

	@After
	public void tearDown() {
		app.removeExitListener(app.exitListener);
		app.exit();
		assertTrue(app.isEnded);
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
	public void testReady() {		
		assertTrue("SimControlApplicationTest is not ready yet!", app.isReady());
	}
	
	@Test
	public void testDefinedCoreActions() {
		Iterator<ActionInfo> iterator = coreActionInfo.iterator();
		iterator.forEachRemaining(aInfo -> verifyActionInfo(aInfo));
	}
	
	@Test
	public void testDefinedSupportActions() {
		Iterator<ActionInfo> iterator = supportActionInfo.iterator();
		iterator.forEachRemaining(aInfo -> verifyActionInfo(aInfo));
	}
	
	@Test
	public void testDefinedMiscActions() {
		Iterator<ActionInfo> iterator = miscActionInfo.iterator();
		iterator.forEachRemaining(aInfo -> verifyActionInfo(aInfo));
	}
    
    @Test
    public void testDefinedKeyText() {
		verifyResourceInfo("fileMenu.text", "&File");
    }
	@Test
	public void testConnectionSuccess() {
		app.editRunningSimList(socketInfo);
		String simDir = app.getRunningSimInfo();

		String expectedDir = getTrickHome();

		expectedDir += "/trick_sims/SIM_basic/S_main";

		assertTrue("SimControlPanel did not connect!", simDir.startsWith(expectedDir));
	}

	private static WaitForSimControlApplication application() {
        return Application.getInstance(WaitForSimControlApplication.class);
    }
}