package trick.rtperf;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ItemEvent;
import java.awt.event.MouseEvent;
import java.net.InetAddress;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Vector;
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
        public double percentage; 
        
        public JobDuration(String jobId, double duration) {
            this.jobId = jobId;
            this.duration = duration;
        }
    }

    private List<JobDuration> currentFrameData = new ArrayList<>();
    private final Map<String, Color> jobColorMap = new HashMap<>();
    
    private JLabel modeLabel;
    private JLabel timeLabel;
    private JComboBox<String> threadComboBox;
    private TrickVariableServerClient vsClient;
    private boolean isInitializingCombo = false;
    
    private JPanel pieChartPanel; 
    private DefaultListModel<String> listModel;
    private JList<String> jobList;

    public RealTimeJobPieChart() {
        setLayout(new BorderLayout());
        
        // --- Top Control Panel ---
        JPanel controlPanel = new JPanel(new BorderLayout());
        controlPanel.setBorder(BorderFactory.createEmptyBorder(5, 10, 5, 10));
        controlPanel.setBackground(new Color(230, 230, 230));
        
        JPanel infoPanel = new JPanel(new FlowLayout(FlowLayout.LEFT, 15, 0));
        infoPanel.setOpaque(false);
        
        timeLabel = new JLabel("Time: 0.000");
        timeLabel.setFont(timeLabel.getFont().deriveFont(Font.BOLD, 14f));
        infoPanel.add(timeLabel);
        
        modeLabel = new JLabel("Mode: Connecting...");
        modeLabel.setFont(modeLabel.getFont().deriveFont(Font.BOLD, 14f));
        infoPanel.add(modeLabel);
        
        controlPanel.add(infoPanel, BorderLayout.WEST);
        
        threadComboBox = new JComboBox<>(new String[]{"Thread 0 (Loading...)"});
        threadComboBox.addItemListener(e -> {
            if (e.getStateChange() == ItemEvent.SELECTED && !isInitializingCombo && vsClient != null) {
                int threadId = threadComboBox.getSelectedIndex();
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
            @Override
            public String getToolTipText(MouseEvent e) {
                return getPieSliceToolTip(e);
            }
        };
        pieChartPanel.setMinimumSize(new Dimension(300, 300));
        pieChartPanel.setBackground(Color.WHITE);
        pieChartPanel.setToolTipText(""); 
        
        // --- Scrollable Job List Panel ---
        listModel = new DefaultListModel<>();
        jobList = new JList<>(listModel);
        
        jobList.setCellRenderer(new ListCellRenderer<String>() {
            private JPanel panel = new JPanel(new BorderLayout());
            private JLabel label = new JLabel();
            private JPanel colorBox = new JPanel();
            {
                panel.setOpaque(true);
                colorBox.setPreferredSize(new Dimension(15, 15));
                panel.add(colorBox, BorderLayout.WEST);
                panel.add(label, BorderLayout.CENTER);
                panel.setBorder(BorderFactory.createEmptyBorder(2, 5, 2, 5));
                label.setBorder(BorderFactory.createEmptyBorder(0, 5, 0, 0));
            }
            @Override
            public Component getListCellRendererComponent(JList<? extends String> list, String value, int index, boolean isSelected, boolean cellHasFocus) {
                label.setText(value);
                String jobName = value.split(" ")[0]; 
                colorBox.setBackground(getJobColor(jobName));
                
                if (isSelected) {
                    panel.setBackground(list.getSelectionBackground());
                    label.setForeground(list.getSelectionForeground());
                } else {
                    panel.setBackground(list.getBackground());
                    label.setForeground(list.getForeground());
                }
                return panel;
            }
        });
        
        JScrollPane scrollPane = new JScrollPane(jobList);
        scrollPane.setMinimumSize(new Dimension(200, 300));
        scrollPane.setBorder(BorderFactory.createTitledBorder("Job Execution (Sorted by Time)"));
        
        JSplitPane splitPane = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, pieChartPanel, scrollPane);
        splitPane.setContinuousLayout(true); 
        splitPane.setResizeWeight(0.65); 
        splitPane.setBorder(null);
        
        add(splitPane, BorderLayout.CENTER);
    }

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

    public void updateFrameData(double simTime, String modeString, List<JobDuration> newFrameData) {
        SwingUtilities.invokeLater(() -> {
            timeLabel.setText(String.format("Time: %.3f", simTime));
            modeLabel.setText("Mode: " + modeString);
            
            if (!newFrameData.isEmpty()) {
                double totalDuration = 0;
                for (JobDuration job : newFrameData) { totalDuration += job.duration; }
                
                for (JobDuration job : newFrameData) {
                    job.percentage = (job.duration / totalDuration) * 100.0;
                }
                
                Collections.sort(newFrameData, new Comparator<JobDuration>() {
                    @Override
                    public int compare(JobDuration j1, JobDuration j2) {
                        return Double.compare(j2.duration, j1.duration); 
                    }
                });

                this.currentFrameData = new ArrayList<>(newFrameData);
                
                // Batch UI updates
                Vector<String> batchUpdate = new Vector<>();
                for (JobDuration job : currentFrameData) {
                    batchUpdate.add(String.format("%s (%.1f%%, %.6fs)", job.jobId, job.percentage, job.duration));
                }
                jobList.setListData(batchUpdate);
                
                pieChartPanel.repaint();
            }
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
            g.drawString("Waiting for frame data...", 20, 20);
            return;
        }

        Graphics2D g2d = (Graphics2D) g;
        g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

        int padding = 20;
        int pieSize = Math.min(pieChartPanel.getWidth(), pieChartPanel.getHeight()) - (padding * 2); 
        int pieX = (pieChartPanel.getWidth() - pieSize) / 2;
        int pieY = (pieChartPanel.getHeight() - pieSize) / 2;

        double currentAngle = 0;
        double totalDuration = 0;
        for (JobDuration job : currentFrameData) { totalDuration += job.duration; }

        for (JobDuration job : currentFrameData) {
            double extentAngle = (job.percentage / 100.0) * 360.0;
            int startAngleInt = (int) Math.round(currentAngle);
            int extentAngleInt = (int) Math.round(extentAngle);

            Color jobColor = getJobColor(job.jobId);
            g2d.setColor(jobColor);
            g2d.fillArc(pieX, pieY, pieSize, pieSize, startAngleInt, extentAngleInt);
            g2d.setColor(Color.DARK_GRAY);
            g2d.drawArc(pieX, pieY, pieSize, pieSize, startAngleInt, extentAngleInt);
            
            currentAngle += extentAngle;
        }
        
        g2d.setColor(Color.BLACK);
        g2d.setFont(g2d.getFont().deriveFont(Font.BOLD));
        g2d.drawString(String.format("Total Frame Time: %.6f s", totalDuration), 10, 20);
    }

    private String getPieSliceToolTip(MouseEvent e) {
        if (currentFrameData == null || currentFrameData.isEmpty()) {
            return null;
        }

        int padding = 20;
        int pieSize = Math.min(pieChartPanel.getWidth(), pieChartPanel.getHeight()) - (padding * 2); 
        int pieX = (pieChartPanel.getWidth() - pieSize) / 2;
        int pieY = (pieChartPanel.getHeight() - pieSize) / 2;
        
        double centerX = pieX + pieSize / 2.0;
        double centerY = pieY + pieSize / 2.0;
        
        double dx = e.getX() - centerX;
        double dy = e.getY() - centerY;
        
        if (Math.sqrt(dx * dx + dy * dy) > pieSize / 2.0) { return null; }

        double angle = Math.toDegrees(Math.atan2(centerY - e.getY(), e.getX() - centerX));
        if (angle < 0) angle += 360.0;

        double currentAngle = 0;
        for (JobDuration job : currentFrameData) {
            double extentAngle = (job.percentage / 100.0) * 360.0;
            if (angle >= currentAngle && angle <= currentAngle + extentAngle) {
                return String.format("%s (%.1f%%, %.6fs)", job.jobId, job.percentage, job.duration);
            }
            currentAngle += extentAngle;
        }
        return null;
    }

    public static void main(String[] args) {
        String currentDir = System.getProperty("user.dir");
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
        } finally { sniffer.setPaused(true); }
        
        RealTimeJobPieChart pieChart = new RealTimeJobPieChart();
        pieChart.setPreferredSize(new Dimension(1000, 600));
        
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
            System.exit(1);
        }
    }
}