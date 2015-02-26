/*
 * $Id: StripchartApplication.java 3519 2014-05-12 18:36:01Z dstrauss $
 */

//========================================
//    Package
//========================================
package trick.stripchart;

//========================================
//Imports
//========================================
import java.awt.Color;
import java.awt.Component;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.Paint;
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.image.BufferedImage;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.UnknownHostException;
import java.util.Arrays;
import java.util.List;
import java.util.Vector;
import java.util.regex.Pattern;

import javax.imageio.ImageIO;
import javax.print.Doc;
import javax.print.DocFlavor;
import javax.print.DocPrintJob;
import javax.print.PrintException;
import javax.print.SimpleDoc;
import javax.print.StreamPrintService;
import javax.print.StreamPrintServiceFactory;
import javax.print.attribute.HashPrintRequestAttributeSet;
import javax.print.attribute.PrintRequestAttributeSet;
import javax.swing.AbstractButton;
import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.JFileChooser;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSeparator;
import javax.swing.SwingWorker;
import javax.swing.Timer;

import org.jdesktop.application.Action;
import org.jdesktop.application.Application;
import org.jdesktop.application.View;
import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.ChartUtilities;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.axis.ValueAxis;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.title.LegendTitle;
import org.jfree.data.xy.XYSeries;
import org.jfree.data.xy.XYSeriesCollection;

import trick.common.TrickApplication;
import trick.common.utils.VariableServerConnection;
import trick.simcontrol.utils.SimState;


/**
 * The Stripchart application class which is a sub class of {@link TrickApplication}.
 *
 * @since Trick 10
 * @author Warwick C. Woodard
 *
 */
public class StripchartApplication extends TrickApplication {

    //========================================
    //    Public data
    //========================================


    //========================================
    //    Protected data
    //========================================
    /** Top level layout manager */
    protected JPanel TopComponent;
    /** Panel for displaying a JFreeChart object (stripchart). */
    protected ChartPanel chartPanel;
    /** Button used to toggle display view between all stripchart data or just a fixed range of data. */
    protected JButton xRangeButton;
    /** Button used to activate/deactivate stripchart graphic updates. */
    protected JButton startStopButton;
    /** Button used to hide/unhide the stripchart's legend box. */
    protected JButton legendButton;
    /** Button used to save the graph to a postscript file. */
    protected JButton psButton;
    /** X/Y graphical plot (stripchart). */
    protected JFreeChart chart;
    /** A collection of Y variables (series) and their X/Y points. */
    protected XYSeriesCollection dataset = new XYSeriesCollection();
    /** A sequence of (x, y) data items for a single Y variable. */
    protected Vector<XYSeries> series = new Vector<XYSeries>();
    /** Backup copy of the legend used for restoring if the original legend is deleted/hidden. */
    protected LegendTitle myLegend;
    /** Used to change graph's colors, background, orientation, and other properties. */
    protected XYPlot plot;
    /** Range and tick mark properties for the X-axis. */
    protected ValueAxis xAxis;
    /** Range and tick mark properties for the Y-axis. */
    protected ValueAxis yAxis;
    /** A mechanism for the user to choose a filename when saving the window to postscript format. */
    protected JFileChooser fileChooser = new JFileChooser();


    //========================================
    //    Private Data
    //========================================
    /** Every X/Y variable name and optional --label, --units tags for each. */
    private String vars_in;
    /** IP addr or machine name of the master simulation. */
    private String host;
    /** In order to start the GUI it is necessary to know what variable server port number to connect to. */
    private int port;
    /** Filename taken from the simulation input command 'trick_vs.stripchart.set_input_file("FILENAME").' */
    private String infile;
    /** Path of the current simulation's executable.  */
    private String default_dir = System.getProperty("user.dir");
    /** Relative directory name that holds the current simulation's input file */
    private String run_dir = System.getProperty("user.dir");
    /** Default path to save graphic postscript files. */
    private String ps_dir = System.getProperty("user.dir");
    /** Geometry spec for the window */
    private String geom = "";
    /** Text to place above stripchart */
    private String title = " ";
    /** User input for the lower bound for the X-axis range. */
    private String x_min = "";
    /** User input for the upper bound for the X-axis range. */
    private String x_max = "";
    /** User input for the lower bound for the Y-axis range. */
    private String y_min = "";
    /** User input for the upper bound for the Y-axis range. */
    private String y_max = "";
    /** Fixed auto range length for the X-axis. */
    private double autorange;
    /** The lower bound for the X-axis range. */
    private double X_LOWER_BOUND = 0.0;
    /** The upper bound for the X-axis range. */
    private double X_UPPER_BOUND = 1.0;
    /** The lower bound for the Y-axis range. */
    private double Y_LOWER_BOUND = 0.0;
    /** The upper bound for the Y-axis range. */
    private double Y_UPPER_BOUND = 1.0;
    /** 0|1 flag; will cause the stripchart to exit when the sim exits. */
    private int shutdown;
    /** Used to assign a different line color for each Y variable. */
    private static Vector<Paint> color = new Vector<Paint>();
    /** X variable element name that is added to the variable server for future value retrieval.  */
    private String x_var_name;
    /** X variable's unit specification during value retrieval. */
    private String x_var_unit;
    /** X variable's alternate name/label to display on the stripchart graph.  */
    private String x_var_label;
    /** X multiplier used to divide "time_tics" into seconds. */
    private double XMultiplier = 1.0;
    /** Y variable element name that is added to the variable server for future value retrieval. */
    private Vector<String> y_vars_name = new Vector<String>();
    /** Y variable's unit specification during value retrieval. */
    private Vector<String> y_vars_unit = new Vector<String>();
    /** Y variable's alternate name/label to display on the stripchart graph. */
    private Vector<String> y_vars_label = new Vector<String>();
    /** Y multiplier used to divide "time_tics" into seconds. */
    private Vector<Double> y_vars_multiplier = new Vector<Double>();
    /** X variable value retrieved from the variable server. */
    private Vector<String> xv = new Vector<String>();
    /** Y variable(s) value retrieved from the variable server. */
    private Vector<String> yv = new Vector<String>();
    /** Notify stripchart graphics to start/continue updating or not. */
    private boolean start = true;
    /** Run stripchart in demonstration/random data sample mode, and don't connect to a simulation. */
    private boolean demo = false;
    /** Count of the total number of Y variables to plot. */
    private int num_curves = 1;
    /** Adjust the sampling/update/refresh rate in seconds (e.g. 0.01). */
    private double sample_rate = 0.050;
    /** Used to track current simulation mode/state (e.g. INITIALIZATION/FREEZE/LDCHCK/DMPCHCK/DEBUG/RUN/EXIT/COMPLETE  */
    private int modeIndex = -1;

    /** VariableServerConnection for sending/receiving Variable Server commands. */
    private VariableServerConnection socket;

    /** List of processes that launch external stripchart applications. */
    private Vector<BackgroundCommand> new_process = new Vector<BackgroundCommand>();


    //========================================
    //    Constructors
    //========================================
    /**
     *    Default constructor.
     */
    public StripchartApplication()
    {

    }


    //========================================
    //    Actions
    //========================================

    /**
     * Toggle the X-axis range between a fixed width (scrolling graph),
     * or a maximized X-axis in which the graph will grow to fit all data.
     * <p>
     * Calls {@link StripchartApplication#setAutoXRange(boolean)}.
     */
    @Action
    public void toggle_xrange() {
        if ( xAxis.getFixedAutoRange() == 0.0 ) {
            setAutoXRange(false);
        } else {
            setAutoXRange(true);
        }
    }

    /**
     * Toggle start or stop of continuous curve updates.
     * (Note: data is still collected in the background when stopped;
     * upon start, all data collected during stop will be drawn to the plot.)
     * <p>
     * Calls {@link StripchartApplication#setStartStop(boolean)}.
     */
    @Action
    public void start_stop() {
        if ( start == false ) {
            setStartStop(true);
        } else {
            setStartStop(false);
        }
    }

    /**
     * Toggle the legend box between hidden/displayed.
     * <p>
     * Calls {@link StripchartApplication#setLengendOnOff(boolean)}.
     */
    @Action
    public void legend_onoff() {
        if ( chart.getLegend() == null ) {
            setLengendOnOff(true);
        } else {
            setLengendOnOff(false);
        }
    }

    /**
     * Output graph to a postscript file.
     * Opens a SaveAs window then calls
     * {@link StripchartApplication#saveImageToFile(BufferedImage, File)}.
     *
     * @see StripchartApplication#saveImageToFile(BufferedImage, File)
     */
    @Action
    public void postscript() {
        File defaultFile = new File("sc.ps");
        File file;
        String curDir, userInput, filename;
        boolean startOver = false;
        int extensionPos, option;

        fileChooser.setSelectedFile(defaultFile);

        if(fileChooser.showSaveDialog(getMainFrame()) == JFileChooser.APPROVE_OPTION) {
            curDir = fileChooser.getCurrentDirectory().getAbsolutePath();
            userInput = fileChooser.getSelectedFile().getName();
            filename = userInput;

            // Verify file extension
            if ( !filename.endsWith(".ps") ) {
                extensionPos = filename.lastIndexOf(".");
                if ( extensionPos != -1 ) {
                    // remove current file extension
                    filename = filename.substring( 0, extensionPos );
                }
                filename += ".ps";
                if (JOptionPane.showConfirmDialog(getMainFrame(),
                        "Invalid File Name\n" +
                        "Changing save name to...\n" +
                        "\"" + filename + "\"",
                        psButton.getText(), JOptionPane.OK_CANCEL_OPTION)
                        != JOptionPane.OK_OPTION ) {
                    startOver = true;
                }
            }

            file = new File(curDir + "/" + filename);
            if ( file.exists() ) {
                option = JOptionPane.showConfirmDialog(getMainFrame(),
                        "Overwrite existing file?", "File Already Exists",
                        JOptionPane.YES_NO_OPTION);
                if (option != JOptionPane.YES_OPTION) {
                    startOver = true;
                }
            }

            if (startOver == true) {
                 postscript();
            } else {
                int width = chartPanel.getWidth();
                int height = chartPanel.getHeight();
                BufferedImage image = chart.createBufferedImage( width, height );
                saveImageToFile(image, file);
            }
        }
    }

    /**
     * Save an image of the StripChart Panel to file.
     *
     * @param image copy of a buffered image of the {@link JFreeChart}
     * @param file  object to write image to
     *
     * @see JFreeChart#createBufferedImage(int, int)
     */
    public void saveImageToFile(BufferedImage image, File file) {

        String extension = file.getName().substring(file.getName().lastIndexOf('.') + 1);
        String[] imgIOFormats = ImageIO.getWriterFormatNames();
        boolean validImageIOformat = false;

        Arrays.sort(imgIOFormats);
        List<String> imgIOFormatsList = Arrays.asList(imgIOFormats);
        for (int i=0; i<imgIOFormatsList.size(); i++) {
            String tmp = imgIOFormatsList.get(i).replaceAll("image/", "");
            imgIOFormatsList.set(i, tmp);
        }

        // BMP, bmp, jpg, JPG, jpeg, wbmp, png, JPEG, PNG, WBMP, GIF, gif
        if (Arrays.binarySearch(imgIOFormatsList.toArray(), extension) > 0) {
            validImageIOformat = true;
        }

        if (validImageIOformat == true) {
            // Note: The ImageIO writer does not support Postscript format
            try {
                ImageIO.write(image, extension, file);
            } catch (IOException e) {
                System.err.println("Write error for " + file.getPath() +
                        ": " + e.getMessage());
            }
        } else if ( extension.compareToIgnoreCase("ps")==0 ) {
            // Begin steps to create a Postscript file...
            DocFlavor inputFormat = DocFlavor.BYTE_ARRAY.PNG;
            String outputMimeType = DocFlavor.BYTE_ARRAY.POSTSCRIPT.getMimeType();

            /* Locate factory which can export image as outputMimeType (e.g. Postscript) */
            StreamPrintServiceFactory[] factories =
                StreamPrintServiceFactory.lookupStreamPrintServiceFactories(
                        inputFormat, outputMimeType);
            if (factories.length > 0) {
                try {
                    /* Put the image into the desired inputFormat */
                    byte[] inputByteArray = ChartUtilities.encodeAsPNG(image);

                    /* Create a Doc object to hold the print data */
                    Doc doc = new SimpleDoc(inputByteArray, inputFormat, null);

                    /* Create a file for the output stream */
                    FileOutputStream fos = new FileOutputStream(file);

                    /* Create a printer for the output */
                    StreamPrintService sps = factories[0].getPrintService(fos);

                    /* Create and call a Print Job for the output */
                    DocPrintJob printerJob = sps.createPrintJob();

                    /* Print the doc as specified */
                    PrintRequestAttributeSet attributes = new HashPrintRequestAttributeSet();
                    printerJob.print(doc, attributes);

                    /* Close output file and releases any system resources associated with it */
                    fos.close();
                } catch (PrintException pe) {
                    System.err.println(pe);
                } catch (IOException ie) {
                    System.err.println(ie);
                }
            } else {
                System.err.println("Error: StripchartApplication:saveChartToFile()");;
                System.err.println("No suitable services available to print an \'"
                        + inputFormat + "\'\nto \'" + outputMimeType + "\'.");
            }
        }
    }


    //========================================
    //    Set/Get methods
    //========================================
    /**
     * Set the X-axis range to maximized, which will make the graph will grow
     * to fit all data, or to a fixed width (scrolling graph),
     *
     * @param auto "true" yields automatic sizing, "false" yields fixed-range
     */
    public void setAutoXRange(boolean auto) {

        if ( auto == true ) {
            // Maximize X axis (graph will grow to fit all data)
            xAxis.setFixedAutoRange(0.0);
            xAxis.setAutoRange(true);
        } else {
            // Limit X axis range to a fixed width (scrolling graph)
            configureAxes();
        }

        setAutoXRangeTxt();
    }

    /**
     * Start/Stop continuous curve updates on the graph.
     *
     * @param refresh determines whether or not to refresh the curves
     *                on the graph with updated data values
     */
    public void setStartStop(boolean refresh) {
        start = refresh;
        setStartStopTxt();
    }

    /**
     * Display or hide the legend box on the graph.
     *
     * @param visible whether to display the legend box
     */
    public void setLengendOnOff(boolean visible) {

        if ( visible == true ) {
            if (myLegend != null) {
                /** Add previously saved copy of legend */
                chart.addLegend(myLegend);  /**< legend - ON */
            }
        } else {
            /** Save copy of current legend */
            myLegend = chart.getLegend();
            chart.removeLegend();  /**< legend - OFF */
        }

        setLengendOnOffTxt();
    }

    /**
     * Sets the TextLabel of the xRangeButton based on the current X-axis autorange state.
     *
     * @see StripchartApplication#setAutoXRange(boolean)
     */
    public void setAutoXRangeTxt() {
        boolean maxRng = (xAxis.getFixedAutoRange()==0.0) ? true:false;

        if ( maxRng == true ) {
            xRangeButton.setText("Strip");
         } else {
             xRangeButton.setText("All");
        }
    }

    /**
     * Sets the TextLabel of the startStopButton based on the current "start" state.
     *
     * @see StripchartApplication#setStartStop(boolean)
     */
    public void setStartStopTxt() {
        if ( start == false ) {
            startStopButton.setText("Start");
        } else {
            startStopButton.setText("Stop");
        }
    }

    /**
     * Sets the TextLabel of the legendButton based on the chart's current legend state.
     *
     * @see StripchartApplication#setLengendOnOff(boolean)
     */
    public void setLengendOnOffTxt() {
        boolean onoff = (chart.getLegend()==null) ? false:true;

        if ( onoff == false ) {
            legendButton.setText("Legend");
        } else {
            legendButton.setText("No Legend");
        }
    }


    //========================================
    //    Methods
    //========================================

    /**
     * Main method for this application.
     *
     * @param args  hostname port x_variable_name y_variable_name(s)
     */
    public static void main(String[] args) {

        Application.launch(StripchartApplication.class, args);
    }

    /**
     * This is called before {@link StripchartApplication#startup()}.
     *
     * @param args  all arguments passes along from
     *              {@link StripchartApplication#main(String[])}.
     *
     * @see TrickApplication#initialize(java.lang.String[])
     */
    @Override
    protected void initialize(String[] args) {
        super.initialize(args);

        init();

        handle_args(args);

        if ( !demo ) {
            try {
                socket = new VariableServerConnection(host, port);
            }
            catch (UnknownHostException host_exception) {
                /** The IP address of the host could not be determined. */
                System.err.println("Error: StripchartApplication:initialize()");
                System.err.println(" Unknown host \""+host+"\"");
                /** Failure received; try connecting to localhost instead */
                System.err.println(
                " initialize() attempting to replace \""
                  +host+"\" with \"localhost\"");
                try {
                    host = "localhost";
                    socket = new VariableServerConnection(host, port);
                    System.out.println(" ...success!");
                } catch (IOException local_exception) {
                    System.err.println(" Error: StripchartApplication:initialize()");
                    System.err.println("   Unknown host \""+host+"\"");
                    return;  /* no need to continue */
                }
            }
            catch (IOException port_exception) {
                /** Port number is unavailable. */
                System.err.println("Error: StripchartApplication:initialize()");
                System.err.println(" Invalid TCP/IP port number \""+port+"\"");
                return;  /* no need to continue */
            }
            catch (NumberFormatException nfe) {
            }
            catch (NullPointerException npe) {
                npe.printStackTrace();
            }

            if ( socket == null ) {
                JOptionPane.showMessageDialog(getMainFrame(),
                    "Stripchart can't connect to host=" +host +" port="+port,
                    "Variable Server Connection Error", JOptionPane.ERROR_MESSAGE);
                System.exit(-1) ;
            }

            try {
                socket.put("trick.var_set_client_tag(\"TRICK_StripChart\")\n");
                socket.put(
                        "trick.var_clear()\n" +
                        "trick.var_add(\"trick_cmd_args.cmd_args.default_dir\")\n" +
                        "trick.var_add(\"trick_cmd_args.cmd_args.run_dir\")\n" +
                        "trick.var_send()");
                default_dir = socket.get().split("\t")[1];
                run_dir = socket.get().split("\t")[2];
                ps_dir = default_dir + "/" + run_dir;
                fileChooser.setCurrentDirectory(new File(ps_dir));
            }
            catch (IOException e) {
            }

            load_var_names();
        }

    }

    /**
     * Creates unique line colors to use per plot curve.
     * This is called by {@link StripchartApplication#initialize(String[])}.
     */
    public void init() {

        color.clear();

        color.add(Color.blue);
        color.add(Color.red);
        color.add(Color.black);
        color.add(Color.magenta);
        color.add(Color.orange);
        color.add(Color.green);
        color.add(Color.cyan);
        color.add(new Color(165,  42,  42));    // brown
        color.add(new Color(  0, 100,   0));    // darkgreen
        color.add(new Color(255,  20, 147));    // deeppink
        color.add(new Color(255, 250, 205));    // lemonchiffon
        color.add(new Color(250, 128, 114));    // salmon
        color.add(new Color(255,  99,  71));    // tomato
        color.add(new Color(160,  82,  45));    // sienna
        color.add(new Color(218, 112, 214));    // orchid
        color.add(new Color(216, 191, 216));    // thistle

    }

    /**
     * Process all user arguments.
     * This is called by {@link StripchartApplication#initialize(String[])}.
     *
     * @param args  all original user arguments including host and port number.
     *              (Note: host and port not needed if "-demo" option used.)
     */
    public void handle_args(String[] args) {
        File my_file;

        if ( args.length==1 && args[0].compareToIgnoreCase("-demo")==0 ) {

            // Chart a random data sample
            start = true;
            demo = true;
            title = "Cannon Trajectory";
            x_var_label = "Time s";
            y_vars_label.add(0, "dyn.cannon.pos[1] m");
            series.add(new XYSeries(y_vars_label.get(0)));
            autorange = 15.0;

        } else if ( args.length < 3 ) {

            usage();

        } else {

            host = args[0];
            try {
                /** Second argument should be an integer */
                port = Integer.parseInt(args[1]);
            } catch(NumberFormatException nFE) {
                /** if not, print usage error */
                usage();
            }

            // Build a var list and get optional args
            autorange = 30.0;
            shutdown = 0;
            vars_in = "";
            for (int ii = 2;  ii < args.length;  ii++) {
                if ( args[ii].compareToIgnoreCase("-input")==0 ) {
                    ii++;
                    infile = args[ii];
                    my_file = new File(infile);
                    if ( !my_file.exists() ) {
                        System.err.println("ERROR: Can't find stripchart input file \"" + infile + "\" ") ;
                        usage();
                    } else {

                        parse_input_file(infile) ;

                        for ( int jj = 0; jj < new_process.size();  jj++ ) {
                            // hold application and wait...
                            while (new_process.get(jj).isAlive()) {
                                /* Eventually, when this new input file process
                                 * dies, double-check to make sure that no other
                                 * input file processes are running, or have
                                 * started, by resetting the for loop to zero
                                 * and re-cycling through the conditional.
                                 */
                                jj = 0;
                            }
                        }

                        /**
                         *  Done processing the input file.
                         *  New stripchart(s) have launched and exited by now.
                         *  So kill this stripchart instance, because
                         *  it should have nothing left to do or display.
                         */
                        exit();

                    }
                } else if ( args[ii].compareToIgnoreCase("-geom")==0 ) {
                    ii++;
                    geom = args[ii];
                } else if ( args[ii].compareToIgnoreCase("-title")==0 ) {
                    ii++;
                    title = args[ii];
                } else if ( args[ii].compareToIgnoreCase("-shutdown")==0 ) {
                    ii++;
                    shutdown = Integer.parseInt(args[ii]);  // convert string to int
                } else if ( args[ii].compareToIgnoreCase("-autorange")==0    ||
                            args[ii].compareToIgnoreCase("-scrolling_range")==0 ) {
                    ii++;
                    autorange = Double.parseDouble(args[ii]);  // convert string to double
                } else if ( args[ii].compareToIgnoreCase("-x_min")==0 ) {
                    ii++;
                    x_min = args[ii];
                } else if ( args[ii].compareToIgnoreCase("-x_max")==0 ) {
                    ii++;
                    x_max = args[ii];
                } else if ( args[ii].compareToIgnoreCase("-y_min")==0 ) {
                    ii++;
                    y_min = args[ii];
                } else if ( args[ii].compareToIgnoreCase("-y_max")==0 ) {
                    ii++;
                    y_max = args[ii];
                } else {
                    vars_in = vars_in +" "+ args[ii];
                }
            }
            /* correct a comma-delimited varlist to space-delimited */
            vars_in = vars_in.replace(",", " ");
            /* Allow invalid Time var; substitute with correct variable */
            vars_in = vars_in.replaceAll("\\s[tT]ime\\s+",
                                         " trick_sys.sched.time_tics ");
            vars_in = vars_in.replaceAll("\\ssys.exec.out.time\\s+",
                                         " trick_sys.sched.time_tics ");
            vars_in = vars_in.replaceAll("\\strick_sys.sched.time_tic_value\\s+",
                                         " trick_sys.sched.time_tics ");
            /* remove leading whitespace */
            vars_in = vars_in.replaceAll("^\\s+", "");
            /* remove whitespace from each side of an equal sign;
             * also remove 's' if "units=" is used instead of "unit=" */
            vars_in = vars_in.replaceAll("\\s*[s]*\\s*=\\s*", "=");
        }
    }

    /**
     * Print a usage error message to the screen.
     * Called by {@link StripchartApplication#handle_args(String[])}
     * if an illegal user input argument is detected.
     */
    public void usage() {
        String usage
= "Name:\n"
+ "    trick_stripchart - Plot data on a line chart.\n"
+ "\n"
+ "Options:\n"
+ "    -demo   view the Trick stripchart in demonstration mode, with fully\n"
+ "            functional buttons and controls; no simulation needed\n"
+ "    -geom <width>x<height>+<xpos>+<ypos>\n"
+ "            sets geometry of window; size and position inputs are given in\n"
+ "            pixels, and position is relative to top left corner of screen\n"
+ "    -h, --help\n"
+ "            display this help and exit\n"
+ "    --label=<name>\n"
+ "            change plot axis and legend labels rather than using the\n"
+ "            variable name as a default label\n"
+ "    -scrolling_range <x-axis-fixed-length>\n"
+ "            automatically adjusts the x-axis maximum and minumum bounds so\n"
+ "            that a fixed-length of the most recent data is always in plot view\n"
+ "    -shutdown <0|1>\n"
+ "            setting to 1 will cause the stripchart to automatically exit\n"
+ "            when the sim shuts down and disconnects from the variable server\n"
+ "    -title \"<title>\"\n"
+ "            add a plot title to the window (note: must include quotes)\n"
+ "    --unit=<valid-trick-unit>\n"
+ "            convert data points from default units to another valid unit\n"
+ "    -x_max <x_max_value>\n"
+ "            sets the upper bound for the x-axis range\n"
+ "    -x_min <x_min_value>\n"
+ "            sets the lower bound for the x-axis range\n"
+ "    -y_max <y_max_value>\n"
+ "            sets the upper bound for the y-axis range\n"
+ "    -y_min <y_min_value>\n"
+ "            sets the lower bound for the y-axis range\n"
+ "Usage:\n"
+ "    trick_stripchart [1m<host> <port>[0m\n"
+ "        [ [-shutdown <0|1>]\n"
+ "          [-title \"<title>\"]\n"
+ "          [-geom <width>x<height>+<xpos>+<ypos>]\n"
+ "          [ [[-x_min <x_min_value>] [-x_max <x_max_value>]]  |\n"
+ "            [ -scrolling_range <x-axis-fixed-length> ]\n"
+ "          ]\n"
+ "          [-y_min <y_min_value>] [-y_max <y_max_value>]\n"
+ "        ]\n"
+ "        [1m\"<xvar>\"[0m [--label=<nospaces_name>] [--unit=<Valid Trick Unit>]\n"
+ "        [1m\"<space-delimited-yvarlist[0m [--label=<name>] [--unit=<conversion>][1m >\"[0m\n"
+ "\n"
+ "    trick_stripchart <host> <port> \"<xvar>\" \"<yvarlist>\"\n"
+ "    trick_stripchart <host> <port> -input <infile>\n"
+ "    trick_stripchart -demo\n"
+ "\n"
+ "Examples:\n"
+ "  % trick_stripchart localhost 7000 \"trick_sys.sched.time_tics\" \"cannon.vel[0]\"\n"
+ "  % trick_stripchart localhost 7001 \"sys.exec.out.time\"  \"dyn.X dyn.Y dyn.Z\"\n"
+ "  % trick_stripchart `hostname` 7000 \"ball.pos[0]\"  \"ball.pos[1]\"\n"
;

        System.err.println(usage);
        System.exit(-1) ;

    }

    /**
     * Parse stripchart input file (if applicable). This is called
     * by {@link StripchartApplication#handle_args(String[])} if the
     * "-input" string is detected as a user argument.
     *
     * @param input_file    name of the stripchart input file containing
     *                      user options.
     */
    public void parse_input_file(String input_file) {
        String my_args = "";

        Vector<String> stripcharts = new Vector<String>();
        int index, first ;
        String myGeom, myTitle, myRange ;
        String myXmin, myXmax ;
        String myYmin, myYmax ;
        String myShutdown ;
        String myXvar ;

        /* Parse file and create an array of stripcharts */
        index = 0 ;
        first = 1 ;
        try{
            /* Open the file */
            /* Get all of the text from the input file */
            File file = new File(input_file);
            BufferedReader br = new BufferedReader(new FileReader(file));
            String line = "";

            /* Read buffered text Line By Line,
             * plus trim line leading and trailing whitespaces. */
            while ((line = br.readLine().trim()) != null)   {

                /* Skip comment lines */
                if ( line.startsWith("#")  ||  line.startsWith("//") ) {
                    continue ;
                }

                /* Skip blank lines */
                if ( line.isEmpty() ) {
                    continue ;
                }

                /* Remove semicolon (if any) */
                if ( line.endsWith(";") ) {
                    line = line.substring(0, line.lastIndexOf(";")) ;
                }

                /* Setup case-insensitive pattern matching of the current Line. */
                if ( Pattern.compile("^\\s*stripchart\\s*:",
                     Pattern.CASE_INSENSITIVE).matcher( line ).find() ) {
                    first = (first!=0) ?  0 : index++ ;
                    stripcharts.add(index, "");
                    stripcharts.set(index, "trick_sys.sched.time_tics") ;
                } else if ( Pattern.compile("^\\s*title\\s*=",
                            Pattern.CASE_INSENSITIVE).matcher( line ).find() ) {
                    /* title = "<Title>" */
                    myTitle = line.replaceFirst("^\\s*title\\s*=\\s*", "");
                    stripcharts.set(index, "-title "+myTitle+" "+stripcharts.get(index)) ;
                } else if ( Pattern.compile("^\\s*x_variable\\s*=",
                            Pattern.CASE_INSENSITIVE).matcher( line ).find() ) {
                    myXvar = line.replaceFirst("^\\s*x_variable\\s*=\\s*", "");
                    stripcharts.set(index,
                                    stripcharts.get(index).replaceFirst(
                                    "trick_sys.sched.time_tics", myXvar)
                                   );
                } else if ( Pattern.compile("^\\s*auto_shutdown\\s*=",
                            Pattern.CASE_INSENSITIVE).matcher( line ).find() ) {
                    /* auto_shutdown = 0 | 1
                     * Setting auto_shutdown will cause the stripchart
                     * to exit when the sim exits. */
                    myShutdown = line.replaceFirst("^\\s*auto_shutdown\\s*=\\s*", "");
                    stripcharts.set(index, "-shutdown "+myShutdown+" "+stripcharts.get(index)) ;
                } else if ( Pattern.compile("^\\s*geometry\\s*=",
                            Pattern.CASE_INSENSITIVE).matcher( line ).find() ) {
                    /* geometry = <width>x<height>+<xpos>+<ypos>
                     * Setting geometry places the stripchart at the
                     * desired location and size on the screen. */
                    myGeom = line.replaceFirst("^\\s*geometry\\s*=\\s*", "");
                    stripcharts.set(index, "-geom "+myGeom+" "+stripcharts.get(index)) ;
                } else if ( Pattern.compile("^\\s*scrolling_range\\s*=",
                            Pattern.CASE_INSENSITIVE).matcher( line ).find() ) {
                    myRange = line.replaceFirst("^\\s*scrolling_range\\s*=\\s*", "");
                    stripcharts.set(index, "-scrolling_range "+myRange+" "+stripcharts.get(index)) ;
                } else if ( Pattern.compile("^\\s*x_min\\s*=",
                            Pattern.CASE_INSENSITIVE).matcher( line ).find() ) {
                    /* x_min = <x range min> */
                    myXmin = line.replaceFirst("^\\s*x_min\\s*=\\s*", "");
                    stripcharts.set(index, "-x_min "+myXmin+" "+stripcharts.get(index)) ;
                } else if ( Pattern.compile("^\\s*x_max\\s*=",
                            Pattern.CASE_INSENSITIVE).matcher( line ).find() ) {
                    /* x_max = <x range max> */
                    myXmax = line.replaceFirst("^\\s*x_max\\s*=\\s*", "");
                    stripcharts.set(index, "-x_max "+myXmax+" "+stripcharts.get(index)) ;
                } else if ( Pattern.compile("^\\s*y_min\\s*=",
                            Pattern.CASE_INSENSITIVE).matcher( line ).find() ) {
                    /* y_min = <y range min> */
                    myYmin = line.replaceFirst("^\\s*y_min\\s*=\\s*", "");
                    stripcharts.set(index, "-y_min "+myYmin+" "+stripcharts.get(index)) ;
                } else if ( Pattern.compile("^\\s*y_max\\s*=",
                            Pattern.CASE_INSENSITIVE).matcher( line ).find() ) {
                    /* y_max = <y range max> */
                    myYmax = line.replaceFirst("^\\s*y_max\\s*=\\s*", "");
                    stripcharts.set(index, "-y_max "+myYmax+" "+stripcharts.get(index)) ;
                } else {
                    /* Anything else is a list of Y variables = <yvarlist> */
                    stripcharts.set(index, stripcharts.get(index)+" "+line) ;
                }
            }
            //Close the input stream
            br.close();
        }
        catch (Exception e) {
        //Catch exception if any
        }

        //! Re-Execute the StripchartApplication with new arguments.
        for ( int ii = 0;  ii < stripcharts.size();  ii++ ) {
            /* Open multiple stripchart windows (if applicable) */
            my_args = host+" "+port+" "+stripcharts.get(ii);
            String[] cmd = {"/bin/sh", "-c",
                            System.getenv("TRICK_HOME")+"/bin/trick_stripchart "+my_args};
            new_process.add(ii, new BackgroundCommand(cmd));
            new_process.get(ii).start();
        }

    }

    /**
     * Run a system command in the background;
     * separate from this thread.
     */
    class BackgroundCommand extends Thread {
        String[] cmd = {""};

        // Constructor
        BackgroundCommand(String[] str) {
            cmd = str;
        }

        @Override
		public void run() {
            try {
                Process p = Runtime.getRuntime().exec(cmd);
                BufferedReader buf = new BufferedReader(new InputStreamReader(p.getInputStream()));
                String line = "";
                while ((line=buf.readLine())!=null) {
                    System.out.println(line);
                }
            } catch (IOException rte) {
                rte.printStackTrace();
                System.exit(-1);
            }
        }
    }

    /**
     * Get X/Y variable names and units after command line
     * arguments are handled. This is called at the end of
     * {@link StripchartApplication#initialize(String[])}.
     */
    public void load_var_names() {

        int var_count;
        int index = 0;
        String sim_val_unit = "";
        String[] words;

        try {
            if ( vars_in != null ) {
                var_count = 0;
                num_curves = 0;
                words = vars_in.split("\\s+");  // split variables into an array of words

                for (int i = 0;  i < words.length;  i++) {
                    String v = words[i];
                    if (v.isEmpty()) {
                        continue;
                    }

                    if ( v.substring(0, 7).compareTo("--unit=")==0 ) {
                        // conversion units specified
                        if (var_count == 1) {
                            x_var_unit = v.substring(7);
                        } else {
                            y_vars_unit.set(index, v.substring(7));
                        }
                    } else if ( v.substring(0, 8).compareTo("--label=")==0 ) {
                        // label specified
                        if (var_count == 1) {
                            x_var_label = v.substring(8);
                        } else {
                            y_vars_label.set(index, v.substring(8));
                        }
                    } else {
                        // get variable names
                        var_count++;
                        if (var_count == 1) {
                            // The X variable is always listed first
                            x_var_name = v;
                            x_var_unit = "xx";  // use default units
                            x_var_label = v;
                        } else {
                            // Any subsequent text represents Y variables
                            num_curves++;
                            index = num_curves - 1;
                            y_vars_name.add(index, v);
                            y_vars_unit.add(index, "xx");  // use default units
                            y_vars_label.add(index, v);
                            y_vars_multiplier.add(index, 1.0);
                            xv.add(index, "");
                            yv.add(index, "");
                        }
                    }
                }

                // X: fill in units & set label to Time if user did not specify his own
                socket.put("trick.var_clear()\n" +
                        "trick.var_add(" +
                                "\""+x_var_name+"\", " +
                                "\""+x_var_unit+"\"" + ")\n" +
                        "trick.var_send()\n" +
                        "trick.var_clear()\n");
                sim_val_unit = socket.get().split("\t")[1];
                sim_val_unit = sim_val_unit.replaceAll("\\s*.*\\s+", "");
                x_var_unit = sim_val_unit.replaceAll("[{}]", "");
                if ( x_var_label.compareTo("trick_sys.sched.time_tics")==0 ) {
                    socket.put("trick.var_add(\"trick_sys.sched.time_tic_value\") \n" +
                            "trick.var_send()\n" +
                            "trick.var_clear()\n");
                    XMultiplier = Double.parseDouble(socket.get().split("\t")[1]);
                    x_var_label = "Time s";
                } else {
                    x_var_label = x_var_label + " " + x_var_unit;
                }
                /* if variable is unitless (--),
                 * then save "xx" as its conversion unit in case it is
                 * added to the variable server again in the future. */
                if ( x_var_unit.compareTo("--")==0 ) {
                    x_var_unit = "xx";
                }

                // Y: fill in units that were not specified & add unit to end of label
                for (int ii = 0;  ii < num_curves;  ii++) {
                    socket.put("trick.var_clear()\n" +
                            "trick.var_add(" +
                                    "\""+y_vars_name.get(ii)+"\", " +
                                    "\""+y_vars_unit.get(ii)+"\"" + ")\n" +
                            "trick.var_send()\n" +
                            "trick.var_clear()\n");
                    sim_val_unit = socket.get().split("\t")[1];
                    sim_val_unit = sim_val_unit.replaceAll("\\s*.*\\s+", "");
                    y_vars_unit.set(ii, sim_val_unit.replaceAll("[{}]", ""));
                    if ( y_vars_label.get(ii).startsWith("trick_sys.sched.time_tics") ) {
                        socket.put("trick.var_clear() \n" +
                                "trick.var_add(\"trick_sys.sched.time_tic_value\")\n" +
                                "trick.var_send()\n" +
                                "trick.var_clear()\n");
                        y_vars_multiplier.set(ii, Double.parseDouble(socket.get().split("\t")[1]));
                        y_vars_label.set(ii, "Time s");
                    } else {
                        y_vars_label.set(ii, y_vars_label.get(ii)+" "+y_vars_unit.get(ii));
                    }
                    /* if variable is unitless (--),
                     * then save "xx" as its conversion unit in case it is
                     * added to the variable server again in the future. */
                    if ( y_vars_unit.get(ii).compareTo("--")==0 ) {
                        y_vars_unit.set(index, "xx");
                    }
                }
            }
        }
        catch (NumberFormatException nfe) {
        }
        catch (IOException e) {
        }
        catch (NullPointerException npe) {
            npe.printStackTrace();
        }
    }

    /**
     * Starts building the GUI. This is called after
     * {@link StripchartApplication#initialize(String[])}. Once startup()
     * is done, {@link StripchartApplication#ready()} is called.
     */
    @Override
    protected void startup() {
        super.startup();

        View view = getMainView();
        view.setComponent(createMainPanel());
        view.setMenuBar(createMenuBar());

        show(view);

        // Resize & Relocate GUI
        if ( !geom.isEmpty() ) {
            String[] bounds = geom.split("[^0-9]+");
            int w = (!bounds[0].isEmpty()) ?  Integer.parseInt(bounds[0]):getMainFrame().getWidth();
            int h = (!bounds[1].isEmpty()) ?  Integer.parseInt(bounds[1]):getMainFrame().getHeight();
            int x = (!bounds[2].isEmpty()) ?  Integer.parseInt(bounds[2]):getMainFrame().getX();
            int y = (!bounds[3].isEmpty()) ?  Integer.parseInt(bounds[3]):getMainFrame().getY();
            setGeometry(x, y, w, h);
        }

        if (demo) {
            gen myGen = new gen();
            new Thread(myGen).start();
        } else {
            sim_init();
        }

        int rate = (int) (sample_rate * 1000);  // convert seconds to milliseconds
        // Schedule a task that executes once every sample_rate
        Timer timer = new Timer(rate, new Draw());
        timer.setInitialDelay(0);
        timer.start();
    }

    /**
     * Calls {@link StripchartApplication#create()} to make the
     * main panel for this application.
     *
     * @return {@link JComponent}
     */
    @Override
	protected JComponent createMainPanel() {

        create();

        // Setup a scroll bar
        JScrollPane TopScrollPane = new JScrollPane(TopComponent,
                JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED,
                JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);

        return TopScrollPane;
    }

    /**
     * Create the application GUI using a {@link JPanel} layout manager.
     * This is called by {@link StripchartApplication#createMainPanel()}.
     */
    public void create() {

        /* Create a layout Manager */
        TopComponent = new JPanel(new GridBagLayout());
        GridBagConstraints ChartPanel_constraints = new GridBagConstraints();
        GridBagConstraints BtnPanel_constraints = new GridBagConstraints();

        for (int i=0;  i < num_curves;  i++) {
            series.add(new XYSeries(y_vars_label.get(i)));
            dataset.addSeries(series.get(i));
        }
        chart = ChartFactory.createXYLineChart(
            title,                      /* chart title */
            x_var_label,                /* domain axis label */
            "",                         /* range axis label */
            dataset,                    /* the dataset for the chart */
            PlotOrientation.VERTICAL,   /* Value orientation */
            true,                       /* include legend */
            true,                       /* include tool tips */
            false                       /* include URLs */
        );
        chart.setBackgroundPaint(null);  // chart bg will be same as window
        plot = chart.getXYPlot();
        plot.setBackgroundPaint(Color.white);
        plot.setDomainGridlinePaint(Color.gray);
        plot.setRangeGridlinePaint(Color.gray);
        for (int i=0;  i < num_curves;  i++) {
            // change the line color of each curve
            plot.getRenderer().setSeriesPaint( i, color.get(i%color.size()) );
        }

        xAxis = plot.getDomainAxis();
        yAxis = plot.getRangeAxis();

        /* Configure X/Y axis range and boundaries */
        X_LOWER_BOUND = 0.0;
        X_UPPER_BOUND = autorange;
        configureAxes();

        chartPanel = new ChartPanel(chart);
        chartPanel.setVisible(true);
        chartPanel.setDefaultDirectoryForSaveAs(new File(ps_dir));

        /* Set stripchart layout constraints */
        ChartPanel_constraints.gridx = 0;   //first column of grid
        ChartPanel_constraints.gridy = 0;   //first row of grid
        ChartPanel_constraints.insets = new Insets(0,20,0,20); //side padding
        ChartPanel_constraints.fill = GridBagConstraints.BOTH;
        ChartPanel_constraints.anchor = GridBagConstraints.PAGE_START;
        ChartPanel_constraints.weightx = 0.9;
        ChartPanel_constraints.weighty = 0.9;
        /* Add stripchart to TopComponent */
        TopComponent.add(chartPanel, ChartPanel_constraints);

        /* Setup all buttons */
        xRangeButton = new JButton(getAction("toggle_xrange"));
        setAutoXRangeTxt();
        startStopButton = new JButton(getAction("start_stop"));
        setStartStopTxt();
        legendButton = new JButton(getAction("legend_onoff"));
        setLengendOnOffTxt();
        psButton = new JButton(getAction("postscript"));
        psButton.setText("Postscript...");

        JPanel buttonPanel = new JPanel();
        buttonPanel.add(xRangeButton);
        buttonPanel.add(startStopButton);
        buttonPanel.add(legendButton);
        buttonPanel.add(psButton);
        buttonPanel.add(new JButton(getAction("quit")));
        // Turn off underlined decoration of char that is associated w/ ALT-key
        for (Component item : buttonPanel.getComponents()) {
            ((AbstractButton) item).setMnemonic(-1);
        }

        /* Set button layout constraints */
        BtnPanel_constraints.gridx = 0; //first column of grid
        BtnPanel_constraints.gridy = 1; //second row of grid
        BtnPanel_constraints.insets = new Insets(20,0,0,0); //top padding
        // anchor button group to the bottom left-hand side of the window
        BtnPanel_constraints.anchor = GridBagConstraints.LAST_LINE_START;
        /* Add buttons to TopComponent */
        TopComponent.add(buttonPanel, BtnPanel_constraints);
        TopComponent.setVisible(true);

    }

    /**
     * Re-scale the X\Y plot axes and change the plot boundaries
     * based on user input.
     */
    protected void configureAxes() {

        xAxis.setFixedAutoRange(autorange);
        if ( !x_min.isEmpty() ){
            X_LOWER_BOUND = Double.parseDouble(x_min);
            X_UPPER_BOUND = Double.parseDouble(x_min) + autorange;
            xAxis.setLowerBound(X_LOWER_BOUND);
            xAxis.setUpperBound(X_UPPER_BOUND);
        }
        if ( !x_max.isEmpty() ){
            X_UPPER_BOUND = Double.parseDouble(x_max);
            xAxis.setUpperBound(X_UPPER_BOUND);
        }

        if ( !y_min.isEmpty() ){
            Y_LOWER_BOUND = Double.parseDouble(y_min);
            yAxis.setLowerBound(Y_LOWER_BOUND);
        }
        if ( !y_max.isEmpty() ){
            Y_UPPER_BOUND = Double.parseDouble(y_max);
            yAxis.setUpperBound(Y_UPPER_BOUND);
        }

        /* Execute Draw once right now (without repeated execution) */
        Timer timer = new Timer(0, new Draw());
        timer.setRepeats(false);  //fire only once
        timer.start();

    }

    /**
     * Create the JMenuBar for this application.  In addition
     * to the @Actions defined here, the menu bar menus include
     * the Show Exit Confirmation Prompt and Exit @Actions that
     * are inherited from the Application class.
     *
     * @see TrickApplication#exitConfirmation()
     * @see TrickApplication#exit()
     */
    @Override
    protected JMenuBar createMenuBar() {
        JMenuBar menuBar = super.createMenuBar();

        // File
        JMenu fileMenu = menuBar.getMenu(0);
        fileMenu.insert(getAction("postscript"), 0);
        fileMenu.add(new JSeparator(), 1);

        // Settings
        String[] settingsMenuActionNames = {
                "toggle_xrange",
                "start_stop",
                "legend_onoff"
        };
        menuBar.add(createMenu("settingsMenu", settingsMenuActionNames), 1);

        return menuBar;
    }

    /**
     * Change the dimensions (Location and Size) of the application.
     * If called during startup, method may not be reliable because of creation
     * timing. Purposely called AFTER {@link StripchartApplication#startup()}
     * when the application is completely ready.
     *
     * @param x         desired horizontal position (from left) of window in pixels
     * @param y         desired vertical position (from top) of window in pixels
     * @param width     desired width of window in pixels
     * @param height    desired height of window in pixels
     */
    protected void setGeometry(int x, int y, int width, int height) {

        Rectangle r = new Rectangle(x, y, width, height);
        getMainFrame().setBounds(r);
    }
    /**
     * Overloaded method to change the Size of the application.
     * If called during startup, method may not be reliable because of creation timing.
     * Purposely called AFTER {@link StripchartApplication#startup()}
     * when the application is completely ready.
     *
     * @param width     desired width of window in pixels
     * @param height    desired height of window in pixels
     */
    protected void setGeometry(int width, int height) {

        int x = getMainFrame().getBounds().x;
        int y = getMainFrame().getBounds().y;
        setGeometry(x, y, width, height);
    }

    /**
     * Starts things such as establishing socket communication, and starting monitor tasks.
     * This is called after {@link StripchartApplication#startup()}.
     */
    @Override
    protected void ready() {
        // TODO: the following code was commented out and moved to the end of startup()
        // due to framework having issues with certain Java versions. In certain Java
        // version such as JDK1.6.0_20, ready() never gets called???
        // NEED TO REVISIT THIS!!!

        /*super.ready();

        // Resize & Relocate GUI
        if ( !geom.isEmpty() ) {
            String[] bounds = geom.split("[^0-9]+");
            int w = (!bounds[0].isEmpty()) ?  Integer.parseInt(bounds[0]):getMainFrame().getWidth();
            int h = (!bounds[1].isEmpty()) ?  Integer.parseInt(bounds[1]):getMainFrame().getHeight();
            int x = (!bounds[2].isEmpty()) ?  Integer.parseInt(bounds[2]):getMainFrame().getX();
            int y = (!bounds[3].isEmpty()) ?  Integer.parseInt(bounds[3]):getMainFrame().getY();
            setGeometry(x, y, w, h);
        }

        if (demo) {
            gen myGen = new gen();
            new Thread(myGen).start();
        } else {
            sim_init();
        }

        int rate = (int) (sample_rate * 1000);  // convert seconds to milliseconds
        // Schedule a task that executes once every sample_rate
        Timer timer = new Timer(rate, new Draw());
        timer.setInitialDelay(0);
        timer.start();*/
    }

    /**
     * Used only if "-demo" user argument is given.
     */
    class gen implements Runnable {
        private double tics = 0.0;

        public void run() {
            double num = 0;
            while(true) {
                num = Math.sin(tics)*10.0 * (Math.abs(100-tics*2)%100)/100;
                series.get(0).add(tics, num, start);
                if ( start==true ) {
                    adjustAxesRanges(tics, num);
                }
                tics += 0.050;
                try {
                    Thread.sleep(050);
                } catch (InterruptedException ex) {
                    System.out.println(ex);
                }
            }
        }

    }

    /**
     * Tell sim what variables to stripchart.
     * Called at the end of {@link StripchartApplication#ready()}.
     */
    public void sim_init() {

        try {
            socket.put("trick.var_clear()\n");
            /** Track FREEZE/RUN mode */
            socket.put("trick.var_add(\"trick_sys.sched.mode\")\n");

            /** Send variables to stripchart in designated units */
            socket.put("trick.var_add(" +
                    "\""+x_var_name+"\", " + "\""+x_var_unit+"\"" + ")\n");
            for (int ii = 0;  ii < num_curves;  ii++ ) {
                socket.put("trick.var_add(" +
                        "\""+y_vars_name.get(ii)+"\", " +
                        "\""+y_vars_unit.get(ii)+"\"" + ")\n");
            }

            socket.put("trick.var_debug(0)\n");
            socket.put("trick.var_cycle(" + sample_rate + ")\n");
            socket.put("trick.var_send()");
        }
        catch (NumberFormatException nfe) {
        }
        catch (IOException e) {
        }
    }

    /**
     * Update the graph cyclically via a {@link Timer} task that
     * was created by {@link StripchartApplication#ready()}.
     */
    class Draw implements ActionListener {

        @Override
        public void actionPerformed(ActionEvent e) {
            if ( !demo )  {
                new GetData().execute(); // background task to get x/y values
            }
        }

    }

    /**
     * Inner class for the task of getting data from simulation.
     */
    private class GetData extends SwingWorker<Void, String[]> {
        double x=0.0, y=0.0;
        double xe, ye;
        String[] data = null;
        int index;

        /**
         * In a background Worker thread, listen for incoming messages
         * from the Variable Server.
         */
        @Override
        public Void doInBackground() throws IOException {

            /* Whenever there is data in the Variable Server, do something.
             * Keep extracting data from the socket until it is clear. */
            if ( (data = socket.get().split("\t")) != null ) {
                try {
                    index = 1;
                    modeIndex = Integer.parseInt(data[index]);
                    index++;

                    /* The fisrt element is the X variable's value; delete units */
                    x  = Double.parseDouble(data[index].replaceAll("\\s*[{}].*", ""));
                    x /= XMultiplier;

                    /* If the sim is not in start, then don't bother */
                    if ( x_var_label.toLowerCase().contains("time") 
                            && modeIndex!=SimState.RUN_MODE ) {
                        adjustAxesRanges(x, y);
                        return null; //goto done()
                    }

                    /* Y vars */
                    for ( int i=0;  i < num_curves;  i++ ) {
                        /* Advance to next item in the array */
                        index++;
                        /* Skip over {units} if applicable */
                        while ( data[index].startsWith("{") ) {
                            index++;
                        }

                        /* Save Y value; delete units */
                        y  = Double.parseDouble(data[index].replaceAll("\\s*[{}].*", ""));
                        y /= y_vars_multiplier.get(i);

                        /* Store previous point */
                        if ( xv.get(i).length() > 0 &&
                             yv.get(i).length() > 0  )  {
                            String[] a = xv.get(i).split("\\s+");
                            String[] b = yv.get(i).split("\\s+");
                            xe = Double.parseDouble( a[a.length-1] );
                            ye = Double.parseDouble( b[b.length-1] );
                            /* If identical point is received, skip */
                            if ( x==xe  &&  y==ye ) {
                                continue;
                            }
                        }

                        /* Append latest values */
                        xv.set(i, xv.get(i)+" "+x);
                        yv.set(i, yv.get(i)+" "+y);
                        series.get(i).add( x, y, start );

                        adjustAxesRanges(x, y);
                    }
                } catch (Exception e) {
                    System.err.println(e.getMessage());
                }
            }
            return null; //goto done()
        }

        /**
         * Executed on the Event Dispatch Thread after the doInBackground
         * method is finished.
         *
         * @see javax.swing.SwingWorker#done()
         */
        @Override
        protected void done() {
            if (socket.results == null) {
                // Sim is gone, leave chart up unless auto shutdown set
                if (shutdown == 1) {
                    System.out.println("Shutting Down");
                    shutdown();
                    System.exit(-1) ;
                } else if (start == true) {
                    setStartStop(false);
                    start = false;
                    startStopButton.setText("Start");
                    JOptionPane.showMessageDialog(getMainFrame(),
                            "The connection to the simulation was lost.",
                            "Lost Connection", JOptionPane.INFORMATION_MESSAGE);
                    setStartStop(false);
                }
            }
        }

    }

    /**
     * Shift X axis if new values are off screen.
     * Shift Y axis if new values are off screen.
     *
     * @param x_val most recent value for the X variable
     * @param y_val most recent value for the Y variable
     */
    protected void adjustAxesRanges(double x_val, double y_val) {

        /* X-Axis */
        if (xAxis.getFixedAutoRange() != 0.0  &&  start == true) {
            /* Normal mode; no x_min or x_max limitations */
            if (x_min.isEmpty()  &&  x_max.isEmpty()) {
                if ( Math.abs(x_val) < autorange ) {
                    if ( 0.0 <= x_val ) {
                        X_LOWER_BOUND = 0.0;
                        xAxis.setLowerBound(X_LOWER_BOUND);
                    } else {
                        X_LOWER_BOUND = x_val;
                        xAxis.setLowerBound(X_LOWER_BOUND);
                    }
                    X_UPPER_BOUND = X_LOWER_BOUND + autorange;
                    xAxis.setUpperBound(X_UPPER_BOUND);
                } else {
                    xAxis.setAutoRange(true);
                }
            } else {
                if ( x_min.isEmpty() ){
                    /* Don't touch X_UPPER_BOUND!  It's already been set */
                    if ( (X_UPPER_BOUND - x_val) > autorange ) {
                        X_LOWER_BOUND = x_val - autorange;
                        if (0.0 <= x_val  &&  X_LOWER_BOUND < 0.0) {
                            X_LOWER_BOUND = 0.0;
                        }
                        xAxis.setLowerBound(X_LOWER_BOUND);
                    } else if (x_val > xAxis.getUpperBound()) {
                        if (x_val <= X_UPPER_BOUND) {
                            xAxis.setLowerBound(x_val - autorange);
                        } else {
                            X_LOWER_BOUND = X_UPPER_BOUND - autorange;
                            if (0.0 <= x_val  &&  X_LOWER_BOUND < 0.0) {
                                X_LOWER_BOUND = 0.0;
                            }
                            xAxis.setLowerBound(X_LOWER_BOUND);
                        }
                    }
                } else if ( x_max.isEmpty() ){
                    /* Don't touch X_LOWER_BOUND!  It's already been set */
                    if (0.0 <= x_val  &&  X_LOWER_BOUND < 0.0) {
                        xAxis.setUpperBound(autorange);
                    }
                    if ( (x_val - xAxis.getLowerBound()) > autorange ) {
                        X_UPPER_BOUND = x_val;
                        xAxis.setUpperBound(X_UPPER_BOUND);
                    } else if (x_val < xAxis.getLowerBound()) {
                        if (x_val >= X_LOWER_BOUND) {
                            X_UPPER_BOUND = x_val + autorange;
                        } else {
                            X_UPPER_BOUND = X_LOWER_BOUND + autorange;
                        }
                        xAxis.setUpperBound(X_UPPER_BOUND);
                    }
                }
            }
        }

        /* Y-Axis */
        if (yAxis.isAutoRange() == false  &&  start == true) {
            double y_tot_range;
            double y_margin_pct;
            double y_margin_len;
            if ( y_min.isEmpty()  &&  y_val < Y_LOWER_BOUND ){
                y_tot_range = yAxis.getRange().getLength();
                y_margin_pct = yAxis.getLowerMargin();
                y_margin_len = y_tot_range * y_margin_pct;
                Y_LOWER_BOUND = y_val;
                yAxis.setLowerBound(Y_LOWER_BOUND - y_margin_len);
            }
            if ( y_max.isEmpty()  &&  y_val > Y_UPPER_BOUND ){
                y_tot_range = yAxis.getRange().getLength();
                y_margin_pct = yAxis.getUpperMargin();
                y_margin_len = y_tot_range * y_margin_pct;
                Y_UPPER_BOUND = y_val;
                yAxis.setUpperBound(Y_UPPER_BOUND + y_margin_len);
            }
        }
    }

    /**
     * Cleans up the socket communication before exiting the application.
     */
    @Override
    protected void shutdown() {
        super.shutdown();
        try {
            if (socket != null) {
                socket.close(); // close variable server connection
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

}

