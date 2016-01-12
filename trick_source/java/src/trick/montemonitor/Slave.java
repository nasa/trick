//========================================
//    Package
//========================================
package trick.montemonitor;

//========================================
//    Imports
//========================================
import javax.swing.ImageIcon;

/**
 * encapsulates the data necessary to represent a simulation
 */
public class Slave {

    /** operational states */
    public enum State {

        UNINITIALIZED("Uninitialized", new ImageIcon(Slave.class.getResource("resources/uninitialized.png"))),
        INITIALIZING("Initializing", new ImageIcon(Slave.class.getResource("resources/initializing.png"))),
        READY("Ready", new ImageIcon(Slave.class.getResource("resources/ready.png"))),
        RUNNING("Running", new ImageIcon(Slave.class.getResource("resources/running.png"))),
        STOPPING("Stopping", new ImageIcon(Slave.class.getResource("resources/stopping.png"))),
        STOPPED("Stopped", new ImageIcon(Slave.class.getResource("resources/stopped.png"))),
        FINISHED("Finished", new ImageIcon(Slave.class.getResource("resources/finished.png"))),
        UNRESPONSIVE_RUNNING("Unresponsive - Running", new ImageIcon(Slave.class.getResource("resources/unresponsive-running.png"))),
        UNRESPONSIVE_STOPPING("Unresponsive - Stopping", new ImageIcon(Slave.class.getResource("resources/unresponsive-stopping.png"))),
        DISCONNECTED("Disconnected", new ImageIcon(Slave.class.getResource("resources/disconnected.png"))),
        UNKNOWN("Unknown", new ImageIcon(Slave.class.getResource("resources/unknown.png")));

        private String name;
        public ImageIcon icon;

        State(String name, ImageIcon icon) {
            this.name = name;
            this.icon = icon;
        }

        @Override
        public String toString() {
            return name;
        }

    };

    /** unique identifier */
    public String id;

    /** used to detect removed slaves */
    public boolean valid = true;

    /** current state */
    public State state;

    /** machine name */
    public String name;

    /** current run number */
    public Integer currentRun = new Integer(-1);

    /** number or results returned */
    public Integer numResults = new Integer(0);

    /**
     * constructs a new Slave whose initial state is UNKNOWN
     * @param id slave id
     */
    public Slave(String id) {
        this.id = id;
        name = "Unknown";
        state = State.UNKNOWN;
    }

}
