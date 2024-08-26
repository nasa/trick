// This file contains the architecture for the single rigid body simulation's 3D graphics client.

package trick;

// AWT is the abstract window toolkit.
import java.awt.Graphics2D;
import java.awt.Graphics;
import java.awt.Color;
import java.awt.event.MouseEvent;

import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.FileReader;

import java.net.Socket;

import java.util.*;

// Swing is a GUI widget toolkit.
import javax.swing.JFrame; // Allows you to manage a top-level window.
import javax.swing.JPanel; // The stuff within the window (controls, buttons, etc.).
import javax.swing.JViewport.ViewListener;
import javax.swing.event.MouseInputAdapter;

import trick.matrixOps.MatrixOps;

/**
 * @author Aditya Girish
 */

 class SRBView extends JPanel { // Main body class of how we view the SRB.
    
    private int screen_half_width;
    private int screen_half_height;

    private double illumination_vector[]; // World coordinates

    private double vantageAzimuth; // Secifies the direction of the celestial light source.
    private double vantageElevation;

    private double vantageDistance;
    private double beta;

    private double worldToVantageRotation[][];

    private Color bodyLineColor;
    private Color bodyFillColor;

    private double[] bodyPos;

    private double bodyToWorldRotation[][];

    private double veh_vrtx_body[][];
    private double veh_vrtx_world[][];
    private int    veh_vrtx_screen[][];

    private int veh_edges[][];
    private int veh_triangles[][];

    private double veh_unit_normals_body[][];
    private double veh_unit_normals_world[][];
    private double veh_unit_normals_vantage[][];

    public SRBView() {

        ViewListener viewListener = new ViewListener();
        addMouseListener(viewListener);
        addMouseMotionListener(viewListener);

        // Direction of light.
        illumination_vector = new double[] {-0.707, 0.0, -0.707};

        // Location of the observer, looking toward the world's origin.
        vantageAzimuth   = Math.toRadians(45.0);
        vantageElevation = Math.toRadians(20.0);
        vantageDistance = 12.0;

        // Half field of view angle
        beta = Math.toRadians(40.0);

        worldToVantageRotation = new double[3][3];
        setAzElRotation(worldToVantageRotation, vantageAzimuth, vantageElevation);

        bodyLineColor =  Color.GRAY;
        bodyFillColor = new Color(255, 255, 250);

        bodyPos = new double[] {0.0, 0.0, 0.0};

        bodyToWorldRotation = new double[][] {{1.0, 0.0, 0.0},
                                              {0.0, 1.0, 0.0},
                                              {0.0, 0.0, 1.0}};

        // NEED TO FILL THIS IN LATER WITH THE REMAINING INFORMATION.


    }

    public void setAzElRotation(double RotationMatrix[][], double azimuth, double elevation) { // Sets the azimuth and elevation rotation.
        double Rotation_about_Y[][] = {
          { Math.cos(elevation), 0.0, Math.sin(elevation)},
          {                 0.0, 1.0,                 0.0},
          {-Math.sin(elevation), 0.0, Math.cos(elevation)}
        };
  
        double Rotation_about_Z[][] = {
          {Math.cos(azimuth), -Math.sin(azimuth), 0.0},
          {Math.sin(azimuth),  Math.cos(azimuth), 0.0},
          {              0.0,                0.0, 1.0}
        };
        MatrixOps.MtimesM( RotationMatrix, Rotation_about_Y, Rotation_about_Z);
      }

 }

 public class SRBDisplay extends JFrame {
  
  private SingleRigidBodyView singleRigidBodyView;
  private BufferedReader in;
  private DataOutputStream out;

  public SRBDisplay( SingleRigidBodyView srbv) { // Creates the display window.
        singleRigidBodyView = srbv;
        add( singlRigidBodyView);
        setTitle("SRB Display");
        setSize(1597, 987);
        setLocationRelativeTo(null);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
  }

  public void connectToServer( String host, int port ) throws IOException { // Connects to the server.
    Socket socket = new Socket(host, port);
    in = new BufferedReader( new InputStreamReader( socket.getInputStream()));
    out = new DataOutputStream(new BufferedOutputStream(socket.getOutputStream()));
  }

  public void drawSRBView() {
    singleRigidBodyView.repaint();
  }

  private static void  printHelpText() {
    System.out.println(
        "----------------------------------------------------------------------\n"
      + "usage: java jar SRBDisplay.jar <port-number>\n"
      + "----------------------------------------------------------------------\n"
      );
  }

  public static void main(String[] args) throws IOException {

    String host = "localHost";
    int port = 0;
    String bodyImageFile = null;

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

    SRBModuleView srbModuleView = new SRBModuleView();

    SRBDisplay sd = new SRBDisplay(SRBModuleView);
    sd.setVisible(true);

    double f_init_x = 0.0;
    double f_init_y = 0.0;
    double f_init_z = 0.0;

    double accX = 0.0;
    double accY = 0.0;
    double accZ = 0.0;

    double angleX = 0.0;
    double angleY = 0.0;
    double angleZ = 0.0;

    double f_angleX = 0.0;
    double f_angleY = 0.0;
    double f_angleZ = 0.0;

    double forceX = 0.0;
    double forceY = 0.0;
    double forceZ = 0.0;

    double omegaX = 0.0;
    double omegaY = 0.0;
    double omegaZ = 0.0;

    double omegaDotX = 0.0;
    double omegaDotY = 0.0;
    double omegaDotZ = 0.0;

    double posX = 0.0;
    double posY = 0.0;
    double posZ = 0.0;

    double Rxx  = 0.0; 
    double Rxy  = 0.0;
    double Rxz  = 0.0;
    double Ryx  = 0.0;
    double Ryy  = 0.0;
    double Ryz  = 0.0;
    double Rzx  = 0.0;
    double Rzy  = 0.0;
    double Rzz  = 0.0;

    double velX = 0.0;
    double velY = 0.0;
    double velZ = 0.0;

    System.out.println("Connecting to: " + host + ":" + port);
    sd.connectToServer(host, port);

    sd.out.writeBytes("trick.var_set_client_tag(\"SRBDisplay\") \n" +
                          "trick.var_pause() \n" +

                          "trick.var_add(\"dyn.body.FORCE_INIT[0]\") \n" + // Ask about the (1) after each number in the QP
                          "trick.var_add(\"dyn.body.FORCE_INIT[1]\") \n" +
                          "trick.var_add(\"dyn.body.FORCE_INIT[2]\") \n" +

                          "trick.var_add(\"dyn.body.acc[0]\") \n" +
                          "trick.var_add(\"dyn.body.acc[1]\") \n" +
                          "trick.var_add(\"dyn.body.acc[2]\") \n" +

                          "trick.var_add(\"dyn.body.angle[0]\") \n" +
                          "trick.var_add(\"dyn.body.angle[1]\") \n" +
                          "trick.var_add(\"dyn.body.angle[2]\") \n" +

                          "trick.var_add(\"dyn.body.angle_force[0]\") \n" +
                          "trick.var_add(\"dyn.body.angle_force[1]\") \n" +
                          "trick.var_add(\"dyn.body.angle_force[2]\") \n" +

                          "trick.var_add(\"dyn.body.force[0]\") \n" +
                          "trick.var_add(\"dyn.body.force[1]\") \n" +
                          "trick.var_add(\"dyn.body.force[2]\") \n" +

                          "trick.var_add(\"dyn.body.omega[0]\") \n" +
                          "trick.var_add(\"dyn.body.omega[1]\") \n" +
                          "trick.var_add(\"dyn.body.omega[2]\") \n" +

                          "trick.var_add(\"dyn.body.omegaDot[0]\") \n" +
                          "trick.var_add(\"dyn.body.omegaDot[1]\") \n" +
                          "trick.var_add(\"dyn.body.omegaDot[2]\") \n" +

                          "trick.var_add(\"dyn.body.pos[0]\") \n" +
                          "trick.var_add(\"dyn.body.pos[1]\") \n" +
                          "trick.var_add(\"dyn.body.pos[2]\") \n" +

                          "trick.var_add(\"dyn.body.rotate[0][0]\") \n" +
                          "trick.var_add(\"dyn.body.rotate[0][1]\") \n" +
                          "trick.var_add(\"dyn.body.rotate[0][2]\") \n" +
                          "trick.var_add(\"dyn.body.rotate[1][0]\") \n" +
                          "trick.var_add(\"dyn.body.rotate[1][1]\") \n" +
                          "trick.var_add(\"dyn.body.rotate[1][2]\") \n" +
                          "trick.var_add(\"dyn.body.rotate[2][0]\") \n" +
                          "trick.var_add(\"dyn.body.rotate[2][1]\") \n" +
                          "trick.var_add(\"dyn.body.rotate[2][2]\") \n" +
                          
                          "trick.var_add(\"dyn.body.vel[0]\") \n" +
                          "trick.var_add(\"dyn.body.vel[1]\") \n" +
                          "trick.var_add(\"dyn.body.vel[2]\") \n" +

                          "trick.var_ascii() \n" +
                          "trick.var_cycle(0.1) \n" +
                          "trick.var_unpause()\n" );
    
    sd.out.flush();

    sd.drawSRBView();

    Boolean go = true;

    while (go) {
      String field[];
      try {
          String line;
          line = sd.in.readLine();
          field = line.split("\t");

          f_init_x = Double.parseDouble( field[1] );
          f_init_y = Double.parseDouble( field[2] );
          f_init_z = Double.parseDouble( field[3] );

          accX = Double.parseDouble( field[4] );
          accY = Double.parseDouble( field[5] );
          accZ = Double.parseDouble( field[6] );

          angleX = Double.parseDouble( field[7] );
          angleY = Double.parseDouble( field[8] );
          angleZ = Double.parseDouble( field[9] );

          f_angleX = Double.parseDouble( field[10] );
          f_angleY = Double.parseDouble( field[11] );
          f_angleZ = Double.parseDouble( field[12] );

          forceX = Double.parseDouble( field[13] );
          forceY = Double.parseDouble( field[14] );
          forceZ = Double.parseDouble( field[15] );

          omegaX = Double.parseDouble( field[16] );
          omegaY = Double.parseDouble( field[17] );
          omegaZ = Double.parseDouble( field[18] );

          omegaDotX = Double.parseDouble( field[19] );
          omegaDotY = Double.parseDouble( field[20] );
          omegaDotZ = Double.parseDouble( field[21] );

          posX = Double.parseDouble( field[21] );
          posY = Double.parseDouble( field[22] );
          posZ = Double.parseDouble( field[23] );

          Rxx = Double.parseDouble( field[24] );
          Rxy = Double.parseDouble( field[25] );
          Rxz = Double.parseDouble( field[26] );
          Ryx = Double.parseDouble( field[27] );
          Ryy = Double.parseDouble( field[28] );
          Ryz = Double.parseDouble( field[29] );
          Rzx = Double.parseDouble( field[30] );
          Rzy = Double.parseDouble( field[31] );
          Rzz = Double.parseDouble( field[32] );

          velX = Double.parseDouble( field[33] );
          velY = Double.parseDouble( field[34] );
          velZ = Double.parseDouble( field[35] );

          // Set the body position
          SRBView.setBodyPos(posX, posY, posZ);

          SRBView.setBodyToWorldRotation( Rxx, Rxy, Rxz,
                                          Ryx, Ryy, Ryz,
                                          Rzx, Rzy, Rzz );

      } catch (IOException | NullPointerException e ) {
          go = false;
      }
      sd.drawSRBView();
    }
  }

 }







































 }