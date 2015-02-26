package trick.tv;

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
 * Test TVApplication life cycle.
 * 
 * @author hchen
 *
 */
public class TVApplicationTest {
		
	@BeforeClass
	public static void setUpBeforeClass() throws Exception {		
		WaitForTVApplication.launchAndWait(WaitForTVApplication.class);
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
		assertTrue("TVApplicationTest is not ready yet!", application().isReady());
	}
	
	/**
     * Verify that all implemented actions exist as well as their text, shortDescription properties.
     */
    /*@Test
    public void testDefinedActions() {
        String[] actionNames = {"newSession", "openSession", "openAndSet", "set", "saveSession", "startMonitoring",
        			            "stopMonitoring", "stripchartSelection", "removeSelection", "addVariablesFromTree",
        			            "addVariablesFromList", "purgeBadRefs", "commitVariableAddition"};
        String[] actionTexts = {"New", "Open", "Open & Set", "Set", "Save", "Start the Monitor", "Stop the Monitor",
        		                "Stripchart Selection", "Remove Selection", "Add Selection", "Add Selection",
        		                "Purge BAD_REFs", "Ok"};
        String[] actionShortDescriptions = {"Clear the variable table.", "Replace the variable table with a TV input file without setting the values.", 
        		                            "Replace the variable table with a TV input file and set each value.", 
        		                            "Set the variable values from a TV input file without changing the variable table.", 
        		                            "Save the variable table to a TV input file.", 
        		                            "Start updating variable values.", "Stop updating variable values.", 
        		                            "Stripchart the selected variables.", "Delete the selected rows.", 
        		                            "Begin watching the selected variables.", "Begin watching the selected variables.", 
        		                            "Remove all variables that failed to resolve.", "Commit the variable additions."};
        for (int i = 0; i < actionNames.length; i++) {            
        	CheckApplicationProperties.checkAction(application().actionMap, actionNames[i]);
        	CheckApplicationProperties.checkActionText(application().actionMap, actionNames[i], actionTexts[i]);
        	CheckApplicationProperties.checkActionShortDescription(application().actionMap, actionNames[i], actionShortDescriptions[i]);
        }
    }*/
    
    @Test
    public void testDefinedKeyText() {
    	CheckApplicationProperties.checkKeyText(application().resourceMap, "fileMenu.text", "&File");
    	/*CheckApplicationProperties.checkKeyText(application().resourceMap, "actionsMenu.text", "&Actions");
    	CheckApplicationProperties.checkKeyText(application().resourceMap, "preferencesMenu.text", "&Preferences");
    	CheckApplicationProperties.checkKeyText(application().resourceMap, "notifyOnDisconnectCheckBoxMenuItem.text", "Not&ify on Disconnect");
    	CheckApplicationProperties.checkKeyText(application().resourceMap, "caseInsensitiveCheckBox.text", "Case Insensitive");
    	CheckApplicationProperties.checkKeyText(application().resourceMap, "containsRadioButton.text", "Contains");
    	CheckApplicationProperties.checkKeyText(application().resourceMap, "regularExpressionRadioButton.text", "RegEx");*/
    }
	
	@Test
	public void testExit() {
		application().removeExitListener(application().exitListener);
		application().exit();
		assertTrue(application().isEnded);
	}
	
	private static WaitForTVApplication application() {
        return Application.getInstance(WaitForTVApplication.class);
    }
}
