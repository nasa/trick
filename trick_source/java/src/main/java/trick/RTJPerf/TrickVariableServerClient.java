package trick.rtperf;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import trick.common.utils.VariableServerConnection;

/**
 * Handles network communication with the Trick Variable Server.
 * Maps jobs at startup, manages thread-specific subscriptions,
 * and processes incoming real-time job execution telemetry.
 * Uses binary format for efficient data transmission.
 */
public class TrickVariableServerClient implements Runnable {

    private VariableServerConnection vsConnection;
    private RealTimeJobPieChart gui;
    private boolean running = true;
    private int numThreads = 1;

    // Single source of truth for cycle rate (seconds). Default: 0.02s (50 Hz)
    private double currentCycleRate = 0.02;

    // Batch size for job metadata auto-discovery (50 jobs per round-trip)
    private static final int BATCH_SIZE = 50;

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
    // Synchronized guard to avoid race conditions during thread switching
    private final List<JobMeta> activeThreadJobs = new ArrayList<>();

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

            // 2. Retrieve the software frame and set it as the default cycle rate
            vsConnection.put("trick.var_send_once(\"trick_sys.sched.software_frame\")\n");
            String frameResponse = vsConnection.get();
            if (frameResponse != null && frameResponse.split("\t").length >= 2) {
                try {
                    double softwareFrame = Double.parseDouble(frameResponse.split("\t")[1].trim());
                    currentCycleRate = softwareFrame;
                    gui.setSoftwareFrameRate(softwareFrame);
                } catch (Exception e) {
                    System.err.println("Failed to parse software frame, using default 0.02s.");
                    currentCycleRate = 0.02;
                }
            }

            vsConnection.put("trick.var_cycle(" + currentCycleRate + ")\n");

            // 3. Pause the variable server while configuring initial mapping
            vsConnection.put("trick.var_pause()\n");
            vsConnection.clear();

            // 4. Get Thread Count from the simulation
            vsConnection.put("trick.var_send_once(\"trick_frame_log.frame_log.num_threads\")\n");
            String threadResponse = vsConnection.get();
            if (threadResponse != null && threadResponse.split("\t").length >= 2) {
                try {
                    numThreads = Integer.parseInt(threadResponse.split("\t")[1].trim());
                } catch (Exception e) {
                    System.err.println("Failed to parse thread count, defaulting to 1.");
                }
            }

            // 5. Get Job Count from the scheduler's job vector
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

            // 6. Batched Job Mapping (Chunks of 50 to minimize socket round-trips)
            for (int batchStart = 0; batchStart < numberOfJobs; batchStart += BATCH_SIZE) {
                int batchEnd = Math.min(batchStart + BATCH_SIZE, numberOfJobs);
                StringBuilder varList = new StringBuilder();

                for (int i = batchStart; i < batchEnd; i++) {
                    if (i > batchStart) {
                        varList.append(", ");
                    }
                    varList.append("trick_sys.sched.all_jobs_vector[").append(i).append("].name, ");
                    varList.append("trick_sys.sched.all_jobs_vector[").append(i).append("].thread");
                }

                int expectedCount = batchEnd - batchStart;
                // Build the command with the variable list as a single Python string
                String cmd = "trick.var_send_once(\"" + varList.toString() + "\", " + (expectedCount * 2) + ")\n";

                vsConnection.put(cmd);
                String batchResponse = vsConnection.get();

                if (batchResponse != null && batchResponse.split("\t").length > 0) {
                    String[] tokens = batchResponse.split("\t");

                    // Token 0 is msg type (5). Each job produces 2 values (name & thread ID).
                    if (tokens.length >= (expectedCount * 2) + 1) {
                        for (int i = 0; i < expectedCount; i++) {
                            try {
                                int nameIdx = 1 + (i * 2);
                                int threadIdx = nameIdx + 1;

                                String name = tokens[nameIdx].trim().replace("\"", "");
                                int threadId = Integer.parseInt(tokens[threadIdx].trim());
                                int jobIndex = batchStart + i;

                                allJobs.add(new JobMeta(jobIndex, name, threadId));
                            } catch (Exception e) {
                                System.err.println(
                                        "Error parsing batch job at relative index " + i + ": " + e.getMessage());
                            }
                        }
                    } else {
                        System.err.println("Warning: Batch response had fewer tokens than expected. " + "Expected "
                                + ((expectedCount * 2) + 1) + ", got " + tokens.length);
                    }
                }
            }

            // Register all discovered jobs with the GUI
            List<String> jobNamesList = new ArrayList<>();
            for (JobMeta job : allJobs) {
                jobNamesList.add(job.name);
            }
            gui.registerAllJobs(jobNamesList);

            // Initialize GUI thread selection and subscribe to Thread 0
            gui.initializeThreads(numThreads, this);
            subscribeToThread(0);

            // 7. Data Reading Loop (Binary format)
            while (running) {
                try {
                    // Get data from Variable Server in binary format
                    // The get() method handles binary parsing and returns tab-delimited values
                    String line = vsConnection.get(3 + activeThreadJobs.size());

                    if (line == null || line.isEmpty()) {
                        continue;
                    }

                    String[] tokens = line.split("\t");
                    if (tokens.length >= 4) {
                        processFrameData(tokens);
                    }
                } catch (Exception e) {
                    System.err.println("Error reading data: " + e.getMessage());
                }
            }
        } catch (IOException e) {
            System.err.println("Variable Server connection lost: " + e.getMessage());
        }
    }

    /**
     * Processes frame data from the Variable Server.
     * Expected format: [time_tics, time_tic_value, mode, job_duration_1, job_duration_2, ...]
     */
    private void processFrameData(String[] tokens) {
        double simTime = 0.0;
        int mode = 5;

        try {
            double timeTics = Double.parseDouble(tokens[0].trim());
            double timeTicValue = Double.parseDouble(tokens[1].trim());
            if (timeTicValue > 0) {
                simTime = timeTics / timeTicValue;
            }
            mode = Integer.parseInt(tokens[2].trim());
        } catch (Exception e) {
            System.err.println("Error parsing time/mode data: " + e.getMessage());
            return;
        }

        String modeStr = getModeString(mode);
        List<RealTimeJobPieChart.JobDuration> frameData = new ArrayList<>();
        Map<String, Double> aggregatedDurations = new HashMap<>();

        // Snapshot active jobs to prevent data race during thread switches
        List<JobMeta> currentJobsSnapshot;
        synchronized (this) {
            currentJobsSnapshot = new ArrayList<>(activeThreadJobs);
        }

        // Parse job durations
        for (int i = 0; i < currentJobsSnapshot.size(); i++) {
            int tokenIdx = i + 3;
            if (tokenIdx < tokens.length) {
                try {
                    double duration = Double.parseDouble(tokens[tokenIdx].trim());
                    if (duration > 0.0 && duration < 100.0) {
                        if (duration < 0.000001) {
                            duration = 0.000001; // Floor value for UI rendering
                        }
                        JobMeta meta = currentJobsSnapshot.get(i);
                        aggregatedDurations.put(meta.name, aggregatedDurations.getOrDefault(meta.name, 0.0) + duration);
                    }
                } catch (Exception e) {
                    // Skip this job if parsing fails
                }
            }
        }

        for (Map.Entry<String, Double> entry : aggregatedDurations.entrySet()) {
            frameData.add(new RealTimeJobPieChart.JobDuration(entry.getKey(), entry.getValue()));
        }

        gui.updateFrameData(simTime, modeStr, frameData);
    }

    /**
     * Dynamically updates Variable Server subscription to monitor jobs on a specific thread.
     * Batches var_add commands to avoid exceeding the 8192-byte message limit.
     *
     * @param threadId The ID of the execution thread to monitor.
     */
    public synchronized void subscribeToThread(int threadId) {
        try {
            // Step 1: Pause and clear the old subscription
            vsConnection.put("trick.var_pause()\n");
            vsConnection.put("trick.var_clear()\n");
            vsConnection.put("trick.var_set_copy_mode(1)\n");
            vsConnection.put("trick.var_cycle(" + currentCycleRate + ")\n");

            // Step 2: Switch to binary format (no names to reduce packet size)
            vsConnection.setBinaryNoNames();

            // Step 3: Add the core simulation state variables
            vsConnection.put("trick.var_add(\"trick_sys.sched.time_tics\")\n");
            vsConnection.put("trick.var_add(\"trick_sys.sched.time_tic_value\")\n");
            vsConnection.put("trick.var_add(\"trick_sys.sched.mode\")\n");

            // Step 4: Build the list of active jobs for this thread
            activeThreadJobs.clear();
            for (JobMeta job : allJobs) {
                if (job.thread == threadId) {
                    activeThreadJobs.add(job);
                }
            }

            // Step 5: Batch var_add commands to avoid exceeding 8192-byte message limit
            // Each var_add line is roughly 70-80 bytes. Use batch size of 30 to stay well under limit.
            final int VAR_ADD_BATCH_SIZE = 30;
            for (int i = 0; i < activeThreadJobs.size(); i += VAR_ADD_BATCH_SIZE) {
                int batchEnd = Math.min(i + VAR_ADD_BATCH_SIZE, activeThreadJobs.size());

                StringBuilder batchCommands = new StringBuilder();
                for (int j = i; j < batchEnd; j++) {
                    JobMeta job = activeThreadJobs.get(j);
                    batchCommands
                            .append("trick.var_add(\"trick_sys.sched.all_jobs_vector[")
                            .append(job.index)
                            .append("].prev_frame_time_seconds\")\n");
                }

                vsConnection.put(batchCommands.toString());
                // Small delay to allow Variable Server to process batch
                Thread.sleep(5);
            }

            // Step 6: Unpause and start the cyclic stream
            vsConnection.put("trick.var_unpause()\n");
            vsConnection.put("trick.var_send()\n");

        } catch (IOException e) {
            System.err.println("Failed to update thread subscription: " + e.getMessage());
        } catch (InterruptedException e) {
            System.err.println("Thread interrupted during subscription: " + e.getMessage());
        }
    }

    /**
     * Updates the data cycle rate for both the Variable Server stream and local tracking state.
     *
     * @param cycleSeconds The new cycle period in seconds (e.g., 0.02 = 50 Hz).
     */
    public synchronized void setCycleRate(double cycleSeconds) {
        this.currentCycleRate = cycleSeconds;
        try {
            StringBuilder cmd = new StringBuilder();
            cmd.append("trick.var_pause()\n");
            cmd.append("trick.var_cycle(").append(cycleSeconds).append(")\n");
            cmd.append("trick.var_unpause()\n");
            cmd.append("trick.var_send()\n");
            vsConnection.put(cmd.toString());
        } catch (IOException e) {
            System.err.println("Failed to update cycle rate: " + e.getMessage());
        }
    }

    private String getModeString(int modeId) {
        switch (modeId) {
            case 0:
                return "Initialization";
            case 1:
                return "Freeze";
            case 4:
                return "Step";
            case 5:
                return "Run";
            case 6:
                return "Exit";
            default:
                return "Unknown (" + modeId + ")";
        }
    }
}
