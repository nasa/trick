import javax.swing.*;
import java.awt.*;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;

// Trick Java Imports
import trick.common.utils.vs.VariableServerConnection;
import trick.sniffer.SimulationInformation;
import trick.sniffer.SimulationListener;
import trick.sniffer.SimulationSniffer;

public class RealTimeJobPieChart extends JPanel {

    // Simple data class to hold job info for the current frame
    public static class JobDuration {
        public String jobId;
        public double duration;

        public JobDuration(String jobId, double duration) {
            this.jobId = jobId;
            this.duration = duration;
        }
    }

    private List<JobDuration> currentFrameData = new ArrayList<>();
    private final Map<String, Color> jobColorMap = new HashMap<>();

    public RealTimeJobPieChart() {
        setPreferredSize(new Dimension(800, 500));
        setBackground(Color.WHITE);
    }

    public void updateFrameData(List<JobDuration> newFrameData) {
        SwingUtilities.invokeLater(() -> {
            this.currentFrameData = new ArrayList<>(newFrameData);
            this.repaint();
        });
    }

    private Color getJobColor(String jobId) {
        return jobColorMap.computeIfAbsent(jobId, id -> {
            float hue = Math.abs(id.hashCode() % 360) / 360f;
            return Color.getHSBColor(hue, 0.7f, 0.9f);
        });
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);

        if (currentFrameData == null || currentFrameData.isEmpty()) {
            g.drawString("Waiting for frame data from Variable Server...", 20, 20);
            return;
        }

        Graphics2D g2d = (Graphics2D) g;
        g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

        double totalDuration = 0;
        for (JobDuration job : currentFrameData) {
            totalDuration += job.duration;
        }

        int padding = 20;
        int pieSize = Math.min(getWidth() - 300, getHeight()) - (padding * 2); 
        int pieX = padding;
        int pieY = (getHeight() - pieSize) / 2;

        double currentAngle = 0;
        int legendY = padding;
        int legendX = pieX + pieSize + padding;

        for (JobDuration job : currentFrameData) {
            double extentAngle = (job.duration / totalDuration) * 360.0;
            
            int startAngleInt = (int) Math.round(currentAngle);
            int extentAngleInt = (int) Math.round(extentAngle);

            Color jobColor = getJobColor(job.jobId);
            g2d.setColor(jobColor);
            
            g2d.fillArc(pieX, pieY, pieSize, pieSize, startAngleInt, extentAngleInt);
            
            g2d.setColor(Color.DARK_GRAY);
            g2d.drawArc(pieX, pieY, pieSize, pieSize, startAngleInt, extentAngleInt);
            
            // Draw Legend
            g2d.setColor(jobColor);
            g2d.fillRect(legendX, legendY, 15, 15);
            g2d.setColor(Color.BLACK);
            
            double percentage = (job.duration / totalDuration) * 100.0;
            String legendText = String.format("%s (%.1f%%, %.5fs)", job.jobId, percentage, job.duration);
            g2d.drawString(legendText, legendX + 25, legendY + 12);
            
            legendY += 25;
            currentAngle += extentAngle;
        }
        
        g2d.setColor(Color.BLACK);
        g2d.setFont(g2d.getFont().deriveFont(Font.BOLD));
        g2d.drawString(String.format("Total Frame Time: %.5f s", totalDuration), legendX, legendY + 10);
    }

    // --- MAIN METHOD ---
    public static void main(String[] args) {
        
        String currentDir = System.getProperty("user.dir");
        System.out.println("Searching for Trick simulation running in: " + currentDir);
        
        int discoveredPort = -1;
        String discoveredHost = "localhost";

        // 1. Start the SimulationSniffer to find our sim via Multicast
        SimulationSniffer sniffer = new SimulationSniffer();
        CountDownLatch searchLatch = new CountDownLatch(1);
        
        // Final array wrapper because variables accessed in anonymous classes must be effectively final
        final int[] portWrapper = new int[] {-1};
        final String[] hostWrapper = new String[] {"localhost"};
        
        try {
            String localHostName = InetAddress.getLocalHost().getHostName();
            
            sniffer.addSimulationListener(new SimulationListener() {
                @Override
                public void simulationAdded(SimulationInformation simInfo) {
                    // Check if this simulation matches our current working directory
                    // and is running on this machine (to avoid picking up a sim on someone else's machine)
                    if (simInfo.simDirectory.equals(currentDir) && 
                       (simInfo.machine.equalsIgnoreCase(localHostName) || simInfo.machine.equalsIgnoreCase("localhost"))) {
                        
                        // Grab the default VS handshake port
                        portWrapper[0] = Integer.parseInt(simInfo.handshakePort);
                        hostWrapper[0] = simInfo.machine;
                        searchLatch.countDown(); // Found it, unblock the main thread!
                    }
                }

                @Override
                public void simulationRemoved(SimulationInformation simInfo) {}
                @Override
                public void exceptionOccurred(Exception e) {}
            });
            
            sniffer.start();
            
            // Wait up to 3 seconds for the multicast announcement
            if (searchLatch.await(3, TimeUnit.SECONDS)) {
                discoveredPort = portWrapper[0];
                discoveredHost = hostWrapper[0];
                System.out.println("Found simulation! Host: " + discoveredHost + " | Port: " + discoveredPort);
            } else {
                System.err.println("Timeout: Could not automatically discover a simulation in the current directory.");
                System.err.println("Ensure the simulation is running and broadcasting.");
                System.exit(1);
            }
            
        } catch (Exception e) {
            System.err.println("Error during multicast discovery: " + e.getMessage());
            System.exit(1);
        } finally {
            sniffer.setPaused(true); // Stop listening to multicast
        }
        
        // 2. Launch the GUI
        RealTimeJobPieChart pieChart = new RealTimeJobPieChart();
        SwingUtilities.invokeLater(() -> {
            JFrame frame = new JFrame("Real-Time Job Performance");
            frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
            frame.add(pieChart);
            frame.pack();
            frame.setLocationRelativeTo(null);
            frame.setVisible(true);
        });

        // 3. Connect to the Variable Server and Start Data Feed
        try {
            VariableServerConnection vsConnection = new VariableServerConnection(discoveredHost, discoveredPort);
            TrickVariableServerClient client = new TrickVariableServerClient(vsConnection, pieChart);
            
            Thread clientThread = new Thread(client);
            clientThread.start();
            
        } catch (Exception e) {
            System.err.println("Failed to connect to Variable Server: " + e.getMessage());
            System.exit(1);
        }
    }
}