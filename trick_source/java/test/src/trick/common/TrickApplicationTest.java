package trick.common;

import static org.junit.Assert.assertTrue;

import org.jdesktop.application.Application;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

/**
 * Test TrickApplication life cycle.
 * 
 * @author hchen
 *
 */
public class TrickApplicationTest {
		
	@BeforeClass
	public static void setUpBeforeClass() throws Exception {		
		WaitForTrickApplication.launchAndWait(WaitForTrickApplication.class);
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
		assertTrue("TrickApplicationTest is not ready yet!", application().isReady());
	}
	
	@Test
	public void testPropertyLocation() {
		application();
        application();
        boolean rightLocation = (TrickApplication.propDirectory.equals(System.getenv("HOME") + java.io.File.separator + ".trick"))
		                        || (TrickApplication.propDirectory.equals(System.getenv("TRICK_USER_HOME") + java.io.File.separator + ".trick"));
		assertTrue("The default property location is not at ../.trick!", rightLocation);
	}
	
	/**
     * Verify that all implemented actions exist as well as their text, shortDescription properties.
     */
    @Test
    public void testDefinedActions() {
        String[] actionNames = {"exitConfirmation", "helpContents", "showAboutBox", "closeAboutBox"}; 
        String[] actionTexts = {"Show Exit Confirmation Prompt", "Help Contents", "About...", "OK"};
        String[] actionShortDescriptions = {"Turn On/Off popup box that verifies if the user really wants to close this application.",
        		                            "Show Help Contents", "Show the application's about box dialog", "Close the about box dialog"};
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
	
	private static WaitForTrickApplication application() {
        return Application.getInstance(WaitForTrickApplication.class);
    }
}
