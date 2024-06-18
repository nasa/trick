
//========================================
//	Package
//========================================
package trick.common.framework;

import java.awt.Component;

import javax.swing.ActionMap;

import trick.common.framework.Session;
import trick.common.framework.TaskService;

public class AppContext {
	public Class getApplicationClass() {
		return null;
	}

	public ActionMap getActionMap(Class c, Object o) {
		return null;
	}

	public Session getSessionStorage() {
		return null;
	}

	public Session getLocalStorage() {
		return null;
	}

	public TaskService getTaskService() {
		return null;
	}

	public void addComponent(Component c) {

	}
}
