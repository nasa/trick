package trick.rtjperf;

import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ItemEvent;
import java.awt.event.MouseEvent;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Vector;
import javax.swing.*;
import trick.common.ui.panels.ConnectionStatusBar;
import trick.common.utils.VariableServerConnection;

/**
 * The main GUI class for RTJPerf. It renders a real-time pie chart of job execution
 * durations alongside a sorted list of jobs and their frame percentages.
 *
 * A single rate field configures both the Variable Server data push frequency
 * and the local Swing redraw timer. A threshold field filters jobs below a minimum
 * percentage to reduce visual noise and improve list stability.
 *
 * A rolling total of job execution times over the last 100 received samples is maintained
 * and displayed on the left. A tabbed interface allows switching between the current
 * frame job list and a searchable list of all jobs with pinning capability.
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

    // Rolling total of job durations over the last 100 received samples.
    private final Map<String, RollingTotal> jobTotals = new HashMap<>();
    private static final int ROLLING_WINDOW_SIZE = 100;

    // Pinned jobs that are always displayed (LinkedHashSet maintains insertion order)
    private final Set<String> pinnedJobs = new LinkedHashSet<>();

    // All jobs discovered from the simulation
    private final List<String> allJobNames = new ArrayList<>();

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
            frameDurations.add(0.0);
            if (frameDurations.size() > maxSize) {
                frameDurations.remove(0);
            }
            framesSinceLastSeen++;
        }

        public boolean isStale() {
            // The caller retains pinned jobs even after their history expires.
            return framesSinceLastSeen >= maxSize;
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
    private RTJPerfVarServerClient vsClient;
    private boolean isInitializingCombo = false;

    private JPanel pieChartPanel;
    private DefaultListModel<String> unpinnedTotalListModel;
    private JList<String> unpinnedTotalList;
    private DefaultListModel<String> pinnedTotalListModel;
    private JList<String> pinnedTotalList;
    private DefaultListModel<String> currentFrameListModel;
    private JList<String> currentFrameList;
    private DefaultListModel<String> pinnedJobsListModel;
    private JList<String> pinnedJobsList;
    private DefaultListModel<String> allJobsListModel;
    private JList<String> allJobsList;
    private JTextField searchField;
    private JButton clearSearchButton;
    private JTabbedPane rightTabbedPane;

    // Latest snapshot and rolling history are owned by the EDT.
    private double pendingSimTime = 0.0;
    private String pendingModeString = "Connecting...";
    private List<JobDuration> pendingFrameData = null;
    // Invalidates samples already queued when the EDT resets or switches threads.
    private volatile long sampleGeneration = 0;
    private double recordedJobTime;
    private final JLabel realtimeHealthLabel = new JLabel("Realtime health: No samples");
    private final JLabel deadlineLabel = new JLabel("Deadline: No samples");
    private final JLabel telemetryLabel = new JLabel("Telemetry: No samples");
    private boolean healthConnected = true;
    private boolean hasHealthSample;
    private boolean realtimeActive;
    private double healthSoftwareFrame;
    private double deadlineLateness;
    private long totalOverruns;
    private long consecutiveOverruns;
    private double peakLateness;
    private long completedFrameSequence;
    private long lastTelemetryNanos;
    private long lastFrameProgressNanos;
    private String healthUnavailableReason;

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

        threadComboBox = new JComboBox<>(new String[] {"Thread 0 (Loading...)"});
        threadComboBox.addItemListener(e -> {
            if (e.getStateChange() == ItemEvent.SELECTED && !isInitializingCombo && vsClient != null) {
                int threadId = threadComboBox.getSelectedIndex();
                vsClient.subscribeToThread(threadId);
                clearRollingTotals();
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
        thresholdField.setToolTipText(
                "Filters current-frame jobs and pie slices; rolling totals retain all received samples.");
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

        JPanel healthPanel = new JPanel(new GridLayout(3, 1, 0, 2));
        healthPanel.setOpaque(false);
        healthPanel.setBorder(BorderFactory.createTitledBorder("Realtime health (main thread)"));
        healthPanel.add(realtimeHealthLabel);
        healthPanel.add(deadlineLabel);
        healthPanel.add(telemetryLabel);
        controlPanel.add(healthPanel, BorderLayout.SOUTH);

        add(controlPanel, BorderLayout.NORTH);

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

        // Left panel: Rolling totals (split into unpinned and pinned)
        unpinnedTotalListModel = new DefaultListModel<>();
        unpinnedTotalList = new JList<>(unpinnedTotalListModel);
        unpinnedTotalList.setCellRenderer(new ListCellRenderer<String>() {
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
            public Component getListCellRendererComponent(
                    JList<? extends String> list, String value, int index, boolean isSelected, boolean cellHasFocus) {
                String displayText = value;
                String jobName = value.split(" ")[0];

                label.setText(displayText);
                colorBox.setBackground(getJobColor(jobName));
                panel.setBackground(isSelected ? list.getSelectionBackground() : list.getBackground());
                label.setForeground(isSelected ? list.getSelectionForeground() : list.getForeground());
                return panel;
            }
        });

        unpinnedTotalList.addMouseListener(new java.awt.event.MouseAdapter() {
            @Override
            public void mousePressed(MouseEvent e) {
                if (e.isPopupTrigger()) {
                    showUnpinnedTotalListContextMenu(e);
                }
            }

            @Override
            public void mouseReleased(MouseEvent e) {
                if (e.isPopupTrigger()) {
                    showUnpinnedTotalListContextMenu(e);
                }
            }
        });

        JScrollPane unpinnedTotalScrollPane = new JScrollPane(unpinnedTotalList);
        unpinnedTotalScrollPane.setMinimumSize(new Dimension(200, 150));
        unpinnedTotalScrollPane.setBorder(BorderFactory.createTitledBorder("Rolling Total"));

        // Pinned total jobs list
        pinnedTotalListModel = new DefaultListModel<>();
        pinnedTotalList = new JList<>(pinnedTotalListModel);
        pinnedTotalList.setCellRenderer(new ListCellRenderer<String>() {
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
            public Component getListCellRendererComponent(
                    JList<? extends String> list, String value, int index, boolean isSelected, boolean cellHasFocus) {
                String jobName = value.split(" ")[0];
                String displayText = "🔒 " + value;
                label.setText(displayText);
                colorBox.setBackground(getJobColor(jobName));
                panel.setBackground(isSelected ? list.getSelectionBackground() : list.getBackground());
                label.setForeground(isSelected ? list.getSelectionForeground() : list.getForeground());
                return panel;
            }
        });

        pinnedTotalList.addMouseListener(new java.awt.event.MouseAdapter() {
            @Override
            public void mousePressed(MouseEvent e) {
                if (e.isPopupTrigger()) {
                    showPinnedTotalListContextMenu(e);
                }
            }

            @Override
            public void mouseReleased(MouseEvent e) {
                if (e.isPopupTrigger()) {
                    showPinnedTotalListContextMenu(e);
                }
            }
        });

        JScrollPane pinnedTotalScrollPane = new JScrollPane(pinnedTotalList);
        pinnedTotalScrollPane.setMinimumSize(new Dimension(200, 150));
        pinnedTotalScrollPane.setBorder(BorderFactory.createTitledBorder("Pinned Jobs"));

        // Split pane for rolling total and pinned total jobs
        JSplitPane totalSplit =
                new JSplitPane(JSplitPane.VERTICAL_SPLIT, unpinnedTotalScrollPane, pinnedTotalScrollPane);
        totalSplit.setContinuousLayout(true);
        totalSplit.setResizeWeight(0.6);
        totalSplit.setBorder(null);

        // Right panel: Tabbed pane with current frame jobs and all jobs
        rightTabbedPane = new JTabbedPane();

        // Tab 1: Current frame jobs (split into current and pinned)
        currentFrameListModel = new DefaultListModel<>();
        currentFrameList = new JList<>(currentFrameListModel);
        currentFrameList.setCellRenderer(new ListCellRenderer<String>() {
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
            public Component getListCellRendererComponent(
                    JList<? extends String> list, String value, int index, boolean isSelected, boolean cellHasFocus) {
                String jobName = value.split(" ")[0];
                String displayText = value;
                label.setText(displayText);
                colorBox.setBackground(getJobColor(jobName));
                panel.setBackground(isSelected ? list.getSelectionBackground() : list.getBackground());
                label.setForeground(isSelected ? list.getSelectionForeground() : list.getForeground());
                return panel;
            }
        });

        currentFrameList.addMouseListener(new java.awt.event.MouseAdapter() {
            @Override
            public void mousePressed(MouseEvent e) {
                if (e.isPopupTrigger()) {
                    showCurrentFrameContextMenu(e);
                }
            }

            @Override
            public void mouseReleased(MouseEvent e) {
                if (e.isPopupTrigger()) {
                    showCurrentFrameContextMenu(e);
                }
            }
        });

        JScrollPane currentFrameScrollPane = new JScrollPane(currentFrameList);
        currentFrameScrollPane.setMinimumSize(new Dimension(200, 150));
        currentFrameScrollPane.setBorder(BorderFactory.createTitledBorder("Current Frame"));

        // Pinned jobs list
        pinnedJobsListModel = new DefaultListModel<>();
        pinnedJobsList = new JList<>(pinnedJobsListModel);
        pinnedJobsList.setCellRenderer(new ListCellRenderer<String>() {
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
            public Component getListCellRendererComponent(
                    JList<? extends String> list, String value, int index, boolean isSelected, boolean cellHasFocus) {
                String jobName = value.split(" ")[0];
                String displayText = "🔒 " + value;
                label.setText(displayText);
                colorBox.setBackground(getJobColor(jobName));
                panel.setBackground(isSelected ? list.getSelectionBackground() : list.getBackground());
                label.setForeground(isSelected ? list.getSelectionForeground() : list.getForeground());
                return panel;
            }
        });

        pinnedJobsList.addMouseListener(new java.awt.event.MouseAdapter() {
            @Override
            public void mousePressed(MouseEvent e) {
                if (e.isPopupTrigger()) {
                    showPinnedJobsContextMenu(e);
                }
            }

            @Override
            public void mouseReleased(MouseEvent e) {
                if (e.isPopupTrigger()) {
                    showPinnedJobsContextMenu(e);
                }
            }
        });

        JScrollPane pinnedJobsScrollPane = new JScrollPane(pinnedJobsList);
        pinnedJobsScrollPane.setMinimumSize(new Dimension(200, 150));
        pinnedJobsScrollPane.setBorder(BorderFactory.createTitledBorder("Pinned Jobs"));

        // Split pane for current frame and pinned jobs
        JSplitPane currentFrameSplit =
                new JSplitPane(JSplitPane.VERTICAL_SPLIT, currentFrameScrollPane, pinnedJobsScrollPane);
        currentFrameSplit.setContinuousLayout(true);
        currentFrameSplit.setResizeWeight(0.6);
        currentFrameSplit.setBorder(null);

        rightTabbedPane.addTab("Current Frame", currentFrameSplit);

        // Tab 2: All jobs with search
        JPanel allJobsPanel = new JPanel(new BorderLayout());

        JPanel searchPanel = new JPanel(new FlowLayout(FlowLayout.LEFT, 5, 5));
        searchPanel.setBackground(new Color(240, 240, 240));
        searchPanel.add(new JLabel("Search:"));
        searchField = new JTextField(15);
        searchField.addKeyListener(new java.awt.event.KeyAdapter() {
            @Override
            public void keyReleased(java.awt.event.KeyEvent e) {
                updateAllJobsListFilter();
            }
        });
        searchPanel.add(searchField);

        clearSearchButton = new JButton("Clear Search");
        clearSearchButton.addActionListener(e -> {
            searchField.setText("");
            updateAllJobsListFilter();
        });
        searchPanel.add(clearSearchButton);

        allJobsPanel.add(searchPanel, BorderLayout.NORTH);

        allJobsListModel = new DefaultListModel<>();
        allJobsList = new JList<>(allJobsListModel);
        allJobsList.setCellRenderer(new ListCellRenderer<String>() {
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
            public Component getListCellRendererComponent(
                    JList<? extends String> list, String value, int index, boolean isSelected, boolean cellHasFocus) {
                String displayText = value;
                if (pinnedJobs.contains(value)) {
                    displayText = "🔒 " + value;
                }
                label.setText(displayText);
                colorBox.setBackground(getJobColor(value));
                panel.setBackground(isSelected ? list.getSelectionBackground() : list.getBackground());
                label.setForeground(isSelected ? list.getSelectionForeground() : list.getForeground());
                return panel;
            }
        });

        allJobsList.addMouseListener(new java.awt.event.MouseAdapter() {
            @Override
            public void mousePressed(MouseEvent e) {
                if (e.isPopupTrigger()) {
                    showAllJobsContextMenu(e);
                }
            }

            @Override
            public void mouseReleased(MouseEvent e) {
                if (e.isPopupTrigger()) {
                    showAllJobsContextMenu(e);
                }
            }
        });

        JScrollPane allJobsScrollPane = new JScrollPane(allJobsList);
        allJobsPanel.add(allJobsScrollPane, BorderLayout.CENTER);
        rightTabbedPane.addTab("All Jobs", allJobsPanel);

        // Center: Pie chart
        JPanel centerPanel = new JPanel(new BorderLayout());
        centerPanel.add(pieChartPanel, BorderLayout.CENTER);

        // Left-Center split: Rolling totals and Pie Chart
        JSplitPane leftCenterSplit = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, totalSplit, centerPanel);
        leftCenterSplit.setContinuousLayout(true);
        leftCenterSplit.setResizeWeight(0.3);
        leftCenterSplit.setBorder(null);

        // Full split: (Totals + Pie) and Job List
        JSplitPane mainSplit = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, leftCenterSplit, rightTabbedPane);
        mainSplit.setContinuousLayout(true);
        mainSplit.setResizeWeight(0.65);
        mainSplit.setBorder(null);
        add(mainSplit, BorderLayout.CENTER);

        // Drives Swing repaints matched to default 0.02s (20ms / 50 Hz)
        refreshTimer = new Timer(20, e -> renderLatestFrame());
        refreshTimer.start();
    }

    /**
     * Shows context menu for jobs in the unpinned rolling total list.
     */
    private void showUnpinnedTotalListContextMenu(MouseEvent e) {
        int index = unpinnedTotalList.locationToIndex(e.getPoint());
        if (index < 0) return;

        unpinnedTotalList.setSelectedIndex(index);
        String selectedValue = unpinnedTotalList.getSelectedValue();
        if (selectedValue == null) return;

        String jobName = selectedValue.split(" ")[0];

        // Create final reference for use in lambda
        final String finalJobName = jobName;

        JPopupMenu menu = new JPopupMenu();

        JMenuItem pinItem = new JMenuItem("Pin Job");
        pinItem.addActionListener(ev -> {
            pinnedJobs.add(finalJobName);
            unpinnedTotalList.repaint();
            pinnedTotalList.repaint();
            currentFrameList.repaint();
            pinnedJobsList.repaint();
            allJobsList.repaint();
        });
        menu.add(pinItem);

        menu.show(unpinnedTotalList, e.getX(), e.getY());
    }

    /**
     * Shows context menu for jobs in the pinned rolling total list.
     */
    private void showPinnedTotalListContextMenu(MouseEvent e) {
        int index = pinnedTotalList.locationToIndex(e.getPoint());
        if (index < 0) return;

        pinnedTotalList.setSelectedIndex(index);
        String selectedValue = pinnedTotalList.getSelectedValue();
        if (selectedValue == null) return;

        String jobName = selectedValue.split(" ")[0];
        // Remove lock icon if present
        if (jobName.startsWith("🔒")) {
            jobName = jobName.substring(2).trim();
        }

        // Create final reference for use in lambda
        final String finalJobName = jobName;

        JPopupMenu menu = new JPopupMenu();

        JMenuItem unlockItem = new JMenuItem("Unlock Job");
        unlockItem.addActionListener(ev -> {
            pinnedJobs.remove(finalJobName);
            unpinnedTotalList.repaint();
            pinnedTotalList.repaint();
            currentFrameList.repaint();
            pinnedJobsList.repaint();
            allJobsList.repaint();
        });
        menu.add(unlockItem);

        menu.show(pinnedTotalList, e.getX(), e.getY());
    }

    /**
     * Shows context menu for jobs in the current frame list.
     */
    private void showCurrentFrameContextMenu(MouseEvent e) {
        int index = currentFrameList.locationToIndex(e.getPoint());
        if (index < 0) return;

        currentFrameList.setSelectedIndex(index);
        String selectedValue = currentFrameList.getSelectedValue();
        if (selectedValue == null) return;

        String jobName = selectedValue.split(" ")[0];

        // Create final reference for use in lambda
        final String finalJobName = jobName;

        JPopupMenu menu = new JPopupMenu();

        JMenuItem pinItem = new JMenuItem("Pin Job");
        pinItem.addActionListener(ev -> {
            pinnedJobs.add(finalJobName);
            currentFrameList.repaint();
            pinnedJobsList.repaint();
            unpinnedTotalList.repaint();
            pinnedTotalList.repaint();
            allJobsList.repaint();
        });
        menu.add(pinItem);

        menu.show(currentFrameList, e.getX(), e.getY());
    }

    /**
     * Shows context menu for jobs in the pinned jobs list.
     */
    private void showPinnedJobsContextMenu(MouseEvent e) {
        int index = pinnedJobsList.locationToIndex(e.getPoint());
        if (index < 0) return;

        pinnedJobsList.setSelectedIndex(index);
        String selectedValue = pinnedJobsList.getSelectedValue();
        if (selectedValue == null) return;

        String jobName = selectedValue.split(" ")[0];
        // Remove lock icon if present
        if (jobName.startsWith("🔒")) {
            jobName = jobName.substring(2).trim();
        }

        // Create final reference for use in lambda
        final String finalJobName = jobName;

        JPopupMenu menu = new JPopupMenu();

        JMenuItem unlockItem = new JMenuItem("Unlock Job");
        unlockItem.addActionListener(ev -> {
            pinnedJobs.remove(finalJobName);
            currentFrameList.repaint();
            pinnedJobsList.repaint();
            unpinnedTotalList.repaint();
            pinnedTotalList.repaint();
            allJobsList.repaint();
        });
        menu.add(unlockItem);

        menu.show(pinnedJobsList, e.getX(), e.getY());
    }

    /**
     * Shows context menu for jobs in the all jobs list.
     */
    private void showAllJobsContextMenu(MouseEvent e) {
        int index = allJobsList.locationToIndex(e.getPoint());
        if (index < 0) return;

        allJobsList.setSelectedIndex(index);
        String selectedValue = allJobsList.getSelectedValue();
        if (selectedValue == null) return;

        JPopupMenu menu = new JPopupMenu();

        if (pinnedJobs.contains(selectedValue)) {
            JMenuItem unlockItem = new JMenuItem("Unlock Job");
            unlockItem.addActionListener(ev -> {
                pinnedJobs.remove(selectedValue);
                currentFrameList.repaint();
                pinnedJobsList.repaint();
                unpinnedTotalList.repaint();
                pinnedTotalList.repaint();
                allJobsList.repaint();
            });
            menu.add(unlockItem);
        } else {
            JMenuItem pinItem = new JMenuItem("Pin Job");
            pinItem.addActionListener(ev -> {
                pinnedJobs.add(selectedValue);
                currentFrameList.repaint();
                pinnedJobsList.repaint();
                unpinnedTotalList.repaint();
                pinnedTotalList.repaint();
                allJobsList.repaint();
            });
            menu.add(pinItem);
        }

        menu.show(allJobsList, e.getX(), e.getY());
    }

    /**
     * Updates the all jobs list based on the search filter.
     */
    private void updateAllJobsListFilter() {
        String searchText = searchField.getText().toLowerCase().trim();
        allJobsListModel.clear();

        for (String jobName : allJobNames) {
            if (searchText.isEmpty() || jobName.toLowerCase().contains(searchText)) {
                allJobsListModel.addElement(jobName);
            }
        }
    }

    /**
     * Clears all sim-specific state so the GUI can be reused for a new connection.
     * Called when (re)connecting to a simulation, since job names, thread counts,
     * and rolling totals are only meaningful for the previously connected sim.
     */
    public void reset() {
        Runnable clearState = () -> {
            sampleGeneration++;
            vsClient = null;
            allJobNames.clear();
            pinnedJobs.clear();
            jobTotals.clear();
            currentFrameData.clear();
            pendingFrameData = null;
            pendingSimTime = 0.0;
            pendingModeString = "Connecting...";
            recordedJobTime = 0.0;
            clearRealtimeHealth(false);

            isInitializingCombo = true;
            threadComboBox.removeAllItems();
            threadComboBox.addItem("Thread 0 (Loading...)");
            isInitializingCombo = false;

            unpinnedTotalListModel.clear();
            pinnedTotalListModel.clear();
            currentFrameListModel.clear();
            pinnedJobsListModel.clear();
            allJobsListModel.clear();
            searchField.setText("");

            timeLabel.setText("Time: 0.000");
            modeLabel.setText("Mode: Connecting...");
            pieChartPanel.repaint();
        };
        if (SwingUtilities.isEventDispatchThread()) {
            clearState.run();
        } else {
            SwingUtilities.invokeLater(clearState);
        }
    }

    public void initializeThreads(int numThreads, RTJPerfVarServerClient client) {
        SwingUtilities.invokeLater(() -> {
            this.vsClient = client;
            healthConnected = true;
            renderRealtimeHealth(System.nanoTime());
            isInitializingCombo = true;
            threadComboBox.removeAllItems();
            for (int i = 0; i < numThreads; i++) {
                threadComboBox.addItem("Thread " + i + (i == 0 ? " (Main)" : ""));
            }
            isInitializingCombo = false;
        });
    }

    /**
     * Registers all discovered jobs from the simulation.
     */
    public void registerAllJobs(List<String> jobNames) {
        SwingUtilities.invokeLater(() -> {
            allJobNames.clear();
            allJobNames.addAll(jobNames);
            Collections.sort(allJobNames);
            updateAllJobsListFilter();
        });
    }

    /**
     * Sets the cycle rate field and timer based on the software frame value.
     * Called from the Variable Server client after retrieving the software frame.
     */
    public void setSoftwareFrameRate(double softwareFrame) {
        SwingUtilities.invokeLater(() -> {
            cycleRateField.setText(String.format("%.6f", softwareFrame));
            applyCycleRate();
        });
    }

    /**
     * Clears the rolling totals when switching threads.
     */
    private void clearRollingTotals() {
        sampleGeneration++;
        jobTotals.clear();
        pendingFrameData = null;
        currentFrameData.clear();
        recordedJobTime = 0.0;
        currentFrameListModel.clear();
        pinnedJobsListModel.clear();
        unpinnedTotalListModel.clear();
        pinnedTotalListModel.clear();
        pieChartPanel.repaint();
    }

    /**
     * Called from the Variable Server network thread whenever a new cyclic
     * data update arrives. Copies the sample, then records its history on the
     * EDT independently of the redraw timer.
     */
    public void updateFrameData(double simTime, String modeString, List<JobDuration> newFrameData) {
        long generation = sampleGeneration;
        List<JobDuration> snapshot = new ArrayList<>();
        double totalDuration = 0;
        for (JobDuration job : newFrameData) {
            snapshot.add(new JobDuration(job.jobId, job.duration));
            totalDuration += job.duration;
        }
        for (JobDuration job : snapshot) {
            job.percentage = totalDuration > 0.0 ? (job.duration / totalDuration) * 100.0 : 0.0;
        }

        Collections.sort(snapshot, (j1, j2) -> Double.compare(j2.duration, j1.duration));
        final double snapshotTotal = totalDuration;
        SwingUtilities.invokeLater(() -> {
            if (generation != sampleGeneration) {
                return;
            }
            // Threshold is presentation-only; history retains every received job.
            // Advance every job once per received sample, including absent jobs.
            Map<String, Double> durations = new HashMap<>();
            for (JobDuration job : snapshot) {
                durations.merge(job.jobId, job.duration, Double::sum);
            }
            for (Map.Entry<String, RollingTotal> entry : jobTotals.entrySet()) {
                Double duration = durations.remove(entry.getKey());
                if (duration == null) {
                    entry.getValue().incrementFramesSinceLastSeen();
                } else {
                    entry.getValue().add(duration);
                }
            }
            for (Map.Entry<String, Double> entry : durations.entrySet()) {
                RollingTotal total = new RollingTotal(ROLLING_WINDOW_SIZE);
                total.add(entry.getValue());
                jobTotals.put(entry.getKey(), total);
            }
            jobTotals.entrySet().removeIf(entry ->
                    entry.getValue().isStale() && !pinnedJobs.contains(entry.getKey()));
            pendingSimTime = simTime;
            pendingModeString = modeString;
            pendingFrameData = snapshot;
            recordedJobTime = snapshotTotal;
        });
    }

    /**
     * Receives main-thread realtime metrics on every telemetry response, including
     * duplicate completed-frame sequences. Durations are seconds; positive lateness
     * means a missed deadline and negative lateness is remaining headroom.
     * Safe to call from the Variable Server network thread.
     */
    public void updateRealtimeHealth(boolean active, double softwareFrame, double lateness,
            long totalOverruns, long consecutiveOverruns, double peakLateness, long frameSequence) {
        long generation = sampleGeneration;
        long receivedNanos = System.nanoTime();
        SwingUtilities.invokeLater(() -> {
            if (generation != sampleGeneration) {
                return;
            }
            if (!hasHealthSample || completedFrameSequence != frameSequence) {
                lastFrameProgressNanos = receivedNanos;
            }
            hasHealthSample = true;
            healthUnavailableReason = null;
            healthConnected = true;
            realtimeActive = active;
            healthSoftwareFrame = softwareFrame;
            deadlineLateness = lateness;
            this.totalOverruns = totalOverruns;
            this.consecutiveOverruns = consecutiveOverruns;
            this.peakLateness = peakLateness;
            completedFrameSequence = frameSequence;
            lastTelemetryNanos = receivedNanos;
            renderRealtimeHealth(System.nanoTime());
        });
    }

    /** Reports a telemetry response from a simulation without realtime metrics. */
    public void updateRealtimeHealthUnavailable(String reason) {
        long generation = sampleGeneration;
        long receivedNanos = System.nanoTime();
        SwingUtilities.invokeLater(() -> {
            if (generation != sampleGeneration) {
                return;
            }
            hasHealthSample = false;
            healthConnected = true;
            healthUnavailableReason = reason == null || reason.trim().isEmpty()
                    ? "Required metrics are unavailable" : reason;
            lastTelemetryNanos = receivedNanos;
            renderRealtimeHealth(System.nanoTime());
        });
    }

    private void clearRealtimeHealth(boolean connected) {
        healthConnected = connected;
        hasHealthSample = false;
        healthUnavailableReason = null;
        realtimeActive = false;
        healthSoftwareFrame = 0.0;
        deadlineLateness = 0.0;
        totalOverruns = 0;
        consecutiveOverruns = 0;
        peakLateness = 0.0;
        completedFrameSequence = 0;
        lastTelemetryNanos = 0;
        lastFrameProgressNanos = 0;
        renderRealtimeHealth(System.nanoTime());
    }

    private void renderRealtimeHealth(long nowNanos) {
        if (healthUnavailableReason != null) {
            realtimeHealthLabel.setText("Realtime health: Unavailable — " + healthUnavailableReason);
            deadlineLabel.setText("Deadline / overruns / completed-frame progress: Unavailable");
            telemetryLabel.setText(String.format("Telemetry age: %.3f s",
                    Math.max(0.0, (nowNanos - lastTelemetryNanos) / 1.0e9)));
            return;
        }
        if (!hasHealthSample) {
            String status = healthConnected ? "No samples" : "Disconnected — no samples";
            realtimeHealthLabel.setText("Realtime health: " + status);
            deadlineLabel.setText("Deadline: No samples");
            telemetryLabel.setText("Telemetry: " + status);
            return;
        }
        realtimeHealthLabel.setText(String.format(
                "Realtime: %s | Software frame budget: %.6f s | Overruns: %d total / %d consecutive",
                realtimeActive ? "active" : "inactive", healthSoftwareFrame,
                totalOverruns, consecutiveOverruns));
        String deadline = realtimeActive
                ? String.format("Last running-frame deadline: %+.6f s (%s)",
                        deadlineLateness, deadlineLateness > 0.0 ? "late" : "headroom")
                : "Deadline: not applicable (realtime inactive)";
        deadlineLabel.setText(String.format("%s | Retained peak lateness: %.6f s", deadline, peakLateness));
        double telemetryAge = Math.max(0.0, (nowNanos - lastTelemetryNanos) / 1.0e9);
        double progressAge = Math.max(0.0, (nowNanos - lastFrameProgressNanos) / 1.0e9);
        double warningAge = Math.max(1.0,
                3.0 * Math.max(healthSoftwareFrame, refreshTimer.getDelay() / 1000.0));
        boolean progressExpected = realtimeActive && "Run".equalsIgnoreCase(pendingModeString);
        String progress = progressExpected
                ? (progressAge > warningAge ? "STALLED" : "advancing")
                : "not expected (inactive or not running)";
        telemetryLabel.setText(String.format(
                "Telemetry age: %.3f s%s | Completed frame: %d | Progress age: %.3f s — %s",
                telemetryAge, telemetryAge > warningAge ? " (stale)" : "",
                completedFrameSequence, progressAge, progress));
    }

    /**
     * Runs on the EDT via refreshTimer. Paints whatever the most recently
     * received frame snapshot is at the configured rate. Filters jobs below
     * the percentage threshold without advancing rolling history.
     */
    private void renderLatestFrame() {
        renderRealtimeHealth(System.nanoTime());
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

        // Add pinned jobs even if they don't meet threshold
        for (String pinnedJob : pinnedJobs) {
            boolean found = false;
            for (JobDuration job : filteredData) {
                if (job.jobId.equals(pinnedJob)) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                // Find the job in the full frame data
                for (JobDuration job : frameData) {
                    if (job.jobId.equals(pinnedJob)) {
                        filteredData.add(job);
                        found = true;
                        break;
                    }
                }
                // If pinned job wasn't in this frame, create a zero-duration entry
                if (!found) {
                    filteredData.add(new JobDuration(pinnedJob, 0.0));
                }
            }
        }

        this.currentFrameData = filteredData;

        // Update current frame list (unpinned jobs only, sorted by duration)
        Vector<String> currentFrameUpdate = new Vector<>();
        List<JobDuration> unpinnedJobs = new ArrayList<>();
        for (JobDuration job : currentFrameData) {
            if (!pinnedJobs.contains(job.jobId)) {
                unpinnedJobs.add(job);
            }
        }
        // Sort unpinned jobs by duration
        unpinnedJobs.sort((j1, j2) -> Double.compare(j2.duration, j1.duration));
        for (JobDuration job : unpinnedJobs) {
            currentFrameUpdate.add(String.format("%s (%.1f%%, %.6fs)", job.jobId, job.percentage, job.duration));
        }
        currentFrameListModel.clear();
        for (String value : currentFrameUpdate) {
            currentFrameListModel.addElement(value);
        }

        // Update pinned jobs list (maintain insertion order, don't sort)
        Vector<String> pinnedUpdate = new Vector<>();
        for (String pinnedJobName : pinnedJobs) {
            // Find the job in currentFrameData
            for (JobDuration job : currentFrameData) {
                if (job.jobId.equals(pinnedJobName)) {
                    pinnedUpdate.add(String.format("%s (%.1f%%, %.6fs)", job.jobId, job.percentage, job.duration));
                    break;
                }
            }
        }
        pinnedJobsListModel.clear();
        for (String value : pinnedUpdate) {
            pinnedJobsListModel.addElement(value);
        }

        // Update rolling total list (unpinned jobs sorted by total)
        List<Map.Entry<String, RollingTotal>> totalEntries = new ArrayList<>(jobTotals.entrySet());
        totalEntries.sort((e1, e2) ->
                Double.compare(e2.getValue().getTotal(), e1.getValue().getTotal()));

        Vector<String> unpinnedTotalUpdate = new Vector<>();
        List<Map.Entry<String, RollingTotal>> unpinnedTotalEntries = new ArrayList<>();
        Map<String, RollingTotal> pinnedTotalMap = new HashMap<>();

        for (Map.Entry<String, RollingTotal> entry : totalEntries) {
            if (pinnedJobs.contains(entry.getKey())) {
                pinnedTotalMap.put(entry.getKey(), entry.getValue());
            } else {
                unpinnedTotalEntries.add(entry);
            }
        }

        // Add unpinned jobs first (sorted by total)
        for (Map.Entry<String, RollingTotal> entry : unpinnedTotalEntries) {
            double total = entry.getValue().getTotal();
            unpinnedTotalUpdate.add(String.format("%s (%.6fs)", entry.getKey(), total));
        }
        unpinnedTotalListModel.clear();
        for (String value : unpinnedTotalUpdate) {
            unpinnedTotalListModel.addElement(value);
        }

        // Add pinned jobs (maintain insertion order from pinnedJobs LinkedHashSet)
        Vector<String> pinnedTotalUpdate = new Vector<>();
        for (String pinnedJobName : pinnedJobs) {
            if (pinnedTotalMap.containsKey(pinnedJobName)) {
                double total = pinnedTotalMap.get(pinnedJobName).getTotal();
                pinnedTotalUpdate.add(String.format("%s (%.6fs)", pinnedJobName, total));
            }
        }
        pinnedTotalListModel.clear();
        for (String value : pinnedTotalUpdate) {
            pinnedTotalListModel.addElement(value);
        }

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

            // 1. Queue the update for the Variable Server worker.
            vsClient.setCycleRate(seconds);

            // 2. Adjust local Swing timer rate (convert seconds to milliseconds)
            int millis = (int) Math.round(seconds * 1000.0);
            refreshTimer.setDelay(Math.max(millis, 1));

        } catch (NumberFormatException ex) {
            JOptionPane.showMessageDialog(
                    this,
                    "Enter a valid cycle time in seconds (e.g., 0.02 for 50 Hz).",
                    "Invalid Rate",
                    JOptionPane.WARNING_MESSAGE);
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
            JOptionPane.showMessageDialog(
                    this,
                    "Enter a valid percentage threshold (0.0 to 100.0, e.g., 0.1 for 0.1%).",
                    "Invalid Threshold",
                    JOptionPane.WARNING_MESSAGE);
        }
    }

    private Color getJobColor(String jobId) {
        return jobColorMap.computeIfAbsent(
                jobId, id -> Color.getHSBColor(Math.abs(id.hashCode() % 360) / 360f, 0.7f, 0.9f));
    }

    private void drawPieChart(Graphics g) {
        Graphics2D g2d = (Graphics2D) g;
        g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        g2d.setColor(Color.BLACK);
        g2d.setFont(g2d.getFont().deriveFont(Font.BOLD));
        g2d.drawString(String.format("Recorded job time: %.6f s", recordedJobTime), 10, 20);
        if (currentFrameData == null || currentFrameData.isEmpty()) return;
        int padding = 20;
        int pieSize = Math.min(pieChartPanel.getWidth(), pieChartPanel.getHeight()) - (padding * 2);
        int pieX = (pieChartPanel.getWidth() - pieSize) / 2;
        int pieY = (pieChartPanel.getHeight() - pieSize) / 2;
        double currentAngle = 0, totalDuration = 0;
        for (JobDuration job : currentFrameData) {
            totalDuration += job.duration;
        }
        if (totalDuration == 0) {
            g2d.drawString("No job data available", 10, 40);
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
        RealTimeJobPieChart pieChart = new RealTimeJobPieChart();
        pieChart.setPreferredSize(new Dimension(1200, 600));

        // Holds the currently connected client so the Disconnect/Connect actions,
        // which are constructed before the ConnectionStatusBar exists, can reach it.
        final RTJPerfVarServerClient[] activeClient = new RTJPerfVarServerClient[1];
        final ConnectionStatusBar[] statusBarHolder = new ConnectionStatusBar[1];

        AbstractAction connectAction = new AbstractAction("Connect") {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                ConnectionStatusBar connectionStatusBar = statusBarHolder[0];
                setEnabled(false);

                final String host;
                final int port;
                try {
                    host = connectionStatusBar.getHostName();
                    port = connectionStatusBar.getPort();
                } catch (IllegalArgumentException illegalArgumentException) {
                    JOptionPane.showMessageDialog(
                            pieChart, illegalArgumentException, "Invalid Connection", JOptionPane.ERROR_MESSAGE);
                    setEnabled(true);
                    return;
                }

                new Thread(() -> {
                            try {
                                VariableServerConnection vsConnection = new VariableServerConnection(host, port);
                                SwingUtilities.invokeLater(() -> {
                                    final RTJPerfVarServerClient[] clientHolder = new RTJPerfVarServerClient[1];
                                    RTJPerfVarServerClient client = new RTJPerfVarServerClient(
                                            vsConnection, pieChart, () -> SwingUtilities.invokeLater(() -> {
                                                // A retiring client must not reset a newer connection.
                                                if (activeClient[0] == clientHolder[0]) {
                                                    activeClient[0] = null;
                                                    connectionStatusBar.setConnectionState(false);
                                                    pieChart.reset();
                                                }
                                            }));
                                    clientHolder[0] = client;
                                    activeClient[0] = client;
                                    pieChart.reset();
                                    connectionStatusBar.setConnectionState(true);
                                    setEnabled(true);
                                    new Thread(client, "RTJPerf variable server").start();
                                });
                            } catch (Exception exception) {
                                SwingUtilities.invokeLater(() -> {
                                    JOptionPane.showMessageDialog(
                                            pieChart, exception, "Failed to Connect", JOptionPane.ERROR_MESSAGE);
                                    setEnabled(true);
                                });
                            }
                        })
                        .start();
            }
        };

        AbstractAction disconnectAction = new AbstractAction("Disconnect") {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                if (activeClient[0] != null) {
                    activeClient[0].stop();
                    activeClient[0] = null;
                }
                statusBarHolder[0].setConnectionState(false);
                pieChart.reset();
            }
        };

        AbstractAction stopSearchingAction = new AbstractAction("Stop") {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                statusBarHolder[0].cancelAutoConnect();
            }
        };

        ConnectionStatusBar connectionStatusBar =
                new ConnectionStatusBar(connectAction, disconnectAction, stopSearchingAction);
        statusBarHolder[0] = connectionStatusBar;

        SwingUtilities.invokeLater(() -> {
            JFrame frame = new JFrame("RTJPerf - Real-Time Job Performance");
            frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
            frame.add(pieChart, BorderLayout.CENTER);
            frame.add(connectionStatusBar, BorderLayout.SOUTH);
            frame.pack();
            frame.setLocationRelativeTo(null);
            frame.setVisible(true);
        });
    }
}
