package trick.simcontrol;

import javax.swing.JFrame;
import javax.swing.JComboBox;
import javax.swing.JEditorPane;
import javax.swing.JTextField;
import javax.swing.JToggleButton;
import javax.swing.text.Document;

import java.awt.AWTException;
import java.awt.Component;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.Robot;
import java.awt.event.InputEvent;
import java.awt.event.KeyEvent;
import java.awt.event.WindowEvent;

import org.jdesktop.application.Application;
import org.jdesktop.swingx.JXEditorPane;

import trick.common.GUIController;
import trick.simcontrol.SimControlApplication;


public class WaitForSimControlApplication extends SimControlApplication {
    static Object lock = new Object(); 
    
    boolean isEnded;
	public GUIController controller;

	public WaitForSimControlApplication() throws AWTException{
		super();
		controller = new GUIController();
		// controller.setAutoDelay(250);
	}
	
	public void sleep(long ms) {
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
		JTextField runDir = getSimRunDirField();

		selectAndClearField(list);
		controller.typeString(socketInfo);
		controller.waitForIdle();
		selectComponent(runDir); // Change the focus to lock in the typed info
	}

	public void toggleDataRecButton() {
		JToggleButton button = getDataRecButton();
		selectComponent(button);
	}

	public void toggleRealTimeButton() {
		JToggleButton button = getRealTimeButton();
		selectComponent(button);
	}

	private void selectAndClearField(Component field) {
		selectComponent(field);
		controller.clearTextField();
		controller.waitForIdle();
	}

	private void selectComponent(Component comp) {
		controller.mouseClickAt(InputEvent.BUTTON1_DOWN_MASK, comp);
		controller.waitForIdle();
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

	public String getStatusMessages() {
		JXEditorPane status = getEditorPane();
		String msg = "";
		try {
			Document doc = status.getDocument();
			msg = doc.getText(0, doc.getLength());
		} catch (Exception e) {
			msg = e.getMessage();
		}
		return msg;
	}

	public boolean isDataRecOn() { return getDataRecButton().isSelected(); }
	public boolean isRealTimeOn() { return getRealTimeButton().isSelected(); }
}