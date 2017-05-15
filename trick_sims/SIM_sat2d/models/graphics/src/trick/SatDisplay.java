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

class SkyView extends JPanel {

    private double scale; // Pixels per meter
    private Color skyColor;
    private Color planetColor;
    private Color satelliteColor;
    private int planetRadius;
    private double[] satellitePos;

    // Origin of world coordinates in jpanel coordinates.
    private int worldOriginX;
    private int worldOriginY;

    public SkyView( double mapScale ) {
        scale = mapScale;
        setScale(mapScale);
        skyColor = Color.BLACK;
        planetColor = new Color(100,200,200);
        satelliteColor = new Color(255,0,0);
        planetRadius = 6378000;
        satellitePos  = new double[] {0.0, planetRadius + 200000};
    }

    public void setSatPos (double x, double y) {
        satellitePos[0] = x;
        satellitePos[1] = y;
    }

    public void setScale (double mapScale) {
        if (mapScale < 0.00005) {
            scale = 0.00005;
        } else {
            scale = mapScale;
        }
    }

    public void drawCenteredCircle(Graphics2D g, int x, int y, int r) {
        x = x-(r/2);
        y = y-(r/2);
        g.fillOval(x,y,r,r);
    }

    private void doDrawing(Graphics g) {
        Graphics2D g2d = (Graphics2D) g;

        int width = getWidth();
        int height = getHeight();

        worldOriginX = (width/2);
        worldOriginY = (height/2);

        g2d.setPaint(Color.BLACK);
        g2d.fillRect(0, 0, width, height);

        //  Draw Planet
        g2d.setPaint(planetColor);
        drawCenteredCircle(g2d, worldOriginX, worldOriginY, (int)(scale * 2 * planetRadius));

        //  Draw Satellite
        g2d.setPaint(satelliteColor);
        int sx = (int)(worldOriginX + scale * satellitePos[0]);
        int sy = (int)(worldOriginY - scale * satellitePos[1]);
        drawCenteredCircle(g2d, sx, sy, (int)(10));
    }

    @Override
    public void paintComponent(Graphics g) {
        super.paintComponent(g);
        doDrawing(g);
    }
}

public class SatDisplay extends JFrame {

    private SkyView skyView;
    private BufferedReader in;
    private DataOutputStream out;

    public SatDisplay(SkyView sky) {
        skyView = sky;
        add( skyView);
        setTitle("Orbit Display");
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
          + "usage: java jar SatDisplay.jar <port-number>\n"
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

        double mapScale = 0.00005; // 20,000 meters per pixel
        SkyView skyview = new SkyView( mapScale);
        SatDisplay sd = new SatDisplay(skyview);
        sd.setVisible(true);
        double satX = 0.0;
        double satY = 0.0;

        System.out.println("Connecting to: " + host + ":" + port);
        sd.connectToServer(host, port);

        sd.out.writeBytes("trick.var_set_client_tag(\"SatDisplay\") \n");
        sd.out.flush();

        sd.out.writeBytes("trick.var_add(\"dyn.satellite.pos[0]\") \n" +
                          "trick.var_add(\"dyn.satellite.pos[1]\") \n" );
        sd.out.flush();

        sd.out.writeBytes("trick.var_ascii() \n" +
                           "trick.var_cycle(0.1) \n" +
                           "trick.var_send() \n" );
        sd.out.flush();

        Boolean go = true;

        while (go) {
            String field[];
            try {
                String line;
                line = sd.in.readLine();
                field = line.split("\t");
                satX = Double.parseDouble( field[1] );
                satY = Double.parseDouble( field[2] );

                // Set the Satellite position
                skyview.setSatPos(satX, satY);

            } catch (IOException | NullPointerException e ) {
                go = false;
            }
            sd.drawSkyView();
        }
    }
}
