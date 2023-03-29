/*
 * Trick
 * 2023 (c) National Aeronautics and Space Administration (NASA)
 */
import java.awt.*;
import java.awt.event.MouseEvent;
import javax.swing.event.MouseInputAdapter;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
import java.util.*;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.sound.sampled.*;
import java.net.URL;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.geom.AffineTransform;

import javax.swing.text.NumberFormatter;
import java.text.NumberFormat;
import javax.swing.JFormattedTextField;
import java.awt.Dimension;

import javax.swing.BorderFactory;
import javax.swing.border.EtchedBorder;
import java.awt.Component;
/**
 *
 * @author penn
 */

class Link {
  public double l; // length of the link (meters).
  public double q; // joint angle (radians).

  public Link() {
    l = 1.0;
    q = 0.0;
  }
}

class ScenePoly {
    public Color color;
    public int n;
    public double[] x;
    public double[] y;
}

class Point {
    public double x;
    public double y;
}

class PathTrace {
    int capacity;
    public Point[] path;
    public int head;
    public int tail;
    public PathTrace() {
        capacity = 200;
        path = new Point[capacity];
        head = 0;
        tail = 0;
    }
    public void insert (double x, double y) {
        if (path[tail] == null) {
            path[tail] = new Point();
        }
        path[tail].x = x;
        path[tail].y = y;
        tail = (tail + 1) % capacity;
        if (tail == head) head = (head + 1) % capacity;
    }
    public void clear() {
        head = tail = 0;
    }
    public int count () {
        if (head == tail) return 0;
        if (head < tail) {
            return (tail - head);
        } else {
            return (tail - head + capacity);
        }
    }
    public int advance (int i ) {
        i = (i + 1) % capacity;
        return i;
    }
    public int begin () { return head; }
    public int end () { return tail; }
    public Point get(int i) {
        return path[i];
    }
}

class RangeView extends JPanel {

    private int scale;
    int nlinks;
    private Color sunGlowColor;
    private Color grey200Color;
    private Color grey150Color;
    private Color pathTraceColor;

    private ScenePoly link_arm;

    // Origin of world coordinates in jpanel coordinates.
    private int worldOriginX;
    private int worldOriginY;

    public Link[] links;

    // private ScenePoly upright;

    private int[] workPolyX, workPolyY;

    public PathTrace pathTrace;
    public boolean pathTraceEnabled;
    /**
     * Class constructor.
     */
    public RangeView( int mapScale, int numberOfLinks) {

      setScale(mapScale);
      nlinks = numberOfLinks;

      sunGlowColor = new Color(255,204,51);
      grey200Color = new Color(200,200,200);
      grey150Color = new Color(150,150,150);
      pathTraceColor = new Color(85,20,230);

      links = new Link[nlinks];
      for (int ii=0 ; ii<nlinks ; ii++) {
          links[ii] = new Link();
      }

      link_arm = new ScenePoly();
      link_arm.color = grey200Color;
      link_arm.n = 4;
      link_arm.x = new double[4];
      link_arm.y = new double[4];

      workPolyX = new int[30];
      workPolyY = new int[30];

      pathTrace = new PathTrace();
      pathTraceEnabled = false;

    }

    public void drawScenePoly(Graphics2D g, ScenePoly p, double angle_r , double x, double y) {
        for (int ii = 0; ii < p.n; ii++) {
            workPolyX[ii] = (int)(worldOriginX + scale *
                ( Math.cos(angle_r) * p.x[ii] - Math.sin(angle_r) * p.y[ii] + x));
            workPolyY[ii] = (int)(worldOriginY - scale *
                ( Math.sin(angle_r) * p.x[ii] + Math.cos(angle_r) * p.y[ii] + y));
        }
        g.setPaint(p.color);
        g.fillPolygon(workPolyX, workPolyY, p.n);
    }

    public void setScale (int mapScale) {
        if (mapScale < 2) {
            scale = 2;
        } else if (mapScale > 512) {
            scale = 512;
        } else {
            scale = mapScale;
        }
        repaint();
    }

    public int getScale() {
        return scale;
    }

    public void drawCenteredCircle(Graphics2D g, int x, int y, int d) {
        x = x-(d/2);
        y = y-(d/2);
        g.drawOval(x,y,d,d);
    }

    public void fillCenteredCircle(Graphics2D g, int x, int y, int d) {
        x = x-(d/2);
        y = y-(d/2);
        g.fillOval(x,y,d,d);
    }

    private void doDrawing(Graphics g) {
        Graphics2D g2d = (Graphics2D) g;

        RenderingHints rh = new RenderingHints(
                RenderingHints.KEY_ANTIALIASING,
                RenderingHints.VALUE_ANTIALIAS_ON);

        rh.put(RenderingHints.KEY_RENDERING,
               RenderingHints.VALUE_RENDER_QUALITY);

        int width  = getWidth();
        int height = getHeight();

        worldOriginX = (width/2);
        worldOriginY = (height/2);

        // ===============================================================================
        //  Draw Robot
        // ===============================================================================
        double q_sum = 0.0;
        double l_sum = 0.0;
        double base_x = 0.0;
        double base_y = 0.0;
        double end_x = 0.0;
        double end_y = 0.0;

        double base_width;
        double end_width;

        // Find the total length of the links
        double extended_length = 0.0;
        for (int ii=0 ; ii<nlinks ; ii++) {
            extended_length += links[ii].l;
        }
        base_width = 0.10 * extended_length;

        g2d.setPaint(Color.RED);
        drawCenteredCircle(g2d,
            (int)(worldOriginX),
            (int)(worldOriginY),
            (int)(scale * 2 * extended_length )
         );

        for (int ii=0 ; ii<nlinks ; ii++) {
            q_sum += links[ii].q;
            l_sum += links[ii].l;

            // Calculate the width of the current link.
            end_width = 0.10 * (extended_length - l_sum);

            // Calculate the position of the end of the current link.
            end_x = base_x + links[ii].l * Math.cos(q_sum);
            end_y = base_y + links[ii].l * Math.sin(q_sum);

            // SLOPING LINKS
            // link_arm.x[0] = 0.0;          link_arm.y[0] =  base_width/2.0;
            // link_arm.x[1] = links[ii].l;  link_arm.y[1] =  end_width/2.0;
            // link_arm.x[2] = links[ii].l;  link_arm.y[2] = -end_width/2.0;
            // link_arm.x[3] = 0.0;          link_arm.y[3] = -base_width/2.0;

            // STRAIGHT LINKS
            link_arm.x[0] = 0.0;          link_arm.y[0] =  base_width/2.0;
            link_arm.x[1] = links[ii].l;  link_arm.y[1] =  base_width/2.0;
            link_arm.x[2] = links[ii].l;  link_arm.y[2] = -base_width/2.0;
            link_arm.x[3] = 0.0;          link_arm.y[3] = -base_width/2.0;

            drawScenePoly(g2d, link_arm, q_sum, base_x, base_y);

            g2d.setPaint(sunGlowColor);
            fillCenteredCircle(g2d,
                (int)(worldOriginX + scale * base_x),
                (int)(worldOriginY - scale * base_y),
                (int)(scale * base_width )
            );

            g2d.setPaint(grey150Color);
            fillCenteredCircle(g2d,
               (int)(worldOriginX + scale * base_x),
               (int)(worldOriginY - scale * base_y),
               (int)(scale * 0.5 * base_width )
            );

            // The base position of the next link is the end position of the current link.
            base_x = end_x;
            base_y = end_y;
            base_width = end_width;
        }

        if (pathTraceEnabled) {
            // Save <end_x, end_y> into the path tracing array (circular).
            pathTrace.insert(end_x, end_y);

            // Render the end-effector path trace.
            if ( pathTrace.count() > 1 ) {
                int x1, y1, x2, y2;
                g2d.setPaint(pathTraceColor);
                int i = pathTrace.begin();
                Point p1 = pathTrace.get(i);
                x1 = (int)(worldOriginX + scale * p1.x);
                y1 = (int)(worldOriginY - scale * p1.y);
                for (i = pathTrace.advance(i); i != pathTrace.end() ; i = pathTrace.advance(i)) {
                    Point p2 = pathTrace.get(i);
                    x2 = (int)(worldOriginX + scale * p2.x);
                    y2 = (int)(worldOriginY - scale * p2.y);
                    g2d.drawLine(x1, y1, x2, y2);
                    x1 = x2;
                    y1 = y2;
                }
            }
        }

        // ===============================================================================
        // Draw Information
        // ===============================================================================
        g2d.setPaint(Color.BLACK);
        g2d.drawString ( String.format("SCALE: %d pixels/meter",scale), 20,20);
    }

    @Override
    public void paintComponent(Graphics g) {
        super.paintComponent(g);
        doDrawing(g);
    }
}

class TrickSimMode {
    public static final int INIT = 0;
    public static final int FREEZE = 1;
    public static final int RUN = 5;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class VelocityCtrlPanel extends JPanel {
    private RangeView rangeView;
    private JButton traceButton;
    private int mouse_x, mouse_y;
    private Color padColor, arrowColor;
    private int padDiameter;
    private int padCenterX, padCenterY;

    public VelocityCtrlPanel(RangeView view) {
        padDiameter = 100;
        padCenterX = padDiameter/2;
        padCenterY = padDiameter/2;
        rangeView = view;
        padColor = new Color(150,150,150);
        arrowColor = new Color(50,30,140);
        mouse_x = padCenterX;
        mouse_y = padCenterY;
        ViewListener viewListener = new ViewListener();
        addMouseListener(viewListener);
        addMouseMotionListener(viewListener);
        setPreferredSize(new Dimension(padDiameter, padDiameter));
        setMaximumSize(  new Dimension(padDiameter, padDiameter));
    }

    @Override
    public void paintComponent(Graphics g) {
        super.paintComponent(g);
        Graphics2D g2d = (Graphics2D) g;
        g2d.setPaint(padColor);
        g2d.fillOval(0, 0, padDiameter, padDiameter);
        g2d.setPaint(arrowColor);
        g2d.drawLine(padCenterX, padCenterY, mouse_x, mouse_y);
    }

    private class ViewListener extends MouseInputAdapter {
        public void mouseReleased(MouseEvent e) {
          mouse_x = e.getX();
          mouse_y = e.getY();
          repaint();
        }
    }
} // class VelocityCtrlPanel

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class TraceCtrlPanel extends JPanel implements ActionListener {
    private RangeView rangeView;
    private JButton traceButton;
    public TraceCtrlPanel(RangeView view) {
        rangeView = view;
        setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

        traceButton = new JButton("Trace");
        traceButton.addActionListener(this);
        traceButton.setActionCommand("trace");
        traceButton.setToolTipText("Trace end-effector");
        add(traceButton);

    }
    public void actionPerformed(ActionEvent e) {
        String s = e.getActionCommand();
        switch (s) {
            case "trace":
                rangeView.pathTraceEnabled = !rangeView.pathTraceEnabled;
                if (rangeView.pathTraceEnabled) {
                    rangeView.pathTrace.clear();
                    traceButton.setForeground(Color.GREEN);
                } else {
                    traceButton.setForeground(Color.BLACK);
                }
                break;
            default:
                System.out.println("Unknown Action Command:" + s);
                break;
        }
    }
} // class TraceCtrlPanel

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class ZoomCtrlPanel extends JPanel implements ActionListener {
    private RangeView rangeView;
    private JButton zoomOutButton, zoomInButton;
    public ZoomCtrlPanel(RangeView view) {
        rangeView = view;
        setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

        zoomOutButton = new JButton("Out");
        zoomOutButton.addActionListener(this);
        zoomOutButton.setActionCommand("zoomout");
        zoomOutButton.setToolTipText("Zoom Out");
        add(zoomOutButton);

        zoomInButton = new JButton("In");
        zoomInButton.addActionListener(this);
        zoomInButton.setActionCommand("zoomin");
        zoomInButton.setToolTipText("Zoom In");
        add(zoomInButton);
    }
    public void actionPerformed(ActionEvent e) {
        String s = e.getActionCommand();
        switch (s) {
            case "zoomout":
                rangeView.setScale( rangeView.getScale() / 2 );
                break;
            case "zoomin":
                rangeView.setScale( rangeView.getScale() * 2 );
                break;
            default:
                System.out.println("Unknown Action Command:" + s);
                break;
        }
    }
} // class ZoomCtrlPanel

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class ControlPanel extends JPanel {

    private RangeView rangeView;
    private ZoomCtrlPanel zoomCtrlPanel;
    private TraceCtrlPanel traceCtrlPanel;
    private VelocityCtrlPanel velocityCtrlPanel;

    // private ManualCtrlPanel manualCtrlPanel;

    public ControlPanel(RangeView view) {

        rangeView = view;
        setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

        JPanel labeledZoomCtrlPanel = new JPanel();
        labeledZoomCtrlPanel.setBorder( BorderFactory.createEtchedBorder(EtchedBorder.RAISED));
        labeledZoomCtrlPanel.setLayout(new BoxLayout(labeledZoomCtrlPanel, BoxLayout.Y_AXIS));

        JLabel zoomControlLabel = new JLabel("Zoom");
        zoomControlLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
        labeledZoomCtrlPanel.add(zoomControlLabel);

        zoomCtrlPanel = new ZoomCtrlPanel(rangeView);
        labeledZoomCtrlPanel.add( zoomCtrlPanel );
        add(labeledZoomCtrlPanel);

        traceCtrlPanel = new TraceCtrlPanel(rangeView);
        add(traceCtrlPanel);

        velocityCtrlPanel = new VelocityCtrlPanel(rangeView);
        add(velocityCtrlPanel);
    }
} // class ControlPanel
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public class RobotDisplay extends JFrame {

    private RangeView rangeView;
    private BufferedReader in;
    private DataOutputStream out;
    private JPanel mainPanel;
    private JPanel viewPanel;
    private ControlPanel controlPanel;

    public RobotDisplay() {
        rangeView  = null;
        setTitle("Robot Range");
        setLocationRelativeTo(null);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setFocusable(true);
    }

    public void connectToServer(String host, int port ) throws IOException {
        Socket socket = new Socket(host, port);
        in = new BufferedReader( new InputStreamReader( socket.getInputStream()));
        out = new DataOutputStream(new BufferedOutputStream(socket.getOutputStream()));
    }

    public void createGUI( int mapScale, int numberOfLinks ) {

        setTitle("Robot Range");
        rangeView = new RangeView(mapScale, numberOfLinks);

        JPanel viewPanel = new JPanel();
        viewPanel.setLayout(new BoxLayout(viewPanel, BoxLayout.X_AXIS));
        viewPanel.setBorder( BorderFactory.createEtchedBorder(EtchedBorder.RAISED));
        viewPanel.add(rangeView);

        ControlPanel controlPanel = new ControlPanel(rangeView);

        JPanel mainPanel = new JPanel();
        mainPanel.setLayout(new BoxLayout(mainPanel, BoxLayout.Y_AXIS));
        mainPanel.setBorder( BorderFactory.createEtchedBorder(EtchedBorder.RAISED));
        mainPanel.add(viewPanel);
        mainPanel.add(controlPanel);
        viewPanel.setPreferredSize(new Dimension(800, 650));
        controlPanel.setPreferredSize(new Dimension(800, 150));

        add(mainPanel);
        pack();
        setVisible(true);
    }

    public void drawRangeView() {
        rangeView.repaint();
    }

    private static void  printHelpText() {
        System.out.println(
            "----------------------------------------------------------------------\n"
          + "usage: java jar RobotDisplay.jar <port-number>\n"
          + "----------------------------------------------------------------------\n"
          );
    }

    public enum ModelState { INACTIVE, READY, ACTIVE }

    public static void main(String[] args) throws IOException, InterruptedException {

        String host = "localHost";
        int port = 0;
        boolean boom = false;

        // ==========================================================
        // Handle program arguments.
        // ==========================================================
        int ii = 0;
        while (ii < args.length) {
            switch (args[ii]) {
                case "-help" :
                case "--help" : {
                    printHelpText();
                    System.exit(0);
                } break;
                default : {
                    port = (Integer.parseInt(args[ii]));
                } break;
            }
            ++ii;
        }

        boolean go = true;
        double  dt = 0.100; // Time between updates (seconds).

        int nlinks = 0;
        boolean standalone = false;

        int mapScale = 128 ; // pixels per meter.

        if (port == 0) {
            System.out.println("No variable server port specified.");
            printHelpText();
            System.exit(0);
        }

       RobotDisplay robotDisplay = new RobotDisplay();

        // Connect to the Trick simulation's variable server
        System.out.println("Connecting to: " + host + ":" + port);
        robotDisplay.connectToServer(host, port);
        robotDisplay.out.writeBytes("trick.var_set_client_tag(\"RobotDisplay\") \n");
        robotDisplay.out.flush();

        // Have the Variable Server send us the number of links once.
        robotDisplay.out.writeBytes(
            "trick.var_add(\"dyn.arm.nlinks\")\n" +
            "trick.var_send() \n" +
            "trick.var_clear() \n");
        robotDisplay.out.flush();
        try {
            String line;
            String field[];
            line = robotDisplay.in.readLine();
            field = line.split("\t");
            nlinks  = Integer.parseInt( field[1]);
        } catch (IOException | NullPointerException e ) {
            go = false;
        }

        System.out.println("Number of Links: " + nlinks);

        robotDisplay.createGUI(mapScale, nlinks);
        //robotDisplay.setSize(800, 800);

        // Configure the Variable Server to cyclically send data.
        robotDisplay.out.writeBytes( "trick.var_pause() \n");
        for ( ii = 0; ii < nlinks; ii ++) {
            robotDisplay.out.writeBytes(
                  String.format("trick.var_add(\"dyn.arm.links[%d][0].l\")\n", ii)
                + String.format("trick.var_add(\"dyn.arm.links[%d][0].q\")\n", ii)
            );
        }
        robotDisplay.out.writeBytes(
          "trick.var_ascii() \n" +
          String.format("trick.var_cycle(%.3f)\n", dt) +
          "trick.var_unpause() \n" );
        robotDisplay.out.flush();

        //  Update the scene.
        robotDisplay.drawRangeView();

        while (go) {

            // Recieve and parse periodic data response from the variable server.
            try {
                String line;
                String field[];
                line = robotDisplay.in.readLine();
                field   = line.split("\t");

                for ( ii=0; ii < nlinks; ii++) {
                    robotDisplay.rangeView.links[ii].l = Double.parseDouble( field[ii*2 +1]);
                    //System.out.println("robotDisplay.rangeView.links["+ ii+ "].l: = " + robotDisplay.rangeView.links[ii].l);
                    robotDisplay.rangeView.links[ii].q = Double.parseDouble( field[ii*2 +2]);
                    //System.out.println("robotDisplay.rangeView.links["+ ii+ "].q: = " + robotDisplay.rangeView.links[ii].q);
                }
            } catch (IOException | NullPointerException e ) {
                go = false;
            }

            //  Update the scene.
            robotDisplay.drawRangeView();

        } // while
    } // main
} // class
