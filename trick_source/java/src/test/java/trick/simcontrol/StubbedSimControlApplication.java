package trick.simcontrol;


import org.jdesktop.application.Application;

import trick.simcontrol.utils.SimControlActionController;

import java.awt.*;
import java.awt.event.ActionEvent;
import java.util.Stack;

import javax.swing.Action;
import javax.swing.AbstractAction;


public class StubbedSimControlApplication extends SimControlApplication {

	public Stack<ActionID> actions;

	public StubbedSimControlApplication() {
		this("", -1);
	}

    public StubbedSimControlApplication(String hostname, int portNum) {
		super.setHostPort(hostname, portNum);
		actionController = new SimControlActionController();
		actions = new Stack<ActionID>();
	}

	@Override
	protected Action getAction(String key) { 
		return new AbstractAction() {
			public void actionPerformed(ActionEvent UNUSED) {}
		}; 
	}

	@Override
	protected void updateGUI() { /* UNUSED */ }

	@Override
	protected void startStatusMonitors() { /* UNUSED */}

	@Override
	public void connect() {
		actions.push(ActionID.CONNECT);
	}
}

enum ActionID {
	CONNECT,
	LITE,
	LOAD_CHKPNT,
	DUMP_CHKPNT,
	PART_CHKPNT,
	START,
	FREEZE,
	FREEZE_IN,
	FREEZE_AT,
	STEP,
	SHUTDOWN,
	REALTIME,
	RECORDING,
	THROTTLE,
	MTV,
	TV,
	SAVE_STATUS,
	CLEAR_STATUS,
	SET_FONT
}