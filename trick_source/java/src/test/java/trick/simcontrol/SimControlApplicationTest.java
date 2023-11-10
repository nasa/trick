package trick.simcontrol;

import static org.junit.Assert.assertTrue;

import java.util.ArrayList;
import java.util.Iterator;

import org.jdesktop.application.Application;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

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
public class SimControlApplicationTest extends ApplicationTest {
		
	@BeforeClass
	public static void setUpBeforeClass() throws Exception {		
		WaitForSimControlApplication.launchAndWait(WaitForSimControlApplication.class);
	}

	@AfterClass
	public static void tearDownAfterClass() throws Exception {
	}

	@Before
	public void setUp() throws Exception {
		setupExpectedActionInfo();

		actionContext = application().actionMap;
		resourceContext = application().resourceMap;
	}

	@After
	public void tearDown() throws Exception {
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
		assertTrue("SimControlApplicationTest is not ready yet!", application().isReady());
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
	public void testExit() {
		application().removeExitListener(application().exitListener);
		application().exit();
		assertTrue(application().isEnded);
	}	

	private static WaitForSimControlApplication application() {
        return Application.getInstance(WaitForSimControlApplication.class);
    }
}