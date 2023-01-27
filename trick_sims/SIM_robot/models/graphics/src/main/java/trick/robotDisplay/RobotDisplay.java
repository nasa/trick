/*
 * Trick
 * 2023 (c) National Aeronautics and Space Administration (NASA)
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

class Link {
  public double l; // length of the link (meters).
  public double q; // joint angle (radians).

  public Link() {
    l = 1.0;
    q = 0.0;

  }
}

class ScenePoly {
    public Color color;
    public int n;
    public double[] x;
    public double[] y;
}

class RangeView extends JPanel {

    private int scale;
    int nlinks;
    private Color sunglowColor;
    private Color grey200Color;
    private Color grey150Color;
    private ScenePoly link_arm;

    // Origin of world coordinates in jpanel coordinates.
    private int worldOriginX;
    private int worldOriginY;

    public Link[] links;

    // private ScenePoly upright;

    private int[] workPolyX, workPolyY;

    /**
     * Class constructor.
     */
    public RangeView( int mapScale, int numberOfLinks) {

      setScale(mapScale);
      nlinks = numberOfLinks;

      sunglowColor = new Color(255,204,51);
      grey200Color = new Color(200,200,200);
      grey150Color = new Color(150,150,150);

      links = new Link[nlinks];
      for (int ii=0 ; ii<nlinks ; ii++) {
          links[ii] = new Link();
      }

      link_arm = new ScenePoly();
      link_arm.color = grey200Color;
      link_arm.n = 4;
      link_arm.x = new double[4];
      link_arm.y = new double[4];


      workPolyX = new int[30];
      workPolyY = new int[30];
    }

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

    public void setScale (int mapScale) {
        if (mapScale < 2) {
            scale = 2;
        } else if (mapScale > 512) {
            scale = 512;
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
        g.drawOval(x,y,d,d);
    }

    public void fillCenteredCircle(Graphics2D g, int x, int y, int d) {
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

        // ===============================================================================
        //  Draw Robot
        // ===============================================================================
        double q_sum = 0.0;
        double l_sum = 0.0;
        double base_x = 0.0;
        double base_y = 0.0;
        double end_x;
        double end_y;

        double base_width;
        double end_width;

        // Find the total length of the links
        double extended_length = 0.0;
        for (int ii=0 ; ii<nlinks ; ii++) {
            extended_length += links[ii].l;
        }
        base_width = 0.10 * extended_length;

        g2d.setPaint(Color.RED);
        drawCenteredCircle(g2d,
            (int)(worldOriginX),
            (int)(worldOriginY),
            (int)(scale * 2 * extended_length )
         );

        for (int ii=0 ; ii<nlinks ; ii++) {
            q_sum += links[ii].q;
            l_sum += links[ii].l;

            // Calculate the width of the current link.
            end_width = 0.10 * (extended_length - l_sum);

            // Calculate the position of the end of the current link.
            end_x = base_x + links[ii].l * Math.cos(q_sum);
            end_y = base_y + links[ii].l * Math.sin(q_sum);

            // link_arm.x[0] = 0.0;          link_arm.y[0] =  base_width/2.0;
            // link_arm.x[1] = links[ii].l;  link_arm.y[1] =  end_width/2.0;
            // link_arm.x[2] = links[ii].l;  link_arm.y[2] = -end_width/2.0;
            // link_arm.x[3] = 0.0;          link_arm.y[3] = -base_width/2.0;

            link_arm.x[0] = 0.0;          link_arm.y[0] =  base_width/2.0;
            link_arm.x[1] = links[ii].l;  link_arm.y[1] =  base_width/2.0;
            link_arm.x[2] = links[ii].l;  link_arm.y[2] = -base_width/2.0;
            link_arm.x[3] = 0.0;          link_arm.y[3] = -base_width/2.0;

            drawScenePoly(g2d, link_arm, q_sum, base_x, base_y);

            g2d.setPaint(sunglowColor);
            fillCenteredCircle(g2d,
                (int)(worldOriginX + scale * base_x),
                (int)(worldOriginY - scale * base_y),
                (int)(scale * base_width )
            );

            g2d.setPaint(grey150Color);
            fillCenteredCircle(g2d,
               (int)(worldOriginX + scale * base_x),
               (int)(worldOriginY - scale * base_y),
               (int)(scale * 0.5 * base_width )
            );

            // Draw a circle centered around each link base, of radius l.
            // g2d.drawOval( (int)(worldOriginX + scale*(base_x - links[ii].l)),
            //               (int)(worldOriginY - scale*(base_y - links[ii].l)),
            //               (int)(scale* (links[ii].l/2.0) ),
            //               (int)(scale* (links[ii].l/2.0)));

            // The base position of the next link is the end position of the current link.
            base_x = end_x;
            base_y = end_y;
            base_width = end_width;
        }

        // ===============================================================================
        // Draw Information
        // ===============================================================================
        g2d.setPaint(Color.BLACK);
        g2d.drawString ( String.format("SCALE: %d pixels/meter",scale), 20,20);


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

class ControlPanel extends JPanel implements ActionListener {

    private RangeView rangeView;
    private JButton zoomOutButton, zoomInButton;
    private JButton shutDownButton;

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

public class RobotDisplay extends JFrame {

    private RangeView rangeView;
    private BufferedReader in;
    private DataOutputStream out;
    private JPanel panel0;
    private JPanel panel1;
    private ControlPanel controlPanel;

    public RobotDisplay() {

        rangeView  = null;
        setTitle("Robot Range");
        setSize(800, 800);
        setLocationRelativeTo(null);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setFocusable(true);
    }

    public void connectToServer(String host, int port ) throws IOException {
        Socket socket = new Socket(host, port);
        in = new BufferedReader( new InputStreamReader( socket.getInputStream()));
        out = new DataOutputStream(new BufferedOutputStream(socket.getOutputStream()));
    }

    public void createGUI( int mapScale, int numberOfLinks ) {

        setTitle("Robot Range");
        rangeView = new RangeView(mapScale, numberOfLinks);

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

    public void drawRangeView() {
        rangeView.repaint();
    }

    private static void  printHelpText() {
        System.out.println(
            "----------------------------------------------------------------------\n"
          + "usage: java jar RobotDisplay.jar <port-number>\n"
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

        int nlinks = 0;
        boolean standalone = false;

        int mapScale = 128 ; // pixels per meter.

        if (port == 0) {
            System.out.println("No variable server port specified.");
            printHelpText();
            System.exit(0);
        }

       RobotDisplay robotDisplay = new RobotDisplay();

        // Connect to the Trick simulation's variable server
        System.out.println("Connecting to: " + host + ":" + port);
        robotDisplay.connectToServer(host, port);
        robotDisplay.out.writeBytes("trick.var_set_client_tag(\"RobotDisplay\") \n");
        robotDisplay.out.flush();

        // Have the Variable Server send us the number of links once.
        robotDisplay.out.writeBytes(
            "trick.var_add(\"dyn.arm.nlinks\")\n" +
            "trick.var_send() \n" +
            "trick.var_clear() \n");
        robotDisplay.out.flush();
        try {
            String line;
            String field[];
            line = robotDisplay.in.readLine();
            field = line.split("\t");
            nlinks  = Integer.parseInt( field[1]);
        } catch (IOException | NullPointerException e ) {
            go = false;
        }

        System.out.println("Number of Links: " + nlinks);

        robotDisplay.createGUI(mapScale, nlinks);

        // Configure the Variable Server to cyclically send data.
        robotDisplay.out.writeBytes( "trick.var_pause() \n");
        for ( ii = 0; ii < nlinks; ii ++) {
            robotDisplay.out.writeBytes(
                  String.format("trick.var_add(\"dyn.arm.links[%d][0].l\")\n", ii)
                + String.format("trick.var_add(\"dyn.arm.links[%d][0].q\")\n", ii)
            );
        }
        robotDisplay.out.writeBytes(
          "trick.var_ascii() \n" +
          String.format("trick.var_cycle(%.3f)\n", dt) +
          "trick.var_unpause() \n" );
        robotDisplay.out.flush();

        //  Update the scene.
        robotDisplay.drawRangeView();

        while (go) {

            // Recieve and parse periodic data response from the variable server.
            try {
                String line;
                String field[];
                line = robotDisplay.in.readLine();
                field   = line.split("\t");

                for ( ii=0; ii < nlinks; ii++) {
                    robotDisplay.rangeView.links[ii].l = Double.parseDouble( field[ii*2 +1]);
                    //System.out.println("robotDisplay.rangeView.links["+ ii+ "].l: = " + robotDisplay.rangeView.links[ii].l);
                    robotDisplay.rangeView.links[ii].q = Double.parseDouble( field[ii*2 +2]);
                    //System.out.println("robotDisplay.rangeView.links["+ ii+ "].q: = " + robotDisplay.rangeView.links[ii].q);
                }
            } catch (IOException | NullPointerException e ) {
                go = false;
            }

            //  Update the scene.
            robotDisplay.drawRangeView();

        } // while
    } // main
} // class
