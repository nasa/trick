package trick.common;

import java.awt.AWTException;
import java.awt.Component;
import java.awt.Point;
import java.awt.Robot;
import java.awt.event.InputEvent;
import java.awt.event.KeyEvent;

import java.util.Timer;
import java.util.TimerTask;

public class GUIController extends Robot {
	private long depressTime;

	public GUIController() throws AWTException {
		super();
		depressTime = 25;
	}

	public GUIController(long ms) throws AWTException {
		super();
		depressTime = ms;
	}
	
	public void typeString(String entry) {
		for (char c : entry.toCharArray()) {
			if (Character.isUpperCase(c)) {
				keyPress(KeyEvent.VK_SHIFT);
			} else {
				keyRelease(KeyEvent.VK_SHIFT);
			}
			int keycode = KeyEvent.getExtendedKeyCodeForChar(c);
			keyTap(keycode);
			sleep(25);
		}
	}

	public void clearTextField() {
		keyPress(KeyEvent.VK_CONTROL);
		sleep(depressTime);

		keyTap('A');
		sleep(depressTime);

		keyRelease(KeyEvent.VK_CONTROL);
		sleep(depressTime);

		keyTap(KeyEvent.VK_BACK_SPACE);
	}

	public void keyTap(int keycode) {
		keyPress(keycode);
		sleep(depressTime);
		keyRelease(keycode);
	}

	public void delayedKeyTap(int keycode, long delay) {
		Timer timer = new Timer();
		timer.schedule(new TimerTask() {
			@Override
			public void run() {  keyTap(keycode);  }
		}, delay);
	}

	public void mouseClick(int buttons) {
		mousePress(buttons);
		sleep(depressTime);
		mouseRelease(buttons);
	}

	public void mouseClickAt(int buttons, Point pos) {
		mouseMove(pos.x, pos.y);
		mouseClick(buttons);
	}

	public void mouseClickAt(int buttons, Component comp) {
		Point pos = comp.getLocationOnScreen();
		pos.translate(comp.getWidth()/2, comp.getHeight()/2);
		mouseClickAt(buttons, pos);
	}
	
	private void sleep(long ms) {
		try {  Thread.sleep(ms);  } catch (Exception ignored) {  }
	}
}
