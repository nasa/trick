package trick.common.ui.panels;

import java.awt.Color;
import java.awt.Font;
import java.awt.Insets;

import javax.swing.Action;
import javax.swing.JComboBox;
import javax.swing.JOptionPane;
import javax.swing.SwingUtilities;
import javax.swing.border.EmptyBorder;

import org.jdesktop.swingx.JXButton;
import org.jdesktop.swingx.JXLabel;
import org.jdesktop.swingx.JXStatusBar;
import org.jdesktop.swingx.plaf.basic.BasicStatusBarUI;

import trick.sniffer.SimulationInformation;
import trick.sniffer.SimulationListener;
import trick.sniffer.SimulationSniffer;

/**
 * provides a common GUI element for connecting to the Variable Server on a given host and port
 *
 * @author Derek Bankieris
 */
public class ConnectionStatusBar extends JXStatusBar {

    /** label describing the connection state */
    protected JXLabel stateLabel = new JXLabel() {{
        setFont(getFont().deriveFont(Font.BOLD));
    }};

    /** combo box which presents all the available sims on the network */
    @SuppressWarnings("unchecked")
    protected JComboBox simulationComboBox = new JComboBox() {

        {
        setEditable(true);
        setSelectedItem(new SimulationInformation("localhost", "0"));
        }

        @Override
        public void addItem(Object object) {
            SimulationInformation simulationInformation = (SimulationInformation)object;
            for (int i = 0; i < getItemCount(); ++i) {
                if (simulationInformation.compareTo((SimulationInformation)getItemAt(i)) <= 0) {
                    super.insertItemAt(simulationInformation, i);
                    return;
                }
            }
            super.addItem(object);
        }

        @Override
        public void setSelectedItem(Object object) {
            if (object instanceof SimulationInformation) {
                SimulationInformation simulationInformation =
                  (SimulationInformation)object;
                super.setSelectedItem(simulationInformation.machine + ":" +
                  simulationInformation.handshakePort);
            }
        }

    };

    /** toggles connection */
    protected JXButton connectionButton;

    /** action to be performed when the button is pressed while disconnected */
    public final Action connectAction;

    /** action to be performed when the button is pressed while connected */
    public final Action disconnectAction;

    /** action to be performed when the button is pressed while auto-connecting */
    public final Action autoConnectingAction;

    /** the connection state */
    private boolean connected = false;

    /** the auto-connecting state */
    private boolean autoConnecting = false;

    /** exception to be thrown in response to invalid host/port specification */
    protected IllegalArgumentException illegalArgumentException =
      new IllegalArgumentException("Connection specification must be of the form host:port");

    /** the thread that listens on the multicast channel for simulations */
    private SimulationSniffer simulationSniffer = new SimulationSniffer() {{
        addSimulationListener(new SimulationListener() {

            @Override
            public void simulationAdded(final SimulationInformation simulationInformation) {
                if (simulationInformation.getMajorVersionNumber() >= 10) {
                    SwingUtilities.invokeLater(new Runnable() {
                        @Override
                        @SuppressWarnings("unchecked")
                        public void run() {
                            simulationComboBox.addItem(simulationInformation);
                        }
                    });
                }
            }

            @Override
            public void simulationRemoved(final SimulationInformation simulationInformation) {
                SwingUtilities.invokeLater(new Runnable() {
                    @Override
                    public void run() {
                        simulationComboBox.removeItem(simulationInformation);
                    }
                });
            }

            @Override
            public void exceptionOccurred(Exception exception) {
                JOptionPane.showMessageDialog(ConnectionStatusBar.this, exception,
                  "Simulation List Error", JOptionPane.ERROR_MESSAGE);
            }

        });

        start();
    }};

    /** a darker green */
    protected Color darkGreen = new Color(0, 150, 0);

    /** a darker yellow */
    protected Color darkYellow = new Color(255, 165, 0);

    /**
     * constructor
     *
     * @param connectAction the action to execute when the button is pressed in
     * the disconnected state
     * @param disconnectAction the action to execute when the button is pressed
     * in the connected state
     * @param autoConnectingAction the action to execute when the button is pressed
     * in the auto-connecting state
     */
    public ConnectionStatusBar(Action connectAction, Action disconnectAction, Action autoConnectingAction) {
        this.connectAction = connectAction;
        this.disconnectAction = disconnectAction;
        this.autoConnectingAction = autoConnectingAction;

        simulationComboBox.getEditor().addActionListener(connectAction);

        putClientProperty(BasicStatusBarUI.AUTO_ADD_SEPARATOR, false);
        setBorder(new EmptyBorder(5, 5, 3, 5));

        add(stateLabel);

        add(simulationComboBox, new JXStatusBar.Constraint(
          JXStatusBar.Constraint.ResizeBehavior.FILL, new Insets(0, 5, 0, 5)));

        connectionButton = new JXButton(connectAction);
        add(connectionButton);

        updateState();
    }

    /**
     * gets the host name
     *
     * @return the host name
     */
    public String getHostName() {
        Object object = simulationComboBox.getEditor().getItem();

        if (object instanceof SimulationInformation) {
            return ((SimulationInformation)object).machine;
        }

        if (object instanceof String) {
            String string = (String)object;
            int index = string.lastIndexOf(":");
            if (index != -1) {
                return string.substring(0, index).trim();
            }
            else {
                throw illegalArgumentException;
            }
        }

        return null;
    }

    /**
     * gets the port
     *
     * @return the port
     */
    public int getPort() {
        Object object = simulationComboBox.getEditor().getItem();

        if (object instanceof SimulationInformation) {
            return Integer.parseInt(((SimulationInformation)object).handshakePort);
        }

        if (object instanceof String) {
            String string = (String)object;
            int index = string.lastIndexOf(":");
            if (index != -1) {
                return Integer.parseInt(string.substring(index + 1).trim());
            }
            else {
                throw illegalArgumentException;
            }
        }

        return -1;
    }

    /**
     * set the target simulation
     *
     * @param hostName the name of the host machine
     * @param port the port of the host machine
     */
    public void setTarget(String hostName, int port) {
        simulationComboBox.setSelectedItem(new SimulationInformation(
          hostName, Integer.toString(port)));
    }

    /**
     * returns the connection state
     *
     * @return the connection state
     */
    public boolean getConnectionState() {
        return connected;
    }

    /**
     * sets the connection state, updating the display and action
     * enabled states appropriately
     *
     * @param connected the connection state
     */
    public void setConnectionState(boolean connected) {
        if (this.connected != connected) {
            this.connected = connected;
            updateState();
        }
    }

    /**
     * updates the display and action enabled states based on the connection state
     */
    protected void updateState() {
        simulationSniffer.setPaused(connected);
        connectionButton.setAction(connected ? disconnectAction : connectAction);
        simulationComboBox.setEnabled(!connected);
        connectAction.setEnabled(!connected);
        disconnectAction.setEnabled(connected);
        if (connected) {
            stateLabel.setText("Connected To: ");
            stateLabel.setForeground(darkGreen);
        }
        else {
            stateLabel.setText("Not Connected ");
            stateLabel.setForeground(Color.red);
        }
    }

    /**
     * continuously attempt to connect to the target simulation
     */
    public void autoConnect() {
        if (!autoConnecting) {
            connected = false;
            simulationSniffer.setPaused(false);
            connectionButton.setAction(autoConnectingAction);
            simulationComboBox.setEnabled(false);
            connectAction.setEnabled(false);
            disconnectAction.setEnabled(false);
            stateLabel.setText("Searching For: ");
            stateLabel.setForeground(darkYellow);
            autoConnecting = true;

            final String host = getHostName();
            final int port = getPort();

            new Thread() {
                @Override
                public void run() {
                    while(autoConnecting) {
                        try {
                            sleep(1000);
                        }
                        catch (InterruptedException interruptedException) {}

                        for (SimulationInformation simulationInformation :
                          simulationSniffer.getSimulationInformation()) {
                            if (simulationInformation.machine.equals(host) &&
                              simulationInformation.noHandshakePort.equals(Integer.toString(port)) &&
                              simulationInformation.getMajorVersionNumber() >= 10) {
                                SwingUtilities.invokeLater(new Runnable() {
                                    @Override
                                    public void run() {
                                        if (autoConnecting) {
                                            autoConnecting = false;
                                            connectAction.actionPerformed(null);
                                        }
                                    }
                                });
                                return;
                            }
                        }
                    }
                }
            }.start();
        }
    }

    /**
     * stop continuously attempting to connect to the target simulation, if active
     */
    public void cancelAutoConnect() {
        if (autoConnecting) {
            autoConnecting = false;
            updateState();
        }
    }

    /**
     * returns whether or not this instance is currently attempting to
     * automatically connect to the target simulation
     *
     * @return <code>true</code> if auto-connect is currently in progress
     */
    public boolean isAutoConnecting() {
        return autoConnecting;
    }

}
