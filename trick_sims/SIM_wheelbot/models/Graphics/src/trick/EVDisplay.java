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

class Feature {

    public double north, west;
    public double heading;
    public BufferedImage bufImage;

    public Feature(double N, double W, double H, String imageFile) {
        north = N;
        west = W;
        heading = H;

        Image img = new ImageIcon(imageFile).getImage();
        bufImage = new BufferedImage(img.getWidth(null), img.getHeight(null),
                BufferedImage.TYPE_INT_ARGB);
        Graphics2D gContext = bufImage.createGraphics();
        gContext.drawImage(img,0,0,null);
        gContext.dispose();
    }

    public void setState(double N, double W, double H) {
        north = N;
        west = W;
        heading = H;
    }
}

class ArenaMap extends JPanel {

    private List<Feature> featureList;
    private double metersPerPixel;
    private Color groundColor;

    public ArenaMap(List<Feature> flist, double mapScale) {
        featureList = flist;
        metersPerPixel = mapScale;
        SetScale(mapScale);
        groundColor = new Color(255,255,255);
    }

    public void SetScale (double mapScale) {
        if (mapScale < 0.00001) {
            metersPerPixel = 0.00001;
        } else {
            metersPerPixel = mapScale;
        }
    }

    private void doDrawing(Graphics g) {
        Graphics2D g2d = (Graphics2D) g;

        int width = getWidth();
        int height = getHeight();

        g2d.setPaint(groundColor);
        g2d.fillRect(0, 0, width, height);

        // Translate map origin to the center of the panel.
        Graphics2D gCenter = (Graphics2D)g2d.create();
        gCenter.translate(width/2, height/2);

        //gCenter.scale(2.0, 2.0); // Makes pixels bigger.

        for (int ii=0 ; ii < featureList.size() ; ++ii ) {

            Feature feature = featureList.get(ii);

            int featureX = (int) -(feature.west / metersPerPixel);
            int featureY = (int) -(feature.north / metersPerPixel);

            double drawHeading = -feature.heading;

            int ImgOffsetX = feature.bufImage.getWidth()/2;
            int ImgOffsetY = feature.bufImage.getHeight()/2;

            int drawLocationX = featureX - ImgOffsetX;
            int drawLocationY = featureY - ImgOffsetY;

            AffineTransform tx = AffineTransform.getRotateInstance( drawHeading, ImgOffsetX, ImgOffsetY);
            AffineTransformOp op = new AffineTransformOp( tx, AffineTransformOp.TYPE_BILINEAR);
            gCenter.drawImage( op.filter( feature.bufImage, null), drawLocationX, drawLocationY, null);
        }
        gCenter.dispose();
    }

    @Override
    public void paintComponent(Graphics g) {
        super.paintComponent(g);
        doDrawing(g);
    }
}

public class EVDisplay extends JFrame {

    private ArenaMap arenaMap;
    private BufferedReader in;
    private DataOutputStream out;

    public EVDisplay(ArenaMap arena) {
        arenaMap = arena;
        add( arenaMap);
        setTitle("Vehicle Arena");
        setSize(800, 800);
        setLocationRelativeTo(null);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
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
          + "usage: java jar EVDisplay.jar <port-number>\n"
          + "----------------------------------------------------------------------\n"
          );
    }

    public static void main(String[] args) throws IOException {

        String host = "localHost";
        int port = 0;
        String wayPointsFile = null;
        String vehicleImageFile = null;

        int ii = 0;
        while (ii < args.length) {
            switch (args[ii]) {
                case "-help" :
                case "--help" : {
                    printHelpText();
                    System.exit(0);
                } break;
                case "-v" : {
                    ++ii;
                    if (ii < args.length) {
                        vehicleImageFile = args[ii];
                    }
                } break;
                case "-w" : {
                    ++ii;
                    if (ii < args.length) {
                        wayPointsFile = args[ii];
                    }
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

        if (wayPointsFile == null) {
            System.out.println("No waypoints file specified. Use the -w option to specify a waypoints file.");
            printHelpText();
            System.exit(0);
        }

        if (vehicleImageFile == null) {
            System.out.println("No vehicle image file specified. Use the -v option to specify the vehicle image file.");
            printHelpText();
            System.exit(0);
        }

        List<Feature> featureList = new ArrayList<>();

        try ( BufferedReader br = new BufferedReader( new FileReader( wayPointsFile))) {
            String line;
            while ((line = br.readLine()) != null) {
                String field[] = line.split(",");
                double N = Double.parseDouble(field[0]);
                double W = Double.parseDouble(field[1]);
                double H = Double.parseDouble(field[2]);
                String imageFileName = field[3];
                featureList.add(new Feature( N, W, H, imageFileName));
            }
        }

        Feature vehicle = new Feature(0, 0, Math.toRadians(0), vehicleImageFile);
        featureList.add(vehicle);

        double mapScale = 0.005; // 5 millimeters per pixel
        EVDisplay evd = new EVDisplay( new ArenaMap( featureList, mapScale));
        evd.setVisible(true);

        System.out.println("Connecting to: " + host + ":" + port);
        evd.connectToServer(host, port);

        evd.out.writeBytes("trick.var_set_client_tag(\"EVDisplay\") \n" +
                           "trick.var_pause() \n" +
                           "trick.var_add(\"veh.vehicle.position[0]\") \n" +
                           "trick.var_add(\"veh.vehicle.position[1]\") \n" +
                           "trick.var_add(\"veh.vehicle.heading\") \n" + 
                           "trick.var_ascii() \n" +
                           "trick.var_cycle(0.1) \n" +
                           "trick.var_unpause() \n" );
        evd.out.flush();

        Boolean go = true;

        while (go) {
            String field[];
            try {
                String line;
                line = evd.in.readLine();
                field = line.split("\t");
                double N = Double.parseDouble( field[1] );
                double W = Double.parseDouble( field[2] );
                double H = Double.parseDouble( field[3] );
                vehicle.setState(N,W,H);

            } catch (IOException | NullPointerException e ) {
                go = false;
            }
            evd.drawArenaMap();
        }
    }
}
