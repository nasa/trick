package trick.common;

import static org.junit.Assert.assertEquals;
import static org.junit.Assume.assumeNotNull;

import java.util.ArrayList;

import javax.swing.Action;
import javax.swing.ActionMap;

import trick.common.ActionInfo;

public abstract class ApplicationTest {
	protected ArrayList<ActionInfo> coreActionInfo, supportActionInfo, miscActionInfo;
	protected ActionMap actionContext;
	protected ResourceMap resourceContext;

	public void verifyActionInfo(ActionInfo aInfo) {
		Action action = getActionFromKey(aInfo.name);
		assumeNotNull(String.format("ActionMap.get(\"%s\") = null", aInfo.name), action);

		String actualText = getActionText(action);
		String actualDesc = getActionShortDescription(action);

		assertEquals(aInfo.text, actualText);
		assertEquals(aInfo.description, actualDesc);
	}

	public void verifyResourceInfo(String key, String expectedStr) {
		String resourceText, errMsg = String.format("No ResourceMap set. Resource '%s' cannot be searched for.\n", key);
		assumeNotNull(errMsg, resourceContext);

		resourceText = resourceContext.get(key);
		assertEquals(expectedStr, resourceText);
	}

	protected void setupExpectedActionInfo() {
		coreActionInfo = new ArrayList<ActionInfo>();
		supportActionInfo = new ArrayList<ActionInfo>();
		miscActionInfo = new ArrayList<ActionInfo>();

		getCoreActionInfo();
		getSupportActionInfo();
		getMiscActionInfo();
	}

	protected abstract void getCoreActionInfo();
	protected abstract void getSupportActionInfo();
	protected abstract void getMiscActionInfo();

    private String getActionText(Action action) {
        return (String)action.getValue(Action.NAME);
    }

    private String getActionShortDescription(Action action) {
        return (String)action.getValue(Action.SHORT_DESCRIPTION);
    }

	private Action getActionFromKey(String actionKey) {
		String errMsg = String.format("No ActionMap set. Action '%s' cannot be searched for.\n", actionKey);
		assumeNotNull(errMsg, actionContext);
		return actionContext.get(actionKey);
	}
}
