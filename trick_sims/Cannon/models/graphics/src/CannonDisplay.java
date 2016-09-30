/*
 * Trick
 * 2016 (c) National Aeronautics and Space Administration (NASA)
 */

//package trick;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.RenderingHints;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.geom.AffineTransform;
import java.awt.image.AffineTransformOp;
import java.awt.image.BufferedImage;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
import java.util.*;
import javax.swing.BoxLayout;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

/**
 *
 * @author penn
 */

class ArenaMap extends JPanel {

    private int scale;
    private Color skyColor;
    private Color groundColor;
    private Color barrelColor;
    private Color cannonBallColor;
    private Color carriageColor;
    private Color wheelColor;
    private Color[] explosionColors;
    int     explosionColors_n;

    // Origin of world coordinates in jpanel coordinates.
    int Orig_w_j_x;
    int Orig_w_j_y;

    int      barrelAngle;
    int      muzzleVelocity;
    double[] cannonBallPos;

    double   explosionTime;
    double[] barrelPivotPos;
    double[] explosionPos;
    double[] wheelCenterPos;
    double   wheelHubRadius;
    double   cannonBallRadius;

    double[] barrel_x, barrel_y;
    int barrel_n;

    double[] carriage_x, carriage_y;
    int carriage_n;

    double[] wheel_section_x, wheel_section_y;
    int wheel_section_n;

    double[] explosion_poly_x, explosion_poly_y;
    int explosion_poly_n;

    int[] poly_x, poly_y;

    public ArenaMap( int mapScale) {

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
        muzzleVelocity = 0;

        explosionTime = 0.0;
        cannonBallPos  = new double[] 
        //  { -4.00, 37.00};
            {-0.1020, 0.9400};
        barrelPivotPos = new double[]
        //  { -4.00, 37.00};
            {-0.1020, 0.9400};
        explosionPos = new double[]
            { 5.0800, 0.0000};
        wheelCenterPos = new double[]
        //  {  0.00, 25.00};
            { 0.0000,0.6350,};
        wheelHubRadius   = 0.1524;
        cannonBallRadius = 0.1524;

        barrel_x = new double[]
          //{ 50.00,  3.00,-19.00,-23.00,-25.00,-25.00,-25.00,-23.00,-19.00,  3.00,
          //  50.00};
            { 1.2700, 0.0762,-0.4826,-0.5842,-0.6350,-0.6350,-0.6350,-0.5842,-0.4826, 0.0762,
              1.2700};

        barrel_y = new double[]
         // {  4.00,  6.00,  6.00,  5.00,  2.00,  0.00, -2.00, -5.00, -6.00, -6.00,
         //   -4.00};
            { 0.1016, 0.1524, 0.1524, 0.1270, 0.0508, 0.0000,-0.0508,-0.1270,-0.1524,-0.1524,
             -0.1016};
        barrel_n = 11;

        carriage_x = new double[]
         // {-49.00,  1.00,  0.00,  6.00,  8.00, 17.00, 14.00,  1.00,  2.00,  2.00,
         //    0.00, -1.00, -2.00,-52.00,-55.00,-57.00,-58.00,-58.00,-57.00,-56.00};
            {-1.2446, 0.0254, 0.0000, 0.1524, 0.2032, 0.4318, 0.3556, 0.0254, 0.0508, 0.0508,
              0.0000,-0.0254,-0.0508,-1.3208,-1.3970,-1.4478,-1.4732,-1.4732,-1.4478,-1.4224};
        carriage_y = new double[]
         // {-37.00,-13.00,-10.00, -7.00,-10.00, -6.00,  2.00,  2.00,  0.00, -1.00,
         //   -2.00, -1.00,  0.00,-30.00,-30.00,-31.00,-32.00,-35.00,-36.00,-37.00};
            {-0.9398,-0.3302,-0.2540,-0.1778,-0.2540,-0.1524, 0.0508, 0.0508, 0.0000,-0.0254,
             -0.0508,-0.0254, 0.0000,-0.7620,-0.7620,-0.7874,-0.8128,-0.8890,-0.9144,-0.9398};
        carriage_n = 20;

        wheel_section_x = new double[]
         // {  1.00,  1.00,  5.69,  6.47,  3.26,  0.00, -3.26, -6.47, -5.69, -1.00,
         //   -1.00};
            { 0.0254, 0.0254, 0.1445, 0.1643, 0.0828, 0.0000,-0.0828,-0.1643,-0.1445,-0.0254,
             -0.0254};
        wheel_section_y = new double[]
         // {  3.00, 22.00, 21.25, 24.15, 24.78, 25.00, 24.78, 24.15, 21.25, 22.00,
         //    3.00};
            { 0.0762, 0.5588, 0.5398, 0.6134, 0.6294, 0.6350, 0.6294, 0.6134, 0.5398, 0.5588,
              0.0762};
        wheel_section_n = 11;

        explosion_poly_x = new double[]
         // {  0.70,  0.97,  0.63,  0.78,  0.44,  0.43,  0.16,  0.00, -0.16, -0.43,
         //   -0.44, -0.78, -0.63, -0.97, -0.70, -0.97, -0.63, -0.78, -0.44, -0.43,
         //   -0.16,  0.00,  0.16,  0.43,  0.44,  0.78,  0.63,  0.97,  0.70};
            { 0.0178, 0.0246, 0.0160, 0.0198, 0.0112, 0.0109, 0.0041, 0.0000,-0.0041,-0.0109,
             -0.0112,-0.0198,-0.0160,-0.0246,-0.0178,-0.0246,-0.0160,-0.0198,-0.0112,-0.0109,
             -0.0041, 0.0000, 0.0041, 0.0109, 0.0112, 0.0198, 0.0160, 0.0246, 0.0178};
        explosion_poly_y = new double[]
         // {  0.00,  0.22,  0.30,  0.62,  0.55,  0.90,  0.68,  1.00,  0.68,  0.90,
         //    0.55,  0.62,  0.30,  0.22,  0.00, -0.22, -0.30, -0.62, -0.55, -0.90,
         //   -0.68, -1.00, -0.68, -0.90, -0.55, -0.62, -0.30, -0.22,  0.00};
            { 0.0000, 0.0056, 0.0076, 0.0157, 0.0140, 0.0229, 0.0173, 0.0254, 0.0173, 0.0229,
              0.0140, 0.0157, 0.0076, 0.0056, 0.0000,-0.0056,-0.0076,-0.0157,-0.0140,-0.0229,
             -0.0173,-0.0254,-0.0173,-0.0229,-0.0140,-0.0157,-0.0076,-0.0056, 0.0000};
        explosion_poly_n = 29;

        poly_x = new int[30];
        poly_y = new int[30];
    }

    public void initCannonballPos() {
        cannonBallPos[0] = -0.1020;
        cannonBallPos[1] =  0.9400;
    }

    public void setBarrelAngle(int value) {
        barrelAngle = value;
        repaint();
    }

    public int getBarrelAngle() {
        return barrelAngle;
    }

    public void setMuzzleVelocity(int value) {
        muzzleVelocity = value;
        repaint();
    }

    public int getMuzzleVelocity() {
        return muzzleVelocity;
    }

    public void setCannonBallPos(double x, double y) {
        cannonBallPos[0] = x;
        cannonBallPos[1] = y;
        repaint();
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

        // NOTE: <Orig_w_j_x, Orig_w_j_y> is the orig of the world coordinate system expressed in jpanel coords.
        // This origin tracks the position of the cannonball.
        Orig_w_j_x = (width/2) - (int)(scale * cannonBallPos[0]);
        Orig_w_j_y = getHeight()-20;

        // Draw Sky
        g2d.setPaint(skyColor);
        g2d.fillRect(0, 0, width, Orig_w_j_y);

        // |jpanel_x| = |origin_x| + scale * |1  0| * | cosa -sina| * |world_x|
        // |jpanel_y|   |origin_y|           |0 -1|   | sina  cosa|   |world_y|


        //  Draw cannon ball
          g2d.setPaint(cannonBallColor);
          int bx = (int)(Orig_w_j_x + scale * cannonBallPos[0]);
          int by = (int)(Orig_w_j_y - scale * cannonBallPos[1]);
          drawCenteredCircle(g2d, bx, by, (int)(scale * cannonBallRadius));

        // Draw cannon barrel
        double angle =  Math.toRadians(barrelAngle);
        for (ii = 0; ii<barrel_n; ii++) {
            poly_x[ii] = (int)(Orig_w_j_x + scale * ( Math.cos(angle) * barrel_x[ii] - Math.sin(angle) * barrel_y[ii] + barrelPivotPos[0]));
            poly_y[ii] = (int)(Orig_w_j_y - scale * ( Math.sin(angle) * barrel_x[ii] + Math.cos(angle) * barrel_y[ii] + barrelPivotPos[1]));
        }
        g2d.setPaint(barrelColor);
        g2d.fillPolygon(poly_x, poly_y, barrel_n);

        // Draw carriage
        angle =  Math.toRadians(0);
        for (ii = 0; ii<carriage_n; ii++) {
            poly_x[ii] = (int)(Orig_w_j_x + scale * ( Math.cos(angle) * carriage_x[ii] - Math.sin(angle) * carriage_y[ii] + barrelPivotPos[0]));
            poly_y[ii] = (int)(Orig_w_j_y - scale * ( Math.sin(angle) * carriage_x[ii] + Math.cos(angle) * carriage_y[ii] + barrelPivotPos[1]));
        }
        g2d.setPaint(carriageColor);
        g2d.fillPolygon(poly_x, poly_y, carriage_n);

        // Draw wheel
        for (jj = 0; jj<12; jj++) {
            angle = Math.toRadians(jj*30);
            for (ii = 0; ii<wheel_section_n; ii++) {
                poly_x[ii] = (int)(Orig_w_j_x + scale * ( Math.cos(angle) * wheel_section_x[ii] - Math.sin(angle) * wheel_section_y[ii] + wheelCenterPos[0]));
                poly_y[ii] = (int)(Orig_w_j_y - scale * ( Math.sin(angle) * wheel_section_x[ii] + Math.cos(angle) * wheel_section_y[ii] + wheelCenterPos[1]));
            }
            g2d.setPaint(wheelColor);
            g2d.fillPolygon(poly_x, poly_y, wheel_section_n);
        }

        // Draw wheel hub
        g2d.setPaint(wheelColor);
        int WheelCenter_x = (int)(Orig_w_j_x + scale * wheelCenterPos[0]);
        int WheelCenter_y = (int)(Orig_w_j_y - scale * wheelCenterPos[1]);
        drawCenteredCircle(g2d, WheelCenter_x, WheelCenter_y, (int)(scale * wheelHubRadius));

        // Draw Explosion
        if (explosionTime > 0.0) {
            for (jj = 0; jj < explosionColors_n ; jj ++) {
                for (ii = 0; ii < explosion_poly_n ; ii ++) {
                    double ex = explosionTime * explosionTime * 5.0 * (explosionColors_n - jj) * explosion_poly_x[ii]; 
                    double ey = explosionTime * explosionTime * 5.0 * (explosionColors_n - jj) * explosion_poly_y[ii]; 
                    poly_x[ii] = (int)(Orig_w_j_x + scale * (ex + explosionPos[0] ));
                    poly_y[ii] = (int)(Orig_w_j_y - scale * (ey + explosionPos[1] ));
                }
                g2d.setPaint(explosionColors[jj]);
                g2d.fillPolygon(poly_x, poly_y, explosion_poly_n);
            }
        }

        // Draw Ground
        g2d.setPaint(groundColor);
        g2d.fillRect(0, Orig_w_j_y, width, height);


        // Draw Range Markers
        int tickRange = 50;
        if (scale >=  8) tickRange =20;
        if (scale >= 16) tickRange =10;
        if (scale >= 32) tickRange = 5;
        if (scale >= 64) tickRange = 1;

        double lower = -Orig_w_j_x / scale;
        double upper = (width - Orig_w_j_x) / scale;

        int LL = ((int)(lower/tickRange) + 1) * tickRange;
        int UU = ((int)(upper/tickRange) + 1) * tickRange;

        g2d.setPaint(Color.BLACK);

        for (int kk = LL ; kk < UU ; kk += tickRange) {
            int mx = (int)(Orig_w_j_x + scale * kk);
            g2d.drawLine( mx, Orig_w_j_y, mx, Orig_w_j_y+20);
            g2d.drawString ( String.format("%d",kk), mx, Orig_w_j_y+15);

        }

        g2d.drawString ( String.format("SCALE: %d",scale), 20,20);
        g2d.drawString ( String.format("Cannon barrel angle: [%d]",barrelAngle), 20,40);
        g2d.drawString ( String.format("Cannon muzzle velocity: [%d]",muzzleVelocity), 20,60);
        g2d.drawString ( String.format("Cannonball Pos: [%.2f, %.2f]",cannonBallPos[0],cannonBallPos[1]), 20,80);
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
    static final int ANGLE_INIT  = 45;

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

    public void stateChanged(ChangeEvent e) {
        JSlider source = (JSlider)e.getSource();
        if (!source.getValueIsAdjusting()) {
            cannonDisplay.setMuzzleVelocity( source.getValue() );
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

        armButton = new JButton("ARM");
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

    public void AcknowledgeArmed() {
        armCommand = false;
        armAcknowledged = true;
        fireButton.setForeground(Color.red);
    }

    public void AcknowledgeFired() {
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


public class CannonDisplay extends JFrame {
       
    private ArenaMap arenaMap;
    private ElevationPanel elevPanel;
    private MuzzleVelPanel velPanel;
    private BufferedReader in;
    private DataOutputStream out;
    private JPanel panelGroup0;
    private JPanel panelGroup1;
    private ButtonPanel buttonPanel;
    private JButton b1, b2, b3, b4, b5;

    public CannonDisplay(ArenaMap arena) {
        setTitle("Cannon Range");

        elevPanel = new ElevationPanel(this);
        velPanel  = new MuzzleVelPanel(this);
        arenaMap  = arena;

        panelGroup1 = new JPanel();
        panelGroup1.setLayout(new BoxLayout(panelGroup1, BoxLayout.X_AXIS));
        panelGroup1.add(elevPanel);
        panelGroup1.add(velPanel);
        panelGroup1.add(arenaMap);

        buttonPanel = new ButtonPanel(this);

        panelGroup0 = new JPanel();
        panelGroup0.setLayout(new BoxLayout(panelGroup0, BoxLayout.Y_AXIS));
        panelGroup0.add(panelGroup1);
        panelGroup0.add(buttonPanel);
        add(panelGroup0);

        setScale(32);
        setBarrelAngle(30);
        setMuzzleVelocity(500);
        setCannonBallPos (-0.1020, 0.9400);


        setSize(800, 500);
        setLocationRelativeTo(null);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    }

    public int getBarrelAngle() {
        return arenaMap.getBarrelAngle();
    }

    public void setBarrelAngle(int value) {
        arenaMap.setBarrelAngle(value);
        elevPanel.setValue(value);
    }

    public int getMuzzleVelocity() {
        return arenaMap.getMuzzleVelocity();
    }

    public void setMuzzleVelocity(int value) {
        arenaMap.setMuzzleVelocity(value);
        velPanel.setValue(value);
    }

    public void setCannonBallPos(double x, double y) {
        arenaMap.setCannonBallPos(x,y);
    }

    public void setScale(int value) {
        arenaMap.setScale(value);
    }

    public int getScale() {
        return arenaMap.getScale();
    }

    public void AcknowledgeArmed() {
        buttonPanel.AcknowledgeArmed();
    }

    public void AcknowledgeFired() {
        buttonPanel.AcknowledgeFired();
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

    public void drawArenaMap() {
        arenaMap.repaint();
    }

    private static void  printHelpText() {
        System.out.println(
            "----------------------------------------------------------------------\n"
          + "usage: java jar CannonDisplay.jar <port-number>\n"
          + "----------------------------------------------------------------------\n"
          );
    }

    public static void main(String[] args) throws IOException, InterruptedException {

        String host = "localHost";
        int port = 0;

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

//        if (port == 0) {
//            System.out.println("No variable server port specified.");
//            printHelpText();
//            System.exit(0);
//        }

        int mapScale = 32 ; // pixels per meter.
        ArenaMap arenaMap = new ArenaMap( mapScale);
        CannonDisplay evd = new CannonDisplay( arenaMap);
        evd.setVisible(true);

//        System.out.println("Connecting to: " + host + ":" + port);
//        evd.connectToServer(host, port);
//
//        evd.out.writeBytes("trick.var_set_client_tag(\"CannonDisplay\") \n");
//        evd.out.flush();
//
//        evd.out.writeBytes("trick.var_add(\"dyn.cannon.pos[0]\") \n" +
//                           "trick.var_add(\"dyn.cannon.pos[1]\") \n" +
//                           "trick.var_add(\"dyn.cannon.pos[1]\") \n");
//        evd.out.flush();
//
//        evd.out.writeBytes("trick.var_ascii() \n" +
//                           "trick.var_cycle(0.1) \n" +
//                           "trick.var_send() \n" );
//        evd.out.flush();


        boolean fireCommand = false;
        boolean armCommand = false;
        boolean armed = false;
        boolean fired = false;
        boolean impact = false;
        double  impactTime =  0.0;
        int     ticks = 0;
        int     cTimeRate = 0;
        double  cTime = 0.0;
        int     cticks = 0;
        int     exTimeRate = 0; // 0 or 1
        double  exTime = 0.0;
        int     exticks = 0;
        double  posx = 0.0;
        double  posy = 0.0;
        double  posx0 = 0.0;
        double  posy0 = 0.0;
        double  exposx = 0.0;
        double  exposy = 0.0;
        double  speed = 0.0;
        double  angle = 0.0;
        double  elev = 0.0;
        double  simdt = 0.100;

        evd.drawArenaMap();

        Boolean go = true;
        while (go) {
            Thread.sleep((int)(1000 * simdt));
            ticks ++; 

            // === DATA OUT TO THE SIM ===
            angle = evd.getBarrelAngle();
            speed = evd.getMuzzleVelocity();

            armCommand  = evd.getArmCommand();
            fireCommand = evd.getFireCommand();
            // =============================

            if (cTimeRate == 0) {
                if (armCommand) {
                    posx0 = posx = -0.1020;
                    posy0 = posy =  0.9400;
                    armed = true;
                    fired = false;
                }
                if (fireCommand && armed ) {
                    elev = angle * (3.14159/180.0);
                    cticks = 0;
                    cTimeRate = 1;
                    armed = false;
                    fired = true;
                }
            } else {
                cticks ++; 
                cTime = cticks * cTimeRate * simdt;

                posx = posx0 + cTime *  speed * Math.cos(elev) ;
                posy = posy0 + cTime * (speed * Math.sin(elev) + 0.5 * -9.81 * cTime);

                if (posy < 0.0) {
                   impact = true;
                   impactTime = ((2 * speed * Math.sin(elev)) / 9.81);
                   posx = impactTime * speed * Math.cos(elev); 
                   posy = 0;
                   cTimeRate = 0; //Stop Cannonball 
                }
            }

            if (exTimeRate == 0) {
                if (impact) {
                    impact = false;
                    exposx = posx;
                    exposy = posy;

                    // Start Explosion
                    exticks = 0;
                    exTimeRate = 1;

                }
            } else {
                exticks ++;
                exTime = exticks * exTimeRate * simdt;

                if (exTime > 4.0) {
                    exTimeRate = 0;
                    exTime = 0.0;
                }
            }

            System.out.print("cTime = " + String.format("%.5f",cTime) );
            System.out.print(" exTime = " + String.format("%.5f",exTime) );
            System.out.println(" ticks = " + String.format("%d",ticks) );

            // ==== DATA IN FROM THE SIM ====
            evd.setCannonBallPos(posx, posy);
            if (armed) {
                evd.AcknowledgeArmed();
            }
            if (fired) {
                evd.AcknowledgeFired();
            }
            arenaMap.explosionTime   = exTime;
            arenaMap.explosionPos[0] = exposx;
            arenaMap.explosionPos[1] = exposy;
           
            // =============================


            evd.drawArenaMap();
        }
    }
}
