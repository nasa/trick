
//========================================
//	Package
//========================================
package trick.common.framework;

import java.awt.event.ActionEvent;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.awt.event.HierarchyEvent;
import java.awt.event.HierarchyListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.Component;
import java.awt.Container;
import java.awt.Frame;
import java.awt.Window;

import java.beans.Beans;

import java.lang.IllegalAccessException;
import java.lang.InstantiationException;
import java.lang.NoSuchMethodException;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;

import java.util.concurrent.CopyOnWriteArrayList;
import java.util.EventObject;
import java.util.List;

import javax.swing.JComponent;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JRootPane;
import javax.swing.RootPaneContainer;

import trick.common.framework.AppContext;
import trick.common.framework.View;

public abstract class BaseApplication {

	//========================================
	//	Attributes
	//========================================
	
	// -- PUBLIC -----------------------------

	// -- PRIVATE ----------------------------
	private View mainView;
	private List<ExitListener> exitListeners;

	private final AppContext context;
	private final SetUpCloseOperations MAIN_FRAME_SETUP = (Component c) -> initMainFrameClose();
	private final SetUpCloseOperations WINDOW_SETUP = (Component c) -> initWindowClose(c);
	
	private static BaseApplication application = null;

	private static final String INITIALIZATION_MARKER="BaseApplication.initRootPaneContainer";
	private static final String WINDOW_STATE_NORMAL_BOUNDS = "WindowState.normalBounds";

	// -- PROTECTED --------------------------

	//========================================
	//	Constructors
	//========================================
	protected BaseApplication() {
		exitListeners = new CopyOnWriteArrayList<ExitListener>();
		context = new AppContext();
	}

	//========================================
	//	Public Methods
	//========================================

	public static void launch(Class<? extends BaseApplication> c, String[] args) {

	}

	public void show() {

	}

	public void show(JDialog d) {
		if(d != null) {
			initRootPaneContainer(d);
			d.setVisible(true);
		}
	}

	public void show(View v) {

	}

	public void exit() {  exit(null);  }

	public void exit(final EventObject event) {

	}

	public void addExitListener(ExitListener listener) {
		exitListeners.add(listener);
	}

	public void removeExitListener(ExitListener listener) {
		exitListeners.remove(listener);
	}

	public void saveSession(Window win) {

	}

	//========================================
	//	Protected Methods
	//========================================
	
	protected void configureWindow(Window root) {
		if (root != null)
			context.addComponent(root);
	}

	//========================================
	//	Private Methods
	//========================================

	private static <T extends BaseApplication> T create(Class<T> appClass) {
		T app;
		// AppContext ctx;

		// TODO: Set up System property
		/*	if (!Beans.isDesignTime()) {
					
				try {
					System.setProperty("java.net.useSystemProxies", "true");
				} catch (SecurityException ignoreException) {
					// Unsigned apps can't set this property. 
				}
			}
		*/

		// Create an instance of <T>
		try {
			app = createInstanceOf(appClass);
		} catch (Exception e) {
			System.err.println(e.getMessage());
			return null;
		}
		
		// ctx = app.getContext();

		// TODO: Add platform info to resource map
		/*	ResourceMap appResourceMap = ctx.getResourceMap();
			final PlatformType platform = AppHelper.getPlatform();
			appResourceMap.putResource(ResourceMap.KEY_PLATFORM, platform);

			//Generic registration with the Mac OS X application menu
			if (PlatformType.OS_X.equals(platform)) {
				try {
					OSXAdapter.setQuitHandler(application, Application.class.getDeclaredMethod("handleQuit", (Class[])null));
				} catch (Exception e) {
					logger.log(Level.SEVERE, "Cannot set Mac Os X specific handler for Quit event", e);
				}
			}
		*/
		
		// TODO: Set up Look and Feel
		/*	if (!Beans.isDesignTime()) {
				String key = "Application.lookAndFeel";
				String lnfResource = appResourceMap.getString(key);
				String lnf = (lnfResource == null) ? "system" : lnfResource;
				try {
					if (lnf.equalsIgnoreCase("system")) {
						String name = UIManager.getSystemLookAndFeelClassName();
						UIManager.setLookAndFeel(name);
					} else if (lnf.equalsIgnoreCase("nimbus")) {    
						for (LookAndFeelInfo info : UIManager.getInstalledLookAndFeels()) {
							if ("Nimbus".equals(info.getName())) {
								UIManager.setLookAndFeel(info.getClassName());
								break;
							}
						}
					} else if (!lnf.equalsIgnoreCase("default")) {
						UIManager.setLookAndFeel(lnf);
					}
				} catch (Exception e) {
					String s = "Couldn't set LookandFeel " + key + " = \"" + lnfResource + "\"";
					logger.log(Level.WARNING, s, e);
				}
			}
		*/

		return app;
	}

	private void initRootPaneContainer(RootPaneContainer c) {
		SetUpCloseOperations closeOps = WINDOW_SETUP;
		if(c == getMainFrame())  closeOps = MAIN_FRAME_SETUP;

		Component root = c.getRootPane().getParent();

		try {  initializeOnce(c);  }
		catch(IllegalArgumentException ignored) {  return;  }

		configureWindow(safe_cast(root, Window.class));
		closeOps.setupClose(root);
		handleContainerBounds(root);
		reloadContainerState(root);
		centerWindow(root);
		
	}

	//========================================
	//	Helper Methods
	//========================================

	private static <T> T createInstanceOf(Class<T> cls)  throws NoSuchMethodException, 
																InstantiationException, 
																IllegalAccessException, 
																InvocationTargetException {
		Constructor<T> clsCtor = cls.getDeclaredConstructor();

		if(!clsCtor.canAccess(null)) {
			try { clsCtor.setAccessible(true); }
			catch (SecurityException UNUSED) { }
		}

		return clsCtor.newInstance();
	}

	private String getSessionFileName(Window w) {
		if(w == null) return null;
		
		return window.getName() + ".session.xml";
	}

	private static boolean appIsLaunched() {  return application != null;  }

	private void initializeOnce(RootPaneContainer c) {
		JComponent rootPane = c.getRootPane();
		Object prop = rootPane.getClientProperty(INITIALIZATION_MARKER);
		
		if (prop != null)
			throw new IllegalArgumentException("Container already initialized.");
		
		rootPane.putClientProperty(rootPane, Boolean.TRUE);
	}

	private <T> T safe_cast(Object o, Class<T> c) {
		try {  return c.cast(o);  }
		catch (ClassCastException e) {  return null;  }
	}

	private void initMainFrameClose() {
		JFrame mf = getMainFrame();

		mf.addWindowListener(new WindowAdapter() {
			@Override
			public void windowClosing(WindowEvent e) {
				exit(e);
			}	
		});

		mf.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
	}

	private void initWindowClose(Component c) {
		Window window = safe_cast(c, Window.class);
		window.addHierarchyListener(new HierarchyListener() {
			@Override
			public void hierarchyChanged(HierarchyEvent event) {
				boolean showing_changed = (event.getChangeFlags() & HierarchyEvent.SHOWING_CHANGED) != 0;
				if (showing_changed) {
					Window secondaryWindow = safe_cast(event.getSource(), Window.class);
					if(secondaryWindow != null && !secondaryWindow.isShowing()) {
						saveSession(secondaryWindow);
					}
				}
			}
		});
	}

	private void handleContainerBounds(Component comp) {
		Window wind = safe_cast(comp, Window.class);

		if(safe_cast(comp, JFrame.class) != null) {
			handleJFrameBounds(comp);
		}

		if (wind != null) {
			boolean bounds_not_set =  !comp.isValid() 
									|| comp.getWidth() == 0 
									|| comp.getHeight() == 0;
			String filename = getSessionFileName(wind);

			if (bounds_not_set)  wind.pack();

			try {
				if(filename != null) {
					context.getSessionStorage().restore(comp, filename);
				}
			} catch (Exception e) {
				// TODO: handle exception
			}
		}
	}

	private void handleJFrameBounds(Component comp) {
		comp.addComponentListener(new ComponentListener() {
			@Override
			public void componentResized(ComponentEvent e) {
				JFrame frame = safe_cast(e.getComponent(), JFrame.class);
				boolean both_not_maxxed = (frame.getExtendedState() & Frame.MAXIMIZED_BOTH) == 0;
				if (both_not_maxxed) {
					JRootPane rootPane = frame.getRootPane();
					rootPane.putClientProperty(WINDOW_STATE_NORMAL_BOUNDS, frame.getBounds());
				}
			}

			@Override
			public void componentMoved(ComponentEvent e) {}

			@Override
			public void componentHidden(ComponentEvent e) {}

			@Override
			public void componentShown(ComponentEvent e) {}
		});
	}

	private void reloadContainerState(Component c) {

	}

	private void centerWindow(Component c) {

	}

	//========================================
	//	Getters/Setters
	//========================================

	public static BaseApplication getInstance() throws IllegalStateException {
		return getInstance(GenericApplication.class);
	}

	public static <T extends BaseApplication> T getInstance(Class<T> appClass) throws IllegalStateException {
		if (Beans.isDesignTime() && application == null) {
			application = create(appClass);
		}

		if (appIsLaunched()) {
			return appClass.cast(application);
		} else {
			throw new IllegalStateException("Application did not launch...");
		}
	}

	public JFrame getMainFrame() {
		if(mainView != null)
			return mainView.getFrame();
		else
			return null;
	}

	public void setMainFrame(JFrame nFrame) {
		if(mainView != null)
			mainView.setFrame(nFrame);
	}

	public View getMainView() {
		return mainView;
	}

	public void setMainView(View nView) {
		mainView = nView;
	}

	public final AppContext getContext() {
		return context;
	}

	public ExitListener[] getExitListeners() {
		int size = exitListeners.size();
		ExitListener[] el_array = exitListeners.toArray(new ExitListener[size]);

		return el_array;
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

	private class GenericApplication extends BaseApplication {
		public GenericApplication() {
			super();
		}

		@Override
		protected void initialize(String[] args) { /* UNUSED */ }

		@Override
		protected void startup() { /* UNUSED */ }

	}

	private interface SetUpCloseOperations {
		public void setupClose(Component c);
	}
}
