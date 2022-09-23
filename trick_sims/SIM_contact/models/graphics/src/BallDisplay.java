/*
 * Trick
 * 2020 (c) National Aeronautics and Space Administration (NASA)
 *
 * @author penn
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
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JPanel;

class Ball {
    public Color color;
    public double x;
    public double y;
    public double radius;
    public int identity;
    public Ball (int id) {
        identity = id;
        x = 0.0;
        y = 0.0;
        radius = 0.5;
        if (id == 0) {
            color = Color.GREEN;
        } else {
            color = Color.LIGHT_GRAY;
        }
    }
}

class ControlPanel extends JPanel implements ActionListener {

    private RangeView rangeView;
    private JButton zoomOutButton, zoomInButton;

    public ControlPanel(RangeView view) {

        rangeView = view;
        setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

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

class RangeView extends JPanel {

    private int scale;
    private Color backGroundColor;

    // Origin of world coordinates in jpanel coordinates.
    private int worldOriginX;
    private int worldOriginY;

    public Ball[] balls;

    /**
     * Class constructor.
     */
    public RangeView( int mapScale, int numberOfBalls) {
        setScale(mapScale);
        backGroundColor = Color.WHITE;
        balls = new Ball[numberOfBalls];
        for (int ii=0 ; ii<numberOfBalls ; ii++) {
            balls[ii] = new Ball(ii);
        }
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

    public void drawCenteredCircle(Graphics2D g, int x, int y, int d) {
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

        // Draw Background
        g2d.setPaint(backGroundColor);
        g2d.fillRect(0, 0, width, height);

        //  Draw balls
        for (int ii = 0; ii < balls.length ; ii++) {
            g2d.setPaint(balls[ii].color);
            int bx = (int)(worldOriginX + scale * balls[ii].x);
            int by = (int)(worldOriginY - scale * balls[ii].y);
            drawCenteredCircle(g2d, bx, by, (int)(scale * 2 * balls[ii].radius));
            g2d.setPaint(Color.BLACK);
            g2d.drawString ( String.format("%d",ii), bx,by);
        }

        g2d.drawString ( String.format("SCALE: %d pixels/meter",scale), 20,20);

    }

    @Override
    public void paintComponent(Graphics g) {
        super.paintComponent(g);
        doDrawing(g);
    }
}

public class BallDisplay extends JFrame {

    private RangeView rangeView;
    private BufferedReader in;
    private DataOutputStream out;

    public BallDisplay() {

        rangeView = null;
        setTitle("Ball Arena");
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

    public void createGUI( int mapScale, int numberOfBalls ) {

        rangeView = new RangeView(mapScale, numberOfBalls);
        JPanel panel1 = new JPanel();
        panel1.setLayout(new BoxLayout(panel1, BoxLayout.X_AXIS));
        panel1.add(rangeView);

        ControlPanel controlPanel = new ControlPanel(rangeView);
        JPanel panel0 = new JPanel();
        panel0.setLayout(new BoxLayout(panel0, BoxLayout.Y_AXIS));
        panel0.add(panel1);
        panel0.add(controlPanel);
        add(panel0);
        setVisible(true);
    }

    private static void  printHelpText() {
        System.out.println(
            "----------------------------------------------------------------------\n"
          + "usage: java jar BallDisplay.jar <port-number>\n"
          + "----------------------------------------------------------------------\n"
          );
    }

    public static void main(String[] args) throws IOException, InterruptedException {

        String host = "localHost";
        int port = 0;
        boolean boom = false;

        // Handle program arguments.
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
        double dt = 0.100;  // Time between updates (seconds).
        int mapScale = 32 ; // pixels per meter.
        int nballs = 7;

        if (port == 0) {
            System.out.println("No variable server port specified.");
            printHelpText();
            System.exit(0);
        }

        BallDisplay ballDisplay = new BallDisplay();

        // Connect to the Trick simulation's variable server.
        System.out.println("Connecting to: " + host + ":" + port);
        ballDisplay.connectToServer(host, port);
        ballDisplay.out.writeBytes("trick.var_set_client_tag(\"BallDisplay\") \n");
        ballDisplay.out.flush();

        // Get the number of  balls.
        ballDisplay.out.writeBytes(
            "trick.var_add(\"dyn.contact.nballs\")\n" +
            "trick.var_send() \n" +
            "trick.var_clear() \n");
        ballDisplay.out.flush();
        try {
            String line;
            String field[];
            line = ballDisplay.in.readLine();
            field = line.split("\t");
            nballs = Integer.parseInt( field[1]);
        } catch (IOException | NullPointerException e ) {
            go = false;
        }

        ballDisplay.createGUI(mapScale, nballs);

        // Get the Radii of the balls.
        for ( ii = 0; ii < nballs; ii ++) {
            ballDisplay.out.writeBytes( String.format("trick.var_add(\"dyn.contact.balls[%d][0].radius\")\n", ii));
        }
        ballDisplay.out.flush();
        ballDisplay.out.writeBytes(
            "trick.var_send() \n" +
            "trick.var_clear() \n");
        ballDisplay.out.flush();
        try {
            String line;
            String field[];
            line = ballDisplay.in.readLine();
            field = line.split("\t");
            for ( ii=0; ii < nballs; ii++) {
                ballDisplay.rangeView.balls[ii].radius = Double.parseDouble( field[ii+1]);
            }
        } catch (IOException | NullPointerException e ) {
            go = false;
        }

        // Get the Positions of the balls, and update the display, periodically.
        ballDisplay.out.writeBytes( "trick.var_pause() \n");
        for ( ii = 0; ii < nballs; ii ++) {
            ballDisplay.out.writeBytes(
                  String.format("trick.var_add(\"dyn.contact.balls[%d][0].pos[0]\")\n", ii)
                + String.format("trick.var_add(\"dyn.contact.balls[%d][0].pos[1]\")\n", ii)
            );
        }
        ballDisplay.out.writeBytes("trick.var_ascii() \n" +
                                   String.format("trick.var_cycle(%.3f)\n", dt) +
                                   "trick.var_unpause() \n" );
        ballDisplay.out.flush();
        while (go) {
            try {
                String line;
                String field[];
                line = ballDisplay.in.readLine();
                // System.out.println("Sim->Client:" + line);
                field = line.split("\t");
                for ( ii=0; ii < nballs; ii++) {
                    ballDisplay.rangeView.balls[ii].x = Double.parseDouble( field[2*ii+1]);
                    ballDisplay.rangeView.balls[ii].y = Double.parseDouble( field[2*ii+2]);
                }
            } catch (IOException | NullPointerException e ) {
                go = false;
            }
            //  Update the scene.
            ballDisplay.rangeView.repaint();
        } // while
    } // main
} // class
