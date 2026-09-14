package trick.rtperf;

import trick.common.utils.VariableServerConnection;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class TrickVariableServerClient implements Runnable {

    private VariableServerConnection vsConnection;
    private RealTimeJobPieChart gui;
    private boolean running = true;
    
    private int numThreads = 1;
    
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
            System.out.println("Configuring Variable Server Stream...");
            vsConnection.put("trick.var_pause()");
            vsConnection.clear(); 
            
            // 1. Discover the number of threads via Python stdout redirection
            vsConnection.put("import sys");
            vsConnection.put("trick.var_set_send_stdio(True)");
            vsConnection.put("sys.stdout.write(str(trick.exec_get_num_threads()))");
            String threadResponse = vsConnection.get();
            if (threadResponse != null) {
                String[] tokens = threadResponse.split("\t");
                if (tokens.length >= 3 && tokens[0].equals("4")) {
                    try { numThreads = Integer.parseInt(tokens[2].trim()); } catch (NumberFormatException e) {}
                }
            }
            vsConnection.put("trick.var_set_send_stdio(False)");
            
            // 2. Discover the total number of jobs
            int numberOfJobs = 0;
            vsConnection.put("trick.var_get_stl_size(\"trick_sys.sched.all_jobs_vector\")");
            String sizeResponse = vsConnection.get(); 
            if (sizeResponse != null) {
                String[] tokens = sizeResponse.split("\t");
                if (tokens.length >= 2) {
                    try { numberOfJobs = Integer.parseInt(tokens[1].trim()); } catch (NumberFormatException e) { return; }
                }
            }
            
            System.out.println("Discovered " + numThreads + " threads and " + numberOfJobs + " jobs.");

            // 3. CHUNKED Batch request to safely map all jobs
            System.out.println("Mapping jobs to threads...");
            int chunkSize = 50; 
            for (int i = 0; i < numberOfJobs; i += chunkSize) {
                StringBuilder metaQuery = new StringBuilder();
                int endIdx = Math.min(i + chunkSize, numberOfJobs);
                
                for (int j = i; j < endIdx; j++) {
                    metaQuery.append("trick.var_send_once(\"trick_sys.sched.all_jobs_vector[").append(j).append("].name\")\n");
                    metaQuery.append("trick.var_send_once(\"trick_sys.sched.all_jobs_vector[").append(j).append("].thread\")\n");
                }
                
                vsConnection.put(metaQuery.toString()); 

                for (int j = i; j < endIdx; j++) {
                    String nameResponse = vsConnection.get();
                    String threadIdResponse = vsConnection.get();
                    
                    if (nameResponse != null && threadIdResponse != null) {
                        try {
                            if (nameResponse.startsWith("1") && threadIdResponse.startsWith("1")) {
                                String name = nameResponse.split("\t")[1].trim().replace("\"", "");
                                int threadId = Integer.parseInt(threadIdResponse.split("\t")[1].trim());
                                allJobs.add(new JobMeta(j, name, threadId));
                            }
                        } catch (Exception e) {}
                    }
                }
            }

            gui.initializeThreads(numThreads, this);
            subscribeToThread(0);

            // 4. Data Reading Loop
            String line;
            while (running && (line = vsConnection.get()) != null) {
                String[] tokens = line.split("\t");
                
                // Expected: [0]=indicator(0), [1]=time_tics, [2]=time_tic_value, [3]=mode, [4...N]=prev_frame_time_seconds
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
                    } catch (NumberFormatException e) {}
                    
                    String modeStr = getModeString(mode);
                    List<RealTimeJobPieChart.JobDuration> frameData = new ArrayList<>();
                    
                    // Using a Map just in case a job executes multiple times a frame and needs aggregating
                    Map<String, Double> aggregatedDurations = new HashMap<>();
                    
                    for (int i = 0; i < activeThreadJobs.size(); i++) {
                        int tokenIdx = i + 4; 
                        if (tokenIdx < tokens.length) {
                            try {
                                double duration = Double.parseDouble(tokens[tokenIdx].trim());
                                
                                // 1. Filter out Uninitialized memory (Huge numbers >= 100.0)
                                // 2. Filter out explicit 0.0 (Hasn't run this frame or not tracked)
                                // 3. Filter out negative numbers (C++ garbage)
                                if (duration > 0.0 && duration < 100.0) {
                                    
                                    // If a job is faster than 1 microsecond, give it a 1 microsecond floor so the GUI can render it!
                                    if (duration < 0.000001) {
                                        duration = 0.000001; 
                                    }
                                    
                                    JobMeta meta = activeThreadJobs.get(i);
                                    aggregatedDurations.put(meta.name, aggregatedDurations.getOrDefault(meta.name, 0.0) + duration);
                                }
                            } catch (NumberFormatException e) {
                                // Ignore parse errors silently
                            }
                        }
                    }
                    
                    for (Map.Entry<String, Double> entry : aggregatedDurations.entrySet()) {
                        frameData.add(new RealTimeJobPieChart.JobDuration(entry.getKey(), entry.getValue()));
                    }
                    
                    gui.updateFrameData(simTime, modeStr, frameData);
                }
            }
        } catch (IOException e) {
            System.err.println("Variable Server connection error: " + e.getMessage());
        }
    }
    
    public synchronized void subscribeToThread(int threadId) {
        try {
            // Build the ENTIRE subscription sequence into ONE string payload!
            // The Variable Server processes commands newline-by-newline, so this prevents 
            // the buffer flush issue we saw with var_pause/var_add desynchronization.
            StringBuilder subQuery = new StringBuilder();
            
            subQuery.append("trick.var_pause()\n");
            subQuery.append("trick.var_clear()\n");
            subQuery.append("trick.var_set_copy_mode(1)\n"); 
            subQuery.append("trick.var_cycle(0.02)\n"); 
            
            subQuery.append("trick.var_add(\"trick_sys.sched.time_tics\")\n");
            subQuery.append("trick.var_add(\"trick_sys.sched.time_tic_value\")\n");
            subQuery.append("trick.var_add(\"trick_sys.sched.mode\")\n");
            
            activeThreadJobs.clear();
            
            for (JobMeta job : allJobs) {
                if (job.thread == threadId) {
                    activeThreadJobs.add(job);
                    // Append every single job var_add command with a newline
                    subQuery.append("trick.var_add(\"trick_sys.sched.all_jobs_vector[").append(job.index).append("].prev_frame_time_seconds\")\n");
                }
            }

            subQuery.append("trick.var_unpause()\n");
            
            // Send the entire block over the socket in ONE put() call.
            vsConnection.put(subQuery.toString());
            
        } catch (IOException e) {
            System.err.println("Failed to subscribe to Thread " + threadId + ": " + e.getMessage());
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
    
    public void stopClient() {
        running = false;
        try { vsConnection.close(); } catch (Exception e) {}
    }
}