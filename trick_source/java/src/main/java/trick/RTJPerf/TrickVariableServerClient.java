package trick.rtperf;

import trick.common.utils.VariableServerConnection;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Handles the network communication with the Trick Variable Server.
 * It maps the jobs at startup, manages thread-specific subscriptions,
 * and processes the incoming real-time job execution data.
 */
public class TrickVariableServerClient implements Runnable {

    private VariableServerConnection vsConnection;
    private RealTimeJobPieChart gui;
    private boolean running = true;
    private int numThreads = 1;
    
    // Metadata structure to track job properties
    private static class JobMeta {
        int index; 
        String name; 
        int thread;
        public JobMeta(int index, String name, int thread) {
            this.index = index; 
            this.name = name; 
            this.thread = thread;
        }
    }
    
    private List<JobMeta> allJobs = new ArrayList<>();
    private List<JobMeta> activeThreadJobs = new ArrayList<>();

    public TrickVariableServerClient(VariableServerConnection vsConnection, RealTimeJobPieChart gui) {
        this.vsConnection = vsConnection;
        this.gui = gui;
    }

    @Override
    public void run() {
        try {
            // 1. Ensure frame logging is turned on so prev_frame_time_seconds is populated
            vsConnection.put("trick.frame_log_on()\n");
            vsConnection.put("trick.var_debug(1)\n"); // Enable debug for troubleshooting
            vsConnection.put("trick.var_cycle(0.01)\n"); // 100 Hz data collection
            
            // 2. Pause the variable server while we configure the initial mapping
            vsConnection.put("trick.var_pause()\n");
            vsConnection.clear(); 
            
            // 3. Get Thread Count from the simulation
            vsConnection.put("trick.var_send_once(\"trick_frame_log.frame_log.num_threads\")\n");
            String threadResponse = vsConnection.get();
            if (threadResponse != null && threadResponse.split("\t").length >= 2) {
                try { 
                    numThreads = Integer.parseInt(threadResponse.split("\t")[1].trim()); 
                } catch (Exception e) {
                    System.err.println("Failed to parse thread count, defaulting to 1.");
                }
            }
            
            // 4. Get Job Count from the scheduler's job vector
            int numberOfJobs = 0;
            vsConnection.put("trick.var_get_stl_size(\"trick_sys.sched.all_jobs_vector\")\n");
            String sizeResponse = vsConnection.get(); 
            if (sizeResponse != null && sizeResponse.split("\t").length >= 2) {
                try { 
                    numberOfJobs = Integer.parseInt(sizeResponse.split("\t")[1].trim()); 
                } catch (Exception e) { 
                    System.err.println("Failed to parse job vector size.");
                    return; 
                }
            }

            // 5. Synchronized Job Mapping (Prevents TCP Deserialization Desync)
            for (int i = 0; i < numberOfJobs; i++) {
                vsConnection.put("trick.var_send_once(\"trick_sys.sched.all_jobs_vector[" + i + "].name\")\n");
                String nameResponse = vsConnection.get();
                
                vsConnection.put("trick.var_send_once(\"trick_sys.sched.all_jobs_vector[" + i + "].thread\")\n");
                String threadIdResponse = vsConnection.get();
                
                if (nameResponse != null && threadIdResponse != null) {
                    try {
                        // CRITICAL FIX: var_send_once returns message type "5" (VS_SEND_ONCE), not "1" (VS_VAR_EXISTS)
                        if (nameResponse.startsWith("5") && threadIdResponse.startsWith("5")) {
                            String name = nameResponse.split("\t")[1].trim().replace("\"", "");
                            int threadId = Integer.parseInt(threadIdResponse.split("\t")[1].trim());
                            allJobs.add(new JobMeta(i, name, threadId));
                        }
                    } catch (Exception e) {
                        System.err.println("Error mapping job index: " + i);
                    }
                }
            }

            // Initialize the GUI thread selection and subscribe to the main thread (Thread 0)
            gui.initializeThreads(numThreads, this);
            subscribeToThread(0);

            // 6. Data Reading Loop
            String line;
            while (running && (line = vsConnection.get()) != null) {
                String[] tokens = line.split("\t");
                // Token 0 represents the message type. "0" is a cyclic data update.
                if (tokens.length >= 4 && tokens[0].equals("0")) { 
                    
                    double simTime = 0.0;
                    int mode = 5; 
                    try { 
                        double timeTics = Double.parseDouble(tokens[1].trim()); 
                        double timeTicValue = Double.parseDouble(tokens[2].trim());
                        if (timeTicValue > 0) {
                            simTime = timeTics / timeTicValue;
                        }
                        mode = Integer.parseInt(tokens[3].trim());
                    } catch (Exception e) {}
                    
                    String modeStr = getModeString(mode);
                    List<RealTimeJobPieChart.JobDuration> frameData = new ArrayList<>();
                    Map<String, Double> aggregatedDurations = new HashMap<>();
                    
                    // Parse the job durations from the remaining tokens
                    for (int i = 0; i < activeThreadJobs.size(); i++) {
                        int tokenIdx = i + 4; 
                        if (tokenIdx < tokens.length) {
                            try {
                                double duration = Double.parseDouble(tokens[tokenIdx].trim());
                                // Filter out uninitialized memory or skipped jobs
                                if (duration > 0.0 && duration < 100.0) {
                                    if (duration < 0.000001) {
                                        duration = 0.000001; // Floor value for UI rendering
                                    }
                                    JobMeta meta = activeThreadJobs.get(i);
                                    aggregatedDurations.put(meta.name, aggregatedDurations.getOrDefault(meta.name, 0.0) + duration);
                                }
                            } catch (Exception e) {}
                        }
                    }
                    
                    for (Map.Entry<String, Double> entry : aggregatedDurations.entrySet()) {
                        frameData.add(new RealTimeJobPieChart.JobDuration(entry.getKey(), entry.getValue()));
                    }
                    gui.updateFrameData(simTime, modeStr, frameData);
                }
            }
        } catch (IOException e) {
            System.err.println("Variable Server connection lost: " + e.getMessage());
        }
    }
    
    /**
     * Dynamically updates the Variable Server subscription to monitor jobs on a specific thread.
     * 
     * @param threadId The ID of the execution thread to monitor.
     */
    public synchronized void subscribeToThread(int threadId) {
        try {
            // Build the subscription payload into a single transaction to prevent buffer overlap
            StringBuilder subQuery = new StringBuilder();
            subQuery.append("trick.var_pause()\n");
            subQuery.append("trick.var_clear()\n");
            subQuery.append("trick.var_set_copy_mode(1)\n"); 
            subQuery.append("trick.var_cycle(0.02)\n"); // 50 Hz UI Refresh Rate
            
            // Subscribe to simulation state variables
            subQuery.append("trick.var_add(\"trick_sys.sched.time_tics\")\n");
            subQuery.append("trick.var_add(\"trick_sys.sched.time_tic_value\")\n");
            subQuery.append("trick.var_add(\"trick_sys.sched.mode\")\n");
            
            activeThreadJobs.clear();
            for (JobMeta job : allJobs) {
                if (job.thread == threadId) {
                    activeThreadJobs.add(job);
                    subQuery.append("trick.var_add(\"trick_sys.sched.all_jobs_vector[")
                            .append(job.index)
                            .append("].prev_frame_time_seconds\")\n");
                }
            }
            subQuery.append("trick.var_unpause()\n");
            subQuery.append("trick.var_send()\n"); // Start the cyclic data stream
            
            vsConnection.put(subQuery.toString());
            
        } catch (IOException e) {
            System.err.println("Failed to update thread subscription: " + e.getMessage());
        }
    }
    
    private String getModeString(int modeId) {
        switch(modeId) {
            case 0: return "Initialization"; 
            case 1: return "Freeze"; 
            case 4: return "Step";
            case 5: return "Run"; 
            case 6: return "Exit"; 
            default: return "Unknown (" + modeId + ")";
        }
    }
}