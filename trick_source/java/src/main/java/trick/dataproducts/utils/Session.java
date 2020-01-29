
//========================================
//  Package
//========================================
package trick.dataproducts.utils;

//========================================
//  Imports
//========================================
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

/**
 * Defines Session data for Trick DP GUI.
 * The session data would be mapping to the session XML file.
 *
 *
 * @since Trick 10
 */
public class Session {
    //========================================
    //  Public data
    //========================================
    public final static int PLOT_MODE  = 0;
    public final static int TABLE_MODE = 1;
    public final static String[] MODE_OPTIONS = {"Plot", "Table"};

    public final static int SIMPLE_PRESENTATION        = 0;
    public final static int COMPARISON_PRESENTATION    = 1;
    public final static int DELTA_PRESENTATION        = 2;
    public final static int CONTRAST_PRESENTATION    = 3;
    public final static String[] PRESENTATION_OPTIONS = {"Simple", "Comparison", "Delta", "Contrast"};

    public final static int TERMINAL_DEVICE           = 0;
    public final static int PRINTER_DEVICE        = 1;
    public final static int FILE_DEVICE               = 2;
    public final static String[] DEVICE_OPTIONS = {"Terminal", "Printer", "File"};

    public final static int X11_GNUPLOT_TERMINAL        = 0;
    public final static int PS_COLOR_GNUPLOT_TERMINAL    = 1;
    public final static int PS_BW_GNUPLOT_TERMINAL        = 2;
    public final static int PNG_GNUPLOT_TERMINAL        = 3;
    public final static int EPS_GNUPLOT_TERMINAL        = 4;
    public final static int AQUA_GNUPLOT_TERMINAL        = 5;
    public final static String[] GNUPLOT_TERMINAL_OPTIONS = {"X11", "PS-Color", "PS-BW", "PNG", "EPS", "AQUA"};

    //========================================
    //  Protected data
    //========================================


    //========================================
    //  Private Data
    //========================================
    private String version;
    private String presentation;
    private String machine;
    // In Session XML, the device can be [Terminal|Printer|File:<deviceFileName]
    private String device;
    private String deviceFileName;
    private String mode;
    private String gnuplotTerminal;
    private int port;
    private double tstart;
    private double tstop;
    private double frequency;

    private List<SessionRun> runs;
    private List<String> productFiles;

    //========================================
    //  Constructors
    //========================================
    /**
     * Constructor with specified version and presentation information for the session.
     *
     * @param version        The session version.
     * @param presentation    The format of presentation.
     */
    public Session(String version, String presentation) {
        this.version = version;
        this.presentation = presentation;
        runs = new ArrayList<SessionRun>();
        productFiles = new ArrayList<String>();

    }

    //========================================
    //  Set/Get methods
    //========================================
    /**
     * Sets the machine name for the session.
     *
     * @param machine    The machine name running the session.
     */
    public void setMachine(String machine) {
    this.machine = machine;
    }

    /**
     * Sets the port number for the session.
     *
     * @param port        The port number of the machine.
     */
    public void setPort(int port) {
    this.port = port;
    }

    /**
     * Sets the device for the session.
     *
     * <ul>
     * <li>Terminal
     * <li>Printer
     * <li>File
     * </ul>
     *
     * @param device    The device name.
     */
    public void setDevice(String device) {
        this.device = device;
    }

    /**
     * Sets the mode.
     *
     * <ul>
     * <li>Plot
     * <li>Table
     * </ul>
     * TODO: Assume it's Plot, Table, it seems we need it for gnuplot.
     *       Make sure it's right.
     *
     * @param mode        The mode.
     */
    public void setMode(String mode) {
        this.mode = mode;
    }


    /**
     * Sets the terminal for Gnuplot.
     *
     * <ul>
     * <li>X11
     * <li>PS-Color
     * <li>PS-BW
     * <li>PNG
     * <li>EPS
     * <li>AQUA
     * </ul>
     *
     * @param terminal    The terminal for Gnuplot.
     */
    public void setGnuplotTerminal(String terminal) {
        gnuplotTerminal = terminal;
    }

    /**
     * Sets the start time for the session.
     *
     * @param tstart    The start time.
     */
    public void setTStart(double tstart) {
        this.tstart = tstart;
    }

    /**
     * Sets the stop time for the session.
     *
     * @param tstop        The stop time.
     */
    public void setTStop(double tstop) {
        this.tstop = tstop;
    }

    /**
     * Sets the frequency for the session.
     *
     * @param frequency    The frequency.
     */
    public void setFrequency(double frequency) {
        this.frequency = frequency;
    }

    /**
     * Sets the list of data product files for the session.
     *
     * @param files    A {@link List} of data product file names.
     */
    public void setProductFiles(List<String> files) {
        Iterator<String> it = files.iterator();
        while(it.hasNext()) {
            productFiles.add(it.next().toString());
        }
    }

    /**
     * Gets the device file name.
     *
     * @return The file name for the file device.
     *
     */
    public String getDeviceFileName() {
        return deviceFileName;
    }

    /**
     * Sets the device file name if the device if File.
     *
     * @param deviceFileName device name string
     *
     * @see #DEVICE_OPTIONS
     *
     */
    public void setDeviceFileName(String deviceFileName) {
        this.deviceFileName = deviceFileName;
    }

    /**
     * Sets the version.
     *
     * @param version    The specified version.
     */
    public void setVersion(String version) {
        this.version = version;
    }

    /**
     * Gets the session version.
     *
     * @return The session version.
     */
    public String getVersion() {
        return version;
    }

    /**
     * Gets the session presentation format.
     *
     * @return The session presentation format.
     */
    public String getPresentation() {
        if (presentation == null || presentation.isEmpty()) {
            presentation = Session.PRESENTATION_OPTIONS[Session.SIMPLE_PRESENTATION];
        }
        return presentation;
    }

    /**
     * Gets the machine name.
     *
     * @return The machine name.
     */
    public String getMachine() {
        return machine;
    }

    /**
     * Gets the port number.
     *
     * @return The port number.
     */
    public int getPort() {
        return port;
    }

    /**
     * Gets the device name.
     *
     * @return    The device name.
     */
    public String getDevice() {
        return device;
    }

    /**
     * Gets the mode.
     *
     * @return The mode.
     */
    public String getMode() {
        return mode;
    }

    /**
     * Gets the terminal for Gnuplot.
     *
     * @return The terminal.
     */
    public String getGnuplotTerminal() {
        return gnuplotTerminal;
    }

    /**
     * Gets the start time.
     *
     * @return The start time.
     */
    public double getTStart() {
        return tstart;
    }

    /**
     * Gets the stop time.
     *
     * @return The stop time.
     */
    public double getTStop() {
        return tstop;
    }

    /**
     * Gets the frequency.
     *
     * @return The frequency.
     */
    public double getFrequency() {
        return frequency;
    }

    /**
     * Gets the list of runs from the session.
     *
     * @return A {@link List} of runs from the session.
     */
    public List<SessionRun> getRuns() {
        return runs;
    }

    /**
     * Gets the list of data product files from the session.
     *
     * @return A {@link List} of data product files from the session.
     */
    public List<String> getProductFiles() {
        return productFiles;
    }


    //========================================
    //  Methods
    //========================================
    /**
     * Adds a {@link SessionRun} to the current session.
     *
     * @param sr    An instance of {@link SessionRun}.
     */
    public void addRun(SessionRun sr) {
        runs.add(sr);
    }

    /**
     * Adds a data product file to the current session.
     *
     * @param f        A product file that needs adding.
     */
    public void addProductFile(String f) {
        productFiles.add(f);
    }

    /**
     * Prints the session data.
     */
    public void printData() {
        System.out.println("version:\t" + version);
        System.out.println("presentation:\t" + presentation);
        System.out.println("machine:\t" + machine);
        System.out.println("port:\t\t" + port);
        System.out.println("tstart:\t\t" + tstart);
        System.out.println("tstop:\t\t" + tstop);
        System.out.println("frequency:\t" + frequency);
        System.out.println("===runs===");
        for (int i = 0; i < runs.size(); i++) {
            (runs.get(i)).printData();
        }
        System.out.println("===product files===");
        for (int i = 0; i < productFiles.size(); i++){
            System.out.println("\t" + productFiles.get(i).toString());
        }

    }
}
