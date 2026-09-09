package trick.rtperf;

import trick.common.utils.vs.VariableServerConnection;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class TrickVariableServerClient implements Runnable {

    private VariableServerConnection vsConnection;
    private RealTimeJobPieChart pieChart;
    private boolean running = true;

    public TrickVariableServerClient(VariableServerConnection vsConnection, RealTimeJobPieChart pieChart) {
        this.vsConnection = vsConnection;
        this.pieChart = pieChart;
    }

    @Override
    public void run() {
        try {
            System.out.println("Configuring Variable Server Stream...");
            vsConnection.put("trick.var.pause()");
            vsConnection.clear(); 
            vsConnection.put("trick.var.set_copy_mode(1)");

            vsConnection.put("trick.var.send(\"trick_exec.exec.all_jobs_vector.size()\")");
            int numberOfJobs = 0;
            String sizeResponse = vsConnection.get(); 
            if (sizeResponse != null) {
                String[] tokens = sizeResponse.split("\t");
                if (tokens.length >= 2) {
                    try {
                        numberOfJobs = Integer.parseInt(tokens[1].trim());
                        System.out.println("Discovered " + numberOfJobs + " jobs in the simulation.");
                    } catch (NumberFormatException e) {
                        System.err.println("Could not parse job count: " + tokens[1]);
                        return;
                    }
                }
            }

            for (int i = 0; i < numberOfJobs; i++) {
                vsConnection.add("trick_exec.exec.all_jobs_vector[" + i + "]->name");
                vsConnection.add("trick_exec.exec.all_jobs_vector[" + i + "]->frame_time_seconds"); 
            }

            vsConnection.put("trick.var.unpause()");

            String line;
            while (running && (line = vsConnection.get()) != null) {
                String[] tokens = line.split("\t");
                if (tokens.length > 1) { 
                    List<RealTimeJobPieChart.JobDuration> frameData = new ArrayList<>();
                    int tokenIdx = 1;
                    for (int i = 0; i < numberOfJobs; i++) {
                        if (tokenIdx + 1 < tokens.length) {
                            String jobName = tokens[tokenIdx].trim().replace("\"", ""); 
                            double duration = 0.0;
                            try {
                                duration = Double.parseDouble(tokens[tokenIdx + 1]);
                            } catch (NumberFormatException e) {}
                            
                            if (duration > 0.0) {
                                frameData.add(new RealTimeJobPieChart.JobDuration(jobName, duration));
                            }
                            tokenIdx += 2; 
                        }
                    }
                    pieChart.updateFrameData(frameData);
                }
            }
        } catch (IOException e) {
            System.err.println("Variable Server connection error: " + e.getMessage());
        }
    }
    
    public void stopClient() {
        running = false;
        try { vsConnection.close(); } catch (Exception e) {}
    }
}