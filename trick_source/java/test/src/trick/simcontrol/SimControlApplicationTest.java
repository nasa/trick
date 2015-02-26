package trick.simcontrol;

import static org.junit.Assert.assertTrue;

import org.jdesktop.application.Application;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import trick.common.CheckApplicationProperties;

/**
 * 
 * Test SimControlApplication life cycle.
 * 
 * @author hchen
 *
 */
public class SimControlApplicationTest {
		
	@BeforeClass
	public static void setUpBeforeClass() throws Exception {		
		WaitForSimControlApplication.launchAndWait(WaitForSimControlApplication.class);
	}

	@AfterClass
	public static void tearDownAfterClass() throws Exception {
	}

	@Before
	public void setUp() throws Exception {		
	}

	@After
	public void tearDown() throws Exception {
	}
	
	@Test
	public void testReady() {		
		assertTrue("SimControlApplicationTest is not ready yet!", application().isReady());
	}
	
	/**
     * Verify that all implemented actions exist as well as their text, shortDescription properties.
     */
    @Test
    public void testDefinedActions() {
        String[] actionNames = {"saveStatusMsgs", "clearStatusMsgs", "startTV", "startMTV", "freezeAt", "freezeIn",
        		                "checkpointObjects", "throttle", "connect", "stepSim", "recordingSim", "startSim",
        		                "realtime", "freezeSim", "dumpDMTCPChkpnt", "dumpChkpntASCII", "shutdownSim", "loadChkpnt", "lite"};
        String[] actionTexts = {"Save Status Msgs...", "Clear Status Msgs...", "Start Trick View", 
        		                "Start Event/Malfunction Trick View", "Freeze At...", "Freeze In...", 
        		                "Checkpoint Objects...", "Throttle...", "Connect", "Step", "Data Rec On",
        		                "Start", "RealTime On", "Freeze", "Dump DMTCP Chkpnt", "Dump Chkpnt", "Shutdown", "Load Chkpnt", "Lite"};
        String[] actionShortDescriptions = {"Save Status Messages", "Clear Status Messages", "Start Trick View (TV)",
        		                            "Start Event/Malfunction Trick View (MTV)", "Freeze At", "Freeze In",
        		                            "Checkpoint the specified objects", "Throttle", "Connect to the specified server at specified port",
        		                            "Step through sim initialization", "Data Recording On/Off", "Start the simulation",
        		                            "Realtime on/off", "Freeze/Stop the simulation", "Dump DMTCP Checkpoint", "Dump ASCII Checkpoint",
        		                            "Shutdown Simulation", "Load ASCII Checkpoint", "Lite/Full"};
        
        for (int i = 0; i < actionNames.length; i++) {            
        	CheckApplicationProperties.checkAction(application().actionMap, actionNames[i]); 
        	CheckApplicationProperties.checkActionText(application().actionMap, actionNames[i], actionTexts[i]);
        	CheckApplicationProperties.checkActionShortDescription(application().actionMap, actionNames[i], actionShortDescriptions[i]);            
        }             
    }
    
    @Test
    public void testDefinedKeyText() {
    	CheckApplicationProperties.checkKeyText(application().resourceMap, "fileMenu.text", "&File");
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
