package trick.common;

import java.awt.BorderLayout;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import java.io.IOException;
import java.util.Arrays;
import java.util.ArrayList;

import javax.swing.AbstractAction;
import javax.swing.Box;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JDialog;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JRootPane;
import javax.swing.JSeparator;
import javax.swing.KeyStroke;
import javax.swing.border.TitledBorder;

import joptsimple.OptionParser;
import joptsimple.OptionSet;
import joptsimple.OptionSpec;

import org.jdesktop.swingx.JXButton;
import org.jdesktop.swingx.JXLabel;
import org.jdesktop.swingx.JXPanel;
import org.jdesktop.swingx.JXTextField;

import trick.common.ui.panels.ConnectionStatusBar;
import trick.common.utils.VariableServerConnection;

/**
 * Abstract base class for Trick run time GUIs, providing basic connection/disconnection functionality.
 *
 * @author Derek Bankieris
 * @since Trick 10
 */
public abstract class RunTimeTrickApplication extends TrickApplication {

    /** enumeration for application behavior on disconnect */
    protected enum DisconnectBehavior {Close, Notify, Nothing}

    /** indicates that the target simulation has been specified */
    protected boolean targetSpecified = false;

    /** reconnect on lost connetions */
    protected boolean autoReconnect = false;

    /** reconnecting state */
    private boolean reconnecting = false;

    /** application behavior on disconnect */
    protected DisconnectBehavior disconnectBehavior = DisconnectBehavior.Notify;

    /** handles the sending and receiving of commands to and from the Variable Server */
    protected VariableServerConnection variableServerConnection;

    /** status bar displaying text fields for the host and port, and a connect/disconnect button */
    private ConnectionStatusBar connectionStatusBar;

    /** represents the current connection status */
    private boolean connected = false;

    /** update period in seconds */
    private double cyclePeriod;

    /** minimum update period in seconds */
    private double minimumCyclePeriod;

    /** the settings dialog */
    protected SettingsDialog settingsDialog;

    /** property keys */
    protected static final String hostKey = "host";
    protected static final String portKey = "port";
    protected static final String autoReconnectKey = "autoReconnect";
    protected static final String disconnectBehaviorKey = "disconnectBehavior";
    protected static final String minimumCyclePeriodKey = "minCycle";
    protected static final String cyclePeriodKey = "cycle";
    protected static final String xKey = "x";
    protected static final String yKey = "y";
    protected static final String widthKey = "width";
    protected static final String heightKey = "height";

    /** attempts to connect to the simulation specified by the current host name and port
     * @throws IOException IOException
     */
    protected void connect() throws IOException {
        if (!connected) {
            variableServerConnection = new VariableServerConnection(
              getHostName(), getPort());
            setConnectionState(true);
        }
        else {
            throw new IOException("Already connected.");
        }
    }

    /** attempts to disconnect from the simulation
     * @throws IOException IOException
     */
    protected void disconnect() throws IOException {
        disconnect(null);
    }

    /**
     * attempts to disconnect from the simulation. If  the disconnect behavior is
     * <code>DisconnectBehavior.Notify</code> and <code>exception</code> is not
     * <code>null</code>, it is displayed as the reason for the disconnection.
     *
     * @param exception (optional) the reason for the disconnection
     * @throws IOException IOException
     */
    protected void disconnect(Exception exception) throws IOException {
        if (connected) {
            try {
                variableServerConnection.close();
            }
            finally {
                setConnectionState(false);
                switch (disconnectBehavior) {
                    case Close:
                        exit();
                        break;
                    case Notify:
                        if (exception != null) {
                            JOptionPane.showMessageDialog(getMainFrame(), exception,
                              "Lost Connection", JOptionPane.ERROR_MESSAGE);
                        }
                        break;
                }
                if (autoReconnect && exception != null) {
                    reconnecting = true;
                    connectionStatusBar.autoConnect();
                }
            }
        }
        else {
            throw new IOException("Not currently connected.");
        }
    }

    /**
     * sets the connection state for both this application and the connection status bar
     *
     * @param connected the connection state
     */
    protected void setConnectionState(boolean connected) {
        reconnecting = false;
        this.connected = connected;
        connectionStatusBar.setConnectionState(connected);
    }

    /**
     * gets the connection state
     *
     * @return the connection state
     */
    protected boolean getConnectionState() {
        return connected;
    }

    /**
     * gets the host name
     *
     * @return the host name
     */
    protected String getHostName() {
        return connectionStatusBar.getHostName();
    }

    /**
     * gets the port
     *
     * @return the port
     */
    protected int getPort() {
        return connectionStatusBar.getPort();
    }

    /**
     * sets the target simulation to which to connect
     *
     * @param hostName the name of the host machine
     * @param port the port on which to connect
     */
    public void setTargetSimulation(String hostName, int port) {
        connectionStatusBar.setTarget(hostName, port);
    }

    /** sends the cycle period to the Variable Server */
    protected void sendCyclePeriod() {
        if (connected) {
            try {
                variableServerConnection.setCycle(cyclePeriod);
            }
            catch (Exception exception) {
                try {
                    disconnect(exception);
                }
                catch (Exception anotherException) {}
            }
        }
    }

    /**
     * sets the period at which updates are received
     *
     * @param cyclePeriod the cycle period in seconds
     */
    public void setCyclePeriod(double cyclePeriod) {
        this.cyclePeriod = cyclePeriod < minimumCyclePeriod ? minimumCyclePeriod : cyclePeriod;
        sendCyclePeriod();
    }

    /**
     * gets the period at which updates are received
     *
     * @return the cycle period in seconds
     */
    public double getCyclePeriod() {
        return cyclePeriod;
    }

    /**
     * sets the minimum allowed period at which updates are received
     *
     * @param minimumCyclePeriod the minimum allowed cycle period in seconds
     */
    public void setMinimumCyclePeriod(double minimumCyclePeriod) {
        this.minimumCyclePeriod = minimumCyclePeriod;
        if (cyclePeriod < minimumCyclePeriod) {
            setCyclePeriod(minimumCyclePeriod);
        }
    }

    @Override
    protected void initialize(final String[] arguments) {
        super.initialize(arguments);

        connectionStatusBar = new ConnectionStatusBar(
            new AbstractAction("Connect") {
                {
                putValue(SHORT_DESCRIPTION, "Connect to the simulation.");
                putValue(MNEMONIC_KEY, KeyEvent.VK_C);
                }
                public void actionPerformed(ActionEvent actionEvent) {
                    try {
                        setEnabled(false);
                        connect();
                    }
                    catch (Exception exception) {
                        if (reconnecting) {
                            connectionStatusBar.autoConnect();
                        }
                        else {
                            JOptionPane.showMessageDialog(getMainFrame(), exception,
                              "Failed to Connect", JOptionPane.ERROR_MESSAGE);
                            exception.printStackTrace(System.err);
                            try {
                                disconnect();
                            }
                            catch (IOException ioException) {}
                            setEnabled(true);
                        }
                    }
                }
            },
            new AbstractAction("Disconnect") {
                {
                putValue(SHORT_DESCRIPTION, "Disconnect from the simulation.");
                putValue(MNEMONIC_KEY, KeyEvent.VK_C);
                }
                public void actionPerformed(ActionEvent actionEvent) {
                    try {
                        setEnabled(false);
                        disconnect();
                    }
                    catch (Exception exception) {
                        JOptionPane.showMessageDialog(getMainFrame(), exception,
                          "Error While Disconnecting", JOptionPane.ERROR_MESSAGE);
                        exception.printStackTrace(System.err);
                        setEnabled(true);
                    }
                }
            },
            new AbstractAction("Stop") {
                {
                putValue(SHORT_DESCRIPTION, "Stop searching.");
                }
                @Override
                public void actionPerformed(ActionEvent actionEvent) {
                    connectionStatusBar.cancelAutoConnect();
                    reconnecting = false;
                }
            }
        );

        autoReconnect = Boolean.parseBoolean(
          trickProperties.getProperty(autoReconnectKey, Boolean.toString(autoReconnect)));

        // Set up and parse all the options.
        try {
            parseArguments(arguments, new OptionParser());
        }
        catch (Exception exception) {
            exception.printStackTrace();
            System.exit(0);
        }

        // Restore previous settings.
        connectionStatusBar.setTarget(
          trickProperties.getProperty(hostKey, getHostName()),
          Integer.parseInt(trickProperties.getProperty(portKey, Integer.toString(getPort()))));

        disconnectBehavior = Enum.valueOf(DisconnectBehavior.class,
          trickProperties.getProperty(disconnectBehaviorKey, disconnectBehavior.toString()));

        setMinimumCyclePeriod(Double.parseDouble(trickProperties.getProperty(minimumCyclePeriodKey, "0.1")));
        setCyclePeriod(Double.parseDouble(trickProperties.getProperty(cyclePeriodKey, "0.5")));
    }

    /**
     * parses the command line arguments
     *
     * @param arguments the arguments
     * @param optionParser the option parser to use
     */
    protected OptionSet parseArguments(String[] arguments, OptionParser optionParser) {
        OptionSpec<String> hostSpec = optionParser.accepts(hostKey, "The name of the machine to connect to.")
          .withRequiredArg().ofType(String.class);

        OptionSpec<Integer> portSpec = optionParser.accepts(portKey, "The port to connect to.")
          .withRequiredArg().ofType(Integer.class);

        OptionSpec<Boolean> autoReconnectSpec = optionParser.accepts(autoReconnectKey, "Reestablish lost connections.")
          .withRequiredArg().ofType(Boolean.class);

        String description = "[";
        for (DisconnectBehavior disconnectBehavior : DisconnectBehavior.values()) {
            description += disconnectBehavior + ", ";
        }
        description = description.substring(0, description.length() - 2) + "] The action to take on disconnect.";
        OptionSpec<DisconnectBehavior> disconnectBehaviorSpec = optionParser.accepts(disconnectBehaviorKey, description)
          .withRequiredArg().ofType(DisconnectBehavior.class);

        OptionSpec<Double> minimumCyclePeriodSpec = optionParser.accepts(minimumCyclePeriodKey,
          "Minimum update cycle period (in seconds).")
          .withRequiredArg().ofType(Double.class);

        OptionSpec<Double> cyclePeriodSpec = optionParser.accepts(cyclePeriodKey,
          "Update cycle period (in seconds).")
          .withRequiredArg().ofType(Double.class);

        OptionSpec<Integer> xSpec = optionParser.accepts(xKey, "X position of the window.")
          .withRequiredArg().ofType(Integer.class);

        OptionSpec<Integer> ySpec = optionParser.accepts(yKey, "Y position of the window.")
          .withRequiredArg().ofType(Integer.class);

        OptionSpec<Integer> widthSpec = optionParser.accepts(widthKey,"Width of the window.")
          .withRequiredArg().ofType(Integer.class);

        OptionSpec<Integer> heightSpec = optionParser.accepts(heightKey, "Height of the window.")
          .withRequiredArg().ofType(Integer.class);

        optionParser.accepts("help", "Print this help message.");

        // Mac application bundles launched via "open" are sometimes passed a "process serial number" of the form -psn_X_YYYYYYY. Ignore it.
        ArrayList<String> sanitizedArguments = new ArrayList<>(Arrays.asList(arguments));
        sanitizedArguments.removeIf(element -> element.startsWith("-psn_"));
        OptionSet options = optionParser.parse(sanitizedArguments.toArray(new String[sanitizedArguments.size()]));
        if (options.has("help") || !options.nonOptionArguments().isEmpty()) {
            try {
                System.out.println("Options may be abbreviated and use a single hyphen if unambiguous.");
                optionParser.printHelpOn(System.out);
            }
            catch (IOException e) {}
            System.exit(0);
        }

        if (options.has(autoReconnectKey)) {
            autoReconnect = options.valueOf(autoReconnectSpec);
        }

        if (options.has(hostKey)) {
            trickProperties.setProperty(hostKey, options.valueOf(hostSpec).toString());
            targetSpecified = true;
        }

        if (options.has(portKey)) {
            trickProperties.setProperty(portKey, (options.valueOf(portSpec).toString()));
            targetSpecified = true;
        }

        if (options.has(disconnectBehaviorKey)) {
            trickProperties.setProperty(disconnectBehaviorKey, options.valueOf(disconnectBehaviorSpec).toString());
        }

        if (options.has(minimumCyclePeriodKey)) {
            Double minimumCyclePeriod = options.valueOf(minimumCyclePeriodSpec);
            if (minimumCyclePeriod < 0) {
                System.out.println("Argument of option ['" + minimumCyclePeriodKey + "'] must be non-negative");
                System.exit(0);
            }
            trickProperties.setProperty(minimumCyclePeriodKey, minimumCyclePeriod.toString());
        }

        if (options.has(cyclePeriodKey)) {
            Double cyclePeriod = options.valueOf(cyclePeriodSpec);
            if (cyclePeriod < 0) {
                System.out.println("Argument of option ['" + cyclePeriodKey + "'] must be non-negative");
                System.exit(0);
            }
            trickProperties.setProperty(cyclePeriodKey, cyclePeriod.toString());
        }

        if (options.has(xKey)) {
            trickProperties.setProperty(xKey, (options.valueOf(xSpec).toString()));
        }

        if (options.has(yKey)) {
            trickProperties.setProperty(yKey, (options.valueOf(ySpec).toString()));
        }

        if (options.has(widthKey)) {
            trickProperties.setProperty(widthKey, (options.valueOf(widthSpec).toString()));
        }

        if (options.has(heightKey)) {
            trickProperties.setProperty(heightKey, (options.valueOf(heightSpec).toString()));
        }

        return options;
    }

    @Override
    protected void startup() {
        super.startup();
        settingsDialog = new SettingsDialog();
        getMainView().setStatusBar(createStatusBar());
    }

    @Override
    protected void ready() {
        super.ready();
        if (targetSpecified) {
            connectionStatusBar.connectAction.actionPerformed(null);
        }
    }

    @Override
    protected JMenuBar createMenuBar() {
        JMenuBar menuBar = super.createMenuBar();
        JMenu menu = menuBar.getMenu(0);

        // Remove look and feel menu item.
        for (int i = 0; i < 2; ++i) {
            menu.remove(0);
        }

        menu.add(new JSeparator(), 0);

        menu.add(new JMenuItem(new AbstractAction("Settings") {
            {
            putValue(MNEMONIC_KEY, KeyEvent.VK_G);
            }
            public void actionPerformed(ActionEvent actionEvent) {
                settingsDialog.setVisible(true);
            }
        }), 0);

        return menuBar;
    }

    @Override
    protected JComponent createStatusBar() {
        return connectionStatusBar;
    }

    @Override
    protected void shutdown() {
        try {
            String hostName = getHostName();
            int port = getPort();
            trickProperties.setProperty(hostKey, hostName);
            trickProperties.setProperty(portKey, Integer.toString(port));
        }
        catch (Exception exception) {}

        trickProperties.setProperty(autoReconnectKey, Boolean.toString(autoReconnect));
        trickProperties.setProperty(disconnectBehaviorKey, disconnectBehavior.toString());
        trickProperties.setProperty(minimumCyclePeriodKey, Double.toString(minimumCyclePeriod));
        trickProperties.setProperty(cyclePeriodKey, Double.toString(cyclePeriod));

        Rectangle bounds = getMainFrame().getBounds();
        trickProperties.setProperty(xKey, Integer.toString(bounds.x));
        trickProperties.setProperty(yKey, Integer.toString(bounds.y));
        trickProperties.setProperty(widthKey, Integer.toString(bounds.width));
        trickProperties.setProperty(heightKey, Integer.toString(bounds.height));

        if (variableServerConnection != null) {
            try {
                variableServerConnection.close();
            }
            catch (IOException e) {}
        }
        super.shutdown();
    }

    /**
     * dialog for modifying settings
     */
    protected class SettingsDialog extends JDialog {

        ArrayList<CommitChangesListener> commitChangesListeners = new ArrayList<CommitChangesListener>();
        ArrayList<BecomingVisibleListener> becomingVisibleListeners = new ArrayList<BecomingVisibleListener>();

        /** weather or not to continuously attempt to reconnect after losing a connection */
        JCheckBox autoReconnectCheckBox = new JCheckBox("Automatically Reconnect") {{
            setToolTipText("Automatically attempt to reconnect after losing a connection.");
        }};

        /** the behavior to take on disconnect */
        @SuppressWarnings("unchecked") // JComboBox is generic in Java 1.7+
        JComboBox disconnectBehaviorComboBox =
          new JComboBox(DisconnectBehavior.values()) {{
            setToolTipText("The behavior to take when the connection to the simulation is lost.");
          }};

        /** text field for setting the cycle period */
        JXTextField cycleField = new JXTextField() {{
            setColumns(3);
            setHorizontalAlignment(TRAILING);
            setToolTipText("Update period in seconds.");
        }};

        JXPanel mainPanel = new JXPanel(new GridBagLayout()) {{
            GridBagConstraints constraints = new GridBagConstraints() {{
                gridx = 0;
                fill = BOTH;
            }};

            add(new JXPanel(new GridBagLayout()) {{
                setBorder(new TitledBorder("Application Behavior") {{
                    setTitleJustification(CENTER);
                }});

                GridBagConstraints constraints = new GridBagConstraints() {{
                    gridy = 1;
                }};

                add(new JXLabel("On Disconnect: "), constraints);
                add(disconnectBehaviorComboBox, constraints);
                constraints.gridx = constraints.gridy = 0;
                constraints.gridwidth = 2;
                add(autoReconnectCheckBox, constraints);
            }}, constraints);

            add(new JXPanel() {{
                setBorder(new TitledBorder("Cycle Period") {{
                    setTitleJustification(CENTER);
                }});
                add(cycleField);
                add(new JXLabel(" seconds"));
            }}, constraints);
        }};

        /**
         * constructor
         */
        public SettingsDialog() {
            super(getMainFrame(), "Settings", true);

            final AbstractAction okAction = new AbstractAction("OK") {
                {
                putValue(SHORT_DESCRIPTION, "Apply changes.");
                putValue(MNEMONIC_KEY, KeyEvent.VK_O);
                }
                public void actionPerformed(ActionEvent actionEvent) {
                    for (CommitChangesListener commitChangesListener : commitChangesListeners) {
                        if (!commitChangesListener.canCommitChanges()) {
                            return;
                        }
                    }

                    try {
                        double period = Double.parseDouble(cycleField.getText());
                        if (period < 0) {
                            throw new NumberFormatException("Period must be non-negative.");
                        }
                        setCyclePeriod(period);
                    }
                    catch (NumberFormatException numberFormatException) {
                        JOptionPane.showMessageDialog(getMainFrame(), numberFormatException,
                          "Invalid Value", JOptionPane.ERROR_MESSAGE);
                        cycleField.requestFocusInWindow();
                        cycleField.selectAll();
                        return;
                    }

                    autoReconnect = autoReconnectCheckBox.isSelected();
                    disconnectBehavior = (DisconnectBehavior)disconnectBehaviorComboBox.getSelectedItem();

                    for (CommitChangesListener commitChangesListener : commitChangesListeners) {
                        commitChangesListener.commitChanges();
                    }

                    setVisible(false);
                }
            };

            final JXButton okButton = new JXButton(okAction);

            final AbstractAction cancelAction = new AbstractAction("Cancel") {
                {
                putValue(SHORT_DESCRIPTION, "Discard changes.");
                putValue(MNEMONIC_KEY, KeyEvent.VK_C);
                }
                public void actionPerformed(ActionEvent e) {
                   setVisible(false);
                }
            };

            setContentPane(new JXPanel(new BorderLayout()) {{
                add(mainPanel, BorderLayout.CENTER);

                add(new JXPanel(new GridLayout(1, 4)) {{
                    add(Box.createHorizontalGlue());
                    add(okButton);
                    add(new JXButton(cancelAction));
                    add(Box.createHorizontalGlue());
                }}, BorderLayout.SOUTH);
            }});

            JRootPane rootPane = getRootPane();

            rootPane.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW).put(
              KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE, 0), "SettingsDialog.cancel");
            rootPane.getActionMap().put("SettingsDialog.cancel", cancelAction);

            rootPane.setDefaultButton(okButton);

            pack();
        }

        @Override
        public void setVisible(boolean visible) {
            if (visible) {
                autoReconnectCheckBox.setSelected(autoReconnect);
                disconnectBehaviorComboBox.setSelectedItem(disconnectBehavior);
                cycleField.setText(Double.toString(cyclePeriod));
                getRootPane().getDefaultButton().requestFocusInWindow();
                setLocationRelativeTo(getMainFrame());

                for (BecomingVisibleListener becomingVisibleListener : becomingVisibleListeners) {
                    becomingVisibleListener.becomingVisible();
                }
            }
            super.setVisible(visible);
        }

        /**
         * adds <code>panel</code> to this dialog
         *
         * @param panel the panel to add
         * @param constraints constraints
         */
        public void addPanel(JPanel panel, GridBagConstraints constraints) {
            mainPanel.add(panel, constraints);
            pack();
        }

        /**
         * registers <code>becomingVisibleListener</code> to be notified just
         * before this dialog becomes visible
         *
         * @param becomingVisibleListener the listener to be notified
         */
        public void addBecomingVisibleListener(BecomingVisibleListener becomingVisibleListener) {
            becomingVisibleListeners.add(becomingVisibleListener);
        }

        /**
         * registers <code>commitChangesListener</code> to be notified when this
         * dialog's OK button is pressed
         *
         * @param commitChangesListener the listener to be notified
         */
        public void addCommitChangesListener(CommitChangesListener commitChangesListener) {
            commitChangesListeners.add(commitChangesListener);
        }

    }

    /** a listener interface for when the settings dialog becomes visible */
    public interface BecomingVisibleListener {
        /** called just before the settings dialog becomes visible */
        public void becomingVisible();
    }

    /** a listener interface for when the OK button on the settings dialog is pressed */
    public interface CommitChangesListener {
        /**
         * called just before changes are to be committed, allowing listeners to
         * veto the commit. If any listener returns <code>false</code>,
         * <code>commitChanges()</code> will not be called.
         *
         * @return whether or not to proceed with the commit
         */
        public boolean canCommitChanges();

        /** called after all listeners have approved the commit request */
        public void commitChanges();
    }

}
