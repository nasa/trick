package trick.simcontrol;


import org.jdesktop.application.Application;

import trick.simcontrol.utils.SimControlActionController;

import java.awt.*;
import java.awt.event.ActionEvent;

import javax.swing.Action;
import javax.swing.AbstractAction;


public class HeadlessSimControlApplication extends SimControlApplication {

	public HeadlessSimControlApplication() {
		this("", -1);
	}

    public HeadlessSimControlApplication(String hostname, int portNum) {
		super.setHostPort(hostname, portNum);
		actionController = new SimControlActionController();
	}

	@Override
	protected Action getAction(String key) { 
		return new AbstractAction() {
			public void actionPerformed(ActionEvent UNUSED) {}
		}; 
	}

	@Override
	protected void updateGUI() { /* UNUSED */ }
}