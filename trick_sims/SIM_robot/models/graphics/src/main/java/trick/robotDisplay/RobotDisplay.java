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
//------------------------------------------------------------------------------
 class RobotCtrlMode {
     public static final int NOPE = 0;
     public static final int CANCEL = 1;
     public static final int SINGLE_JOINT = 2;
     public static final int MANUAL = 3;
     public static final int EEPOS = 4;
 }

//------------------------------------------------------------------------------
class Link {
  public double l; // length of the link (meters).
  public double q; // joint angle (radians).

  public Link() {
    l = 1.0;
    q = 0.0;
  }
}

//------------------------------------------------------------------------------
class ScenePoly {
    public Color color;
    public int n;
    public double[] x;
    public double[] y;
}

//------------------------------------------------------------------------------
class Point {
    public double x;
    public double y;
}

//------------------------------------------------------------------------------
class PathTrace {
    int capacity;
    public Point[] path;
    public int head;
    public int tail;
    public PathTrace() {
        capacity = 1000;
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

//------------------------------------------------------------------------------
class RangeView extends JPanel {

    private int scale;
    private Color sunGlowColor;
    private Color grey200Color;
    private Color grey150Color;
    private Color pathTraceColor;

    // Origin of world coordinates in jpanel coordinates.
    private int worldOriginX;
    private int worldOriginY;
    private int[] workPolyX, workPolyY;
    private ScenePoly link_arm;

    public int nlinks;
    public Link[] links;
    public PathTrace pathTrace;
    public boolean pathTraceEnabled;
    private int selectedJoint;
    public Point desiredPosition;

    /**
     * Class constructor.
     */
    public RangeView( int mapScale, int numberOfLinks) {

      setScale(mapScale);

      sunGlowColor = new Color(255,204,51);
      grey200Color = new Color(200,200,200);
      grey150Color = new Color(150,150,150);
      pathTraceColor = new Color(85,20,230);

      workPolyX = new int[30];
      workPolyY = new int[30];

      link_arm = new ScenePoly();
      link_arm.color = grey200Color;
      link_arm.n = 4;
      link_arm.x = new double[4];
      link_arm.y = new double[4];

      nlinks = numberOfLinks;
      links = new Link[nlinks];
      for (int ii=0 ; ii<nlinks ; ii++) {
          links[ii] = new Link();
      }

      pathTrace = new PathTrace();
      pathTraceEnabled = false;

      selectedJoint = 0;

      desiredPosition = new Point();
      desiredPosition.x = -0.5;
      desiredPosition.y = -0.5;

      ViewListener viewListener = new ViewListener();
      addMouseListener(viewListener);
      addMouseMotionListener(viewListener);

    }

    private class ViewListener extends MouseInputAdapter {
        public void mouseReleased(MouseEvent e) {
          desiredPosition.x =  (double)(e.getX() - worldOriginX) / (double)scale;
          desiredPosition.y = -(double)(e.getY() - worldOriginY) / (double)scale;
          repaint();
        }
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

    public void setSelectedJoint (int which) {
        selectedJoint = which;
        repaint();
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

         g2d.setPaint(Color.GREEN);
         fillCenteredCircle(g2d,
             (int)(worldOriginX + scale * desiredPosition.x),
             (int)(worldOriginY - scale * desiredPosition.y),
             (int)(scale * 0.2));

        for (int ii=0 ; ii<nlinks ; ii++) {
            q_sum += links[ii].q;
            l_sum += links[ii].l;

            // Calculate the width of the current link.
            end_width = 0.10 * (extended_length - l_sum);

            // Calculate the position of the end of the current link.
            end_x = base_x + links[ii].l * Math.cos(q_sum);
            end_y = base_y + links[ii].l * Math.sin(q_sum);

            link_arm.x[0] = 0.0;          link_arm.y[0] =  base_width/2.0;
            link_arm.x[1] = links[ii].l;  link_arm.y[1] =  base_width/2.0;
            link_arm.x[2] = links[ii].l;  link_arm.y[2] = -base_width/2.0;
            link_arm.x[3] = 0.0;          link_arm.y[3] = -base_width/2.0;

            drawScenePoly(g2d, link_arm, q_sum, base_x, base_y);

            if (ii == selectedJoint) {
                g2d.setPaint(sunGlowColor);
            } else {
                g2d.setPaint(Color.GRAY);
            }

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
} // class RangeView

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class TrickSimMode {
    public static final int INIT = 0;
    public static final int FREEZE = 1;
    public static final int RUN = 5;
} // class TrickSimMode

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class VelocityCtrlPanel extends JPanel {
    private RangeView rangeView;
    private int mouse_x, mouse_y;
    private int padDiameter;
    private float pMag;
    private float nMag, vx, vy;
    public static final int padRadius = 50;

    public VelocityCtrlPanel(RangeView view) {
        rangeView = view;
        padDiameter = 2*padRadius;
        clear();

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
        g2d.setPaint(Color.LIGHT_GRAY);
        g2d.fillOval(0, 0, padDiameter, padDiameter);
        g2d.setPaint(Color.WHITE);
        g2d.fillOval(padRadius - (int)pMag,
                     padRadius - (int)pMag,
                     (int)(pMag*2),
                     (int)(pMag*2));
        g2d.setPaint(Color.BLACK);
        g2d.drawLine(padRadius, padRadius, mouse_x, mouse_y);
    }
    private class ViewListener extends MouseInputAdapter {
        public void mouseReleased(MouseEvent e) {
          int x = e.getX() - padRadius;
          int y = e.getY() - padRadius;

          pMag = (float)Math.sqrt((double)(x*x+y*y));
          vx = (float)(x)/pMag;
          vy = (float)(y)/pMag;
          //System.out.println("<vx, vy> = <" + vx + ", " + vy + ">.");
          // Restrict the vector to the pad circle.
          if (pMag > padRadius) pMag = (float)(padRadius);
          // Normalize the vector magnitude to the range (0..1).
          nMag = pMag/(float)padRadius;
          //System.out.println("nMag = " + nMag + ".");
          mouse_x = (int)(vx * pMag) + padRadius;
          mouse_y = (int)(vy * pMag) + padRadius;

          repaint();

        }
    }
    public float getXvalue() { return  (nMag *  vx); }
    public float getYvalue() { return  (nMag * -vy); }
    public float getMagnitude() { return nMag; }
    public void  clear() {
        mouse_x = padRadius;
        mouse_y = padRadius;
        nMag = (float)0.0;
        vx = (float)0.0;
        vy = (float)0.0;
        repaint();
    }
} // class VelocityCtrlPanel

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class JointRatePanel extends JPanel implements ChangeListener {
    private RangeView rangeView;
    static final int DPS_MIN = -15;
    static final int DPS_MAX =  15;
    static final int DPS_INIT =  0;
    private int value;
    JSlider degreesPerSecond;
    public JointRatePanel(RangeView view) {
        rangeView = view;
        value = DPS_INIT;
        setLayout(new BoxLayout(this, BoxLayout.X_AXIS));
        setPreferredSize(new Dimension(200, 50));
        setMaximumSize(  new Dimension(200, 50));
        degreesPerSecond = new JSlider(JSlider.HORIZONTAL, DPS_MIN, DPS_MAX, DPS_INIT);
        degreesPerSecond.addChangeListener(this);
        degreesPerSecond.setMajorTickSpacing(10);
        degreesPerSecond.setMinorTickSpacing(1);
        degreesPerSecond.setPaintTicks(true);
        degreesPerSecond.setPaintLabels(true);
        degreesPerSecond.setToolTipText("Joint Rate Selector");
        add(degreesPerSecond);
    }
    public void stateChanged(ChangeEvent e) {
        JSlider source = (JSlider)e.getSource();
        if (!source.getValueIsAdjusting()) {
            value = (int)source.getValue();
            System.out.println("Joint Rate : " + value);
        }
    }
    public double getValue() { return ((double)(value) * (Math.PI/180.0)) ; }
    public void setValue( int val) {
        value = val;
        degreesPerSecond.setValue( value);
    }
    public void clear() { setValue(0); }
} // class JointRatePanel

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class JointSelectPanel extends JPanel implements ActionListener {
    private RangeView rangeView;
    JLabel numberLabel;
    int count;
    int value;
    int desiredValue;
    public JointSelectPanel(RangeView view) {
        rangeView = view;
        count = rangeView.nlinks;

        setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

        JButton decButton = new JButton("\u25c0");
        decButton.addActionListener(this);
        decButton.setActionCommand("decrement");
        decButton.setToolTipText("Decrement");
        add(decButton);

        numberLabel = new JLabel("");
        add(numberLabel);

        JButton incButton = new JButton("\u25b6");
        incButton.addActionListener(this);
        incButton.setActionCommand("increment");
        incButton.setToolTipText("Increment");
        add(incButton);

        setValue(0);
    }
    public void actionPerformed(ActionEvent e) {
        String s = e.getActionCommand();
        switch (s) {
            case "increment":
                // setValue( value + 1 );
                desiredValue = ( value + count + 1 ) % count;
                break;
            case "decrement":
                // setValue( value - 1 );
                desiredValue = ( value + count + 1 ) % count;
                break;
            default:
                System.out.println("Unknown Action Command:" + s);
                break;
        }
    }
    public int getValue() { return desiredValue; }
    public void setValue(int val) {
        // NOTE: the expression below looks weird because the result
        // of the Java mod operator isn't necessarily positive.
        value = ( (val % count) + count) % count;
        numberLabel.setText("" + value);
        rangeView.setSelectedJoint(value);
    }
    public void clear() {
        setValue(0);
    }
} // class JointSelectPanel

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class JointCtrlPanel extends JPanel {
    private RangeView rangeView;
    public JointRatePanel jointRatePanel;
    public JointSelectPanel jointSelectPanel;

    public JointCtrlPanel(RangeView view) {
        rangeView = view;
        setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
        JLabel jointRateCtrlLabel = new JLabel("Joint Rate (deg/s)");
        jointRateCtrlLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
        jointRatePanel = new JointRatePanel(rangeView);
        JLabel jointSelectLabel = new JLabel("Joint Select");
        jointSelectLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
        jointSelectPanel = new JointSelectPanel( rangeView);
        add(jointRateCtrlLabel);
        add(jointRatePanel);
        add(jointSelectLabel);
        add(jointSelectPanel);
    }
} // class JointCtrlPanel

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class ModeCtrlPanel extends JPanel implements ActionListener {
    private RangeView rangeView;
    JButton eePosButton;
    JButton manualButton;
    JButton singleJointButton;
    JButton cancelButton;
    int desired_mode;

    public ModeCtrlPanel(RangeView view) {
        rangeView = view;

        int desired_mode = RobotCtrlMode.NOPE;

        setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));

        JPanel modeSelectPanel = new JPanel();
        modeSelectPanel.setLayout( new BoxLayout(modeSelectPanel, BoxLayout.Y_AXIS));
        modeSelectPanel.setBorder( BorderFactory.createEtchedBorder(EtchedBorder.RAISED));

        eePosButton = new JButton("EEPos");
        eePosButton.addActionListener(this);
        eePosButton.setActionCommand("eepos");
        eePosButton.setToolTipText("End-effector Position Mode");
        modeSelectPanel.add(eePosButton);

        manualButton = new JButton("Manual");
        manualButton.addActionListener(this);
        manualButton.setActionCommand("manual");
        manualButton.setToolTipText("Manual Mode");
        modeSelectPanel.add(manualButton);

        singleJointButton = new JButton("SJoint");
        singleJointButton.addActionListener(this);
        singleJointButton.setActionCommand("sjoint");
        singleJointButton.setToolTipText("Single Joint Mode");
        modeSelectPanel.add(singleJointButton);

        add(modeSelectPanel);

        cancelButton = new JButton("Cancel");
        cancelButton.addActionListener(this);
        cancelButton.setActionCommand("mode_cancel");
        cancelButton.setToolTipText("Cancel");

        add(cancelButton);

    }
    public void actionPerformed(ActionEvent e) {
        String s = e.getActionCommand();
        switch (s) {
            case "eepos":
                desired_mode = RobotCtrlMode.EEPOS;
                break;
            case "manual":
                desired_mode = RobotCtrlMode.MANUAL;
                break;
            case "sjoint":
                desired_mode = RobotCtrlMode.SINGLE_JOINT;
                break;
            case "mode_cancel":
                desired_mode = RobotCtrlMode.CANCEL;
                break;
            default:
                System.out.println("Unknown Action Command:" + s);
                break;
        }
    }
    public void clear() {
        // desired_mode = RobotCtrlMode.NOPE;
        eePosButton.setForeground(Color.BLACK);
        manualButton.setForeground(Color.BLACK);
        singleJointButton.setForeground(Color.BLACK);
    }
    public void setMode(int mode) {
        clear();
        if (mode == RobotCtrlMode.SINGLE_JOINT) {
            singleJointButton.setForeground(Color.GREEN);
        } else if (mode == RobotCtrlMode.MANUAL) {
            manualButton.setForeground(Color.GREEN);
        } else if (mode == RobotCtrlMode.EEPOS) {
            eePosButton.setForeground(Color.GREEN);
        }
    }
    public int desiredMode() {
        return desired_mode;
    }
    public void resetDesiredMode() {
        desired_mode = RobotCtrlMode.NOPE;
    }

} // class ModeCtrlPanel

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class TraceCtrlPanel extends JPanel implements ActionListener {
    private RangeView rangeView;
    JButton traceButton;

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

    public ZoomCtrlPanel(RangeView view) {
        rangeView = view;
        setBorder( BorderFactory.createEtchedBorder(EtchedBorder.RAISED));
        setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
        JButton zoomInButton = new JButton("+");
        zoomInButton.addActionListener(this);
        zoomInButton.setActionCommand("zoomin");
        zoomInButton.setToolTipText("Zoom In");
        add(zoomInButton);
        JButton zoomOutButton = new JButton("-");
        zoomOutButton.addActionListener(this);
        zoomOutButton.setActionCommand("zoomout");
        zoomOutButton.setToolTipText("Zoom Out");
        add(zoomOutButton);
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

    public ModeCtrlPanel modeCtrlPanel;
    public JointCtrlPanel jointCtrlPanel;
    public VelocityCtrlPanel velocityCtrlPanel;

    public ControlPanel(RangeView view) {

        rangeView = view;
        setLayout(new BoxLayout(this, BoxLayout.X_AXIS));
        // ---------------------------------------------------------------------
        JLabel zoomControlLabel = new JLabel("Zoom");
        zoomControlLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
        ZoomCtrlPanel zoomCtrlPanel = new ZoomCtrlPanel(rangeView);
        zoomCtrlPanel.setAlignmentX(Component.CENTER_ALIGNMENT);
        JPanel labeledZoomCtrlPanel = new JPanel();
        labeledZoomCtrlPanel.setLayout( new BoxLayout(labeledZoomCtrlPanel, BoxLayout.Y_AXIS));
        labeledZoomCtrlPanel.add(zoomControlLabel);
        labeledZoomCtrlPanel.add( zoomCtrlPanel );
        add(labeledZoomCtrlPanel);
        // ---------------------------------------------------------------------
        TraceCtrlPanel traceCtrlPanel = new TraceCtrlPanel(rangeView);
        add(traceCtrlPanel);
        // ---------------------------------------------------------------------
        modeCtrlPanel = new ModeCtrlPanel(rangeView);
        add(modeCtrlPanel);
        // ---------------------------------------------------------------------
        JLabel velCtrlLabelTop = new JLabel("EE Velocity");
        velCtrlLabelTop.setAlignmentX(Component.CENTER_ALIGNMENT);

        JLabel velCtrlLabelBottom = new JLabel("[0..1] (m/s)");
        velCtrlLabelBottom.setAlignmentX(Component.CENTER_ALIGNMENT);

        velocityCtrlPanel = new VelocityCtrlPanel(rangeView);
        add(velocityCtrlPanel);

        JPanel labeledVelCtrlPanel = new JPanel();
        labeledVelCtrlPanel.setLayout( new BoxLayout(labeledVelCtrlPanel, BoxLayout.Y_AXIS));

        labeledVelCtrlPanel.add(velCtrlLabelTop);
        labeledVelCtrlPanel.add(velocityCtrlPanel);
        labeledVelCtrlPanel.add(velCtrlLabelBottom);

        add(labeledVelCtrlPanel);
        // ---------------------------------------------------------------------

        jointCtrlPanel = new JointCtrlPanel(rangeView);
        add(jointCtrlPanel);
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

        controlPanel = new ControlPanel(rangeView);

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
        double  dt = 0.050; // Time between updates (seconds).

        int nlinks = 0;
        boolean standalone = false;

        int mapScale = 64 ; // pixels per meter.

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
            "trick.var_add(\"Manip2D.robot.kinemat.ndof\")\n" +
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

        // Configure the Variable Server to cyclically send data.
        robotDisplay.out.writeBytes( "trick.var_pause() \n");

        robotDisplay.out.writeBytes( "trick.var_add(\"Manip2D.robot.mode\")\n");
        robotDisplay.out.writeBytes( "trick.var_add(\"Manip2D.robot.controller.singleJointNum\")\n");

        for ( ii = 0; ii < nlinks; ii ++) {
            robotDisplay.out.writeBytes(
                  String.format("trick.var_add(\"Manip2D.robot.kinemat.joint_l[%d][0]\")\n", ii)
                + String.format("trick.var_add(\"Manip2D.robot.kinemat.joint_q[%d]\")\n", ii)
            );
        }
        robotDisplay.out.writeBytes(
          "trick.var_ascii() \n" +
          String.format("trick.var_cycle(%.3f)\n", dt) +
          "trick.var_unpause() \n" );
        robotDisplay.out.flush();

        //  Update the scene.
        robotDisplay.drawRangeView();

        int robotMode = RobotCtrlMode.NOPE;
        int desiredMode;
        int jointNumber = 0;
        int desiredJointNumber;
        double desiredJointRate;
        while (go) {

            // -----------------------------------------------------------------
            // Recieve and parse periodic data response from the variable server.
            // -----------------------------------------------------------------
            try {
                String line;
                String field[];
                line = robotDisplay.in.readLine();
                field = line.split("\t");
                robotMode = Integer.parseInt( field[1]);
                robotDisplay.controlPanel.modeCtrlPanel.setMode( robotMode );
                jointNumber = Integer.parseInt( field[2]);
                robotDisplay.controlPanel.jointCtrlPanel.jointSelectPanel.setValue( jointNumber );
                for ( int lix=0; lix < nlinks; lix++) {
                    robotDisplay.rangeView.links[lix].l = Double.parseDouble( field[lix*2 + 3]);
                    robotDisplay.rangeView.links[lix].q = Double.parseDouble( field[lix*2 + 4]);
                }
            } catch (IOException | NullPointerException e ) {
                go = false;
            }

            // -----------------------------------------------------------------
            // Send commands to from the variable server.
            // -----------------------------------------------------------------
            desiredMode = robotDisplay.controlPanel.modeCtrlPanel.desiredMode();
            if ( desiredMode != RobotCtrlMode.NOPE ) {
                robotDisplay.controlPanel.modeCtrlPanel.resetDesiredMode();
                robotDisplay.out.writeBytes(String.format("Manip2D.robot.mode = " + desiredMode + ";\n" ));
                robotDisplay.out.flush();
            }

            float Vx = robotDisplay.controlPanel.velocityCtrlPanel.getXvalue();
            robotDisplay.out.writeBytes(String.format("Manip2D.robot.controller.manualCommandRate[0] = " + Vx + ";\n" ));
            float Vy = robotDisplay.controlPanel.velocityCtrlPanel.getYvalue();
            robotDisplay.out.writeBytes(String.format("Manip2D.robot.controller.manualCommandRate[1] = " + Vy + ";\n" ));

            desiredJointNumber = robotDisplay.controlPanel.jointCtrlPanel.jointSelectPanel.getValue();
            if (desiredJointNumber != jointNumber) {
                robotDisplay.out.writeBytes(String.format("Manip2D.robot.controller.singleJointNum = " + desiredJointNumber + ";\n" ));
            }
            desiredJointRate = robotDisplay.controlPanel.jointCtrlPanel.jointRatePanel.getValue();
            robotDisplay.out.writeBytes(String.format("Manip2D.robot.controller.singleJointRate = " +
                                                      desiredJointRate + ";\n" ));

            robotDisplay.out.writeBytes(String.format("Manip2D.robot.controller.desiredPos[0] = " +
                                                      robotDisplay.rangeView.desiredPosition.x + ";\n" ));
            robotDisplay.out.writeBytes(String.format("Manip2D.robot.controller.desiredPos[1] = " +
                                                      robotDisplay.rangeView.desiredPosition.y + ";\n" ));

            //  Update the scene.
            robotDisplay.drawRangeView();

        } // while
    } // main
} // class
