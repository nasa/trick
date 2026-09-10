package trick.rtperf;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ItemEvent;
import java.net.InetAddress;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;

import trick.common.utils.VariableServerConnection;
import trick.sniffer.SimulationInformation;
import trick.sniffer.SimulationListener;
import trick.sniffer.SimulationSniffer;

public class RealTimeJobPieChart extends JPanel {

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
    
    // GUI Control Elements
    private JLabel modeLabel;
    private JComboBox<String> threadComboBox;
    private TrickVariableServerClient vsClient;
    private boolean isInitializingCombo = false;
    private JPanel pieChartPanel; // The panel that actually draws the chart

    public RealTimeJobPieChart() {
        setLayout(new BorderLayout());
        
        // --- Top Control Panel ---
        JPanel controlPanel = new JPanel(new BorderLayout());
        controlPanel.setBorder(BorderFactory.createEmptyBorder(5, 10, 5, 10));
        controlPanel.setBackground(new Color(230, 230, 230));
        
        modeLabel = new JLabel("Mode: Connecting...");
        modeLabel.setFont(modeLabel.getFont().deriveFont(Font.BOLD, 14f));
        controlPanel.add(modeLabel, BorderLayout.WEST);
        
        threadComboBox = new JComboBox<>(new String[]{"Thread 0 (Loading...)"});
        threadComboBox.addItemListener(e -> {
            if (e.getStateChange() == ItemEvent.SELECTED && !isInitializingCombo && vsClient != null) {
                int threadId = threadComboBox.getSelectedIndex();
                System.out.println("Switching to Thread " + threadId);
                // Run the network call in a background thread so we don't freeze the GUI
                new Thread(() -> vsClient.subscribeToThread(threadId)).start();
            }
        });
        controlPanel.add(threadComboBox, BorderLayout.EAST);
        
        add(controlPanel, BorderLayout.NORTH);
        
        // --- Pie Chart Panel ---
        pieChartPanel = new JPanel() {
            @Override
            protected void paintComponent(Graphics g) {
                super.paintComponent(g);
                drawPieChart(g);
            }
        };
        pieChartPanel.setPreferredSize(new Dimension(800, 500));
        pieChartPanel.setBackground(Color.WHITE);
        add(pieChartPanel, BorderLayout.CENTER);
    }

    /**
     * Called by the client once it discovers how many threads the sim has.
     */
    public void initializeThreads(int numThreads, TrickVariableServerClient client) {
        SwingUtilities.invokeLater(() -> {
            this.vsClient = client;
            isInitializingCombo = true;
            threadComboBox.removeAllItems();
            for (int i = 0; i < numThreads; i++) {
                threadComboBox.addItem("Thread " + i + (i == 0 ? " (Main)" : ""));
            }
            isInitializingCombo = false;
        });
    }

    /**
     * Called cyclically by the client to push new data to the UI.
     */
    public void updateFrameData(String modeString, List<JobDuration> newFrameData) {
        SwingUtilities.invokeLater(() -> {
            modeLabel.setText("Mode: " + modeString);
            this.currentFrameData = new ArrayList<>(newFrameData);
            pieChartPanel.repaint();
        });
    }

    private Color getJobColor(String jobId) {
        return jobColorMap.computeIfAbsent(jobId, id -> {
            float hue = Math.abs(id.hashCode() % 360) / 360f;
            return Color.getHSBColor(hue, 0.7f, 0.9f);
        });
    }

    private void drawPieChart(Graphics g) {
        if (currentFrameData == null || currentFrameData.isEmpty()) {
            g.drawString("Waiting for frame data (or no jobs executed this frame)...", 20, 20);
            return;
        }

        Graphics2D g2d = (Graphics2D) g;
        g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

        double totalDuration = 0;
        for (JobDuration job : currentFrameData) { totalDuration += job.duration; }

        int padding = 20;
        int pieSize = Math.min(pieChartPanel.getWidth() - 300, pieChartPanel.getHeight()) - (padding * 2); 
        int pieX = padding;
        int pieY = (pieChartPanel.getHeight() - pieSize) / 2;

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
            
            g2d.setColor(jobColor);
            g2d.fillRect(legendX, legendY, 15, 15);
            g2d.setColor(Color.BLACK);
            
            double percentage = (job.duration / totalDuration) * 100.0;
            String legendText = String.format("%s (%.1f%%, %.6fs)", job.jobId, percentage, job.duration);
            g2d.drawString(legendText, legendX + 25, legendY + 12);
            
            legendY += 25;
            currentAngle += extentAngle;
        }
        
        g2d.setColor(Color.BLACK);
        g2d.setFont(g2d.getFont().deriveFont(Font.BOLD));
        g2d.drawString(String.format("Total Frame Time: %.6f s", totalDuration), legendX, legendY + 10);
    }

    public static void main(String[] args) {
        String currentDir = System.getProperty("user.dir");
        System.out.println("Searching for Trick simulation running in: " + currentDir);
        
        SimulationSniffer sniffer = new SimulationSniffer();
        CountDownLatch searchLatch = new CountDownLatch(1);
        final int[] portWrapper = new int[] {-1};
        final String[] hostWrapper = new String[] {"localhost"};
        
        try {
            String localHostName = InetAddress.getLocalHost().getHostName();
            sniffer.addSimulationListener(new SimulationListener() {
                @Override
                public void simulationAdded(SimulationInformation simInfo) {
                    if (simInfo.simDirectory.equals(currentDir) && 
                       (simInfo.machine.equalsIgnoreCase(localHostName) || simInfo.machine.equalsIgnoreCase("localhost"))) {
                        portWrapper[0] = Integer.parseInt(simInfo.handshakePort);
                        hostWrapper[0] = simInfo.machine;
                        searchLatch.countDown(); 
                    }
                }
                @Override public void simulationRemoved(SimulationInformation simInfo) {}
                @Override public void exceptionOccurred(Exception e) {}
            });
            sniffer.start();
            
            if (!searchLatch.await(3, TimeUnit.SECONDS)) {
                System.err.println("Timeout: Could not auto-discover a simulation in the current directory.");
                System.exit(1);
            }
        } catch (Exception e) {
            System.err.println("Error during multicast discovery: " + e.getMessage());
            System.exit(1);
        } finally {
            sniffer.setPaused(true);
        }
        
        RealTimeJobPieChart pieChart = new RealTimeJobPieChart();
        SwingUtilities.invokeLater(() -> {
            JFrame frame = new JFrame("RTPerf - Real-Time Job Performance");
            frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
            frame.add(pieChart);
            frame.pack();
            frame.setLocationRelativeTo(null);
            frame.setVisible(true);
        });

        try {
            VariableServerConnection vsConnection = new VariableServerConnection(hostWrapper[0], portWrapper[0]);
            Thread clientThread = new Thread(new TrickVariableServerClient(vsConnection, pieChart));
            clientThread.start();
        } catch (Exception e) {
            System.err.println("Failed to connect to Variable Server: " + e.getMessage());
            System.exit(1);
        }
    }
}