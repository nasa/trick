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
    
    private int numberOfJobs = 0;
    private int numThreads = 1;
    private Map<Double, String> jobIdToNameMap = new HashMap<>();

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
            
            // 1. Enable frame log to populate timeline arrays
            vsConnection.put("trick.trick_frame_log.set_enable(True)"); 
            
            // 2. Discover the number of threads
            vsConnection.put("trick.var_send_once(\"trick_frame_log.num_threads\")");
            String threadResponse = vsConnection.get();
            if (threadResponse != null && threadResponse.split("\t").length >= 2) {
                try {
                    numThreads = Integer.parseInt(threadResponse.split("\t")[1].trim());
                } catch (NumberFormatException e) {}
            }
            
            // 3. Discover the number of jobs
            vsConnection.put("trick.var_send_once(\"trick_sys.sched.all_jobs_vector.size()\")");
            String sizeResponse = vsConnection.get(); 
            if (sizeResponse != null && sizeResponse.split("\t").length >= 2) {
                try {
                    numberOfJobs = Integer.parseInt(sizeResponse.split("\t")[1].trim());
                } catch (NumberFormatException e) {
                    System.err.println("Could not parse job count.");
                    return;
                }
            }
            
            System.out.println("Discovered " + numThreads + " threads and " + numberOfJobs + " jobs.");

            // 4. Build the Job ID -> Job Name dictionary
            for (int i = 0; i < numberOfJobs; i++) {
                vsConnection.put("trick.var_send_once(\"trick_sys.sched.all_jobs_vector[" + i + "].frame_id\")");
                String idResponse = vsConnection.get();
                
                vsConnection.put("trick.var_send_once(\"trick_sys.sched.all_jobs_vector[" + i + "].name\")");
                String nameResponse = vsConnection.get();
                
                if (idResponse != null && nameResponse != null) {
                    try {
                        double frameId = Double.parseDouble(idResponse.split("\t")[1].trim());
                        String name = nameResponse.split("\t")[1].trim().replace("\"", "");
                        jobIdToNameMap.put(frameId, name);
                    } catch (Exception e) {}
                }
            }

            // 5. Initialize the GUI Thread Selector (Calls back safely on Swing Thread)
            gui.initializeThreads(numThreads, this);

            // 6. Subscribe to the Main Thread (Thread 0) to start
            subscribeToThread(0);

            // 7. Data Reading Loop
            String line;
            while (running && (line = vsConnection.get()) != null) {
                String[] tokens = line.split("\t");
                if (tokens.length > 2) { 
                    
                    // Parse SIM_MODE (token 1)
                    int mode = 5; // Default Run
                    try {
                        mode = Integer.parseInt(tokens[1].trim());
                    } catch (NumberFormatException e) {}
                    
                    String modeStr = getModeString(mode);
                    
                    // Decide which array to read based on mode
                    // Run(5) and Step(4) use `timeline` (first block). All others use `timeline_other` (second block).
                    int tokenIdx = 2; 
                    if (mode != 4 && mode != 5) {
                        tokenIdx = 2 + (numberOfJobs * 3); // Jump over the cyclic timeline variables
                    }
                    
                    List<RealTimeJobPieChart.JobDuration> frameData = new ArrayList<>();
                    
                    for (int i = 0; i < numberOfJobs; i++) {
                        if (tokenIdx + 2 < tokens.length) {
                            try {
                                double jobId = Double.parseDouble(tokens[tokenIdx].trim());
                                // ID 0 means empty timeline slot
                                if (jobId != 0.0) {
                                    double start = Double.parseDouble(tokens[tokenIdx + 1].trim());
                                    double stop  = Double.parseDouble(tokens[tokenIdx + 2].trim());
                                    double duration = stop - start;
                                    
                                    if (duration > 0.0) {
                                        String jobName = jobIdToNameMap.getOrDefault(jobId, "Unknown_" + jobId);
                                        frameData.add(new RealTimeJobPieChart.JobDuration(jobName, duration));
                                    }
                                }
                            } catch (NumberFormatException e) {}
                            
                            tokenIdx += 3; // Advance to next job triplet
                        }
                    }
                    
                    // Update GUI with Mode and Data
                    gui.updateFrameData(modeStr, frameData);
                }
            }
        } catch (IOException e) {
            System.err.println("Variable Server connection error: " + e.getMessage());
        }
    }
    
    /**
     * Called by the GUI when the user selects a new thread from the ComboBox.
     * Uses synchronized to prevent writing while the read loop might be blocking.
     */
    public synchronized void subscribeToThread(int threadId) {
        try {
            vsConnection.put("trick.var_pause()");
            vsConnection.clear(); 
            vsConnection.put("trick.var_set_copy_mode(1)"); // Top of Frame
            
            // 1. Subscribe to the sim mode
            vsConnection.add("trick_sys.sched.mode");
            
            // 2. Subscribe to the cyclic timeline (Run/Step)
            for (int i = 0; i < numberOfJobs; i++) {
                vsConnection.add("trick_frame_log.timeline[" + threadId + "][" + i + "].id");
                vsConnection.add("trick_frame_log.timeline[" + threadId + "][" + i + "].start"); 
                vsConnection.add("trick_frame_log.timeline[" + threadId + "][" + i + "].stop"); 
            }
            
            // 3. Subscribe to the non-cyclic timeline (Init/Freeze)
            for (int i = 0; i < numberOfJobs; i++) {
                vsConnection.add("trick_frame_log.timeline_other[" + threadId + "][" + i + "].id");
                vsConnection.add("trick_frame_log.timeline_other[" + threadId + "][" + i + "].start"); 
                vsConnection.add("trick_frame_log.timeline_other[" + threadId + "][" + i + "].stop"); 
            }

            vsConnection.put("trick.var_unpause()");
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