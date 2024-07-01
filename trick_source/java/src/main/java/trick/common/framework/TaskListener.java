
//========================================
//	Package
//========================================
package trick.common.framework;

import trick.common.framework.TaskEvent;

public interface TaskListener<T, V> {

	public void doInBackground(TaskEvent<Void> event);
	
	public void finished(TaskEvent<Void> event);

	public void cancelled(TaskEvent<Void> event);

	public void interrupted(TaskEvent<InterruptedException> event);
	
	public void succeeded(TaskEvent<T> event);

	public void failed(TaskEvent<Throwable> event);

}
