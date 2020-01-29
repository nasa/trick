package trick.sniffer;

import java.io.File;
import java.net.InetAddress;
import java.net.UnknownHostException;

/**
 * encapsulates the data necessary to represent a simulation
 *
 * @author Derek Bankieris
 */
public class SimulationInformation implements Comparable<SimulationInformation> {

    /** name of the machine on which the simulation is running */
    public final String machine;

    /**
     * port of the machine on which the simulation is running. This port
     * requires a handshaking protocol for simulations earlier than Trick 10.
     */
    public final String handshakePort;

    /**
     * port of the machine on which the simulation is running. This port
     * requires no handshaking protocol.
     */
    public final String noHandshakePort;

    /** user running the simulation */
    public final String user;

    /** process ID of the simulation */
    public final String processID;

    /** simulation version */
    public final String version;

    /** simulation S_main* file */
    public final String sMainFile;

    /** SIM_* directory */
    public final String simDirectory;

    /** RUN_* directory */
    public final String runDirectory;

    /** user tag */
    public final String tag;

    /** constructor 
     * @param machine the machine the simulation is running on
     * @param port the port the simulation is listening on
     */
    public SimulationInformation(String machine, String port) {
        this(machine, port, "", "", "", "", "", "", "", "");
    }

    /** constructor
     * @param machine the machine the simulation is running on
     * @param handshakePort the port the simulation is listening on
     * @param user the simulation owner
     * @param processID the process ID of the sim
     * @param simDirectory the directory the simulation is running
     * @param sMainFile name of the sim executable
     * @param runDirectory name of the run directory
     * @param version Trick version of sim
     * @param tag user defined tag of sim
     * @param noHandshakePort port number of the no handshake port
     */
    public SimulationInformation(String machine, String handshakePort,
      String user, String processID, String simDirectory, String sMainFile,
      String runDirectory, String version, String tag,
      String noHandshakePort) {
        this.machine = machine;
        this.handshakePort = handshakePort;
        this.noHandshakePort = noHandshakePort;
        this.user = user;
        this.processID = processID;
        this.version = version;
        this.sMainFile = sMainFile;
        this.simDirectory = simDirectory;
        this.runDirectory = runDirectory;
        this.tag = tag;
    }

    /**
     * returns the version up to but not including the first dot
     *
     * @return the major version number
     */
    public int getMajorVersionNumber() {
        int index = version.indexOf(".");
        if (index != -1) {
            return Integer.parseInt(version.substring(0, index));
        }
        return Integer.parseInt(version);
    }

    @Override
    public int compareTo(SimulationInformation simulationInformation) {
        String localHost;
        try {
            localHost = InetAddress.getLocalHost().getHostName();
        }
        catch (UnknownHostException unknownHostException) {
            localHost = "localhost";
        }
        boolean iAmLocal = machine.equalsIgnoreCase(localHost);
        boolean compareToIsLocal = simulationInformation.machine.equalsIgnoreCase(localHost);
        if (!iAmLocal && !compareToIsLocal) {
            int nameComparison = machine.compareToIgnoreCase(simulationInformation.machine);
            if (nameComparison == 0) {
                return handshakePort.compareToIgnoreCase(simulationInformation.handshakePort);
            }
            return nameComparison;
        }
        if (iAmLocal && !compareToIsLocal) {
            return -1;
        }
        if (compareToIsLocal && !iAmLocal) {
            return 1;
        }
        return handshakePort.compareToIgnoreCase(simulationInformation.handshakePort);
    }

    @Override
    public boolean equals(Object object) {
        if (object == this) {
            return true;
        }

        if (!(object instanceof SimulationInformation)) {
            return false;
        }

        SimulationInformation simulationInformation = (SimulationInformation)object;

        return  machine.equals(simulationInformation.machine) &&
          handshakePort.equals(simulationInformation.handshakePort) &&
          noHandshakePort.equals(simulationInformation.noHandshakePort) &&
          user.equals(simulationInformation.user) &&
          processID.equals(simulationInformation.processID) &&
          version.equals(simulationInformation.version) &&
          sMainFile.equals(simulationInformation.sMainFile) &&
          simDirectory.equals(simulationInformation.simDirectory) &&
          runDirectory.equals(simulationInformation.runDirectory) &&
          tag.equals(simulationInformation.tag);
    }

    @Override
    public String toString() {
        String result = machine + ":" + handshakePort + "   ";

        int index = simDirectory.lastIndexOf(File.separator);
        if (index != -1) {
            result += simDirectory.substring(index + 1);
        }
        else {
            result += simDirectory;
        }

        if (!simDirectory.isEmpty() && !runDirectory.isEmpty()) {
            result += File.separator + runDirectory;
        }

        return result.trim();
    }

}
