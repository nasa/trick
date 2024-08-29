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
//import javax.swing.JViewport.ViewListener;
import javax.swing.event.MouseInputAdapter;

import trick.matrixOps.MatrixOps;

/**
 * @author Aditya Girish
 */

class SingleRigidBodyView extends JPanel { // Main body class of how we view the SRB.
  
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

  public SingleRigidBodyView() {

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

      
      // The dodecahedron has 4 "layers" to it. I have labeled them as follows: top (T), upper-middle (UM), lower-middle (LM), and bottom (B)
      veh_vrtx_body = new double[][]
        { {-1.37638,0.000000,0.262866}, // 0 - UM
        {1.37638,0.000000,-0.262866}, // 1 - LM
        {-0.425325,-1.30902,0.262866}, // 2 - UM
        {-0.425325,1.30902,0.262866}, // 3 - UM
        {1.11352,-0.809017,0.262866}, // 4 - UM
        {1.11352,0.809017,0.262866}, // 5 - UM
        {-0.262866,-0.809017,1.11352}, // 6 - T
        {-0.262866,0.809017,1.11352}, // 7 - T
        {-0.688191,-0.5,-1.11352}, // 8 - B
        {-0.688191,0.5,-1.11352}, // 9 - B
        {0.688191,-0.5,1.11352}, // 10 - T
        {0.688191,0.5,1.11352}, // 11 - T
        {0.850651,0.000000,-1.11352}, // 12 - B
        {-1.11352,-0.809017,-0.262866}, // 13 - LM
        {-1.11352,0.809017,-0.262866}, // 14 - LM
        {-0.850651,0.000000,1.11352}, // 15 - T
        {0.262866,-0.809017,-1.11352}, // 16 - B
        {0.262866,0.809017,-1.11352}, // 17 - B
        {0.425325,-1.30902,-0.262866}, // 18 - LM
        {0.425325,1.30902,-0.262866},}; // 19 - LM

        veh_vrtx_world = new double[veh_vrtx_body.length][3];
        veh_vrtx_screen = new int[veh_vrtx_body.length][2];

        // Defines the edges for the dodecahedron.
        veh_edges = new int[][]
        { {0, 2}, {0, 3}, {0, 13}, {0, 14}, {1, 4}, {1, 5}, {1, 12}, {1, 19}, 
          {2, 6}, {2, 13}, {3, 7}, {3, 14}, {4, 10}, {4, 18}, {5, 11}, {5, 19},
          {6, 10}, {6, 13}, {7, 11}, {7, 14}, {8, 12}, {8, 13}, {9, 12}, {9, 14},
          {8, 16}, {8, 17}, {9, 16}, {9, 17}, {10, 15}, {11, 15}, {12, 17}, {12, 18},
          {13, 16}, {14, 17}, {15, 6}, {15, 7}, {16, 18}, {17, 19}, {18, 4}, {19, 5}
        };

        // The order of the vertices in each of the triangles is significant. The points should follow
        // a path counter clock wise about the triangle's normal vector. The position normal side of
        // the triangle is the visable side.

        // NEED TO CHANGE THIS WITH SPHERE MODEL
        veh_triangles = new int[][]
        {
          {0, 2, 6}, {0, 6, 10}, {0, 10, 15}, // First pentagon using vertices 0, 2, 6, 10, 15
          {0, 15, 7}, {0, 7, 3}, {0, 3, 14}, // Second pentagon using vertices 0, 15, 7, 3, 14
          {0, 14, 9}, {0, 9, 13}, {0, 13, 2}, // Third pentagon using vertices 0, 14, 9, 13, 2
          {1, 5, 19}, {1, 19, 17}, {1, 17, 12}, // Fourth pentagon using vertices 1, 5, 19, 17, 12
          {1, 12, 8}, {1, 8, 4}, {1, 4, 10}, // Fifth pentagon using vertices 1, 12, 8, 4, 10
          {1, 18, 10}, {1, 10, 6}, {1, 6, 2}, // Sixth pentagon using vertices 1, 18, 20, 6, 2
          {8, 12, 17}, {8, 17, 16}, {8, 16, 13}, // Seventh pentagon using vertices 8, 12, 17, 16, 13
          {9, 17, 19}, {9, 19, 14}, {9, 14, 13}, // Eighth pentagon using vertices 9, 17, 19, 14, 13
          {4, 10, 15}, {4, 15, 7}, {4, 7, 3}, // Ninth pentagon using vertices 4, 10, 15, 7, 3
          {18, 4, 3}, {18, 3, 14}, {18, 14, 19} // Tenth pentagon using vertices 18, 4, 3, 14, 19
        };

        // Create the normals for the triangles
        veh_unit_normals_body = new double[veh_triangles.length][3];
        double v1[] = {0.0, 0.0, 0.0};
        double v2[] = {0.0, 0.0, 0.0};
        for (int i=0; i < veh_unit_normals_body.length; i++ ) {
            MatrixOps.VminusV (v1, veh_vrtx_body[ veh_triangles[i][1] ], veh_vrtx_body[ veh_triangles[i][0] ]);
            MatrixOps.VminusV (v2, veh_vrtx_body[ veh_triangles[i][2] ], veh_vrtx_body[ veh_triangles[i][0] ]);
            MatrixOps.VcrossV (veh_unit_normals_body[i], v1, v2);
            double vmag = MatrixOps.Vmagnitude(veh_unit_normals_body[i]);
            MatrixOps.Vscale (veh_unit_normals_body[i], veh_unit_normals_body[i], 1.0/vmag);
        }

        veh_unit_normals_world = new double[ veh_unit_normals_body.length ][3];
        veh_unit_normals_vantage = new double[ veh_unit_normals_body.length ][3];

    }

  private class ViewListener extends MouseInputAdapter {
    private int start_x;
    private int start_y;
    public void mousePressed(MouseEvent e) {
        start_x = e.getX();
        start_y = e.getY();
    }
    public void mouseDragged(MouseEvent e) {
      int dx = ( e.getX() - start_x );
      int dy = ( start_y  - e.getY());
      start_x = e.getX();
      start_y = e.getY();
      mouseVantage( dx, dy);
    }
  }

  public void setVantageRange(double range) {
    vantageDistance = range;
  }

  public void setBodyToWorldRotation( double xx, double xy, double xz,
                                      double yx, double yy, double yz,
                                      double zx, double zy, double zz ) {
    bodyToWorldRotation[0][0] = xx;
    bodyToWorldRotation[0][1] = xy;
    bodyToWorldRotation[0][2] = xz;
    bodyToWorldRotation[1][0] = yx;
    bodyToWorldRotation[1][1] = yy;
    bodyToWorldRotation[1][2] = yz;
    bodyToWorldRotation[2][0] = zx;
    bodyToWorldRotation[2][1] = zy;
    bodyToWorldRotation[2][2] = zz;
  }

  public void mouseVantage(int dx, int dy) {
    vantageAzimuth   += (dx * Math.PI) / getWidth();
    if (vantageAzimuth >  Math.PI) vantageAzimuth -= Math.PI;
    if (vantageAzimuth < -Math.PI) vantageAzimuth += Math.PI;
    vantageElevation -= (dy * Math.PI) / getHeight();
    if (vantageElevation >  Math.toRadians( 89.0)) vantageElevation = Math.toRadians( 89.0);
    if (vantageElevation <  Math.toRadians(-89.0)) vantageElevation = Math.toRadians(-89.0);
    setAzElRotation(worldToVantageRotation, vantageAzimuth, vantageElevation);
    repaint();
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

  public void worldToScreenPoint( int result[], double V_world[]) {
    double V_vantage[] = new double[3];
    // Tranform vector in world coordinates to vantage coordinates.
    MatrixOps.MtimesV(V_vantage, worldToVantageRotation, V_world);
    // Perspective projection of point in 3D vantage coordinates to 2D screen coordinates.
    double perspective_scale = screen_half_width/(Math.tan(beta)*(vantageDistance-V_vantage[0]));
    result[0] = (int)(perspective_scale * V_vantage[1] + screen_half_width);
    result[1] = (int)(screen_half_height - perspective_scale * V_vantage[2]);
  }

  public void setBodyPos( double x, double y, double z) {
      bodyPos[0] = x;
      bodyPos[1] = y;
      bodyPos[2] = z;
  }

  public void drawLineSegmentInWorld(Graphics2D g, Color color, double start[], double end[]) {
    g.setPaint(color);
    int start_screen[] = {0, 0};
    int end_screen[] = {0, 0};
    worldToScreenPoint( start_screen, start);
    worldToScreenPoint( end_screen, end);
    g.drawLine( start_screen[0], start_screen[1], end_screen[0], end_screen[1]);
  }

  public void drawLabelInWorld(Graphics2D g, Color color, double loc_world[], String s) {
    g.setPaint(color);
    int loc_screen[] = {0, 0};
    worldToScreenPoint( loc_screen, loc_world);
    g.drawString ( s, loc_screen[0], loc_screen[1]);
  }

  private void doDrawing( Graphics g) {
    Graphics2D g2d = (Graphics2D) g;

    int width  = getWidth();
    int height = getHeight();

    screen_half_width = (width/2);
    screen_half_height = (height/2);

    g2d.setPaint(Color.WHITE);
    g2d.fillRect(0, 0, width, height);

    
    // Draw the vehicle.

    // Transform the vehicle vertices from body -> world, apply the vehicle position offset, and then to 2D screen points.
    for (int i=0; i<veh_vrtx_body.length ; i++) {
      MatrixOps.MtimesV(veh_vrtx_world[i], bodyToWorldRotation, veh_vrtx_body[i]);
      MatrixOps.VplusV(veh_vrtx_world[i], veh_vrtx_world[i], bodyPos);

      worldToScreenPoint (veh_vrtx_screen[i], veh_vrtx_world[i]);
    }

    // Draw Solid Model

    for (int i=0; i<veh_triangles.length ; i++) {
      double LOS_vantage[] = {1.0, 0.0, 0.0};

      // Transform the vehicle triangle normals from Body -> World -> Vantage
      MatrixOps.MtimesV(veh_unit_normals_world[i], bodyToWorldRotation, veh_unit_normals_body[i]);
      MatrixOps.MtimesV(veh_unit_normals_vantage[i], worldToVantageRotation, veh_unit_normals_world[i]);

      // Render the triangle only if it's facing us.
      double facing_angle = MatrixOps.VdotV(veh_unit_normals_vantage[i], LOS_vantage);
      if ( (facing_angle > 0.0) && (facing_angle < Math.toRadians(90))) {

          // Calculate the diffuse reflection intensity.
          double neg_illumination_vector[] = {0.0, 0.0, 0.0};
          MatrixOps.Vscale(neg_illumination_vector, illumination_vector, -1.0);
          double diffuse_intensity = MatrixOps.VdotV(neg_illumination_vector, veh_unit_normals_world[i]);
          if (diffuse_intensity < 0.0) diffuse_intensity = 0.0;

          // Proportion of the total light due to ambient light.
          // (1.0 - ambient) is the proportion of reflected light.
          double ambient = 0.8; // Must be between 0.0 and 1.0.

          // The color intensity is a combination of ambient light intensity,
          // and diffuse reflection intensity.
          double color_intensity = (ambient + (1.0 - ambient) * diffuse_intensity);


          g2d.setPaint(new Color( 220, 220, 220)); // FOR TESTING. ================================================================================================================================
          
          /* g2d.setPaint( new Color( (int)(bodyFillColor.getRed()   * color_intensity),
                                    (int)(bodyFillColor.getGreen() * color_intensity),
                                    (int)(bodyFillColor.getBlue()  * color_intensity))); == CHANGE LATER */

          // Draw the triangle.
          int triangle_poly_x[] = {0, 0, 0};
          int triangle_poly_y[] = {0, 0, 0};
          // For each point of the triangle.
          for (int j=0; j < 3; j++) {

            triangle_poly_x[j] = veh_vrtx_screen[ veh_triangles[i][j] ][0];
            triangle_poly_y[j] = veh_vrtx_screen[ veh_triangles[i][j] ][1];

          }
          g2d.fillPolygon(triangle_poly_x, triangle_poly_y, 3);
        }
      }

      // Draw Wireframe Model

      g2d.setPaint( bodyLineColor );
      
      for (int i = 0; i<veh_edges.length; i++) {
        int point0[] = veh_vrtx_screen[ veh_edges[i][0] ];
        int point1[] = veh_vrtx_screen[ veh_edges[i][1] ];
        g2d.drawLine( point0[0], point0[1], point1[0], point1[1]);
      }

      // ============================
      // Draw Center of Gravity Point
      int CG_screen[] = {0, 0};
      int CG_symbol_size = 15;
      worldToScreenPoint( CG_screen, bodyPos);
      g2d.setPaint( Color.WHITE);
      g2d.fillOval(CG_screen[0]-CG_symbol_size/2, CG_screen[1]-CG_symbol_size/2, CG_symbol_size, CG_symbol_size);
      g2d.setPaint( Color.BLACK);
      g2d.fillArc( CG_screen[0]-CG_symbol_size/2, CG_screen[1]-CG_symbol_size/2, CG_symbol_size, CG_symbol_size,   0, 90 );
      g2d.fillArc( CG_screen[0]-CG_symbol_size/2, CG_screen[1]-CG_symbol_size/2, CG_symbol_size, CG_symbol_size, 180, 90);

      // ==========================
      // Draw World Coordinate Axes
      double origin_world[] = {0.0, 0.0, 0.0};
      double x_axis_world[] = {5.0, 0.0, 0.0};
      drawLineSegmentInWorld(g2d, Color.RED, origin_world, x_axis_world);
      drawLabelInWorld(g2d, Color.RED, x_axis_world, "X");
      double y_axis_world[] = {0.0, 5.0, 0.0};
      drawLineSegmentInWorld(g2d, Color.GREEN, origin_world, y_axis_world);
      drawLabelInWorld(g2d, Color.GREEN, y_axis_world, "Y");
      double z_axis_world[] = {0.0, 0.0, 5.0};
      drawLineSegmentInWorld(g2d, Color.BLUE, origin_world, z_axis_world);
      drawLabelInWorld(g2d, Color.BLUE, z_axis_world, "Z");
    }
  

  @Override
  public void paintComponent( Graphics g) {
      super.paintComponent(g);
      doDrawing(g);
  }

 }

public class SRBDisplay extends JFrame {

  private SingleRigidBodyView singleRigidBodyView;
  private BufferedReader in;
  private DataOutputStream out;

  public SRBDisplay( SingleRigidBodyView srbv) { // Creates the display window.
        singleRigidBodyView = srbv;
        add(singleRigidBodyView);
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

  private static void printHelpText() {
    System.out.println(
        "----------------------------------------------------------------------\n"
      + "usage: java jar SingleRigidBodyDisplay.jar <port-number>\n"
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

    SingleRigidBodyView singleRigidBodyView = new SingleRigidBodyView();

    SRBDisplay sd = new SRBDisplay(singleRigidBodyView);
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
          singleRigidBodyView.setBodyPos(posX, posY, posZ);

          singleRigidBodyView.setBodyToWorldRotation( Rxx, Rxy, Rxz,
                                          Ryx, Ryy, Ryz,
                                          Rzx, Rzy, Rzz );

      } catch (IOException | NullPointerException e ) {
          go = false;
      }
      sd.drawSRBView();
      }
    }

}