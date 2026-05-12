package trick.sniffer;

import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import javax.swing.AbstractAction;
import javax.swing.JComponent;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPopupMenu;
import javax.swing.JScrollPane;
import javax.swing.JSeparator;
import javax.swing.ListSelectionModel;
import javax.swing.SwingUtilities;
import javax.swing.table.DefaultTableModel;
import org.jdesktop.application.Application;
import org.jdesktop.application.View;
import org.jdesktop.swingx.JXTable;
import org.jdesktop.swingx.decorator.EnabledHighlighter;
import org.jdesktop.swingx.decorator.HighlighterFactory;
import org.jdesktop.swingx.decorator.PatternPredicate;
import trick.common.TrickApplication;

/**
 * SimSniffer displays the currently running simulations. It constantly monitors the multicast
 * channel over which all sims broadcast their existance. The user can choose to launch Sim Control
 * Panel or Trick View and connect to any Trick sim.
 *
 * @author Derek Bankieris
 */
public class SimSnifferApplication extends TrickApplication {

    /** the thread that listens for the sims */
    SimulationSniffer simulationSniffer;

    /** displays simulation information */
    JXTable simTable;

    /** active simulations */
    ArrayList<SimulationInformation> simulations = new ArrayList<SimulationInformation>();

    /** launches the Simulation Control Panel */
    AbstractAction controlPanelAction = new AbstractAction("Launch Sim Control Panel") {
        {
            putValue(SHORT_DESCRIPTION, "Launch the Sim Control Panel and connect it to the selected simulation.");
            putValue(MNEMONIC_KEY, KeyEvent.VK_C);
        }

        public void actionPerformed(ActionEvent actionEvent) {
            int row = simTable.getSelectedRow();
            if (row != -1) {
                SimulationInformation simulationInformation = simulations.get(simTable.convertRowIndexToModel(row));
                launchTrickApplication("simcontrol", simulationInformation.machine + " " + simulationInformation.noHandshakePort);
            }
        }
    };

    /** launches Trick View */
    AbstractAction trickViewAction = new AbstractAction("Launch Trick View") {
        {
            putValue(SHORT_DESCRIPTION, "Launch Trick View and connect it to the selected simulation.");
            putValue(MNEMONIC_KEY, KeyEvent.VK_V);
        }

        public void actionPerformed(ActionEvent actionEvent) {
            int row = simTable.getSelectedRow();
            if (row != -1) {
                SimulationInformation simulationInformation = simulations.get(simTable.convertRowIndexToModel(row));
                launchTrickApplication("tv", "--host " + simulationInformation.machine + " --port " + simulationInformation.noHandshakePort);
            }
        }
    };

    /** launches Event/Malfunction Trick View */
    AbstractAction malfunctionTrickViewAction = new AbstractAction("Launch Event/Malfunction Trick View") {
        {
            putValue(SHORT_DESCRIPTION, "Launch Event/Malfunction Trick View and connect it to the selected simulation.");
            putValue(MNEMONIC_KEY, KeyEvent.VK_E);
        }

        public void actionPerformed(ActionEvent actionEvent) {
            int row = simTable.getSelectedRow();
            if (row != -1) {
                SimulationInformation simulationInformation = simulations.get(simTable.convertRowIndexToModel(row));
                launchTrickApplication("mtv", simulationInformation.machine + " " + simulationInformation.noHandshakePort);
            }
        }
    };

    /** launches Monte Monitor */
    AbstractAction monteMonitorAction = new AbstractAction("Launch Monte Monitor") {
        {
            putValue(SHORT_DESCRIPTION, "Launch Monte Monitor and connect it to the selected simulation.");
            putValue(MNEMONIC_KEY, KeyEvent.VK_M);
        }

        public void actionPerformed(ActionEvent actionEvent) {
            int row = simTable.getSelectedRow();
            if (row != -1) {
                SimulationInformation simulationInformation = simulations.get(simTable.convertRowIndexToModel(row));
                launchTrickApplication("mm", " --host " + simulationInformation.machine + " --port " + simulationInformation.noHandshakePort);
            }
        }
    };

    @Override
    protected void initialize(String[] args) {
        super.initialize(args);

        // Disable actions.
        controlPanelAction.setEnabled(false);
        trickViewAction.setEnabled(false);
        malfunctionTrickViewAction.setEnabled(false);
        monteMonitorAction.setEnabled(false);
    }

    @Override
    protected void startup() {
        super.startup();

        // Build the GUI.
        View view = getMainView();
        view.setComponent(createMainPanel());
        view.setMenuBar(createMenuBar());

        // Display the GUI.
        show(view);
    }

    @Override
    protected void ready() {
        simulationSniffer = new SimulationSniffer() {
            {
                addSimulationListener(
                    new SimulationListener() {
                        @Override
                        public void simulationAdded(final SimulationInformation simulationInformation) {
                            SwingUtilities.invokeLater(
                                new Runnable() {
                                    @Override
                                    public void run() {
                                        int row = SimSnifferApplication.this.simulations.size();
                                        SimSnifferApplication.this.simulations.add(simulationInformation);
                                        ((DefaultTableModel) simTable.getModel()).fireTableRowsInserted(row, row);
                                    }
                                }
                            );
                        }

                        @Override
                        public void simulationRemoved(final SimulationInformation simulationInformation) {
                            SwingUtilities.invokeLater(
                                new Runnable() {
                                    @Override
                                    public void run() {
                                        int row = SimSnifferApplication.this.simulations.indexOf(simulationInformation);
                                        if (row != -1) {
                                            SimSnifferApplication.this.simulations.remove(row);
                                            ((DefaultTableModel) simTable.getModel()).fireTableRowsDeleted(row, row);
                                        }
                                    }
                                }
                            );
                        }

                        @Override
                        public void exceptionOccurred(Exception exception) {
                            JOptionPane.showMessageDialog(getMainFrame(), exception, "Simulation List Error", JOptionPane.ERROR_MESSAGE);
                            exit();
                        }
                    }
                );

                start();
            }
        };
    }

    @Override
    protected JComponent createMainPanel() {
        simTable = new JXTable(
            new DefaultTableModel() {
                final String[] columnNames = {
                    "Machine",
                    "Port",
                    "User",
                    "PID",
                    "Version",
                    "S_main File",
                    "Sim Directory",
                    "Run Directory",
                    "Tag",
                    "Variable Server",
                    "Exec Mode",
                };

                @Override
                public int getRowCount() {
                    return simulations.size();
                }

                @Override
                public int getColumnCount() {
                    return columnNames.length;
                }

                @Override
                public String getColumnName(int column) {
                    return columnNames[column];
                }

                @Override
                public Class<?> getColumnClass(int column) {
                    return String.class;
                }

                @Override
                public Object getValueAt(int row, int column) {
                    SimulationInformation sim = simulations.get(row);
                    switch (column) {
                        case 0:
                            return sim.machine;
                        case 1:
                            return sim.noHandshakePort;
                        case 2:
                            return sim.user;
                        case 3:
                            return sim.processID;
                        case 4:
                            return sim.version;
                        case 5:
                            return sim.sMainFile;
                        case 6:
                            return sim.simDirectory;
                        case 7:
                            return sim.runDirectory;
                        case 8:
                            return sim.tag;
                        case 9:
                            if ("1".equals(sim.vsEnabled)) return "Yes";
                            if ("0".equals(sim.vsEnabled)) return "No";
                            return sim.vsEnabled;
                        default:
                            switch (sim.execMode) {
                                case "0":
                                    return "Initialization";
                                case "1":
                                    return "Freeze";
                                case "4":
                                    return "Step";
                                case "5":
                                    return "Run";
                                case "6":
                                    return "Exit";
                                default:
                                    return sim.execMode;
                            }
                    }
                }
            }
        ) {
            {
                getColumnExt(0).setPrototypeValue("XXXXXXXXXX");
                getColumnExt(1).setPrototypeValue("000000");
                getColumnExt(2).setPrototypeValue("XXXXXXXXXX");
                getColumnExt(3).setPrototypeValue("0000000000");
                getColumnExt(4).setPrototypeValue("XXXXXXXXXXXXXXX");
                getColumnExt(5).setPrototypeValue("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
                getColumnExt(6).setPrototypeValue("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" + "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
                getColumnExt(7).setPrototypeValue("XXXXXXXXXXXXXXXXXXXXXXXXX");
                getColumnExt(8).setPrototypeValue("XXXXX");
                getColumnExt(9).setPrototypeValue("XXXXXXX");
                getColumnExt(10).setPrototypeValue("XXXXXXXXXXXXXX");
                initializeColumnWidths();

                // Add a popup menu.
                setComponentPopupMenu(
                    new JPopupMenu() {
                        {
                            add(controlPanelAction);
                            add(trickViewAction);
                            add(malfunctionTrickViewAction);
                            add(monteMonitorAction);
                        }
                    }
                );

                setName("simTable");
                setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
                setColumnControlVisible(true);

                // Provide alternate row coloring.
                setHighlighters(HighlighterFactory.createSimpleStriping());
            }

            @Override
            public void changeSelection(int row, int column, boolean toggle, boolean extend) {
                super.changeSelection(row, column, toggle, extend);
                boolean enable = row != -1;
                controlPanelAction.setEnabled(enable);
                trickViewAction.setEnabled(enable);
                malfunctionTrickViewAction.setEnabled(enable);
                monteMonitorAction.setEnabled(enable);
            }

            @Override
            public boolean isCellEditable(int row, int column) {
                return false;
            }
        };

        return new JScrollPane(simTable) {
            {
                setName("scrollPane");
            }
        };
    }

    @Override
    protected JMenuBar createMenuBar() {
        JMenuBar menuBar = super.createMenuBar();
        JMenu menu = menuBar.getMenu(0);
        menu.add(new JSeparator(), 0);
        menu.add(new JMenuItem(monteMonitorAction), 0);
        menu.add(new JMenuItem(malfunctionTrickViewAction), 0);
        menu.add(new JMenuItem(trickViewAction), 0);
        menu.add(new JMenuItem(controlPanelAction), 0);
        return menuBar;
    }

    /**
     * main method for this application
     * @param args command line arguments
     */
    public static void main(String[] args) {
        Application.launch(SimSnifferApplication.class, args);
    }
}
