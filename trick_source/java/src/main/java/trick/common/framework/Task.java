
//========================================
//	Package
//========================================
package trick.common.framework;

import javax.swing.SwingWorker;

import trick.common.framework.BaseApplication;

public abstract class Task <T, V> extends SwingWorker <T, V>{

	public Task(BaseApplication app) {

	}

	// protected abstract Void doInBackground();
	protected abstract void finished();
	protected void succeeded(Void ignored) {
		
	}

	protected final void setProgress(float percentage) {

	}
	
}
