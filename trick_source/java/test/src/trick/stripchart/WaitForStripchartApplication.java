package trick.stripchart;


import org.jdesktop.application.Application;


public class WaitForStripchartApplication extends StripchartApplication {
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
     * Launch the specified subclsas of StripchartApplication and block
     * (wait) until it's startup() method has run.
     */
    public static void launchAndWait(Class<? extends WaitForStripchartApplication> applicationClass) {
    	synchronized(lock) {
    		String[] args = new String[] {"-demo"};
    		try {
    			Application.launch(applicationClass, args);
    		} catch (Exception e) {
    			lock.notifyAll();
    			return;
    		}
    		
    		while(true) {
    			try {
    				lock.wait();
    			}
    			catch (InterruptedException e) {
    				System.err.println("launchAndWait interrupted!");
    				break;
    			}
    			Application app = Application.getInstance(WaitForStripchartApplication.class);
    			if (app instanceof WaitForStripchartApplication) {
    				if (((WaitForStripchartApplication)app).isReady()) {  
    					break;
    				}
    			}
    		}
    	}
    }
}
