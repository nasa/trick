package trick.rtperf;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ItemEvent;
import java.awt.event.MouseEvent;
import java.net.InetAddress;
import java.util.ArrayList;
import java.util.Collections;
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

/**
 * The main GUI class for RTPerf. It renders a real-time pie chart of job execution
 * durations alongside a sorted list of jobs and their frame percentages.
 *
 * A single rate field configures both the Variable Server data push frequency
 * and the local Swing redraw timer. A threshold field filters jobs below a minimum
 * percentage to reduce visual noise and improve list stability.
 *
 * A rolling total of job execution times over the last 100 frames is maintained
 * and displayed on the left.
 */
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

    // Rolling total of job durations over last 100 frames
    private final Map<String, RollingTotal> jobTotals = new HashMap<>();
    private static final int ROLLING_WINDOW_SIZE = 100;

    private static class RollingTotal {
        private final List<Double> frameDurations = new ArrayList<>();
        private final int maxSize;
        private int framesSinceLastSeen = 0;

        public RollingTotal(int maxSize) {
            this.maxSize = maxSize;
        }

        public void add(double duration) {
            frameDurations.add(duration);
            if (frameDurations.size() > maxSize) {
                frameDurations.remove(0);
            }
            framesSinceLastSeen = 0;
        }

        public void incrementFramesSinceLastSeen() {
            framesSinceLastSeen++;
        }

        public boolean isStale() {
            return framesSinceLastSeen > ROLLING_WINDOW_SIZE;
        }

        public double getTotal() {
            double sum = 0;
            for (double d : frameDurations) {
                sum += d;
            }
            return sum;
        }
    }

    private JLabel modeLabel;
    private JLabel timeLabel;
    private JComboBox<String> threadComboBox;
    private JTextField cycleRateField;
    private JTextField thresholdField;
    private Timer refreshTimer;
    private TrickVariableServerClient vsClient;
    private boolean isInitializingCombo = false;

    private JPanel pieChartPanel;
    private DefaultListModel<String> totalListModel;
    private JList<String> totalList;
    private DefaultListModel<String> listModel;
    private JList<String> jobList;

    // Latest data snapshot written by Variable Server network thread and read by Swing timer
    private volatile double pendingSimTime = 0.0;
    private volatile String pendingModeString = "Connecting...";
    private volatile List<JobDuration> pendingFrameData = null;

    // Threshold for filtering jobs (percentage of frame)
    private volatile double percentageThreshold = 0.1; // Default: 0.1%

    public RealTimeJobPieChart() {
        setLayout(new BorderLayout());

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

        JPanel ratePanel = new JPanel(new FlowLayout(FlowLayout.RIGHT, 8, 0));
        ratePanel.setOpaque(false);
        ratePanel.setBorder(BorderFactory.createTitledBorder("Rate"));

        ratePanel.add(new JLabel("Rate (s):"));
        cycleRateField = new JTextField("0.02", 5);
        cycleRateField.addActionListener(e -> applyCycleRate());
        ratePanel.add(cycleRateField);

        JPanel thresholdPanel = new JPanel(new FlowLayout(FlowLayout.RIGHT, 8, 0));
        thresholdPanel.setOpaque(false);
        thresholdPanel.setBorder(BorderFactory.createTitledBorder("Threshold"));

        thresholdPanel.add(new JLabel("Min %:"));
        thresholdField = new JTextField("0.1", 5);
        thresholdField.addActionListener(e -> applyThreshold());
        thresholdPanel.add(thresholdField);

        JPanel eastPanel = new JPanel();
        eastPanel.setOpaque(false);
        eastPanel.setLayout(new BoxLayout(eastPanel, BoxLayout.X_AXIS));
        eastPanel.add(thresholdPanel);
        eastPanel.add(Box.createHorizontalStrut(12));
        eastPanel.add(ratePanel);
        eastPanel.add(Box.createHorizontalStrut(12));
        eastPanel.add(threadComboBox);

        controlPanel.add(eastPanel, BorderLayout.EAST);

        add(controlPanel, BorderLayout.NORTH);

        pieChartPanel = new JPanel() {
            @Override protected void paintComponent(Graphics g) { super.paintComponent(g); drawPieChart(g); }
            @Override public String getToolTipText(MouseEvent e) { return getPieSliceToolTip(e); }
        };
        pieChartPanel.setMinimumSize(new Dimension(300, 300));
        pieChartPanel.setBackground(Color.WHITE);
        pieChartPanel.setToolTipText("");

        // Left panel: Rolling totals
        totalListModel = new DefaultListModel<>();
        totalList = new JList<>(totalListModel);
        totalList.setCellRenderer(new ListCellRenderer<String>() {
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
                panel.setBackground(isSelected ? list.getSelectionBackground() : list.getBackground());
                label.setForeground(isSelected ? list.getSelectionForeground() : list.getForeground());
                return panel;
            }
        });

        JScrollPane totalScrollPane = new JScrollPane(totalList);
        totalScrollPane.setMinimumSize(new Dimension(200, 300));
        totalScrollPane.setBorder(BorderFactory.createTitledBorder("Rolling Total (Last 100 Frames)"));

        // Right panel: Current frame jobs
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
                colorBox.setBackground(getJobColor(value.split(" ")[0]));
                panel.setBackground(isSelected ? list.getSelectionBackground() : list.getBackground());
                label.setForeground(isSelected ? list.getSelectionForeground() : list.getForeground());
                return panel;
            }
        });

        JScrollPane scrollPane = new JScrollPane(jobList);
        scrollPane.setMinimumSize(new Dimension(200, 300));
        scrollPane.setBorder(BorderFactory.createTitledBorder("Job Execution (Current Frame)"));

        // Center: Pie chart
        JPanel centerPanel = new JPanel(new BorderLayout());
        centerPanel.add(pieChartPanel, BorderLayout.CENTER);

        // Left-Center split: Totals and Pie Chart
        JSplitPane leftCenterSplit = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, totalScrollPane, centerPanel);
        leftCenterSplit.setContinuousLayout(true);
        leftCenterSplit.setResizeWeight(0.3);
        leftCenterSplit.setBorder(null);

        // Full split: (Totals + Pie) and Job List
        JSplitPane mainSplit = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, leftCenterSplit, scrollPane);
        mainSplit.setContinuousLayout(true);
        mainSplit.setResizeWeight(0.65);
        mainSplit.setBorder(null);
        add(mainSplit, BorderLayout.CENTER);

        // Drives Swing repaints matched to default 0.02s (20ms / 50 Hz)
        refreshTimer = new Timer(20, e -> renderLatestFrame());
        refreshTimer.start();
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

    /**
     * Called from the Variable Server network thread whenever a new cyclic
     * data update arrives. Computes percentages and sort order, then stashes
     * the result for the refresh timer to pick up.
     */
    public void updateFrameData(double simTime, String modeString, List<JobDuration> newFrameData) {
        if (newFrameData.isEmpty()) {
            return;
        }

        double totalDuration = 0;
        for (JobDuration job : newFrameData) {
            totalDuration += job.duration;
        }
        for (JobDuration job : newFrameData) {
            job.percentage = (job.duration / totalDuration) * 100.0;
        }

        Collections.sort(newFrameData, (j1, j2) -> Double.compare(j2.duration, j1.duration));

        pendingSimTime = simTime;
        pendingModeString = modeString;
        pendingFrameData = newFrameData;
    }

    /**
     * Runs on the EDT via refreshTimer. Paints whatever the most recently
     * received frame snapshot is at the configured rate. Filters jobs below
     * the percentage threshold. Updates rolling totals.
     */
    private void renderLatestFrame() {
        List<JobDuration> frameData = pendingFrameData;
        if (frameData == null) {
            return;
        }

        timeLabel.setText(String.format("Time: %.3f", pendingSimTime));
        modeLabel.setText("Mode: " + pendingModeString);

        // Filter jobs by threshold
        List<JobDuration> filteredData = new ArrayList<>();
        for (JobDuration job : frameData) {
            if (job.percentage >= percentageThreshold) {
                filteredData.add(job);
            }
        }

        this.currentFrameData = filteredData;

        // Mark all existing jobs as not seen this frame
        for (RollingTotal total : jobTotals.values()) {
            total.incrementFramesSinceLastSeen();
        }

        // Update rolling totals for all jobs that meet the threshold
        for (JobDuration job : filteredData) {
            jobTotals.computeIfAbsent(job.jobId, k -> new RollingTotal(ROLLING_WINDOW_SIZE))
                    .add(job.duration);
        }

        // Remove stale jobs (not seen in more than ROLLING_WINDOW_SIZE frames)
        jobTotals.entrySet().removeIf(entry -> entry.getValue().isStale());

        // Update current frame list
        Vector<String> batchUpdate = new Vector<>();
        for (JobDuration job : currentFrameData) {
            batchUpdate.add(String.format("%s (%.1f%%, %.6fs)", job.jobId, job.percentage, job.duration));
        }
        jobList.setListData(batchUpdate);

        // Update rolling total list (sorted by total duration)
        List<Map.Entry<String, RollingTotal>> totalEntries = new ArrayList<>(jobTotals.entrySet());
        totalEntries.sort((e1, e2) -> Double.compare(e2.getValue().getTotal(), e1.getValue().getTotal()));

        Vector<String> totalUpdate = new Vector<>();
        for (Map.Entry<String, RollingTotal> entry : totalEntries) {
            double total = entry.getValue().getTotal();
            totalUpdate.add(String.format("%s (%.6fs)", entry.getKey(), total));
        }
        totalList.setListData(totalUpdate);

        pieChartPanel.repaint();
    }

    /**
     * Reads the Rate (s) textbox and updates both the Variable Server subscription
     * rate and the local Swing redraw timer.
     */
    private void applyCycleRate() {
        if (vsClient == null) {
            return;
        }
        try {
            double seconds = Double.parseDouble(cycleRateField.getText().trim());
            if (seconds <= 0.001 || seconds > 10.0) {
                throw new NumberFormatException("out of range");
            }

            // 1. Send update to Variable Server via background thread
            new Thread(() -> vsClient.setCycleRate(seconds)).start();

            // 2. Adjust local Swing timer rate (convert seconds to milliseconds)
            int millis = (int) Math.round(seconds * 1000.0);
            refreshTimer.setDelay(Math.max(millis, 1));

        } catch (NumberFormatException ex) {
            JOptionPane.showMessageDialog(this,
                "Enter a valid cycle time in seconds (e.g., 0.02 for 50 Hz).",
                "Invalid Rate", JOptionPane.WARNING_MESSAGE);
        }
    }

    /**
     * Reads the Threshold (%) textbox and updates the minimum percentage filter.
     */
    private void applyThreshold() {
        try {
            double threshold = Double.parseDouble(thresholdField.getText().trim());
            if (threshold < 0.0 || threshold > 100.0) {
                throw new NumberFormatException("out of range");
            }
            this.percentageThreshold = threshold;
        } catch (NumberFormatException ex) {
            JOptionPane.showMessageDialog(this,
                "Enter a valid percentage threshold (0.0 to 100.0, e.g., 0.1 for 0.1%).",
                "Invalid Threshold", JOptionPane.WARNING_MESSAGE);
        }
    }

    private Color getJobColor(String jobId) {
        return jobColorMap.computeIfAbsent(jobId, id -> Color.getHSBColor(Math.abs(id.hashCode() % 360) / 360f, 0.7f, 0.9f));
    }

    private void drawPieChart(Graphics g) {
        if (currentFrameData == null || currentFrameData.isEmpty()) return;
        Graphics2D g2d = (Graphics2D) g;
        g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        int padding = 20;
        int pieSize = Math.min(pieChartPanel.getWidth(), pieChartPanel.getHeight()) - (padding * 2);
        int pieX = (pieChartPanel.getWidth() - pieSize) / 2;
        int pieY = (pieChartPanel.getHeight() - pieSize) / 2;
        double currentAngle = 0, totalDuration = 0;
        for (JobDuration job : currentFrameData) {
            totalDuration += job.duration;
        }
        if (totalDuration == 0) {
            g2d.drawString("No job data available", 10, 20);
            return;
        }
        for (JobDuration job : currentFrameData) {
            double extentAngle = (job.percentage / 100.0) * 360.0;
            g2d.setColor(getJobColor(job.jobId));
            g2d.fillArc(pieX, pieY, pieSize, pieSize, (int) Math.round(currentAngle), (int) Math.round(extentAngle));
            g2d.setColor(Color.DARK_GRAY);
            g2d.drawArc(pieX, pieY, pieSize, pieSize, (int) Math.round(currentAngle), (int) Math.round(extentAngle));
            currentAngle += extentAngle;
        }
        g2d.setColor(Color.BLACK);
        g2d.setFont(g2d.getFont().deriveFont(Font.BOLD));
        g2d.drawString(String.format("Total Frame Time: %.6f s", totalDuration), 10, 20);
    }

    private String getPieSliceToolTip(MouseEvent e) {
        if (currentFrameData == null || currentFrameData.isEmpty()) return null;
        int padding = 20;
        int pieSize = Math.min(pieChartPanel.getWidth(), pieChartPanel.getHeight()) - (padding * 2);
        int pieX = (pieChartPanel.getWidth() - pieSize) / 2;
        int pieY = (pieChartPanel.getHeight() - pieSize) / 2;

        double centerX = pieX + pieSize / 2.0;
        double centerY = pieY + pieSize / 2.0;
        double dx = e.getX() - centerX, dy = e.getY() - centerY;

        if (Math.sqrt(dx * dx + dy * dy) > pieSize / 2.0) return null;
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
                @Override public void simulationAdded(SimulationInformation simInfo) {
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
        } catch (Exception e) {} finally { sniffer.setPaused(true); }

        RealTimeJobPieChart pieChart = new RealTimeJobPieChart();
        pieChart.setPreferredSize(new Dimension(1200, 600));

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
            new Thread(new TrickVariableServerClient(vsConnection, pieChart)).start();
        } catch (Exception e) {
            System.exit(1);
        }
    }
}
