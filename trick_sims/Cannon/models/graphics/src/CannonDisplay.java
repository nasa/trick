/*
 * Trick
 * 2016 (c) National Aeronautics and Space Administration (NASA)
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
/**
 *
 * @author penn
 */

class RangeView extends JPanel {

    private int scale;
    private Color skyColor;
    private Color groundColor;
    private Color barrelColor;
    private Color cannonBallColor;
    private Color carriageColor;
    private Color wheelColor;
    private Color[] explosionColors;
    private int     explosionColors_n;

    // Origin of world coordinates in jpanel coordinates.
    private int worldOriginX;
    private int worldOriginY;

    private int      barrelAngle;
    private int      muzzleSpeed;
    private double[] cannonBallPos;
    private double[] cannonBallVel;

    private double   explosionSize;
    private double[] barrelPivotPos;
    private double[] explosionPos;
    private double[] wheelCenterPos;
    private double   wheelHubRadius;
    private double   cannonBallRadius;

    private double[] barrel_x, barrel_y;
    private int barrel_n;

    private double[] carriage_x, carriage_y;
    private int carriage_n;

    private double[] wheel_section_x, wheel_section_y;
    private int wheel_section_n;

    private double[] explosion_poly_x, explosion_poly_y;
    private int explosion_poly_n;

    private int[] workPolyX, workPolyY;

    /**
     * Class constructor.
     */
    public RangeView( int mapScale) {

        setScale(mapScale);

        skyColor        = new Color(200,200,255);
        groundColor     = new Color(150,150,100);
        barrelColor     = new Color(10,10,10);
        cannonBallColor = new Color(10,10,10);
        carriageColor   = new Color(120, 60, 40);
        wheelColor      = new Color(144, 72, 48);

        explosionColors = new Color[]
            { new Color(255,  63,   3),
              new Color(255, 127,  15),
              new Color(255, 191,  63),
              new Color(255, 255, 255) };
        explosionColors_n = 4;
 
        barrelAngle = 0;
        muzzleSpeed = 0;
        explosionSize = 0.0;

        cannonBallPos  = new double[] 
            {-0.1020, 0.9400};

        cannonBallVel  = new double[] 
            {0.0, 0.0};

        barrelPivotPos = new double[]
            {-0.1020, 0.9400};
        explosionPos = new double[]
            { 5.0800, 0.0000};
        wheelCenterPos = new double[]
            { 0.0000,0.6350,};

        wheelHubRadius   = 0.1524;
        cannonBallRadius = 0.1524;

        // Cannon barrel polygon
        barrel_x = new double[]
            { 1.2700, 0.0762,-0.4826,-0.5842,-0.6350,-0.6350,-0.6350,-0.5842,-0.4826, 0.0762,
              1.2700};
        barrel_y = new double[]
            { 0.1016, 0.1524, 0.1524, 0.1270, 0.0508, 0.0000,-0.0508,-0.1270,-0.1524,-0.1524,
             -0.1016};
        barrel_n = 11;

        // Carriage polygon
        carriage_x = new double[]
            {-1.2446, 0.0254, 0.0000, 0.1524, 0.2032, 0.4318, 0.3556, 0.0254, 0.0508, 0.0508,
              0.0000,-0.0254,-0.0508,-1.3208,-1.3970,-1.4478,-1.4732,-1.4732,-1.4478,-1.4224};
        carriage_y = new double[]
            {-0.9398,-0.3302,-0.2540,-0.1778,-0.2540,-0.1524, 0.0508, 0.0508, 0.0000,-0.0254,
             -0.0508,-0.0254, 0.0000,-0.7620,-0.7620,-0.7874,-0.8128,-0.8890,-0.9144,-0.9398};
        carriage_n = 20;

        // Wheel Section polygon
        wheel_section_x = new double[]
            { 0.0254, 0.0254, 0.1445, 0.1643, 0.0828, 0.0000,-0.0828,-0.1643,-0.1445,-0.0254,
             -0.0254};
        wheel_section_y = new double[]
            { 0.0762, 0.5588, 0.5398, 0.6134, 0.6294, 0.6350, 0.6294, 0.6134, 0.5398, 0.5588,
              0.0762};
        wheel_section_n = 11;

        // Explosion polygon
        explosion_poly_x = new double[]
            { 0.0178, 0.0246, 0.0160, 0.0198, 0.0112, 0.0109, 0.0041, 0.0000,-0.0041,-0.0109,
             -0.0112,-0.0198,-0.0160,-0.0246,-0.0178,-0.0246,-0.0160,-0.0198,-0.0112,-0.0109,
             -0.0041, 0.0000, 0.0041, 0.0109, 0.0112, 0.0198, 0.0160, 0.0246, 0.0178};
        explosion_poly_y = new double[]
            { 0.0000, 0.0056, 0.0076, 0.0157, 0.0140, 0.0229, 0.0173, 0.0254, 0.0173, 0.0229,
              0.0140, 0.0157, 0.0076, 0.0056, 0.0000,-0.0056,-0.0076,-0.0157,-0.0140,-0.0229,
             -0.0173,-0.0254,-0.0173,-0.0229,-0.0140,-0.0157,-0.0076,-0.0056, 0.0000};
        explosion_poly_n = 29;

        workPolyX = new int[30];
        workPolyY = new int[30];
    }

    /**
     @param value angle in degrees
    */
    public void setBarrelAngle(int value) {
        barrelAngle = value;
        repaint();
    }

    public int getBarrelAngle() {
        return barrelAngle;
    }

    public void setMuzzleSpeed(int value) {
        muzzleSpeed = value;
        repaint();
    }

    public int getMuzzleVelocity() {
        return muzzleSpeed;
    }

    public void setCannonBallPos(double x, double y) {
        cannonBallPos[0] = x;
        cannonBallPos[1] = y;
    }

    public void setCannonBallVel(double vx, double vy) {
        cannonBallVel[0] = vx;
        cannonBallVel[1] = vy;
    }

    public void setExplosionSize(double size) {
        explosionSize = size;
    }

    public void setExplosionPos(double x, double y) {
        explosionPos[0] = x;
        explosionPos[1] = y;
    }

    public void setScale (int mapScale) {
        if (mapScale < 4) {
            scale = 4;
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

    public void drawCenteredCircle(Graphics2D g, int x, int y, int r) {
        x = x-(r/2);
        y = y-(r/2);
        g.fillOval(x,y,r,r);
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

        worldOriginX = (width/2) - (int)(scale * cannonBallPos[0]);
        worldOriginY = (height/2) + (int)(scale * cannonBallPos[1]);

        // Draw Sky
        g2d.setPaint(skyColor);
        g2d.fillRect(0, 0, width, worldOriginY);

        // Draw Clouds
        g2d.setPaint(Color.WHITE);
        g2d.fillOval((int)(worldOriginX + scale * 500), (int)(worldOriginY - scale * 20), (int)(scale*20), (int)(scale*5));
        g2d.fillOval((int)(worldOriginX + scale * 510), (int)(worldOriginY - scale * 22), (int)(scale*15), (int)(scale*5));
        g2d.fillOval((int)(worldOriginX + scale * 200), (int)(worldOriginY - scale * 20), (int)(scale*20), (int)(scale*5));
        g2d.fillOval((int)(worldOriginX + scale * 200), (int)(worldOriginY - scale * 22), (int)(scale*15), (int)(scale*5));
        g2d.fillOval((int)(worldOriginX + scale * 0), (int)(worldOriginY - scale * 15), (int)(scale*10), (int)(scale*2));

        // |jpanel_x| = |origin_x| + |scale    0  | * |cos(angle) -sin(angle)| * |world_x|
        // |jpanel_y|   |origin_y|   |  0   -scale|   |sin(angle)  cos(angle)|   |world_y|

        //  Draw cannon ball
        g2d.setPaint(cannonBallColor);
        int bx = (int)(worldOriginX + scale * cannonBallPos[0]);
        int by = (int)(worldOriginY - scale * cannonBallPos[1]);
        drawCenteredCircle(g2d, bx, by, (int)(scale * cannonBallRadius));

        double velMag = Math.sqrt(cannonBallVel[0]*cannonBallVel[0] + cannonBallPos[1]*cannonBallPos[1]);
        if (velMag > 0.1) {
            g2d.setPaint(Color.RED);
            int vx = (int)(worldOriginX + scale * (cannonBallPos[0] - cannonBallVel[0]/velMag));
            int vy = (int)(worldOriginY - scale * (cannonBallPos[1] - cannonBallVel[1]/velMag));
            g2d.drawLine( bx, by, vx, vy);
        }

        // Draw cannon barrel
        double angle =  Math.toRadians(barrelAngle);
        for (ii = 0; ii < barrel_n; ii++) {
            workPolyX[ii] = (int)(worldOriginX + scale * 
                ( Math.cos(angle) * barrel_x[ii] - Math.sin(angle) * barrel_y[ii] + barrelPivotPos[0]));
            workPolyY[ii] = (int)(worldOriginY - scale *
                ( Math.sin(angle) * barrel_x[ii] + Math.cos(angle) * barrel_y[ii] + barrelPivotPos[1]));
        }
        g2d.setPaint(barrelColor);
        g2d.fillPolygon(workPolyX, workPolyY, barrel_n);

        // Draw carriage.
        angle =  Math.toRadians(0);
        for (ii = 0; ii < carriage_n; ii++) {
            workPolyX[ii] = (int)(worldOriginX + scale *
                ( Math.cos(angle) * carriage_x[ii] - Math.sin(angle) * carriage_y[ii] + barrelPivotPos[0]));
            workPolyY[ii] = (int)(worldOriginY - scale *
                ( Math.sin(angle) * carriage_x[ii] + Math.cos(angle) * carriage_y[ii] + barrelPivotPos[1]));
        }
        g2d.setPaint(carriageColor);
        g2d.fillPolygon(workPolyX, workPolyY, carriage_n);

        // Draw wheel.
        for (jj = 0; jj < 12; jj++) {
            angle = Math.toRadians(jj*30);
            for (ii = 0; ii < wheel_section_n; ii++) {
                workPolyX[ii] = (int)(worldOriginX + scale *
                    ( Math.cos(angle) * wheel_section_x[ii] - Math.sin(angle) * wheel_section_y[ii] + wheelCenterPos[0]));
                workPolyY[ii] = (int)(worldOriginY - scale *
                    ( Math.sin(angle) * wheel_section_x[ii] + Math.cos(angle) * wheel_section_y[ii] + wheelCenterPos[1]));
            }
            g2d.setPaint(wheelColor);
            g2d.fillPolygon(workPolyX, workPolyY, wheel_section_n);
        }

        // Draw wheel hub.
        g2d.setPaint(wheelColor);
        int WheelCenter_x = (int)(worldOriginX + scale * wheelCenterPos[0]);
        int WheelCenter_y = (int)(worldOriginY - scale * wheelCenterPos[1]);
        drawCenteredCircle(g2d, WheelCenter_x, WheelCenter_y, (int)(scale * wheelHubRadius));

        // Draw explosion.
        if ( explosionSize > 0.0 ) {
            for (jj = 0; jj < explosionColors_n ; jj ++) {
                for (ii = 0; ii < explosion_poly_n ; ii ++) {
                    double ex = explosionSize * (explosionColors_n - jj) * explosion_poly_x[ii]; 
                    double ey = explosionSize * (explosionColors_n - jj) * explosion_poly_y[ii]; 
                    workPolyX[ii] = (int)(worldOriginX + scale * (ex + explosionPos[0] ));
                    workPolyY[ii] = (int)(worldOriginY - scale * (ey + explosionPos[1] ));
                }
                g2d.setPaint(explosionColors[jj]);
                g2d.fillPolygon(workPolyX, workPolyY, explosion_poly_n);
            }
        }

        // Draw ground.
        g2d.setPaint(groundColor);
        g2d.fillRect(0, worldOriginY, width, height);

        // Draw range markers.
        int tickRange = 50;
        if (scale >=  8) tickRange = 20;
        if (scale >= 16) tickRange = 10;
        if (scale >= 32) tickRange =  5;
        if (scale >= 64) tickRange =  1;

        int lower = ((int)((      - worldOriginX)/(scale * tickRange)) + 1) * tickRange;
        int upper = ((int)((width - worldOriginX)/(scale * tickRange)) + 1) * tickRange;

        g2d.setPaint(Color.BLACK);

        for (ii = lower ; ii < upper ; ii += tickRange) {
            int mx = (int)(worldOriginX + scale * ii);
            g2d.drawLine( mx, worldOriginY, mx, worldOriginY + 20);
            g2d.drawString ( String.format("%d",ii), mx, worldOriginY + 15);
        }

        g2d.drawString ( String.format("SCALE: %d pixels/meter",scale), 20,20);
        g2d.drawString ( String.format("Barrel angle: [%d]", barrelAngle), 20,40);
        g2d.drawString ( String.format("Muzzle speed: [%d]", muzzleSpeed), 20,60);
        g2d.drawString ( String.format("Cannonball Pos: [%.2f, %.2f]", cannonBallPos[0], cannonBallPos[1]), 20,80);
        g2d.drawString ( String.format("Cannonball Vel: [%.2f, %.2f]", cannonBallVel[0], cannonBallVel[1]), 20,100);

    }

    @Override
    public void paintComponent(Graphics g) {
        super.paintComponent(g);
        doDrawing(g);
    }
}

class ElevationPanel extends JPanel implements ChangeListener {

    static final int ANGLE_MIN   =  0;
    static final int ANGLE_MAX   = 90;
    static final int ANGLE_INIT  = 30;

    private CannonDisplay cannonDisplay;
    private JSlider angleSlider ;

    public ElevationPanel (CannonDisplay cd) {
        cannonDisplay = cd;
        setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));

        JLabel sliderLabel = new JLabel("Elev.", JLabel.CENTER);
        sliderLabel.setAlignmentX(CENTER_ALIGNMENT);

        angleSlider = new JSlider(JSlider.VERTICAL, ANGLE_MIN, ANGLE_MAX, ANGLE_INIT);
        angleSlider.addChangeListener(this);
        angleSlider.setToolTipText("Angle of the Cannon barrel.");
        angleSlider.setMajorTickSpacing(10);
        angleSlider.setMinorTickSpacing(1);
        angleSlider.setPaintTicks(true);
        angleSlider.setPaintLabels(true);

        add(sliderLabel);
        add(angleSlider);

    }

    @Override
    public void stateChanged(ChangeEvent e) {
        JSlider source = (JSlider)e.getSource();
        if (!source.getValueIsAdjusting()) {
            cannonDisplay.setBarrelAngle( source.getValue() );
        }
    }

    public void setValue(int value) {
        if (value > ANGLE_MAX) value = ANGLE_MAX;
        if (value < ANGLE_MIN) value = ANGLE_MIN;
        angleSlider.setValue(value);
    }
}

class MuzzleVelPanel extends JPanel implements ChangeListener {

    static final int VEL_MIN  =   0;
    static final int VEL_MAX  = 100;
    static final int VEL_INIT =  50;

    private CannonDisplay cannonDisplay;
    private JSlider velSlider ;

    public MuzzleVelPanel (CannonDisplay cd) {
        cannonDisplay = cd;
        setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));

        JLabel sliderLabel = new JLabel("Vel.", JLabel.CENTER);
        sliderLabel.setAlignmentX(CENTER_ALIGNMENT);

        velSlider = new JSlider(JSlider.VERTICAL, VEL_MIN, VEL_MAX, VEL_INIT);
        velSlider.addChangeListener(this);
        velSlider.setToolTipText("Muzzle Velocity.");
        velSlider.setMajorTickSpacing(10);
        velSlider.setMinorTickSpacing(1);
        velSlider.setPaintTicks(true);
        velSlider.setPaintLabels(true);

        add(sliderLabel);
        add(velSlider);
    }

    @Override
    public void stateChanged(ChangeEvent e) {
        JSlider source = (JSlider)e.getSource();
        if (!source.getValueIsAdjusting()) {
            cannonDisplay.setMuzzleSpeed( source.getValue() );
        }
    }

    public void setValue(int value) {
        if (value > VEL_MAX) value = VEL_MAX;
        if (value < VEL_MIN) value = VEL_MIN;
        velSlider.setValue(value);
    }
}

class ButtonPanel extends JPanel implements ActionListener {

    private CannonDisplay cannonDisplay;
    private boolean fireCommand;
    private boolean armCommand;
    private boolean armAcknowledged;
    private JButton fireButton, armButton, zoomOutButton, zoomInButton;

    public ButtonPanel(CannonDisplay cd) {
        cannonDisplay = cd;
        fireCommand = false;
        armCommand = false;
        armAcknowledged = false;
        setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

        fireButton = new JButton("FIRE");
        fireButton.addActionListener(this);
        fireButton.setActionCommand("fire");
        fireButton.setToolTipText("Fire Cannon");
        add(fireButton);

        armButton = new JButton("RELOAD");
        armButton.addActionListener(this);
        armButton.setActionCommand("arm");
        armButton.setToolTipText("Arm Cannon");
        add(armButton);

        zoomOutButton = new JButton("\u25b2");
        zoomOutButton.addActionListener(this);
        zoomOutButton.setActionCommand("zoomout");
        zoomOutButton.setToolTipText("Zoom in");
        add(zoomOutButton);

        zoomInButton = new JButton("\u25bc");
        zoomInButton.addActionListener(this);
        zoomInButton.setActionCommand("zoomin");
        zoomInButton.setToolTipText("Zoom Out");
        add(zoomInButton);
    }

    public void actionPerformed(ActionEvent e) {
        String s = e.getActionCommand();
        switch (s) {
            case "fire" :
                if (armAcknowledged) {
                    fireCommand = true;
                }
                break;
            case "arm" :
                armCommand = true;
                break;
            case "zoomin":
                cannonDisplay.setScale( cannonDisplay.getScale() / 2 );
                break;
            case "zoomout":
                cannonDisplay.setScale( cannonDisplay.getScale() * 2 );
                break;
            default:
                System.out.println("Unknown Action Command:" + s);
                break;
        }
    }

    public void resetArmCommand() {
        armCommand = false;
        armAcknowledged = true;
        fireButton.setForeground(Color.red);
    }

    public void resetFireCommand() {
        fireCommand = false;
        fireButton.setForeground(Color.gray);
    }

    public boolean getArmCommand() {
        return armCommand;
    }
    public boolean getFireCommand() {
        return fireCommand;
    }

} // class ButtonPanel

class TrickSimMode {
    public static final int INIT = 0;
    public static final int FREEZE = 1;
    public static final int RUN = 5;
}

public class CannonDisplay extends JFrame {

    private RangeView rangeView;
    private ElevationPanel elevPanel;
    private MuzzleVelPanel velPanel;
    private BufferedReader in;
    private DataOutputStream out;
    private JPanel panelGroup0;
    private JPanel panelGroup1;
    private ButtonPanel buttonPanel;

    public enum SoundEffect {
        EXPLOSION("Explosion.wav"),
        CANNONBOOM("CannonBoom.wav");
   
        private Clip clip;
   
        SoundEffect(String soundFileName) {
            try {
            URL url = this.getClass().getClassLoader().getResource(soundFileName);
            AudioInputStream audioInputStream = AudioSystem.getAudioInputStream(url);
            clip = AudioSystem.getClip();
            clip.open(audioInputStream);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
   
        public void play() {
            if (clip.isRunning()) clip.stop(); 
            clip.setFramePosition(0);
            clip.start();
        }
   
        static void init() {
            values();
        }
    }
       
    public CannonDisplay(RangeView arena) {
        setTitle("Cannon Range");

        elevPanel = new ElevationPanel(this);
        velPanel  = new MuzzleVelPanel(this);
        rangeView  = arena;

        panelGroup1 = new JPanel();
        panelGroup1.setLayout(new BoxLayout(panelGroup1, BoxLayout.X_AXIS));
        panelGroup1.add(elevPanel);
        panelGroup1.add(velPanel);
        panelGroup1.add(rangeView);

        buttonPanel = new ButtonPanel(this);

        panelGroup0 = new JPanel();
        panelGroup0.setLayout(new BoxLayout(panelGroup0, BoxLayout.Y_AXIS));
        panelGroup0.add(panelGroup1);
        panelGroup0.add(buttonPanel);
        add(panelGroup0);

        setScale(32);
        setBarrelAngle(30);
        setMuzzleSpeed(50);
        setCannonBallPos (-0.1020, 0.9400);

        setSize(800, 500);
        setLocationRelativeTo(null);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        SoundEffect.init();
    }

    public int getBarrelAngle() {
        return rangeView.getBarrelAngle();
    }

    public void setBarrelAngle(int value) {
        rangeView.setBarrelAngle(value);
        elevPanel.setValue(value);
    }

    public int getMuzzleVelocity() {
        return rangeView.getMuzzleVelocity();
    }

    public void setMuzzleSpeed(int value) {
        rangeView.setMuzzleSpeed(value);
        velPanel.setValue(value);
    }

    public void setCannonBallPos(double x, double y) {
        rangeView.setCannonBallPos(x,y);
    }

    public void setCannonBallVel(double vx, double vy) {
        rangeView.setCannonBallVel(vx,vy);
    }

    public void setExplosionSize(double size) {
        rangeView.setExplosionSize(size);
    }

    public void setExplosionPos(double x, double y) {
        rangeView.setExplosionPos(x,y);
    }

    public void setScale(int value) {
        rangeView.setScale(value);
    }

    public int getScale() {
        return rangeView.getScale();
    }

    public void resetArmCommand() {
        buttonPanel.resetArmCommand();
    }

    public void resetFireCommand() {
        buttonPanel.resetFireCommand();
    }

    public boolean getArmCommand() {
        return buttonPanel.getArmCommand();
    }

    public boolean getFireCommand() {
        return buttonPanel.getFireCommand();
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
          + "usage: java jar CannonDisplay.jar <port-number>\n"
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
        double dt = 0.100; // Time between updates (seconds).
        double posx = 0.0; 
        double posy = 0.0;
        double velx = 0.0; 
        double vely = 0.0;
        double angle;
        double speed;
        boolean impact = false;
        boolean prevImpact = false;
        boolean armCommand;
        boolean fireCommand;
        int simMode = 0;

        int     exticks = 0;
        double  exTime = 0.0;
        ModelState explosionModelState = ModelState.INACTIVE; 

        int mapScale = 32 ; // pixels per meter.

        RangeView rangeView = new RangeView( mapScale);
        CannonDisplay evd = new CannonDisplay( rangeView);
        evd.setVisible(true);
        evd.drawRangeView();


        if (port == 0) {
            System.out.println("No variable server port specified.");
            printHelpText();
            System.exit(0);
        }

        // Connect to the Trick simulation's variable server
        System.out.println("Connecting to: " + host + ":" + port);
        evd.connectToServer(host, port);

        evd.out.writeBytes("trick.var_set_client_tag(\"CannonDisplay\") \n");
        evd.out.flush();

        // Have the Variable Server send us the simulation mode ONCE.
        evd.out.writeBytes( "trick.var_add(\"trick_sys.sched.mode\")\n" +
                            "trick.var_send() \n" +
                            "trick.var_clear() \n");
        evd.out.flush();

        // Read the response and extract the simulation mode.
        try {
            String line;
            String field[];
            line = evd.in.readLine();
            field = line.split("\t");
            simMode = Integer.parseInt( field[1]); 
        } catch (IOException | NullPointerException e ) {
            go = false;
        }

        // If we're in FREEZE, make ready to fire.
        if (simMode == TrickSimMode.FREEZE) {
            evd.resetArmCommand();
        }

        // Configure the Variable Server to cyclically send us the following varibales.
        // Tell the variable server:
        //  1) We want the values of the following variables:
        evd.out.writeBytes( "trick.var_pause() \n" +
                            "trick.var_add(\"dyn.cannon.impact\")\n" +
                            "trick.var_add(\"dyn.cannon.pos[0]\")\n" +
                            "trick.var_add(\"dyn.cannon.pos[1]\")\n" +
                            "trick.var_add(\"dyn.cannon.vel[0]\")\n" +
                            "trick.var_add(\"dyn.cannon.vel[1]\")\n" +
                            "trick.var_add(\"trick_sys.sched.mode\")\n" +
        //  2) We want the responses in ASCII:
                            "trick.var_ascii() \n" +
        //  3) We want values to be updated at the specified rate:
                            String.format("trick.var_cycle(%.3f)\n", dt) +
        //  4) Start sending values as specified.
                            "trick.var_unpause() \n" );
        evd.out.flush();

        while (go) {
             
            // Recieve and parse periodic data response from the variable server.
            try {
                String line;
                String field[];
                line = evd.in.readLine();
                field = line.split("\t");
                impact = ( Integer.parseInt( field[1]) != 0); 
                posx   = Double.parseDouble( field[2]);
                posy   = Double.parseDouble( field[3]);
                velx   = Double.parseDouble( field[4]);
                vely   = Double.parseDouble( field[5]);
                simMode = Integer.parseInt( field[6]); 
            } catch (IOException | NullPointerException e ) {
                go = false;
            }

            if (simMode == TrickSimMode.RUN) {
                if (boom) {
                    SoundEffect.CANNONBOOM.play();
                    boom = false;
                }
            }

            // Update the display data.
            evd.setCannonBallPos(posx, posy);
            evd.setCannonBallVel(velx, vely);

            if (simMode == TrickSimMode.FREEZE) {
                if (evd.getArmCommand()) {
                    evd.out.writeBytes( String.format("dyn.cannon.pos[0] = %.2f ;\n", -0.1020 ));
                    evd.out.writeBytes( String.format("dyn.cannon.pos[1] = %.2f ;\n",  0.9400 ));
                    evd.out.writeBytes( String.format("dyn.cannon.vel[0] = %.2f ;\n",  0.0000 ));
                    evd.out.writeBytes( String.format("dyn.cannon.vel[1] = %.2f ;\n",  0.0000 ));
                    evd.out.flush();
                    evd.resetArmCommand();
                }

                if (evd.getFireCommand()) {
                    angle = evd.getBarrelAngle();
                    speed = evd.getMuzzleVelocity();
                    evd.out.writeBytes( String.format("dyn.cannon.init_angle = %.2f ;\n", Math.toRadians(angle)));
                    evd.out.writeBytes( String.format("dyn.cannon.init_speed = %.2f;\n", speed ));
                    evd.out.writeBytes( String.format("dyn.cannon.pos0[0] = %.2f ;\n", -0.1020 ));
                    evd.out.writeBytes( String.format("dyn.cannon.pos0[1] = %.2f ;\n",  0.9400 ));
                    evd.out.writeBytes( String.format("dyn.cannon.time = %.2f ;\n",  0.0 ));
                    evd.out.writeBytes( String.format("trick.cannon_init( dyn.cannon );\n" ));
                    evd.out.writeBytes( String.format("trick.exec_run();\n" ));
                    evd.out.flush();
                    evd.resetFireCommand();
                    boom = true;
                }
            }

            if (simMode == TrickSimMode.RUN) {
                if (impact && !prevImpact) {
                    evd.out.writeBytes( String.format("trick.exec_freeze();\n" ));
                    evd.out.flush();
                }
            }

            // Explosion Model
            exticks ++;
            exTime = exticks * dt;
            switch(explosionModelState) {
                case INACTIVE:
                    if (impact && !prevImpact) { // Trigger on leading edge.
                        exticks = 0;
                        evd.setExplosionPos(posx, posy);
                        explosionModelState = ModelState.ACTIVE;
                        SoundEffect.EXPLOSION.play();
                    }
                    break; 
                case ACTIVE:
                    if (exTime > 4.0) {
                        evd.setExplosionSize(0.0);
                        explosionModelState = ModelState.INACTIVE;
                    } else if (exTime > 0.0) {
                        evd.setExplosionSize(exTime * exTime * 5.0);
                    } else {
                        evd.setExplosionSize(0.0);
                    }
                    break; 
                default:
                    break; 
            }
            prevImpact = impact;

            //  Update the scene.
            evd.drawRangeView();

        } // while
    } // main
} // class
