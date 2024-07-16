
//========================================
//	Package
//========================================
package trick.common.framework;

import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

import trick.common.framework.Task;

public class TaskService extends ThreadPoolExecutor {
	private String name;

	public TaskService(String name) {
		super(3,10,1L, TimeUnit.SECONDS, new LinkedBlockingQueue<Runnable>());

		this.name = name;

	}

	public void execute(Task t) {
		/**
		 * // Input Validation
		 * if (task == null) {
         *     throw new IllegalArgumentException("null task");
         * }
         * if (!task.isPending() || (task.getTaskService() != null)) {
         *     throw new IllegalArgumentException("task has already been executed");
         * }
		 * 
		 * // Setup for task execution
         * task.setTaskService(this);
         * // TBD: what if task has already been submitted?
         * task.addPropertyChangeListener(taskPCL);
		 * 
		 * // Add Task to Queue
         * List<Task<?, ?>> oldTaskList, newTaskList;
         * synchronized (tasks) {
         *     oldTaskList = tasks.getList();
         *     tasks.addLast(task);
         *     newTaskList = tasks.getList();
         * }
         * firePropertyChange(new TaskPropertyChangeEvent(oldTaskList, newTaskList, null, task));
		 * 
         * maybeBlockTask(task);
         * executorService.execute(task);
		 */
	}

	public final String getName() {  return name;  }

	private void maybeBlockTask(Task task) {

	}
	
}