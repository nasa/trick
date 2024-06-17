
//========================================
//	Package
//========================================
package trick.common.framework;

import java.awt.Window;
import java.awt.event.ActionEvent;
import java.lang.reflect.Constructor;
import java.util.EventObject;
import java.util.List;

import javax.swing.JDialog;
import javax.swing.JFrame;

import trick.common.framework.AppContext;
import trick.common.framework.View;

public abstract class BaseApplication {

	//========================================
	//	Attributes
	//========================================
	
	// -- PUBLIC -----------------------------

	// -- PRIVATE ----------------------------
	private final AppContext context;
	// -- PROTECTED --------------------------

	//========================================
	//	Constructors
	//========================================
	protected BaseApplication() {
		context = new AppContext();
	}

	//========================================
	//	Public Methods
	//========================================

	public static void launch(Class<? extends BaseApplication> c, String[] args) {

	}

	// public static BaseApplication getInstance() {
	// 	return null;
	// }

	public static <T extends BaseApplication> T getInstance(Class<T> appClass) {
		return null;
	}

	public void show() {

	}

	public void show(JDialog d) {

	}

	public void show(View v) {

	}

	public void exit() {  exit(null);  }

	public void exit(ActionEvent event) {

	}

	public void addExitListener(ExitListener e) {

	}

	public void removeExitListener(ExitListener e) {
		
	}

	//========================================
	//	Protected Methods
	//========================================
	
	protected void configureWindow(Window root) {

	}

	//========================================
	//	Private Methods
	//========================================

	private static <T extends BaseApplication> create(Class<T> appClass) {
		T app = createInstanceOf(appClass);
		AppContext ctx = app.getContext();
	}

	//========================================
	//	Helper Methods
	//========================================

	private static <T> createInstanceOf(Class<T> cls) {
		Constructor<T> clsCtor = cls.getDeclaredConstructor();

		if(!canAccess(clsCtor)) {
			try { clsCtor.setAccessible(true); }
			catch (SecurityException UNUSED) { }
		}

		return clsCtor.newInstance();
	}

	//========================================
	//	Getters/Setters
	//========================================

	public JFrame getMainFrame() {
		return null;
	}

	public View getMainView() {
		return null;
	}

	public AppContext getContext() {
		return null;
	}

	public ExitListener[] getExitListeners() {
		return null;
	}

	//========================================
	//	Undefined Methods
	//========================================

	protected abstract void initialize(String[] args);

	protected abstract void startup();

	protected void ready() {}

	protected void shutdown() {}

	//========================================
	//	Inner Classes
	//========================================

	public abstract class ExitListener {
		public abstract boolean canExit(EventObject e);
		public abstract void willExit(EventObject e);
	}
}
