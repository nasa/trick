package trick.ballDisplay;

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

class ScenePoly {
    public Color color;
}

class RangeView extends JPanel {

    private int scale;
    private Color skyColor;
    private Color groundColor;
    private Color ballColor;

    // Origin of world coordinates in jpanel coordinates.
    private int worldOriginX;
    private int worldOriginY;

    private double[] ballPos;
    private double[] ballVel;
    private double gravitationalConstant;

    private ScenePoly ball;

    // Controls
    private double deltaGravity ;

    /**
     * Class constructor.
     */
    public RangeView( int mapScale) {

        setScale(mapScale);

        deltaGravity = 0;

        skyColor    = new Color(22, 29, 54);
        groundColor = new Color(123, 124, 134);
        ballColor = new  Color(254, 181, 36);

        ballPos  = new double[] {0.0, 2.0};
        ballVel  = new double[] {0.0, 0.0};

        gravitationalConstant = -9.81;

        ball = new ScenePoly();
        ball.color = ballColor;
  
    }

    public void drawSceneOval(Graphics2D g2d, Color color, double x, double y, double w, double h) {
        g2d.setPaint(color);
        g2d.fillOval( (int)(worldOriginX+scale*(x-w/2)), (int)(worldOriginY-scale*(y+h/2)), (int)(scale*w), (int)(scale*h));
    }

    public void incGravity() { deltaGravity =    1.00; } 
    public void decGravity() { deltaGravity =   -1.00; }
    public void resetDeltaGravity() { deltaGravity =    0; }
    public double  getDeltaGravity()   { return deltaGravity; }

    public void setBallPos(double x, double y) {
        ballPos[0] = x;
        ballPos[1] = y;
    }
    
    public void setBallVel(double vx, double vy) {
        ballVel[0] = vx;
        ballVel[1] = vy;
    }
    
    public void setGravitationalConstant(double someGravitationalConstant) {
        gravitationalConstant = someGravitationalConstant;
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

        int ii;
        int width  = getWidth();
        int height = getHeight();

        worldOriginX = (width/2)  - (int)(scale * ballPos[0]);
        worldOriginY = (height/2) + (int)(scale * ballPos[1]);

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
        //  Draw Ball
        // ===============================================================================

        double ballRadius = 1.8288;
        drawSceneOval(g2d, ballColor, ballPos[0], ballPos[1], 2*ballRadius, 2*ballRadius);
        
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
        g2d.setPaint(Color.WHITE);
        g2d.drawString ( String.format("SCALE: %d pixels/meter",scale), 20,20);
        g2d.drawString ( String.format("Gravity: %f m/s", gravitationalConstant), 20,60);
        g2d.drawString ( String.format("Ball Pos: %.2f", ballPos[1]), 20,80);
        g2d.drawString ( String.format("Ball Vel:  %.2f", ballVel[1]), 20,100);

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

class GravityCtrlPanel extends JPanel implements ActionListener {
    private RangeView rangeView;
    private JButton increaseGravityButton, decreaseGravityButton;

    public GravityCtrlPanel(RangeView view) {
        rangeView = view;
        setLayout(new BoxLayout(this, BoxLayout.X_AXIS));
        setBorder( BorderFactory.createEtchedBorder(EtchedBorder.RAISED));

        increaseGravityButton = new JButton("\u25b6");
        increaseGravityButton.addActionListener(this);
        increaseGravityButton.setActionCommand("increaseGravity");
        increaseGravityButton.setToolTipText("Increase Gravity");

        decreaseGravityButton = new JButton("\u25c0");
        decreaseGravityButton.addActionListener(this);
        decreaseGravityButton.setActionCommand("decreaseGravity");
        decreaseGravityButton.setToolTipText("Decrease Gravity");

        add(decreaseGravityButton);
        add(increaseGravityButton);


    }

    public void actionPerformed(ActionEvent e) {
        String s = e.getActionCommand();
        switch (s) {
            case "increaseGravity":
                rangeView.incGravity();
                break;
            case "decreaseGravity":
                rangeView.decGravity();
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
    private JButton shutDownButton; // WARNING: The value of the field ControlPanel.shutDownButton is not used
    private GravityCtrlPanel gravityCtrlPanel;

    public ControlPanel(RangeView view) {

        rangeView = view;
        setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

        JPanel labeledGravityCtrlPanel = new JPanel();
        labeledGravityCtrlPanel.setLayout(new BoxLayout(labeledGravityCtrlPanel, BoxLayout.Y_AXIS));
        JLabel gravityControlLabel = new JLabel("Gravity Control");
        gravityControlLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
        labeledGravityCtrlPanel.add(gravityControlLabel);
        gravityCtrlPanel = new GravityCtrlPanel(rangeView);
        labeledGravityCtrlPanel.add( gravityCtrlPanel );
        add(labeledGravityCtrlPanel);

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

public class BallDisplay extends JFrame {

    private RangeView rangeView;
    private BufferedReader in;
    private DataOutputStream out;
    private JPanel panelGroup0;
    private JPanel panelGroup1;
    private ControlPanel controlPanel;

    public BallDisplay(RangeView arena) {
        setTitle("Ball Range");

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
        rangeView.setBallPos (0.0, 2.0);

        setSize(800, 500);
        setLocationRelativeTo(null);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setFocusable(true);

    }

    public void connectToServer(String host, int port ) throws IOException {
        Socket socket = new Socket(host,    port); // WARNING: Resource leak: 'socket' is never closed
        in = new BufferedReader( new InputStreamReader( socket.getInputStream()));
        out = new DataOutputStream(new BufferedOutputStream(socket.getOutputStream()));
    }

    public void drawRangeView() {
        rangeView.repaint();
    }

    private static void  printHelpText() {
        System.out.println(
            "----------------------------------------------------------------------\n"
          + "usage: java jar BallDisplay.jar <port-number>\n"
          + "----------------------------------------------------------------------\n"
          );
    }

    public enum ModelState { INACTIVE, READY, ACTIVE }

    public static void main(String[] args) throws IOException, InterruptedException {

        String host = "localHost";
        int port = 0;
        boolean boom = false; // WARNING: The value of the local variable boom is not used

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
        double  gravitationalConstant = -9.81;

        // Outbound command variables
        double gravity_change_command;

        int simMode = 0; // WARNING: The value of the local variable simMode is not used
        boolean standalone = false; // WARNING: The value of the local variable standalone is not used

        int mapScale = 32 ; // pixels per meter.

        RangeView rangeView = new RangeView( mapScale);
        BallDisplay ballDisplay = new BallDisplay( rangeView);
        ballDisplay.setVisible(true);
        ballDisplay.drawRangeView();

        if (port == 0) {
            System.out.println("No variable server port specified.");
            printHelpText();
            System.exit(0);
        }

        // Connect to the Trick simulation's variable server
        System.out.println("Connecting to: " + host + ":" + port);
        ballDisplay.connectToServer(host, port);

        ballDisplay.out.writeBytes("trick.var_set_client_tag(\"BallDisplay\") \n");
        ballDisplay.out.flush();

        // Have the Variable Server send us the simulation mode ONCE.
        ballDisplay.out.writeBytes( "trick.var_add(\"trick_sys.sched.mode\")\n" +
                            "trick.var_send() \n" +
                            "trick.var_clear() \n");
        ballDisplay.out.flush();

        // Read the response and extract the simulation mode.
        try {
            String line;
            String field[];
            line = ballDisplay.in.readLine();
            field = line.split("\t");
            simMode = Integer.parseInt( field[1]);
        } catch (IOException | NullPointerException e ) {
            go = false;
        }

        // Configure the Variable Server to cyclically send us the following varibales.
        // Tell the variable server:
        //  1) We want the values of the following variables:
        ballDisplay.out.writeBytes( "trick.var_pause() \n" +
                            "trick.var_add(\"dyn.ball.pos\")\n" +
                            "trick.var_add(\"dyn.ball.vel\")\n" +
                            "trick.var_add(\"dyn.ball.gravity\")\n" +
                            "trick.var_add(\"trick_sys.sched.mode\")\n" +
        //  2) We want the responses in ASCII:
                            "trick.var_ascii() \n" +
        //  3) We want values to be updated at the specified rate:
                            String.format("trick.var_cycle(%.3f)\n", dt) +
        //  4) Start sending values as specified.
                            "trick.var_unpause() \n" );
        ballDisplay.out.flush();

        while (go) {

            // Recieve and parse periodic data response from the variable server.
            try {
                String line;
                String field[];
                line = ballDisplay.in.readLine();
                field   = line.split("\t");
                posy    = Double.parseDouble( field[1]);
                vely    = Double.parseDouble( field[2]);
                gravitationalConstant = Double.parseDouble( field[3]);
                simMode = Integer.parseInt( field[4]);
            } catch (IOException | NullPointerException e ) {
                go = false;
            }

            // Update the display data.
            rangeView.setBallPos(posx, posy);
            rangeView.setBallVel(velx, vely);
            rangeView.setGravitationalConstant(gravitationalConstant);

            gravity_change_command= rangeView.getDeltaGravity();
            if (gravity_change_command != 0.0) {
                System.out.println("CHANGING GRAVITY");
                System.out.println(gravity_change_command);
                ballDisplay.out.writeBytes( String.format("dyn.ball.gravity_change_command = %f ;\n", gravity_change_command ));
                rangeView.resetDeltaGravity();
            }

            ballDisplay.out.flush();

            //  Update the scene.
            ballDisplay.drawRangeView();

        } // while
    } // main
} // class
