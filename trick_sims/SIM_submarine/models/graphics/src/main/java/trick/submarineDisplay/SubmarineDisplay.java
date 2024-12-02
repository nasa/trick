/*
 * Trick
 * 2023 (c) National Aeronautics and Space Administration (NASA)
 */
// import java.awt;
import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.Graphics;
import java.awt.RenderingHints;
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
import java.awt.geom.Area;
import java.awt.geom.Rectangle2D;

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

class ScenePoly {
    public Color color;
    public int n;
    public double[] x;
    public double[] y;
}

class RangeView extends JPanel {

    private int scale;
    private Color skyColor;
    private Color oceanColor;
    private Color hull_exterior_color;
    private Color hull_interior_color;
    private Color ballast_air_color;

    // Origin of world coordinates in jpanel coordinates.
    private int worldOriginX;
    private int worldOriginY;

    private double[] submarinePos;
    private double[] submarineVel;
    private double   ballastAirRatio;
    private double   waterPressure;
    private double   pumpPower; /* watts = (joules/second) */
    private double   ballastEnergy;
    private double   hull_thickness;

    private double ohull_oradius = 1.30;
    private double ohull_iradius = 1.25;
    private double ihull_oradius = 1.10;
    private double ihull_iradius = 1.00;
    private double hatch_out_radius = 0.5;

    private ScenePoly hullPoly;

    private int[] workPolyX, workPolyY;

    // Controls
    private int pumpPowerCommand ;

    /**
     * Class constructor.
     */
    public RangeView( int mapScale) {

        setScale(mapScale);

        skyColor   = new Color(184,219,236);
        oceanColor = new Color(58,201,187);
        ballast_air_color = new Color(220,220,250);

        submarinePos  = new double[]
            {0.0, -0.0};
        submarineVel  = new double[]
            {0.0, 0.0};

        ballastAirRatio = 0.0;

        hull_exterior_color = new  Color(100,100,100);
        hull_interior_color = new  Color(240,240,220);

        hull_thickness = 0.3;

        ohull_oradius = 1.30;
        ohull_iradius = 1.25;
        ihull_oradius = 1.10;
        ihull_iradius = 1.00;
        hatch_out_radius = 0.5;

        pumpPower = 0.0;
        ballastEnergy = 0.0;

        hullPoly = new ScenePoly();
        hullPoly.color = hull_exterior_color;

        hullPoly.x = new double[] {
           -0.500,   -0.500,   -0.637,   -0.766,   -0.884,   -0.990,   -1.082,   -1.159,   -1.220,   -1.264,
           -1.291,   -1.300,   -1.291,   -1.264,   -1.220,   -1.159,   -1.082,   -0.990,   -0.884,   -0.766,
           -0.637,   -0.500,   -0.500,   -0.629,   -0.750,   -0.861,   -0.960,   -1.046,   -1.118,   -1.175,
           -1.217,   -1.242,   -1.250,   -1.242,   -1.217,   -1.175,   -1.118,   -1.046,   -0.960,   -0.861,
           -0.750,   -0.629,   -0.500,   -0.500,   -0.626,   -0.741,   -0.842,   -0.929,   -0.999,   -1.052,
           -1.085,   -1.099,   -1.094,   -1.070,   -1.026,   -0.964,   -0.885,   -0.790,   -0.681,   -0.560,
           -0.429,   -0.290,   -0.146,   -0.000,    0.146,    0.290,    0.429,    0.560,    0.681,    0.790,
            0.885,    0.964,    1.026,    1.070,    1.094,    1.099,    1.085,    1.052,    0.999,    0.929,
            0.842,    0.741,    0.626,    0.500,    0.500,    0.629,    0.750,    0.861,    0.960,    1.046,
            1.118,    1.175,    1.217,    1.242,    1.250,    1.242,    1.217,    1.175,    1.118,    1.046,
            0.960,    0.861,    0.750,    0.629,    0.500,    0.500,    0.637,    0.766,    0.884,    0.990,
            1.082,    1.159,    1.220,    1.264,    1.291,    1.300,    1.291,    1.264,    1.220,    1.159,
            1.082,    0.990,    0.884,    0.766,    0.637,    0.500,    0.500,   -0.500};
        hullPoly.y = new double[] {
            1.400,    1.200,    1.133,    1.050,    0.953,    0.843,    0.721,    0.589,    0.449,    0.303,
            0.153,    0.000,   -0.153,   -0.303,   -0.449,   -0.589,   -0.721,   -0.843,   -0.953,   -1.050,
           -1.133,   -1.200,   -1.146,   -1.080,   -1.000,   -0.907,   -0.801,   -0.685,   -0.559,   -0.426,
           -0.287,   -0.145,    0.000,    0.145,    0.287,    0.426,    0.559,    0.685,    0.801,    0.907,
            1.000,    1.080,    1.146,    0.980,    0.905,    0.813,    0.707,    0.589,    0.460,    0.323,
            0.180,    0.034,   -0.112,   -0.257,   -0.397,   -0.530,   -0.654,   -0.766,   -0.864,   -0.947,
           -1.013,   -1.061,   -1.090,   -1.100,   -1.090,   -1.061,   -1.013,   -0.947,   -0.864,   -0.766,
           -0.654,   -0.530,   -0.397,   -0.257,   -0.112,    0.034,    0.180,    0.323,    0.460,    0.589,
            0.707,    0.813,    0.905,    0.980,    1.146,    1.080,    1.000,    0.907,    0.801,    0.685,
            0.559,    0.426,    0.287,    0.145,    0.000,   -0.145,   -0.287,   -0.426,   -0.559,   -0.685,
           -0.801,   -0.907,   -1.000,   -1.080,   -1.146,   -1.200,   -1.133,   -1.050,   -0.953,   -0.843,
           -0.721,   -0.589,   -0.449,   -0.303,   -0.153,    0.000,    0.153,    0.303,    0.449,    0.589,
            0.721,    0.843,    0.953,    1.050,    1.133,    1.200,    1.400,    1.400};

        hullPoly.n = 128;

        workPolyX = new int[150];
        workPolyY = new int[150];
    }

    // |jpanel_x| = |origin_x| + |scale    0  | * |cos(angle) -sin(angle)| * |world_x|
    // |jpanel_y|   |origin_y|   |  0   -scale|   |sin(angle)  cos(angle)|   |world_y|

    public void fillScenePoly(Graphics2D g, ScenePoly p, double angle_r , double x, double y) {
        for (int ii = 0; ii < p.n; ii++) {
            workPolyX[ii] = (int)(worldOriginX + scale *
                ( Math.cos(angle_r) * p.x[ii] - Math.sin(angle_r) * p.y[ii] + x));
            workPolyY[ii] = (int)(worldOriginY - scale *
                ( Math.sin(angle_r) * p.x[ii] + Math.cos(angle_r) * p.y[ii] + y));
        }
        g.setPaint(p.color);
        g.fillPolygon(workPolyX, workPolyY, p.n);
    }

    public void fillSceneOval(Graphics2D g2d, Color color, double x, double y, double w, double h) {
        g2d.setPaint(color);
        g2d.fillOval( (int)(worldOriginX+scale*(x-w/2)), (int)(worldOriginY-scale*(y+h/2)), (int)(scale*w), (int)(scale*h));
    }

    public void fillSceneRect(Graphics2D g2d, Color color, double x, double y, double w, double h) {
        g2d.setPaint(color);
        g2d.fillRect( (int)(worldOriginX+scale*(x-w/2)), (int)(worldOriginY-scale*(y+h/2)), (int)(scale*w), (int)(scale*h));
    }

    public int  getPumpPowerCommand()   {
        return pumpPowerCommand;
    }

    public void setPumpPowerCommand(int power) {
         pumpPowerCommand = power;
    }

    public void setSubmarinePos(double x, double y) {
        submarinePos[0] = x;
        submarinePos[1] = y;
    }

    public void setSubmarineVel(double vx, double vy) {
        submarineVel[0] = vx;
        submarineVel[1] = vy;
    }

    public void setBallastAirRatio(double ratio) {
        ballastAirRatio = ratio;
    }

    public void setWaterPressure(double P) {
        waterPressure = P;
    }

    public void setPumpPower(double power) {
        pumpPower = power;
    }

    public void setBallastEnergy(double energy) {
        ballastEnergy = energy;
    }

    public void setScale (int mapScale) {
        if (mapScale < 2) {
            scale = 2;
        } else if (mapScale > 128) {
            scale = 128;
        } else {
            scale = mapScale;
        }
        repaint();
    }

    public int getScale() {
        return scale;
    }

    private void doDrawing(Graphics g) {
        Graphics2D g2d = (Graphics2D) g;

        RenderingHints rh = new RenderingHints(
                RenderingHints.KEY_ANTIALIASING,
                RenderingHints.VALUE_ANTIALIAS_ON);

        rh.put(RenderingHints.KEY_RENDERING,
               RenderingHints.VALUE_RENDER_QUALITY);

        int ii, jj;
        int width  = getWidth();
        int height = getHeight();
        double R_intensity = 0.0;
        double G_intensity = 0.0;
        double B_intensity = 0.0;

        // |jpanel_x| = |origin_x| + |scale    0  | * |cos(angle) -sin(angle)| * |world_x|
        // |jpanel_y|   |origin_y|   |  0   -scale|   |sin(angle)  cos(angle)|   |world_y|

        worldOriginX = (width/2)  - (int)(scale * submarinePos[0]);
        worldOriginY = (height/2) + (int)(scale * submarinePos[1]);

        // ===============================================================================
        //  Draw Sky & Ocean
        // ===============================================================================
        if (worldOriginY < 0) {

            if (submarinePos[1] >= 0.0) R_intensity = 1.0;
            else if (submarinePos[1] <= -50.0) R_intensity = 0.0;
            else R_intensity = (1.0 - (submarinePos[1]/-50.0));

            if (submarinePos[1] >= 0.0) R_intensity = 1.0;
            else if (submarinePos[1] <= -100.0) R_intensity = 0.0;
            else G_intensity = (1.0 - (submarinePos[1]/-100.0));

            if (submarinePos[1] >= 0.0) R_intensity = 1.0;
            else if (submarinePos[1] <= -200.0) B_intensity = 0.0;
            else B_intensity = (1.0 - (submarinePos[1]/-200.0));

            g2d.setPaint( new Color( (int)(oceanColor.getRed()   * R_intensity),
                                     (int)(oceanColor.getGreen() * G_intensity),
                                     (int)(oceanColor.getBlue()  * B_intensity)));

            g2d.fillRect(0, 0, width, height);
        } else if (worldOriginY > height) {
            g2d.setPaint(skyColor);
            g2d.fillRect(0, 0, width, height);
        } else {
            g2d.setPaint(skyColor);
            g2d.fillRect(0, 0, width, worldOriginY);
            g2d.setPaint(oceanColor);
            g2d.fillRect(0, worldOriginY, width, height);
        }

        // ===============================================================================
        //  Draw Submarine
        // ===============================================================================

        // Draw Ballast Air
        Rectangle2D clipRect =
           new Rectangle2D.Double((int)(worldOriginX+scale*(submarinePos[0]-ohull_iradius)),
                                  (int)(worldOriginY-scale*(submarinePos[1]+ohull_iradius)),
                                  scale*ohull_iradius*2.0, scale*ohull_iradius*2.0*ballastAirRatio);
        g2d.setClip(clipRect);
        fillSceneOval(g2d, ballast_air_color, submarinePos[0], submarinePos[1], ohull_iradius*2.0, ohull_iradius*2.0);
        g2d.setClip(null);

        // Draw Submarine Hull
        fillScenePoly(g2d, hullPoly, 0.0, submarinePos[0], submarinePos[1]);
        fillSceneOval(g2d, hull_interior_color, submarinePos[0], submarinePos[1], ihull_iradius*2.0, ihull_iradius*2.0);

        fillSceneOval(g2d, Color.GRAY, submarinePos[0]-0.6, submarinePos[1]+0.6, 0.4, 0.4);
        fillSceneOval(g2d, ballast_air_color, submarinePos[0]-0.6, submarinePos[1]+0.6, 0.3, 0.3);
        fillSceneOval(g2d, Color.GRAY, submarinePos[0]-0.8, submarinePos[1]+0.3, 0.4, 0.4);
        fillSceneOval(g2d, ballast_air_color, submarinePos[0]-0.8, submarinePos[1]+0.3, 0.3, 0.3);

        fillSceneOval(g2d, Color.GRAY, submarinePos[0]+0.6, submarinePos[1]+0.6, 0.4, 0.4);
        fillSceneOval(g2d, ballast_air_color, submarinePos[0]+0.6, submarinePos[1]+0.6, 0.3, 0.3);
        fillSceneOval(g2d, Color.GRAY, submarinePos[0]+0.8, submarinePos[1]+0.3, 0.4, 0.4);
        fillSceneOval(g2d, ballast_air_color, submarinePos[0]+0.8, submarinePos[1]+0.3, 0.3, 0.3);

        // Floor
        fillSceneRect(g2d, hull_exterior_color, submarinePos[0], submarinePos[1]-0.6, 1.7*ihull_iradius, 0.1);

        // Keel
        fillSceneRect(g2d, hull_exterior_color, submarinePos[0], submarinePos[1]-ihull_oradius-0.1, 0.6, 0.3);

        // ===============================================================================
        // Draw depth markers.
        // ===============================================================================
        int tickRange = 50;
        if (scale >=  8) tickRange = 20;
        if (scale >= 16) tickRange = 10;
        if (scale >= 32) tickRange =  5;
        if (scale >= 64) tickRange =  1;
        int upper = ((int)((       - worldOriginY)/(scale * tickRange)) + 1) * tickRange;
        int lower = ((int)((height - worldOriginY)/(scale * tickRange)) + 1) * tickRange;
        g2d.setPaint(Color.WHITE);
        for (ii = upper ; ii < lower ; ii += tickRange) {
            int my = (int)(worldOriginY + scale * ii);
            g2d.drawLine(worldOriginX + 150, my, worldOriginX + 200, my );
            g2d.drawString ( String.format("%d",ii), worldOriginX + 210, my);
        }

        // ===============================================================================
        // Draw Information
        // ===============================================================================
        g2d.setPaint(Color.WHITE);
        g2d.drawString ( String.format("SCALE: %d pixels/meter",scale), 20,20);
        g2d.drawString ( String.format("Water Pressure (PSI) : [%.2f]", waterPressure), 20,40);
        g2d.drawString ( String.format("Ballast Energy (Joules) : [%.2f]", ballastEnergy), 20,60);
        g2d.drawString ( String.format("Pump Power (Watts) : [%.2f]", pumpPower), 20,80);
        g2d.drawString ( String.format("Submarine Pos: [%.2f, %.2f]", submarinePos[0], submarinePos[1]), 20,100);
        g2d.drawString ( String.format("Submarine Vel: [%.2f, %.2f]", submarineVel[0], submarineVel[1]), 20,120);

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

class PumpPowerCtrlPanel extends JPanel implements ChangeListener {

    static final int POWER_MIN   = -5000;
    static final int POWER_MAX   =  5000;
    static final int POWER_INIT  =  0;

    private RangeView rangeView;
    private JSlider powerSlider ;

    public PumpPowerCtrlPanel (RangeView view) {
        rangeView = view;
        setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

        JLabel sliderLabel = new JLabel("", JLabel.CENTER);
        sliderLabel.setAlignmentX(CENTER_ALIGNMENT);

        powerSlider = new JSlider(JSlider.HORIZONTAL, POWER_MIN, POWER_MAX, POWER_INIT);
        powerSlider.addChangeListener(this);
        powerSlider.setToolTipText("Ballast Pump Power.");
        powerSlider.setMajorTickSpacing(1000);
        powerSlider.setMinorTickSpacing(100);
        powerSlider.setPaintTicks(true);
        powerSlider.setPaintLabels(true);

        add(sliderLabel);
        add(powerSlider);
    }

    @Override
    public void stateChanged(ChangeEvent e) {
        JSlider source = (JSlider)e.getSource();
        if (!source.getValueIsAdjusting()) {
            rangeView.setPumpPowerCommand( source.getValue() );
        }
    }

    public void setValue(int value) {
        if (value > POWER_MAX) value = POWER_MAX;
        if (value < POWER_MIN) value = POWER_MIN;
        powerSlider.setValue(value);
    }
}

class ControlPanel extends JPanel implements ActionListener {

    private RangeView rangeView;
    private JButton zoomOutButton, zoomInButton;
    private JButton shutDownButton;
    private PumpPowerCtrlPanel pumpPowerCtrlPanel;


    public ControlPanel(RangeView view) {

        rangeView = view;
        setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

        JPanel labeledPumpPowerCtrlPanel = new JPanel();
        labeledPumpPowerCtrlPanel.setLayout(new BoxLayout(labeledPumpPowerCtrlPanel, BoxLayout.Y_AXIS));
        JLabel powerPowerControlLabel = new JLabel("-Remove Ballast Air-    Pump Power (Watts)    -Add Ballast Air-");
        powerPowerControlLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
        labeledPumpPowerCtrlPanel.add(powerPowerControlLabel);
        pumpPowerCtrlPanel = new PumpPowerCtrlPanel(rangeView);
        labeledPumpPowerCtrlPanel.add( pumpPowerCtrlPanel );
        add(labeledPumpPowerCtrlPanel);

        zoomOutButton = new JButton("Zoom Out");
        zoomOutButton.addActionListener(this);
        zoomOutButton.setActionCommand("zoomout");
        zoomOutButton.setToolTipText("Zoom Out");
        add(zoomOutButton);

        zoomInButton = new JButton("Zoom In");
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
} // class ControlPanel

public class SubmarineDisplay extends JFrame {

    private RangeView rangeView;
    private BufferedReader in;
    private DataOutputStream out;
    private JPanel panelGroup0;
    private JPanel panelGroup1;
    private ControlPanel controlPanel;

    public SubmarineDisplay(RangeView arena) {
        setTitle("Submarine Range");

        rangeView  = arena;

        panelGroup1 = new JPanel();
        panelGroup1.setLayout(new BoxLayout(panelGroup1, BoxLayout.X_AXIS));
        panelGroup1.add(rangeView);

        controlPanel = new ControlPanel(rangeView);

        panelGroup0 = new JPanel();
        panelGroup0.setLayout(new BoxLayout(panelGroup0, BoxLayout.Y_AXIS));
        panelGroup0.add(panelGroup1);
        panelGroup0.add(controlPanel);

        add(panelGroup0);

        rangeView.setScale(64);
        rangeView.setSubmarinePos (0.0, 0.0);

        setSize(800, 500);
        setLocationRelativeTo(null);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setFocusable(true);

    }

    public void connectToServer(String host, int port ) throws IOException {
        Socket socket = new Socket(host, port);
        in = new BufferedReader( new InputStreamReader( socket.getInputStream()));
        out = new DataOutputStream(new BufferedOutputStream(socket.getOutputStream()));
    }

    public void drawRangeView() {
        rangeView.repaint();
    }

    private static void  printHelpText() {
        System.out.println(
            "----------------------------------------------------------------------\n"
          + "usage: java jar SubmarineDisplay.jar <port-number>\n"
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
        double  posx = 0.0;
        double  posy = 0.0;
        double  velx = 0.0;
        double  vely = 0.0;
        double  ratio = 0.0;
        double  Pwater = 0.0;
        double  pump_power = 0.0;
        double  energy = 0.0;

        // Outbound command variables
        int pump_power_command;
        int simMode = 0;
        boolean standalone = false;

        int mapScale = 32 ; // pixels per meter.

        RangeView rangeView = new RangeView( mapScale);
        SubmarineDisplay submarineDisplay = new SubmarineDisplay( rangeView);
        submarineDisplay.setVisible(true);
        submarineDisplay.drawRangeView();

        if (go) {
            if (port == 0) {
                System.out.println("No variable server port specified.");
                printHelpText();
                System.exit(0);
            }

            // Connect to the Trick simulation's variable server
            System.out.println("Connecting to: " + host + ":" + port);
            submarineDisplay.connectToServer(host, port);

            submarineDisplay.out.writeBytes("trick.var_set_client_tag(\"SubmarineDisplay\") \n");
            submarineDisplay.out.flush();

            // Have the Variable Server send us the simulation mode ONCE.
            submarineDisplay.out.writeBytes( "trick.var_add(\"trick_sys.sched.mode\")\n" +
                                "trick.var_send() \n" +
                                "trick.var_clear() \n");
            submarineDisplay.out.flush();

            // Read the response and extract the simulation mode.
            try {
                String line;
                String field[];
                line = submarineDisplay.in.readLine();
                field = line.split("\t");
                simMode = Integer.parseInt( field[1]);
            } catch (IOException | NullPointerException e ) {
                go = false;
            }

            // Configure the Variable Server to cyclically send us the following varibales.
            // Tell the variable server:
            //  1) We want the values of the following variables:
            submarineDisplay.out.writeBytes( "trick.var_pause() \n" +
                                "trick.var_add(\"dyn.submarine.pos[0]\")\n" +
                                "trick.var_add(\"dyn.submarine.pos[1]\")\n" +
                                "trick.var_add(\"dyn.submarine.vel[0]\")\n" +
                                "trick.var_add(\"dyn.submarine.vel[1]\")\n" +
                                "trick.var_add(\"dyn.submarine.ballast_air_ratio\")\n" +
                                "trick.var_add(\"dyn.submarine.water_pressure_PSI\")\n" +
                                "trick.var_add(\"dyn.submarine.ballast_energy\")\n" +
                                "trick.var_add(\"dyn.submarine.pump_power\")\n" +
                                "trick.var_add(\"trick_sys.sched.mode\")\n" +
            //  2) We want the responses in ASCII:
                                "trick.var_ascii() \n" +
            //  3) We want values to be updated at the specified rate:
                                String.format("trick.var_cycle(%.3f)\n", dt) +
            //  4) Start sending values as specified.
                                "trick.var_unpause() \n" );
            submarineDisplay.out.flush();
        } // if go

        while (go) {

            // Recieve and parse periodic data response from the variable server.
            try {
                String line;
                String field[];
                line = submarineDisplay.in.readLine();
                field   = line.split("\t");
                posx    = Double.parseDouble( field[1]);
                posy    = Double.parseDouble( field[2]);
                velx    = Double.parseDouble( field[3]);
                vely    = Double.parseDouble( field[4]);
                ratio    = Double.parseDouble( field[5]);
                Pwater   = Double.parseDouble( field[6]);
                energy   = Double.parseDouble( field[7]);
                pump_power = Double.parseDouble( field[8]);
                simMode    = Integer.parseInt( field[9]);
            } catch (IOException | NullPointerException e ) {
                go = false;
            }

            // Update the display data.
            rangeView.setSubmarinePos(posx, posy);
            rangeView.setSubmarineVel(velx, vely);
            rangeView.setBallastAirRatio(ratio);
            rangeView.setWaterPressure(Pwater);
            rangeView.setPumpPower(pump_power);
            rangeView.setBallastEnergy(energy);

            pump_power_command = rangeView.getPumpPowerCommand();
            submarineDisplay.out.writeBytes( String.format("dyn.submarine.pump_power_command = %d ;\n", pump_power_command ));

            submarineDisplay.out.flush();

            //  Update the scene.
            submarineDisplay.drawRangeView();

        } // while
    } // main
} // class
