/*
 * Trick
 * 2021 (c) National Aeronautics and Space Administration (NASA)
 */

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
    private Color groundColor;
    private Color envelope_color_1;
    private Color envelope_color_2;
    private Color basket_color;
    private Color envelope_phong;

    // Origin of world coordinates in jpanel coordinates.
    private int worldOriginX;
    private int worldOriginY;

    private double[] balloonPos;
    private double[] balloonVel;
    private int      envelopeAirTemp; /* degrees C */
    private int      windSpeed;
    private double   envelope_radius;
    private double   envelope_theta;

    private ScenePoly cone;
    private ScenePoly upright; // The frame that connects the basket, burner, and balloon
    private ScenePoly basket;

    private int[] workPolyX, workPolyY;

    // Controls
    private int deltaTemp ;
    private int deltaWind ;

    /**
     * Class constructor.
     */
    public RangeView( int mapScale) {

        setScale(mapScale);

        deltaTemp = 0;
        deltaWind = 0;

        skyColor    = new Color(184,202,231);
        groundColor = new Color(100,140, 60);

        balloonPos  = new double[]
            {0.0, 2.0};
        balloonVel  = new double[]
            {0.0, 0.0};

        envelope_color_1 = new  Color(254,181, 36);
        envelope_color_2 = new  Color(251,  0, 21);
        basket_color     = new  Color(220,180,120);
        envelope_phong   = new  Color(220,220,220,80);

        envelope_radius = 8.5;
        envelope_theta = Math.toRadians(45.0);

        envelopeAirTemp = 0;
        windSpeed = 0;

        cone = new ScenePoly();
        cone.color = envelope_color_1;
        cone.x = new double[4];
        cone.y = new double[4];
        cone.n = 4;

        upright = new ScenePoly();
        upright.color = Color.BLACK;
        upright.x = new double[] {-0.40, 0.40, 0.50, 0.40, 0.30,-0.30,-0.40,-0.50};
        upright.y = new double[] { 0.00, 0.00,-1.00,-1.00,-0.10,-0.10,-1.00,-1.00};
        upright.n = 8;

        basket = new ScenePoly();
        basket.color = basket_color;
        basket.x = new double[] {-0.50, 0.50, 0.50,-0.50};
        basket.y = new double[] {-1.00,-1.00,-2.00,-2.00};
        basket.n = 4;

        workPolyX = new int[30];
        workPolyY = new int[30];
    }

    // |jpanel_x| = |origin_x| + |scale    0  | * |cos(angle) -sin(angle)| * |world_x|
    // |jpanel_y|   |origin_y|   |  0   -scale|   |sin(angle)  cos(angle)|   |world_y|

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

    public void drawSceneOval(Graphics2D g2d, Color color, double x, double y, double w, double h) {
        g2d.setPaint(color);
        g2d.fillOval( (int)(worldOriginX+scale*(x-w/2)), (int)(worldOriginY-scale*(y+h/2)), (int)(scale*w), (int)(scale*h));
    }

    public void incTemperature() { deltaTemp =    1; }
    public void decTemperature() { deltaTemp =   -1; }
    public void resetDeltaTemp() { deltaTemp =    0; }
    public int  getDeltaTemp()   { return deltaTemp; }

    public void incWind() { deltaWind =    1; }
    public void decWind() { deltaWind =   -1; }
    public void resetDeltaWind() { deltaWind =    0; }
    public int  getDeltaWind()   { return deltaWind; }

    public void setballoonPos(double x, double y) {
        balloonPos[0] = x;
        balloonPos[1] = y;
    }
    public void setBalloonVel(double vx, double vy) {
        balloonVel[0] = vx;
        balloonVel[1] = vy;
    }
    public void setAirTemp(int temperature) {
        envelopeAirTemp = temperature;
    }
    public void setWindSpeed(int speed) {
        windSpeed = speed;
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

        // |jpanel_x| = |origin_x| + |scale    0  | * |cos(angle) -sin(angle)| * |world_x|
        // |jpanel_y|   |origin_y|   |  0   -scale|   |sin(angle)  cos(angle)|   |world_y|

        worldOriginX = (width/2)  - (int)(scale * balloonPos[0]);
        worldOriginY = (height/2) + (int)(scale * balloonPos[1]);

        // ===============================================================================
        // Draw Sky
        // ===============================================================================
        g2d.setPaint(skyColor);
        g2d.fillRect(0, 0, width, worldOriginY);

        // ===============================================================================
        // Draw ground.
        // ===============================================================================
        g2d.setPaint(groundColor);
        g2d.fillRect(0, worldOriginY, width, height);

        // ===============================================================================
        //  Draw Balloon
        // ===============================================================================

        double r_cone_top = envelope_radius * Math.cos(envelope_theta);
        double h_cone = r_cone_top /  Math.tan(envelope_theta);
        double h = envelope_radius * Math.sin(envelope_theta) + h_cone;
        double balloon_diameter = 2.0 * envelope_radius;

        // Draw Dome
        drawSceneOval(g2d, envelope_color_1, balloonPos[0], balloonPos[1]+h, balloon_diameter*Math.cos(Math.toRadians( 0.0)), balloon_diameter);
        drawSceneOval(g2d, envelope_color_2, balloonPos[0], balloonPos[1]+h, balloon_diameter*Math.cos(Math.toRadians(22.5)), balloon_diameter);
        drawSceneOval(g2d, envelope_color_1, balloonPos[0], balloonPos[1]+h, balloon_diameter*Math.cos(Math.toRadians(37.5)), balloon_diameter);
        drawSceneOval(g2d, envelope_color_2, balloonPos[0], balloonPos[1]+h, balloon_diameter*Math.cos(Math.toRadians(52.5)), balloon_diameter);
        drawSceneOval(g2d, envelope_color_1, balloonPos[0], balloonPos[1]+h, balloon_diameter*Math.cos(Math.toRadians(67.5)), balloon_diameter);
        drawSceneOval(g2d, envelope_color_2, balloonPos[0], balloonPos[1]+h, balloon_diameter*Math.cos(Math.toRadians(82.5)), balloon_diameter);
        drawSceneOval(g2d, envelope_phong,   balloonPos[0], balloonPos[1]+h+4.0, balloon_diameter*Math.cos(Math.toRadians( 40.0)), balloon_diameter*Math.cos(Math.toRadians( 60.0)));

        // Draw Cone
        cone.color = envelope_color_1;
        double rt =  r_cone_top * Math.cos( Math.toRadians( 0.0));
        cone.x[0] =  -rt;
        cone.y[0] =  h_cone;
        cone.x[1] =  rt;
        cone.y[1] =  h_cone;
        cone.x[2] =  0.0;
        cone.y[2] =  0.0;
        cone.x[3] =  0.0;
        cone.y[3] =  0.0;
        drawScenePoly(g2d, cone, 0.0, balloonPos[0], balloonPos[1]);

        cone.color = envelope_color_2;
        rt =  r_cone_top * Math.cos( Math.toRadians(22.5));
        cone.x[0] =  -rt;
        cone.x[1] =  rt;
        drawScenePoly(g2d, cone, 0.0, balloonPos[0], balloonPos[1]);

        cone.color = envelope_color_1;
        rt =  r_cone_top * Math.cos( Math.toRadians(37.5));
        cone.x[0] =  -rt;
        cone.x[1] =  rt;
        drawScenePoly(g2d, cone, 0.0, balloonPos[0], balloonPos[1]);

        cone.color = envelope_color_2;
        rt =  r_cone_top * Math.cos( Math.toRadians(52.5));
        cone.x[0] =  -rt;
        cone.x[1] =  rt;
        drawScenePoly(g2d, cone, 0.0, balloonPos[0], balloonPos[1]);

        cone.color = envelope_color_1;
        rt =  r_cone_top * Math.cos( Math.toRadians(67.5));
        cone.x[0] =  -rt;
        cone.x[1] =  rt;
        drawScenePoly(g2d, cone, 0.0, balloonPos[0], balloonPos[1]);

        cone.color = envelope_color_2;
        rt =  r_cone_top * Math.cos( Math.toRadians(82.5));
        cone.x[0] =  -rt;
        cone.x[1] =  rt;
        drawScenePoly(g2d, cone, 0.0, balloonPos[0], balloonPos[1]);

        // Draw Upright
        drawScenePoly(g2d, upright, 0.0, balloonPos[0], balloonPos[1]);

        // Draw Basket
        drawScenePoly(g2d, basket, 0.0, balloonPos[0], balloonPos[1]);
        // ===============================================================================
        // Draw range markers.
        // ===============================================================================
        int tickRange = 50;
        if (scale >=  8) tickRange = 20;
        if (scale >= 16) tickRange = 10;
        if (scale >= 32) tickRange =  5;
        if (scale >= 64) tickRange =  1;

        int lower = ((int)((      - worldOriginX)/(scale * tickRange)) + 1) * tickRange;
        int upper = ((int)((width - worldOriginX)/(scale * tickRange)) + 1) * tickRange;

        g2d.setPaint(Color.WHITE);

        for (ii = lower ; ii < upper ; ii += tickRange) {
            int mx = (int)(worldOriginX + scale * ii);
            g2d.drawLine( mx, worldOriginY, mx, worldOriginY + 20);
            g2d.drawString ( String.format("%d",ii), mx, worldOriginY + 15);
        }

        // ===============================================================================
        // Draw Information
        // ===============================================================================
        g2d.setPaint(Color.BLACK);
        g2d.drawString ( String.format("SCALE: %d pixels/meter",scale), 20,20);
        g2d.drawString ( String.format("Envelope Air-Temp (°C) : [%d]", envelopeAirTemp), 20,40);
        g2d.drawString ( String.format("Wind Speed (m/s) : [%d]", windSpeed), 20,60);
        g2d.drawString ( String.format("Balloon Pos: [%.2f, %.2f]", balloonPos[0], balloonPos[1]), 20,80);
        g2d.drawString ( String.format("Balloon Vel: [%.2f, %.2f]", balloonVel[0], balloonVel[1]), 20,100);

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

class TemperatureCtrlPanel extends JPanel implements ActionListener {
    private RangeView rangeView;
    private JButton increaseTempButton, decreaseTempButton;

    public TemperatureCtrlPanel(RangeView view) {
        rangeView = view;
        setLayout(new BoxLayout(this, BoxLayout.X_AXIS));
        setBorder( BorderFactory.createEtchedBorder(EtchedBorder.RAISED));

        increaseTempButton = new JButton("\u25b2");
        increaseTempButton.addActionListener(this);
        increaseTempButton.setActionCommand("increaseTemp");
        increaseTempButton.setToolTipText("Increase Temperature");

        decreaseTempButton = new JButton("\u25bc");
        decreaseTempButton.addActionListener(this);
        decreaseTempButton.setActionCommand("decreaseTemp");
        decreaseTempButton.setToolTipText("Decrease Temperature");

        add(increaseTempButton);
        add(decreaseTempButton);

    }

    public void actionPerformed(ActionEvent e) {
        String s = e.getActionCommand();
        switch (s) {
            case "increaseTemp":
                rangeView.incTemperature();
                break;
            case "decreaseTemp":
                rangeView.decTemperature();
                break;
            default:
                System.out.println("Unknown Action Command:" + s);
                break;
        }
    }
}

class WindCtrlPanel extends JPanel implements ActionListener {
    private RangeView rangeView;
    private JButton increaseWindButton, decreaseWindButton;

    public WindCtrlPanel(RangeView view) {
        rangeView = view;
        setLayout(new BoxLayout(this, BoxLayout.X_AXIS));
        setBorder( BorderFactory.createEtchedBorder(EtchedBorder.RAISED));

        increaseWindButton = new JButton("\u25b6");
        increaseWindButton.addActionListener(this);
        increaseWindButton.setActionCommand("increaseWind");
        increaseWindButton.setToolTipText("Increase Wind");

        decreaseWindButton = new JButton("\u25c0");
        decreaseWindButton.addActionListener(this);
        decreaseWindButton.setActionCommand("decreaseWind");
        decreaseWindButton.setToolTipText("Decrease Wind");

        add(decreaseWindButton);
        add(increaseWindButton);


    }

    public void actionPerformed(ActionEvent e) {
        String s = e.getActionCommand();
        switch (s) {
            case "increaseWind":
                rangeView.incWind();
                break;
            case "decreaseWind":
                rangeView.decWind();
                break;
            default:
                System.out.println("Unknown Action Command:" + s);
                break;
        }
    }
}


class ControlPanel extends JPanel implements ActionListener {

    private RangeView rangeView;
    private JButton zoomOutButton, zoomInButton;
    private JButton shutDownButton;
    private TemperatureCtrlPanel temperatureCtrlPanel;
    private WindCtrlPanel windCtrlPanel;

    public ControlPanel(RangeView view) {

        rangeView = view;
        setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

        JPanel labeledTemperatureCtrlPanel = new JPanel();
        labeledTemperatureCtrlPanel.setLayout(new BoxLayout(labeledTemperatureCtrlPanel, BoxLayout.Y_AXIS));
        JLabel temperatureControlLabel = new JLabel("Temperature Control");
        temperatureControlLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
        labeledTemperatureCtrlPanel.add(temperatureControlLabel);
        temperatureCtrlPanel = new TemperatureCtrlPanel(rangeView);
        labeledTemperatureCtrlPanel.add( temperatureCtrlPanel );
        add(labeledTemperatureCtrlPanel);

        JPanel labeledWindCtrlPanel = new JPanel();
        labeledWindCtrlPanel.setLayout(new BoxLayout(labeledWindCtrlPanel, BoxLayout.Y_AXIS));
        JLabel windControlLabel = new JLabel("Wind Control");
        windControlLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
        labeledWindCtrlPanel.add(windControlLabel);
        windCtrlPanel = new WindCtrlPanel(rangeView);
        labeledWindCtrlPanel.add( windCtrlPanel );
        add(labeledWindCtrlPanel);

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

public class BalloonDisplay extends JFrame {

    private RangeView rangeView;
    private BufferedReader in;
    private DataOutputStream out;
    private JPanel panelGroup0;
    private JPanel panelGroup1;
    private ControlPanel controlPanel;

    public BalloonDisplay(RangeView arena) {
        setTitle("Balloon Range");

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

        rangeView.setScale(8);
        rangeView.setballoonPos (0.0, 2.0);

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
          + "usage: java jar BalloonDisplay.jar <port-number>\n"
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
        int     airtemp = 0;
        int     windspeed = 0;

        // Outbound command variables
        int temperature_change_command;
        int wind_change_command;

        int simMode = 0;
        boolean standalone = false;

        int mapScale = 32 ; // pixels per meter.

        RangeView rangeView = new RangeView( mapScale);
        BalloonDisplay balloonDisplay = new BalloonDisplay( rangeView);
        balloonDisplay.setVisible(true);
        balloonDisplay.drawRangeView();

        if (port == 0) {
            System.out.println("No variable server port specified.");
            printHelpText();
            System.exit(0);
        }

        // Connect to the Trick simulation's variable server
        System.out.println("Connecting to: " + host + ":" + port);
        balloonDisplay.connectToServer(host, port);

        balloonDisplay.out.writeBytes("trick.var_set_client_tag(\"BalloonDisplay\") \n");
        balloonDisplay.out.flush();

        // Have the Variable Server send us the simulation mode ONCE.
        balloonDisplay.out.writeBytes( "trick.var_add(\"trick_sys.sched.mode\")\n" +
                            "trick.var_send() \n" +
                            "trick.var_clear() \n");
        balloonDisplay.out.flush();

        // Read the response and extract the simulation mode.
        try {
            String line;
            String field[];
            line = balloonDisplay.in.readLine();
            field = line.split("\t");
            simMode = Integer.parseInt( field[1]);
        } catch (IOException | NullPointerException e ) {
            go = false;
        }

        // Configure the Variable Server to cyclically send us the following varibales.
        // Tell the variable server:
        //  1) We want the values of the following variables:
        balloonDisplay.out.writeBytes( "trick.var_pause() \n" +
                            "trick.var_add(\"dyn.balloon.pos[0]\")\n" +
                            "trick.var_add(\"dyn.balloon.pos[1]\")\n" +
                            "trick.var_add(\"dyn.balloon.vel[0]\")\n" +
                            "trick.var_add(\"dyn.balloon.vel[1]\")\n" +
                            "trick.var_add(\"dyn.balloon.envelope_air_temperature\")\n" +
                            "trick.var_add(\"dyn.balloon.wind_speed\")\n" +
                            "trick.var_add(\"trick_sys.sched.mode\")\n" +
        //  2) We want the responses in ASCII:
                            "trick.var_ascii() \n" +
        //  3) We want values to be updated at the specified rate:
                            String.format("trick.var_cycle(%.3f)\n", dt) +
        //  4) Start sending values as specified.
                            "trick.var_unpause() \n" );
        balloonDisplay.out.flush();

        while (go) {

            // Recieve and parse periodic data response from the variable server.
            try {
                String line;
                String field[];
                line = balloonDisplay.in.readLine();
                field   = line.split("\t");
                posx    = Double.parseDouble( field[1]);
                posy    = Double.parseDouble( field[2]);
                velx    = Double.parseDouble( field[3]);
                vely    = Double.parseDouble( field[4]);
                airtemp = Integer.parseInt( field[5]);
                windspeed = Integer.parseInt( field[6]);
                simMode = Integer.parseInt( field[7]);
            } catch (IOException | NullPointerException e ) {
                go = false;
            }

            // Update the display data.
            rangeView.setballoonPos(posx, posy);
            rangeView.setBalloonVel(velx, vely);
            rangeView.setAirTemp(airtemp);
            rangeView.setWindSpeed(windspeed);

            temperature_change_command = rangeView.getDeltaTemp();
            if (temperature_change_command != 0) {
                balloonDisplay.out.writeBytes( String.format("dyn.balloon.temperature_change_command = %d ;\n", temperature_change_command ));
                rangeView.resetDeltaTemp();
            }

            wind_change_command = rangeView.getDeltaWind();
            if (wind_change_command != 0) {
                balloonDisplay.out.writeBytes( String.format("dyn.balloon.wind_change_command = %d ;\n", wind_change_command ));
                rangeView.resetDeltaWind();
            }

            balloonDisplay.out.flush();

            //  Update the scene.
            balloonDisplay.drawRangeView();

        } // while
    } // main
} // class
