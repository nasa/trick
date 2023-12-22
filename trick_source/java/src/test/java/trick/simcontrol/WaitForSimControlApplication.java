package trick.simcontrol;


import java.util.Timer;
import java.util.TimerTask;

import javax.swing.JComboBox;
import javax.swing.JTextField;

import java.awt.AWTException;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.Robot;
import java.awt.event.InputEvent;
import java.awt.event.KeyEvent;

import org.jdesktop.application.Application;

import trick.common.GUIController;
import trick.simcontrol.SimControlApplication;


public class WaitForSimControlApplication extends SimControlApplication {
    static Object lock = new Object(); 
    
    boolean isEnded;
	private GUIController controller;

	public WaitForSimControlApplication() throws AWTException{
		super();
		controller = new GUIController();
		// controller.setAutoDelay(250);
	}
	
	private void sleep(long ms) {
		try {  Thread.sleep(ms);  } catch (Exception ignored) {  }
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

	public static void updateConnectionInfo(String host, int port) {
		updateConnectionInfo(host + "," + port);
	}
    
	public static void launchAndWait(Class<? extends WaitForSimControlApplication> applicationClass, String hostPortInfo) {
		updateConnectionInfo(hostPortInfo);
		launchAndWait(applicationClass);
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

	//========================================
    //    Robot Methods
    //========================================
	public void editRunningSimList(String socketInfo) {
		JComboBox list = getRunningSimList();
		Point focus = list.getLocationOnScreen();

		// Select the field to be edited
		controller.mouseClickAt(InputEvent.BUTTON1_DOWN_MASK, list);
		sleep(200);
		controller.waitForIdle();

		// Make sure the field is empty
		controller.clearTextField();
		controller.waitForIdle();

		// Type in the connection info
		controller.typeString(socketInfo);
		controller.waitForIdle();

		// Change the focus to lock in the typed info
		focus.translate(0, -list.getHeight() * 5);
		controller.mouseClickAt(InputEvent.BUTTON1_DOWN_MASK, focus);
		sleep(200);

		// Click the 'Connect' button
		focus.translate(list.getWidth() + 50, list.getHeight() * 5);
		controller.mouseClickAt(InputEvent.BUTTON1_DOWN_MASK, focus);
		sleep(500);
	}

	//========================================
    //    Getter Methods
    //========================================
	public String getRunningSimInfo() { 
		JTextField runDir = getSimRunDirField();
		if (runDir == null) {
			return "";
		}
		return runDir.getText(); 
	}
}