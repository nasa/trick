package trick.common;


import javax.swing.JComponent;
import javax.swing.JPanel;

import org.jdesktop.application.Application;


public class WaitForTrickApplication extends TrickApplication {
    static Object lock = new Object(); 
    
    boolean isEnded;
    
    protected JComponent createMainPanel() {
    	return new JPanel();
    }
    
    @Override
    protected void end() {
    	isEnded = true;
    }
  
    @Override
    protected void ready() {
    	super.ready();
    	synchronized(lock) {
    		lock.notifyAll();
    	}
    }
    

    /**
     * Launch the specified subclsas of TrickApplication and block
     * (wait) until it's startup() method has run.
     */
    public static void launchAndWait(Class<? extends WaitForTrickApplication> applicationClass) {
    	synchronized(lock) {
    		Application.launch(applicationClass, new String[]{});
    		while(true) {
    			try {
    				lock.wait();
    			}
    			catch (InterruptedException e) {
    				System.err.println("launchAndWait interrupted!");
    				break;
    			}
    			Application app = Application.getInstance(WaitForTrickApplication.class);
    			if (app instanceof WaitForTrickApplication) {
    				if (((WaitForTrickApplication)app).isReady()) {    					
    					break;
    				}
    			}
    		}
    	}
    }
}
