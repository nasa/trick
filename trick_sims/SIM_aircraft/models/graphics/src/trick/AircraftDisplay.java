/*
 * Trick
 * 2016 (c) National Aeronautics and Space Administration (NASA)
 */

package trick;

import java.awt.Graphics2D;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.geom.AffineTransform;
import java.awt.image.AffineTransformOp;
import java.awt.image.BufferedImage;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.FileReader;
import java.net.Socket;
import java.util.*;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JPanel;
import java.awt.Color;

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

class SkyView extends JPanel {

    private double scale; // Pixels per meter

    private Color groundColor;

    private double[] aircraftPos;
    private double[] aircraftVel;
    private double heading;
    private ScenePoly aircraft;
    private ScenePoly wpmarker;
    private int[] workPolyX, workPolyY;

    // Origin of world coordinates in jpanel coordinates.
    private int worldOriginX;
    private int worldOriginY;

    public SkyView( double mapScale ) {
        scale = mapScale;
        setScale(mapScale);

        groundColor = new Color(210,180,140);

        aircraftPos  = new double[] {0.0, 0.0};
        aircraftVel  = new double[] {0.0, 0.0};

        aircraft = new ScenePoly();
        aircraft.color = Color.GRAY;
        aircraft.x = new double[] { 4.00, 2.00, 0.60,-0.40,-1.00,-1.00, -1.60, -2.00, -2.50, -2.50, -2.40, -2.80, -2.80, -2.40, -2.50, -2.50, -2.00, -1.60, -1.00, -1.00, -0.40, 0.60, 2.00 };
        aircraft.y = new double[] { 0.00, 0.40, 0.80, 2.30, 2.30, 0.60,  0.60,  1.40,  1.40,  0.50,  0.30,  0.30, -0.30, -0.30, -0.50, -1.40, -1.40, -0.60, -0.60, -2.30, -2.30,-0.80,-0.40};
        aircraft.n = 23;

        wpmarker = new ScenePoly();
        wpmarker.color = Color.RED;
        wpmarker.x = new double[] { 1.00, 0.00, -1.00,  0.00 };
        wpmarker.y = new double[] { 0.00, 1.00,  0.00, -1.00 };
        wpmarker.n = 4;

        workPolyX = new int[30];
        workPolyY = new int[30];
    }

    public void setAircraftPos( double n, double w) {
        aircraftPos[0] = n;
        aircraftPos[1] = w;
    }

    public void setAircraftVel( double n, double w) {
        aircraftVel[0] = n;
        aircraftVel[1] = w;
        heading = Math.atan2(w,n);
    }

    public void setScale (double mapScale) {
        if (mapScale < 0.00005) {
            scale = 0.00005;
        } else {
            scale = mapScale;
        }
    }

    public void drawCenteredOval(Graphics2D g, int x, int y, int rh, int rv) {
        x = x-(rh/2);
        y = y-(rv/2);
        g.drawOval(x,y,rh,rv);
    }

    public void fillCenteredCircle(Graphics2D g, int x, int y, int r) {
        x = x-(r/2);
        y = y-(r/2);
        g.fillOval(x,y,r,r);
    }

    public void drawScenePoly(Graphics2D g, ScenePoly p, double angle_r , double north, double west) {

        double mag = 800.0;
        for (int ii = 0; ii < p.n; ii++) {
            workPolyX[ii] = (int)(worldOriginX - scale *
                ( Math.sin(angle_r) * mag * p.x[ii] + Math.cos(angle_r) * mag * p.y[ii] + west));
            workPolyY[ii] = (int)(worldOriginY - scale *
                ( Math.cos(angle_r) * mag * p.x[ii] - Math.sin(angle_r) * mag * p.y[ii] + north));
        }
        g.setPaint(p.color);
        g.fillPolygon(workPolyX, workPolyY, p.n);
    }

    private void doDrawing(Graphics g) {
        Graphics2D g2d = (Graphics2D) g;

        int width = getWidth();
        int height = getHeight();

        worldOriginX = (width/2);
        worldOriginY = (height/2);

        g2d.setPaint(groundColor);
        g2d.fillRect(0, 0, width, height);

        g2d.setPaint(Color.BLACK);
        g2d.drawLine( 0, worldOriginY, width, worldOriginY);
        g2d.drawLine( worldOriginX, 0, worldOriginX, height);

        //  Draw Waypoints
        drawScenePoly(g2d, wpmarker, 0.0,      0.0,  25000.0 );
        drawScenePoly(g2d, wpmarker, 0.0,  21650.0,  12500.0 );
        drawScenePoly(g2d, wpmarker, 0.0,  21650.0, -12500.0 );
        drawScenePoly(g2d, wpmarker, 0.0,      0.0, -25000.0 );
        drawScenePoly(g2d, wpmarker, 0.0, -21650.0, -12500.0 );
        drawScenePoly(g2d, wpmarker, 0.0, -21650.0,  12500.0 );

        //  Draw Aircraft
        drawScenePoly(g2d, aircraft, heading, aircraftPos[0], aircraftPos[1] );

        // Display State Data
        g2d.setPaint(Color.BLACK);
        g2d.drawString ( String.format("Aircraft Pos: [%.2f, %.2f]", aircraftPos[0], aircraftPos[1]), 20,40);
        g2d.drawString ( String.format("Aircraft Vel: [%.2f, %.2f]", aircraftVel[0], aircraftVel[1]), 20,60);
    }

    @Override
    public void paintComponent(Graphics g) {
        super.paintComponent(g);
        doDrawing(g);
    }
}

public class AircraftDisplay extends JFrame {

    private SkyView skyView;
    private BufferedReader in;
    private DataOutputStream out;

    public AircraftDisplay(SkyView sky) {
        skyView = sky;
        add( skyView);
        setTitle("Aircraft Display");
        setSize(800, 800);
        setLocationRelativeTo(null);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    }

    public void connectToServer(String host, int port ) throws IOException {
        Socket socket = new Socket(host, port);
        in = new BufferedReader( new InputStreamReader( socket.getInputStream()));
        out = new DataOutputStream(new BufferedOutputStream(socket.getOutputStream()));
    }

    public void drawSkyView() {
        skyView.repaint();
    }

    private static void  printHelpText() {
        System.out.println(
            "----------------------------------------------------------------------\n"
          + "usage: java jar AircraftDisplay.jar <port-number>\n"
          + "----------------------------------------------------------------------\n"
          );
    }

    public static void main(String[] args) throws IOException {

        String host = "localHost";
        int port = 0;
        String vehicleImageFile = null;

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

        if (port == 0) {
            System.out.println("No variable server port specified.");
            printHelpText();
            System.exit(0);
        }

        double mapScale = 0.01; // 100 meters per pixel
        SkyView skyview = new SkyView( mapScale);
        AircraftDisplay sd = new AircraftDisplay(skyview);
        sd.setVisible(true);
        double posNorth = 0.0;
        double posWest = 0.0;
        double velNorth = 0.0;
        double velWest = 0.0;

        System.out.println("Connecting to: " + host + ":" + port);
        sd.connectToServer(host, port);

        sd.out.writeBytes("trick.var_set_client_tag(\"AircraftDisplay\") \n" +
                          "trick.var_pause() \n" +
                          "trick.var_add(\"dyn.aircraft.pos[0]\") \n" +
                          "trick.var_add(\"dyn.aircraft.pos[1]\") \n" +
                          "trick.var_add(\"dyn.aircraft.vel[0]\") \n" +
                          "trick.var_add(\"dyn.aircraft.vel[1]\") \n" +
                          "trick.var_ascii() \n" +
                          "trick.var_cycle(0.1) \n" +
                          "trick.var_unpause()\n" );
        sd.out.flush();

        Boolean go = true;

        while (go) {
            String field[];
            try {
                String line;
                line = sd.in.readLine();
                field = line.split("\t");
                posNorth = Double.parseDouble( field[1] );
                posWest = Double.parseDouble( field[2] );
                velNorth = Double.parseDouble( field[3] );
                velWest = Double.parseDouble( field[4] );

                // Set the Aircraft position
                skyview.setAircraftPos(posNorth, posWest);
                skyview.setAircraftVel(velNorth, velWest);

            } catch (IOException | NullPointerException e ) {
                go = false;
            }
            sd.drawSkyView();
        }
    }
}
