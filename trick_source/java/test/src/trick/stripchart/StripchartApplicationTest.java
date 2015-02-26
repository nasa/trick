package trick.stripchart;

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
 * Test StripchartApplication life cycle.
 * 
 * @author hchen
 *
 */
public class StripchartApplicationTest {
		
	@BeforeClass
	public static void setUpBeforeClass() throws Exception {		
		WaitForStripchartApplication.launchAndWait(WaitForStripchartApplication.class);
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
		assertTrue("StripchartApplicationTest is not ready yet!", application().isReady());
	}
	
	/**
     * Verify that all implemented actions exist as well as their text, shortDescription properties.
     */
    @Test
    public void testDefinedActions() {
        String[] actionNames = {"toggle_xrange", "start_stop", "legend_onoff", "postscript"};
        String[] actionTexts = {"Toggle Range", "Toggle Start/Stop", "Legend On/Off", "Postscript..."};
        String[] actionShortDescriptions = {"Set x-axis range to automatic scaling (All), or fixed-width (Strip).",
        		                            "Start/Stop graph refreshing.",
        		                            "Hide/Display the graph's Legend.",
        		                            "Save a copy of the graph to postscript file."};
        for (int i = 0; i < actionNames.length; i++) {            
        	CheckApplicationProperties.checkAction(application().actionMap, actionNames[i]);
        	CheckApplicationProperties.checkActionText(application().actionMap, actionNames[i], actionTexts[i]);
        	CheckApplicationProperties.checkActionShortDescription(application().actionMap, actionNames[i], actionShortDescriptions[i]);
        }
    }
    
    @Test
    public void testDefinedKeyText() {
    	CheckApplicationProperties.checkKeyText(application().resourceMap, "fileMenu.text", "&File");
    	CheckApplicationProperties.checkKeyText(application().resourceMap, "settingsMenu.text", "&Settings");
    }
	
	@Test
	public void testExit() {
		application().removeExitListener(application().exitListener);
		application().exit();
		assertTrue(application().isEnded);
	}
	
	private static WaitForStripchartApplication application() {
        return Application.getInstance(WaitForStripchartApplication.class);
    }
}
