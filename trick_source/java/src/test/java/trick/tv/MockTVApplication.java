package trick.tv;


import org.jdesktop.application.Application;


public class MockTVApplication extends TVApplication {
    private boolean connected = false;
    private static String hostname;
    private static int port;
    private static Object lock = new Object(); 
    
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

    public final boolean isConnected() { return connected; }

    public void connectSimulation() {
        setTargetSimulation(hostname, port);
    }
    
    public static void main(String[] args) {
        for(int i = 0; i < args.length; i++) {
            String arg = args[i];
            if((i+1) < args.length) {
                if(arg.equals("--host")) {
                    hostname = args[++i];
                } else if(arg.equals("--port")) {
                    port = Integer.parseInt(args[++i]);
                }
            }
        }

        synchronized(lock) {   		
    		Application.launch(MockTVApplication.class, args);
    		while(true) {
    			try {
    				lock.wait();
    			}
    			catch (InterruptedException e) {
    				System.err.println("launchAndWait interrupted!");
    				break;
    			}
    			Application app = Application.getInstance(MockTVApplication.class);
    			if (app instanceof MockTVApplication) {
    				if (((MockTVApplication) app).isReady()) {  
    					break;
    				}
    			}
    		}
    	}
	}
}