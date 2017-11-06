package trick.sniffer;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.MulticastSocket;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.List;
import java.util.Vector;

import javax.swing.Timer;

/**
 * SimulationSniffer provides registered listeners with a list of currently
 * running simulations. It constantly monitors the multicast channel over which
 * all sims broadcast their existance.
 *
 * @author Derek Bankieris
 */
public class SimulationSniffer extends Thread {

    /** control variable for pausing the thread */
    volatile boolean paused = false;

    /** listens for active sims */
    MulticastSocket multicastSocket;

    /** active sims */
    Vector<SimulationEntry> simulations = new Vector<SimulationEntry>();

    /** registered listeners */
    Vector<SimulationListener> simulationListeners = new Vector<SimulationListener>();

    /**
     * sets the execution state of this thread.
     * Pausing puts this thread to sleep.
     *
     * @param paused the execution state of this thread
     */
    public void setPaused(boolean paused) {
        if (this.paused != paused) {
            this.paused = paused;
            if (paused) {
                for (SimulationEntry simulationEntry : simulations) {
                    simulationEntry.timer.stop();
                }
            }
            else {
                for (SimulationEntry simulationEntry : simulations) {
                    simulationEntry.timer.start();
                }
                synchronized(this) {
                    notify();
                }
            }
        }
    }

    @Override
    public void run() {
        try {
            multicastSocket = new MulticastSocket(9265) {{
                joinGroup(InetAddress.getByName("239.3.14.15"));
            }};
            byte[] buffer = new byte[4096];
            DatagramPacket packet = new DatagramPacket(buffer, buffer.length);

            while (true) {
                while (paused) {
                    synchronized(this) {
                        try {
                            wait();
                        }
                        catch (InterruptedException interruptedException) {}
                    }
                }

                multicastSocket.receive(packet);

                // Remove the trailing newline, and split the tab-delimitted message.
                String[] info = new String(packet.getData(), packet.getOffset(),
                  packet.getLength()).replace("\n", "").split("\t");

                // Reset the packet length or future messages will be clipped.
                packet.setLength(buffer.length);

                SimulationEntry simulationEntry = new SimulationEntry(new SimulationInformation(
                  info[0], info[1], info[2], info[3], info[4], info[5], info[6], info[7], info[8],
                  info.length > 9 ? info[9] : ""));

                // If the sim is already on the list, restart its timer; otherwise, add it.
                int index = simulations.indexOf(simulationEntry);
                if (index == -1) {
                    simulations.add(simulationEntry);
                    simulationEntry.timer.start();
                    for (SimulationListener simulationListener : simulationListeners) {
                        simulationListener.simulationAdded(simulationEntry.simulationInformation);
                    }
                }
                else {
                    simulations.get(index).timer.restart();
                }
            }
        }
        catch (UnknownHostException unknownHostException) {
            for (SimulationListener simulationListener : simulationListeners) {
                simulationListener.exceptionOccurred(unknownHostException);
            }
        }
        catch (IOException ioException) {
            for (SimulationListener simulationListener : simulationListeners) {
                simulationListener.exceptionOccurred(ioException);
            }
        }
    }

    /**
     * indicates that <code>simulationListener</code> should be notified of
     * simulation list changes
     *
     * @param simulationListener the listener to inform
     */
    public void addSimulationListener(SimulationListener simulationListener) {
        if (!simulationListeners.contains(simulationListener)) {
            simulationListeners.add(simulationListener);
        }
    }

    /**
     * indicates that <code>simulationListener</code> should no longer be
     * notified of simulation list changes
     *
     * @param simulationListener the listener to remove
     */
    public void removeSimulationListener(SimulationListener simulationListener) {
        simulationListeners.remove(simulationListener);
    }

    /**
     * returns a list of all simulations on the network
     *
     * @return a list of all available simulations
     */
    public List<SimulationInformation> getSimulationInformation() {
        return new ArrayList<SimulationInformation>(simulations.size()) {{
            for (SimulationEntry simulationEntry : simulations) {
                add(simulationEntry.simulationInformation);
            }
        }};
    }

    /**
     * pairs a SimulationInformation with a Timer that removes it from the list
     * of active sims after five seconds
     */
    class SimulationEntry {

        /** simulation */
        public final SimulationInformation simulationInformation;

        /**
         * After five seconds, remove the simulation from the list unless it
         * reannounces itself over the multicast channel.
         */
        public final Timer timer = new Timer(5000, new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                simulations.remove(SimulationEntry.this);
                for (SimulationListener simulationListener : simulationListeners) {
                    simulationListener.simulationRemoved(simulationInformation);
                }
            }
        }) {{
            setRepeats(false);
        }};

        /**
         * constructor
         *
         * @param simulationInformation the simulation this entry is tracking
         */
        public SimulationEntry(SimulationInformation simulationInformation) {
            this.simulationInformation = simulationInformation;
        }

        @Override
        public boolean equals(Object object) {
            if (object == this) {
                return true;
            }

            if (!(object instanceof SimulationEntry)) {
                return false;
            }

            return simulationInformation.equals(((SimulationEntry)object).simulationInformation);
        }

    }

}
