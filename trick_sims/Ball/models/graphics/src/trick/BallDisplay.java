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

class ArenaView extends JPanel {

    private double scale; // Pixels per meter
    private Color arenaColor;
    private Color ballColor;
    private double[] ballPos;

    // Origin of world coordinates in jpanel coordinates.
    private int worldOriginX;
    private int worldOriginY;

    public ArenaView( double mapScale ) {
        scale = mapScale;
        setScale(mapScale);
        arenaColor = Color.WHITE;
        ballColor = new Color(1,0,0, 0.5f);
        ballPos  = new double[] {0.0, 0.0};
    }

    public void setBallPos (double x, double y) {
        ballPos[0] = x;
        ballPos[1] = y;
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

        g2d.setPaint(arenaColor);
        g2d.fillRect(0, 0, width, height);

        // Horizontal Lines
        g2d.setPaint(Color.LIGHT_GRAY);
        for (int Y = worldOriginY % (int)scale ; Y < height ; Y += (int)scale ) {
            g2d.drawLine(0, Y, width, Y);
        }
        // Vertical Lines
        g2d.setPaint(Color.LIGHT_GRAY);
        for (int X = worldOriginX % (int)scale ; X < width ; X += (int)scale ) {
            g2d.drawLine(X, 0, X, height);
        }
        // Coordinate Axes
        g2d.setPaint(Color.BLACK);
        g2d.drawLine(0, worldOriginY, width, worldOriginY);
        g2d.drawLine(worldOriginX, 0, worldOriginX, height);
        //  Draw Ball
        g2d.setPaint(ballColor);
        int sx = (int)(worldOriginX + scale * ballPos[0]);
        int sy = (int)(worldOriginY - scale * ballPos[1]);
        drawCenteredCircle(g2d, sx, sy, (int)(scale));
    }

    @Override
    public void paintComponent(Graphics g) {
        super.paintComponent(g);
        doDrawing(g);
    }
}

public class BallDisplay extends JFrame {

    private ArenaView arenaView;
    private BufferedReader in;
    private DataOutputStream out;

    public BallDisplay(ArenaView arena) {
        arenaView = arena;
        add( arenaView);
        setTitle("Ball Display");
        setSize(800, 800);
        setLocationRelativeTo(null);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    }

    public void connectToServer(String host, int port ) throws IOException {
        Socket socket = new Socket(host, port);
        in = new BufferedReader( new InputStreamReader( socket.getInputStream()));
        out = new DataOutputStream(new BufferedOutputStream(socket.getOutputStream()));
    }

    public void drawArenaView() {
        arenaView.repaint();
    }

    private static void  printHelpText() {
        System.out.println(
            "----------------------------------------------------------------------\n"
          + "usage: java jar BallDisplay.jar <port-number>\n"
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

        double mapScale = 20; // 20 pixels per meter
        ArenaView arenaview = new ArenaView( mapScale);
        BallDisplay sd = new BallDisplay(arenaview);
        sd.setVisible(true);
        double ballX = 0.0;
        double ballY = 0.0;

        System.out.println("Connecting to: " + host + ":" + port);
        sd.connectToServer(host, port);

        sd.out.writeBytes("trick.var_set_client_tag(\"BallDisplay\") \n");
        sd.out.flush();

        sd.out.writeBytes("trick.var_pause()\n");
        sd.out.writeBytes("trick.var_add(\"ball.state.output.position[0]\") \n" +
                          "trick.var_add(\"ball.state.output.position[1]\") \n" );
        sd.out.flush();

        sd.out.writeBytes("trick.var_ascii() \n" +
                          "trick.var_cycle(0.1) \n" +
                          "trick.var_unpause() \n" );
        sd.out.flush();

        Boolean go = true;

        while (go) {
            String field[];
            try {
                String line;
                line = sd.in.readLine();
                field = line.split("\t");
                ballX = Double.parseDouble( field[1] );
                ballY = Double.parseDouble( field[2] );

                // Set the Ball position
                arenaview.setBallPos(ballX, ballY);

            } catch (IOException | NullPointerException e ) {
                go = false;
            }
            sd.drawArenaView();
        }
    }
}
