package trick.tv;


import org.jdesktop.application.Application;


public class WaitForTVApplication extends TVApplication {
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
     * Launch the specified subclsas of TVApplication and block
     * (wait) until it's startup() method has run.
     */
    public static void launchAndWait(Class<? extends WaitForTVApplication> applicationClass) {
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
    			Application app = Application.getInstance(WaitForTVApplication.class);
    			if (app instanceof WaitForTVApplication) {
    				if (((WaitForTVApplication)app).isReady()) {  
    					break;
    				}
    			}
    		}
    	}
    }
}
