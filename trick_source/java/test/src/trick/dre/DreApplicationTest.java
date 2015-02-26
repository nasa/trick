package trick.dre;

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
 * Test DreApplication life cycle.
 * 
 * @author hchen
 *
 */
public class DreApplicationTest {
		
	@BeforeClass
	public static void setUpBeforeClass() throws Exception {		
		WaitForDreApplication.launchAndWait(WaitForDreApplication.class);
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
		assertTrue("DreApplicationTest is not ready yet!", application().isReady());
	}
	
	/**
     * Verify that all implemented actions exist as well as their text, shortDescription properties.
     */
    @Test
    public void testDefinedActions() {
        String[] actionNames = {"openDR", "saveDR", "selectDRBinary", "selectDRAscii", "selectDRHDF5", 
        		                "selectDRAlways",  "selectDRChanges", "selectDRStepChanges", "toggleSinglePrecision",
        		                "selectDRBuffer", "selectDRNoBuffer", "selectDRRingBuffer", "selectDRThreadBuffer",
        		                "addVariables", "removeSelected", "removeAll"};
        String[] actionTexts = {"Open...", "Save...", "DR Binary", "DR Ascii", "DR HDF5", "DR Always", "DR Changes",
        		                "DR Step Changes", "Single Precision", "DR Buffer", "DR No Buffer", "DR Ring Buffer",
        		                "DR Thread Buffer", "Add", "Remove Selected", "Remove All"};
        
        for (int i = 0; i < actionNames.length; i++) {            
        	CheckApplicationProperties.checkAction(application().actionMap, actionNames[i]); 
        	CheckApplicationProperties.checkActionText(application().actionMap, actionNames[i], actionTexts[i]);       	
        }
    }
    
    @Test
    public void testDefinedKeyText() {
    	CheckApplicationProperties.checkKeyText(application().resourceMap, "fileMenu.text", "&File");
    	CheckApplicationProperties.checkKeyText(application().resourceMap, "optionsMenu.text", "&Options");
    }
	
	@Test
	public void testExit() {
		application().removeExitListener(application().exitListener);
		application().exit();
		assertTrue(application().isEnded);
	}
	
	private static WaitForDreApplication application() {
        return Application.getInstance(WaitForDreApplication.class);
    }
}
