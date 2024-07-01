
//========================================
//	Package
//========================================
package trick.common.framework;

import java.awt.Component;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;

import javax.swing.ActionMap;

import trick.common.TrickApplication;
import trick.common.framework.BaseApplication;
import trick.common.framework.Session;
import trick.common.framework.TaskService;
import trick.common.utils.TrickResources;

public class AppContext {
	
	private final Class appClass;
	private TrickResources resources = null;
	private Session session;

	public AppContext(Class<? extends BaseApplication> appClass) {
		this.appClass = appClass;
		session = new Session();
	}

	public Class getApplicationClass() {
		return appClass;
	}

	public ActionMap getActionMap(Class c, Object o) {
		return null;
	}

	public TrickResources getResourceMap() {
		if (resources == null)
			parseResources();
		
		return resources;
	}

	public Session getSessionStorage() {
		return session;
	}

	public Session getLocalStorage() {
		return session;
	}

	public TaskService getTaskService() {
		return null;
	}

	public void addComponent(Component c) {

	}
    
    private TrickResources parseResources(Class<? extends BaseApplication> app) throws IOException, FileNotFoundException {
		TrickResources prop;
		File resource;
		String path;
		
    	if(app.getSimpleName().equals("TrickApplication")) {
    		prop = new TrickResources();
    	} else {
    		Class superApp = app.getSuperclass();
    		prop = new TrickResources(parseResources(superApp));
		}
    		
    	path = TrickApplication.getResourcePath(app);
    	resource = new File(path);
    	prop.loadProperties(resource);
    	
    	int filePos = path.lastIndexOf("/") + 1;
    	path = path.substring(0, filePos);
    	if(prop.getProperty("PATH") != null)
	    	path += ";" + prop.getProperty("PATH");
    	
    	prop.setProperty("PATH", path);
    	
    	return prop;
    }

    private void parseResources() {
    	try {
	    	resources = parseResources(appClass);
    	} catch(IOException ioe) {
    		System.err.println(ioe.getMessage());
    	}
    }
}
