package trick.simcontrol;


import org.jdesktop.application.Application;


public class WaitForSimControlApplication extends SimControlApplication {
    static Object lock = new Object(); 
    
    boolean isEnded;
    
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
     * Launch the specified subclsas of SimControlApplication and block
     * (wait) until it's startup() method has run.
     */
    public static void launchAndWait(Class<? extends WaitForSimControlApplication> applicationClass) {
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
    			Application app = Application.getInstance(WaitForSimControlApplication.class);
    			if (app instanceof WaitForSimControlApplication) {
    				if (((WaitForSimControlApplication)app).isReady()) {  
    					break;
    				}
    			}
    		}
    	}
    }
}
