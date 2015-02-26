package trick.common;


import static org.junit.Assert.assertTrue;

import org.jdesktop.application.Application;
import org.junit.BeforeClass;
import org.junit.Test;



/**
 * Test the Actions that are defined by the {@link RunTimeTrickApplication}.
 * Depends on ResourceBundle resources/RunTimeTrickApplication.properties
 *
 * @author Hong Chen
 */
public class RunTimeTrickApplicationTest {

    @BeforeClass
    public static void unitSetup() {
        WaitForRunTimeTrickApplication.launchAndWait(WaitForRunTimeTrickApplication.class);
    }

    @Test
	public void testReady() {		
		assertTrue("TrickApplicationTest is not ready yet!", application().isReady());
	}

    /**
     * Verify that all implemented actions exist as well as their text, shortDescription properties.
     */
    /*@Test
    public void testDefinedActions() {
        String[] actionNames = {"connect", "disconnect"};
        String[] actionTexts = {"Connect", "Disconnect"};
        String[] actionShortDescriptions = {"Connect to the simulation.", "Disconnect from the simulation."};
        for (int i = 0; i< actionNames.length; i++) {
        	CheckApplicationProperties.checkAction(application().actionMap, actionNames[i]); 
        	CheckApplicationProperties.checkActionText(application().actionMap, actionNames[i], actionTexts[i]);
        	CheckApplicationProperties.checkActionShortDescription(application().actionMap, actionNames[i], actionShortDescriptions[i]);
        }
    }*/

    @Test
	public void testExit() {
		application().removeExitListener(application().exitListener);
		application().exit();
		assertTrue(application().isEnded);
	}
    
    private static WaitForRunTimeTrickApplication application() {
       return Application.getInstance(WaitForRunTimeTrickApplication.class);
    }

}
