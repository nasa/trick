package trick.jobperf;

import java.awt.*;
import java.awt.event.*;
import java.awt.image.BufferedImage;
import java.io.*;
import java.lang.Math;
import java.util.*;
import java.util.List;
import javax.swing.*;
import javax.swing.event.*;

class JobExecutionEvent {
    public String id;
    public boolean isEOF;
    public boolean isTOF;
    public double start;
    public double stop;

    public JobExecutionEvent(String identifier, boolean isTopOfFrame, boolean isEndOfFrame, double start_time, double stop_time) {
        id = identifier;
        isEOF = isEndOfFrame;
        isTOF = isTopOfFrame;
        start = start_time;
        stop = stop_time;
    }

    public String toString() {
        return ( "JobExecutionEvent: " + id + "," + start + "," + stop );
    }
}

class KeyedColorMap {
    private Map<String, Color> colorMap;
    int minColorIntensity;

    public KeyedColorMap() {
        colorMap = new HashMap<String, Color>();
        minColorIntensity = 100;
    }

    private Color generateColor () {
        Random rand = new Random();
        boolean found = false;
        int R = 0;
        int G = 0;
        int B = 0;

        while (!found) {
            R = rand.nextInt(256);
            G = rand.nextInt(256);
            B = rand.nextInt(256);
            found = true;
            if ((R < minColorIntensity) && (G < minColorIntensity) && (B < minColorIntensity)) {
                found = false;
            }
        }
        return new Color( R,G,B);
    }

    public void addKey( String identifier ) {
        if (!colorMap.containsKey(identifier)) {
            colorMap.put(identifier, generateColor());
        }
    }

    // Given the key, return the color.
    public Color getColor(String identifier) {
        return colorMap.get(identifier);
    }

    // Given the color, return the key.
    public String getKeyOfColor(Color search_color) {
        for (Map.Entry<String, Color> entry : colorMap.entrySet()) {
            String id = entry.getKey();
            Color color = entry.getValue();
            if (color.getRGB() == search_color.getRGB()) {
                return id;
            }
        }
        return null;
    }

    public void readFile(String fileName) throws IOException {
        try {
            BufferedReader in = new BufferedReader( new FileReader(fileName) );
            String line;
            String field[];

            while( (line = in.readLine()) !=null) {
                field   = line.split(",");
                String id    = field[0];
                int R = Integer.parseInt( field[1]);
                int G = Integer.parseInt( field[2]);
                int B = Integer.parseInt( field[3]);
                colorMap.put(id, new Color(R,G,B));
            }
            in.close();
        } catch ( java.io.FileNotFoundException e ) {
           System.out.println("File \"" + fileName + "\" not found.\n");
        }
    }

    public void writeFile(String fileName) throws IOException {
        BufferedWriter out = new BufferedWriter( new FileWriter(fileName) );
        for (Map.Entry<String, Color> entry : colorMap.entrySet()) {
            String id = entry.getKey();
            Color color = entry.getValue();
            String line = String.format(id + "," + color.getRed() +
                                             "," + color.getGreen() +
                                             "," + color.getBlue() + "\n");
            out.write(line, 0, line.length());
        }
        out.flush();
        out.close();
    }
} // KeyedColorMap

class TraceViewPanel extends JPanel {

    public static final int MIN_TRACE_WIDTH = 4;
    public static final int MAX_TRACE_WIDTH = 30;
    public static final int LEFT_MARGIN = 100;
    public static final int RIGHT_MARGIN = 100;
    public static final int TOP_MARGIN = 20;
    public static final int BOTTOM_MARGIN = 20;

    private int traceWidth;
    private double frameDuration;
    private List<JobExecutionEvent> jobExecList;
    private KeyedColorMap idToColorMap;
    private BufferedImage image;
    private SouthToolBar sToolBar;
    private Cursor crossHairCursor;
    private Cursor defaultCursor;

    public TraceViewPanel( String fileName, SouthToolBar southToolBar ) {

        traceWidth = 10;
        frameDuration = 1.0;
        image = null;
        sToolBar = southToolBar;
        crossHairCursor = new Cursor( Cursor.CROSSHAIR_CURSOR );
        defaultCursor = new Cursor( Cursor.DEFAULT_CURSOR );
        double smallestJobTime = Double.MAX_VALUE;
        double largestJobTime = -Double.MAX_VALUE;

        // Read the Job execution file to build the jobExecList.
        try {
           BufferedReader in = new BufferedReader( new FileReader(fileName) );
           jobExecList = new ArrayList<JobExecutionEvent>();
           idToColorMap = new KeyedColorMap();
           idToColorMap.readFile("IdToColors.txt");
           Random rand = new Random();
           String line;
           String field[];

           // Strip the header off the CSV file.
           line = in.readLine();
           System.out.println("LINE = \"" + line + "\".\n");

           boolean wasTOF = false;
           double startOfFrame = 0.0;
           double lastStartOfFrame = 0.0;
           double frameSizeSum = 0.0;
           int frameNumber = 0;
           int frameSizeCount = 0;

           while( (line = in.readLine()) !=null) {
                field   = line.split(",");
                String id    = field[0];
                boolean isTOF = false;
                boolean isEOF = false;
                if (Integer.parseInt(field[1]) == 1) isTOF = true;
                if (Integer.parseInt(field[2]) == 1) isEOF = true;
                double start = Double.parseDouble( field[3]);
                double stop  = Double.parseDouble( field[4]);

                if (start < stop) {
                    JobExecutionEvent evt = new JobExecutionEvent(id, isTOF, isEOF, start, stop);
                    jobExecList.add( evt);
                    if (start < smallestJobTime) smallestJobTime = start;
                    if (stop  > largestJobTime)  largestJobTime  = stop;
                    // Calculate the average frame size.
                    if (!wasTOF && isTOF) {
                        startOfFrame = start;
                        if (frameNumber > 0) {
                            double frameSize = (startOfFrame - lastStartOfFrame);
                            frameSizeSum += frameSize;
                            frameSizeCount ++;
                        }
                        lastStartOfFrame = startOfFrame;
                        frameNumber++;
                    }
                    wasTOF = isTOF;
                }
                idToColorMap.addKey(id);
            }

            // Calculate the average frame size.
            frameDuration = frameSizeSum / frameSizeCount;
            idToColorMap.writeFile("IdToColors.txt");

           System.out.println("File loaded.\n");
        } catch ( java.io.FileNotFoundException e ) {
           System.out.println("File not found.\n");
           System.exit(0);
        } catch ( java.io.IOException e ) {
           System.out.println("IO Exception.\n");
           System.exit(0);
        }

        int preferredHeight = traceWidth * (int)((largestJobTime - smallestJobTime) / frameDuration) + TOP_MARGIN;
        setPreferredSize(new Dimension(500, preferredHeight));

        ViewListener viewListener = new ViewListener();
         addMouseListener(viewListener);
         addMouseMotionListener(viewListener);
    }

    public double getFrameDuration() {
        return frameDuration;
    }

    public void setFrameDuration(double duration) {
        frameDuration = duration;
        repaint();
    }

    public void increaseTraceWidth() {
        if (traceWidth < MAX_TRACE_WIDTH) {
            traceWidth ++;
            repaint();
        }
    }

    public void decreaseTraceWidth() {
        if (traceWidth > MIN_TRACE_WIDTH) {
            traceWidth --;
            repaint();
        }
    }

    private boolean traceRectContains(int x, int y) {
        int traceRectXMax = getWidth() - RIGHT_MARGIN;
        if ( x < (LEFT_MARGIN)) return false;
        if ( x > (traceRectXMax)) return false;
        if ( y < TOP_MARGIN) return false;
        return true;
    }

    private boolean timeRectContains(int x, int y) {
        int timeRectXMin = 30;
        int timeRectXMax = LEFT_MARGIN;
        if ( x < 30 ) return false;
        if ( x > LEFT_MARGIN) return false;
        if ( y < TOP_MARGIN) return false;
        return true;
    }

    private class ViewListener extends MouseInputAdapter {
        public void mouseReleased(MouseEvent e) {
            int x = e.getX();
            int y = e.getY();
            Color color = new Color ( image.getRGB(x,y) );

            String id = idToColorMap.getKeyOfColor( color );
            sToolBar.setJobID(id);

            if ( y > TOP_MARGIN) {
                int frameNumber = (y - TOP_MARGIN) / traceWidth;
                sToolBar.setFrameNumber(frameNumber);
            }
            if ( traceRectContains(x, y)) {
                double pixelsPerSecond = (double)calcTraceRectWidth() / frameDuration;
                double subFrameTime = (x - LEFT_MARGIN) / pixelsPerSecond;
                sToolBar.setSubFrameTime(subFrameTime);
            }
        }

        public void mouseMoved(MouseEvent e) {
            int x = e.getX();
            int y = e.getY();
            if ( traceRectContains(x, y)) {
                setCursor(crossHairCursor);
            } else {
                setCursor(defaultCursor);
            }
        }
    }

    private int calcTraceRectHeight() {
        return ( getHeight() - TOP_MARGIN - BOTTOM_MARGIN);
    }

    private int calcTraceRectWidth() {
        return ( getWidth() - LEFT_MARGIN - RIGHT_MARGIN);
    }

    private void doDrawing(Graphics g) {
        Graphics2D g2d = (Graphics2D) g;

        RenderingHints rh = new RenderingHints(
                RenderingHints.KEY_ANTIALIASING,
                RenderingHints.VALUE_ANTIALIAS_ON);

        rh.put(RenderingHints.KEY_RENDERING,
               RenderingHints.VALUE_RENDER_QUALITY);

        int traceRectHeight = calcTraceRectHeight();
        int traceRectWidth = calcTraceRectWidth();
        double pixelsPerSecond = (double)traceRectWidth / frameDuration;

        // Panel Background Color Fill
        g2d.setPaint(Color.WHITE);
        g2d.fillRect(0, 0, getWidth(), getHeight());

        // Frame Trace Rectangle Fill
        g2d.setPaint(Color.BLACK);
        g2d.fillRect(LEFT_MARGIN, TOP_MARGIN, traceRectWidth, traceRectHeight);

        boolean wasEOF = false;
        boolean wasTOF = false;
        double startOfFrame = 0.0;
        int frameNumber = 0;

        for (JobExecutionEvent jobExec : jobExecList ) {

            if (!wasTOF && jobExec.isTOF) {
                startOfFrame = jobExec.start;
                frameNumber ++;
            }

            wasTOF = jobExec.isTOF;
            wasEOF = jobExec.isEOF;

            int jobY = TOP_MARGIN + frameNumber * traceWidth;
            int jobStartX = LEFT_MARGIN + (int)((jobExec.start - startOfFrame) * pixelsPerSecond);
            int jobWidth  = (int)( (jobExec.stop - jobExec.start) * pixelsPerSecond);

            g2d.setPaint(Color.BLACK);
            g2d.drawString ( String.format("%8.3f", startOfFrame), 30, jobY + traceWidth/2);
            g2d.setPaint( idToColorMap.getColor( jobExec.id ) );
            g2d.fillRect(jobStartX, jobY, jobWidth, traceWidth-2);

        } // for
    } // doDrawing

    @Override
    public void paintComponent(Graphics g) {
        super.paintComponent(g);
        image = new BufferedImage(getWidth(), getHeight(), BufferedImage.TYPE_INT_RGB);
        Graphics2D g2 = image.createGraphics();
        doDrawing(g2);
        g.drawImage(image, 0, 0, this);
        g2.dispose();
    }
} // class TraceViewPanel

class NorthToolBar extends JToolBar implements ActionListener {

    private TraceViewPanel traceView;
    private JTextField frameDurationField;

    public NorthToolBar (TraceViewPanel tv) {
        traceView = tv;
        add( new JLabel(" Frame Size: "));
        frameDurationField = new JTextField(15);
        frameDurationField.setText( String.format("%8.4f", traceView.getFrameDuration()) );
        add(frameDurationField);

        JButton setButton = new JButton("Set");
        setButton.addActionListener(this);
        setButton.setActionCommand("setFrameSize");
        setButton.setToolTipText("Set frame size in seconds.");
        add(setButton);
    }
    public void actionPerformed(ActionEvent event) {
        String s = event.getActionCommand();
        switch (s) {
            case "setFrameSize":
                double newFrameSize = 0.0;
                try {
                    newFrameSize = Double.parseDouble( frameDurationField.getText() );
                } catch ( NumberFormatException e) {
                    frameDurationField.setText( String.format("%8.4f", traceView.getFrameDuration()) );
                }
                if ( newFrameSize > 0.0) {
                    traceView.setFrameDuration( newFrameSize );
                }
            break;
            default:
                System.out.println("Unknown Action Command:" + s);
            break;
        }
    }
} // class NorthToolBar

class SouthToolBar extends JToolBar {
    private JTextField IDField;
    private JTextField frameNumberField;
    private JTextField subFrameTimeField;

    public SouthToolBar () {

        add( new JLabel(" Job ID: "));
        IDField = new JTextField(15);
        IDField.setEditable(false);
        IDField.setText( "");
        add(IDField);

        add( new JLabel(" Frame Number: "));
        frameNumberField = new JTextField(15);
        frameNumberField.setEditable(false);
        frameNumberField.setText( "0");
        add(frameNumberField);

        add( new JLabel(" Subframe Time: "));
        subFrameTimeField = new JTextField(15);
        subFrameTimeField.setEditable(false);
        subFrameTimeField.setText( "0.00");
        add(subFrameTimeField);
    }
    public void setJobID(String id) {
        IDField.setText( id );
    }
    public void setFrameNumber(int fn) {
        frameNumberField.setText( String.format("%d", fn));
    }
    public void setSubFrameTime(double time) {
        subFrameTimeField.setText( String.format("%8.4f", time));
    }
} // class SouthToolBar

class PerfMenuBar extends JMenuBar implements ActionListener {

    private TraceViewPanel traceView;

    public PerfMenuBar(TraceViewPanel tv) {
        traceView = tv;

        JMenu fileMenu = new JMenu("File");
        JMenuItem fileMenuExit = new JMenuItem("Exit");
        fileMenuExit.setActionCommand("exit");
        fileMenuExit.addActionListener(this);
        fileMenu.add(fileMenuExit);
        add(fileMenu);

        JMenu optionsMenu = new JMenu("Options");
        JMenu traceSizeMenu = new JMenu("TraceSize");
        JMenuItem traceSizeMenuIncrease = new JMenuItem("Increase Trace Width");
        traceSizeMenuIncrease.setActionCommand("increase-trace_width");
        KeyStroke ctrlPlus  = KeyStroke.getKeyStroke('P', InputEvent.CTRL_MASK );
        traceSizeMenuIncrease.setAccelerator(ctrlPlus);
        traceSizeMenuIncrease.addActionListener(this);
        traceSizeMenu.add(traceSizeMenuIncrease);
        JMenuItem traceSizeMenuDecrease = new JMenuItem("Decrease Trace Width");
        traceSizeMenuDecrease.setActionCommand("decrease-trace_width");
        KeyStroke ctrlMinus = KeyStroke.getKeyStroke('-', InputEvent.CTRL_MASK);
        traceSizeMenuDecrease.setAccelerator(ctrlMinus);
        traceSizeMenuDecrease.addActionListener(this);
        traceSizeMenu.add(traceSizeMenuDecrease);
        optionsMenu.add(traceSizeMenu);
        add(optionsMenu);

    }
    public void actionPerformed(ActionEvent e) {
        String s = e.getActionCommand();
        switch (s) {
            case "increase-trace_width":
                traceView.increaseTraceWidth();
            break;
            case "decrease-trace_width":
                traceView.decreaseTraceWidth();
            break;
            case "exit":
                System.exit(0);
            default:
                System.out.println("Unknown Action Command:" + s);
            break;
        }
    }
} // class PerfMenuBar

public class JobPerf extends JFrame {

    private List<JobExecutionEvent> JobExecutionEventlist;

    public JobPerf( TraceViewPanel traceView, SouthToolBar southToolBar) {

        PerfMenuBar menuBar = new PerfMenuBar(traceView);
        setJMenuBar(menuBar);

        NorthToolBar nToolBar = new NorthToolBar( traceView );
        add(nToolBar, BorderLayout.NORTH);

        JScrollPane scrollPane = new JScrollPane( traceView );
        scrollPane.setPreferredSize(new Dimension(800, 400));

        JPanel tracePanel = new JPanel();
        tracePanel.setPreferredSize(new Dimension(800, 400));
        tracePanel.add(scrollPane);
        tracePanel.setLayout(new BoxLayout(tracePanel, BoxLayout.X_AXIS));

        JPanel mainPanel  = new JPanel();
        mainPanel.setLayout(new BoxLayout(mainPanel, BoxLayout.Y_AXIS));
        mainPanel.add(tracePanel);

        add(southToolBar, BorderLayout.SOUTH);

        setTitle("JobPerf");
        setSize(800, 500);
        add(mainPanel);
        pack();
        setVisible(true);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setFocusable(true);
        setVisible(true);

        traceView.repaint();
    }

    private static void  printHelpText() {
        System.out.println(
            "----------------------------------------------------------------------\n"
          + "usage: java jar JobPerf.jar <file-name>\n"
          + "----------------------------------------------------------------------\n"
          );
    }

    public static void main(String[] args) {
        int ii = 0;
        String fileName = "in.csv";
        while (ii < args.length) {
            switch (args[ii]) {
                case "-help" :
                case "--help" : {
                    printHelpText();
                    System.exit(0);
                } break;
                default : {
                    fileName = args[ii];
                } break;
            }
            ++ii;
        } // while

        SouthToolBar sToolBar = new SouthToolBar();
        TraceViewPanel traceView = new TraceViewPanel( fileName, sToolBar);
        JobPerf jobPerf = new JobPerf(traceView, sToolBar);

    } // main
} // class JobPerf
