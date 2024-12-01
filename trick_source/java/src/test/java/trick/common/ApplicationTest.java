package trick.common;

import java.util.ArrayList;

import javax.swing.Action;
import javax.swing.ActionMap;
import org.jdesktop.application.ResourceMap;

import trick.common.ActionInfo;

public abstract class ApplicationTest {
	protected ArrayList<ActionInfo> coreActionInfo, supportActionInfo, miscActionInfo;
	protected ActionMap actionContext;
	protected ResourceMap resourceContext;
    
    public static String getTrickHome() {
        String path;
        
        // Getting TRICK_HOME environment variable
        path = System.getenv("TRICK_HOME");
        
        // Getting Trick's home directory if TRICK_HOME isn't set
        if(path != null) {
            if(path.endsWith("/"))
                path.substring(0, path.length() - 1);
        } else {
            path = System.getProperty("user.dir");
            int cutoff = path.indexOf("/trick_source");
            path = (cutoff > 0) ? path.substring(0, cutoff) : "";
        }

        return path;
    }

	public boolean compStringArray(String[] a, String[] b) {
		boolean same = true;
		for(int i = 0; i < a.length && i < b.length; i++) {
			same = same && sameLetters(a[i], b[i]);
			if(!same) return same;
		}
		return same;
	}

	public boolean sameLetters(String str1, String str2) {
		String a = str1.replaceAll("\\s+", "").toLowerCase(),
			   b = str2.replaceAll("\\s+", "").toLowerCase();
		return a.equals(b);
	}

	public static void sleep(long ms) {
		try {Thread.sleep(ms);} catch(Exception ignored) {}
	}

    private String getActionText(Action action) {
        return (String)action.getValue(Action.NAME);
    }

    private String getActionShortDescription(Action action) {
        return (String)action.getValue(Action.SHORT_DESCRIPTION);
    }

	private Action getActionFromKey(String actionKey) {
		String errMsg = String.format("No ActionMap set. Action '%s' cannot be searched for.\n", actionKey);
		// assumeNotNull(errMsg, actionContext);
		return actionContext.get(actionKey);
	}

	protected void setupExpectedActionInfo() {
		coreActionInfo = new ArrayList<ActionInfo>();
		supportActionInfo = new ArrayList<ActionInfo>();
		miscActionInfo = new ArrayList<ActionInfo>();

		getCoreActionInfo();
		getSupportActionInfo();
		getMiscActionInfo();
	}

	// protected void verifyActionInfo(ActionInfo aInfo) {
	// 	Action action = getActionFromKey(aInfo.name);
	// 	assumeNotNull(String.format("ActionMap.get(\"%s\") = null", aInfo.name), action);

	// 	String actualText = getActionText(action);
	// 	String actualDesc = getActionShortDescription(action);

	// 	assertEquals(aInfo.text, actualText);
	// 	assertEquals(aInfo.description, actualDesc);
	// }

	// protected void verifyResourceInfo(String key, String expectedStr) {
	// 	String resourceText, errMsg = String.format("No ResourceMap set. Resource '%s' cannot be searched for.\n", key);
	// 	assumeNotNull(errMsg, resourceContext);

	// 	resourceText = resourceContext.getString(key);
	// 	assertEquals(expectedStr, resourceText);
	// }

	protected abstract void getCoreActionInfo();
	protected abstract void getSupportActionInfo();
	protected abstract void getMiscActionInfo();
}
