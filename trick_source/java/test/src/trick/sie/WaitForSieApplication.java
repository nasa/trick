package trick.sie;


import org.jdesktop.application.Application;


public class WaitForSieApplication extends SieApplication {
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
     * Launch the specified subclsas of SieApplication and block
     * (wait) until it's startup() method has run.
     */
    public static void launchAndWait(Class<? extends WaitForSieApplication> applicationClass) {
    	synchronized(lock) {
    		sieResourcePath = "resources" + java.io.File.separator + "S_sie.resource";
    		Application.launch(applicationClass, new String[]{});
    		while(true) {
    			try {
    				lock.wait();
    			}
    			catch (InterruptedException e) {
    				System.err.println("launchAndWait interrupted!");
    				break;
    			}
    			Application app = Application.getInstance(WaitForSieApplication.class);
    			if (app instanceof WaitForSieApplication) {
    				if (((WaitForSieApplication)app).isReady()) {  
    					break;
    				}
    			}
    		}
    	}
    }
}
