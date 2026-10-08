package trick.rtjperf;

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
public class RTJPerfVarServerClient implements Runnable {

    private VariableServerConnection vsConnection;
    private RealTimeJobPieChart gui;
    private volatile boolean running = true;
    private final Runnable onDisconnected;
    private int requestedThread = 0;
    private int numThreads = 1;

    // Single source of truth for cycle rate (seconds). Default: 0.02s (50 Hz)
    private volatile double currentCycleRate = 0.02;

    // Batch size for job metadata auto-discovery (50 jobs per round-trip)
    private static final int BATCH_SIZE = 50;
    private static final int STATE_VARIABLE_COUNT = 3;
    // Order is the wire format consumed by processFrameData.
    private static final String[] HEALTH_VARIABLES = {
        "trick_real_time.rt_sync.active",
        "trick_sys.sched.software_frame",
        "trick_real_time.rt_sync.frame_overrun",
        "trick_real_time.rt_sync.total_overrun",
        "trick_real_time.rt_sync.frame_overrun_cnt",
        "trick_real_time.rt_sync.peak_frame_overrun",
        "trick_real_time.rt_sync.completed_frame_count"
    };
    private boolean healthAvailable;
    private String healthUnavailableReason;

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
    // Only the network worker changes the subscription and its decoding map.
    private final List<JobMeta> activeThreadJobs = new ArrayList<>();

    public RTJPerfVarServerClient(VariableServerConnection vsConnection, RealTimeJobPieChart gui) {
        this(vsConnection, gui, () -> {});
    }

    public RTJPerfVarServerClient(
            VariableServerConnection vsConnection, RealTimeJobPieChart gui, Runnable onDisconnected) {
        this.vsConnection = vsConnection;
        this.gui = gui;
        this.onDisconnected = onDisconnected;
    }

    /**
     * Stops the read loop and closes the underlying Variable Server connection.
     * Safe to call from any thread (e.g. in response to a Disconnect button).
     */
    public void stop() {
        running = false;
        synchronized (this) {
            notifyAll();
        }
        try {
            vsConnection.close();
        } catch (IOException e) {
            // Connection is being torn down anyway.
        }
    }

    @Override
    public void run() {
        try {
            // 1. Ensure frame logging is turned on so prev_frame_time_seconds is populated
            vsConnection.put("trick.frame_log_on()\n");

            // 2. Retrieve the software frame and set it as the default cycle rate
            vsConnection.put("trick.var_send_once(\"trick_sys.sched.software_frame\")\n");
            String frameResponse = readResponse(1);
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
            String threadResponse = readResponse(1);
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
            String sizeResponse = readResponse(1);
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
                String batchResponse = readResponse(1);

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
            healthAvailable = discoverRealtimeHealth();
            int subscribedThread = -1;
            double configuredCycleRate = Double.NaN;

            // Keep cyclic sending paused. Each request is fully consumed before
            // reconfiguration, so buffered data can never use another thread's map.
            while (running) {
                int thread;
                synchronized (this) {
                    thread = requestedThread;
                }
                if (thread != subscribedThread) {
                    configureSubscription(thread);
                    subscribedThread = thread;
                }
                double cycleRate = currentCycleRate;
                if (cycleRate != configuredCycleRate) {
                    // The server's command-processing loop uses this interval even
                    // while cyclic sending is paused.
                    vsConnection.put("trick.var_cycle(" + cycleRate + ")\n");
                    configuredCycleRate = cycleRate;
                }
                long start = System.nanoTime();
                vsConnection.put("trick.var_send()\n");
                String line = readResponse(jobDataOffset() + activeThreadJobs.size());
                String[] tokens = line.split("\t");
                synchronized (this) {
                    // A selection made during the read invalidates this sample.
                    if (running && thread == requestedThread && tokens.length >= 3) {
                        processFrameData(tokens);
                    }
                    long remaining = (long) (currentCycleRate * 1_000_000_000L)
                            - (System.nanoTime() - start);
                    if (running && thread == requestedThread && remaining > 0) {
                        wait(remaining / 1_000_000L, (int) (remaining % 1_000_000L));
                    }
                }
            }
        } catch (IOException e) {
            if (running) {
                System.err.println("Variable Server connection lost: " + e.getMessage());
            }
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        } finally {
            stop();
            onDisconnected.run();
        }
    }

    private String readResponse(int variableCount) throws IOException {
        String response = vsConnection.get(variableCount);
        if (response == null) {
            throw new IOException("Connection closed");
        }
        return response;
    }

    private boolean discoverRealtimeHealth() throws IOException {
        for (String variable : HEALTH_VARIABLES) {
            vsConnection.put("trick.var_exists(\"" + variable + "\")\n");
            String[] response = readResponse(1).split("\t");
            if (response.length != 2 || !"1".equals(response[1].trim())) {
                healthUnavailableReason =
                        "Missing " + variable + "; rebuild the simulation with RTJPerf health support.";
                gui.updateRealtimeHealthUnavailable(healthUnavailableReason);
                return false;
            }
        }
        return true;
    }

    private int jobDataOffset() {
        return STATE_VARIABLE_COUNT + (healthAvailable ? HEALTH_VARIABLES.length : 0);
    }

    /**
     * Processes frame data from the Variable Server.
     * Format: time_tics, time_tic_value, mode, optional HEALTH_VARIABLES, job durations.
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
        if (healthAvailable) {
            try {
                if (tokens.length < jobDataOffset()) {
                    throw new IllegalArgumentException("Incomplete health sample");
                }
                boolean active = Integer.parseInt(tokens[3].trim()) != 0;
                double softwareFrame = Double.parseDouble(tokens[4].trim());
                double lateness = Double.parseDouble(tokens[5].trim());
                long totalOverruns = Long.parseLong(tokens[6].trim());
                long consecutiveOverruns = Long.parseLong(tokens[7].trim());
                double peakLateness = Double.parseDouble(tokens[8].trim());
                long frameSequence = Long.parseLong(tokens[9].trim());
                if (!Double.isFinite(softwareFrame) || softwareFrame <= 0
                        || !Double.isFinite(lateness) || !Double.isFinite(peakLateness)
                        || totalOverruns < 0 || consecutiveOverruns < 0 || frameSequence < 0) {
                    throw new IllegalArgumentException("Invalid health sample");
                }
                gui.updateRealtimeHealth(active, softwareFrame, lateness, totalOverruns,
                        consecutiveOverruns, peakLateness, frameSequence);
            } catch (RuntimeException e) {
                gui.updateRealtimeHealthUnavailable("Invalid real-time health sample: " + e.getMessage());
            }
        } else {
            gui.updateRealtimeHealthUnavailable(healthUnavailableReason);
        }
        List<RealTimeJobPieChart.JobDuration> frameData = new ArrayList<>();
        Map<String, Double> aggregatedDurations = new HashMap<>();

        // Parse job durations
        for (int i = 0; i < activeThreadJobs.size(); i++) {
            int tokenIdx = i + jobDataOffset();
            if (tokenIdx < tokens.length) {
                try {
                    double duration = Double.parseDouble(tokens[tokenIdx].trim());
                    if (Double.isFinite(duration) && duration > 0.0) {
                        JobMeta meta = activeThreadJobs.get(i);
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
        requestedThread = threadId;
        notifyAll();
    }

    private void configureSubscription(int threadId) throws IOException, InterruptedException {
        // Step 1: Pause and clear the old subscription
        vsConnection.put("trick.var_pause()\n");
        vsConnection.put("trick.var_clear()\n");
        // Keep copies on the variable-server thread, not the sim thread.
        vsConnection.put("trick.var_set_copy_mode(0)\n");

        // Step 2: Switch to binary format (no names to reduce packet size)
        vsConnection.setBinaryNoNames();

        // Step 3: Add the core simulation state variables
        vsConnection.put("trick.var_add(\"trick_sys.sched.time_tics\")\n");
        vsConnection.put("trick.var_add(\"trick_sys.sched.time_tic_value\")\n");
        vsConnection.put("trick.var_add(\"trick_sys.sched.mode\")\n");
        if (healthAvailable) {
            for (String variable : HEALTH_VARIABLES) {
                vsConnection.put("trick.var_add(\"" + variable + "\")\n");
            }
        }

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
    }

    /**
     * Updates the interval between snapshot requests and wakes the network worker.
     *
     * @param cycleSeconds The new cycle period in seconds (e.g., 0.02 = 50 Hz).
     */
    public synchronized void setCycleRate(double cycleSeconds) {
        if (!Double.isFinite(cycleSeconds) || cycleSeconds <= 0) {
            throw new IllegalArgumentException("Cycle period must be finite and positive");
        }
        this.currentCycleRate = cycleSeconds;
        notifyAll();
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
