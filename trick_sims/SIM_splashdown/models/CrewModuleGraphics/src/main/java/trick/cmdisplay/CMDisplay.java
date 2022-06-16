
package trick;

import java.awt.Graphics2D;
import java.awt.Graphics;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.FileReader;
import java.net.Socket;
import java.util.*;
import javax.swing.JFrame;
import javax.swing.JPanel;
import java.awt.Color;
import java.awt.event.MouseEvent;
import javax.swing.event.MouseInputAdapter;
import trick.matrixOps.MatrixOps;

/**
 * @author penn
 */

class CrewModuleView extends JPanel {

    private int screen_half_width;
    private int screen_half_height;

    private double illumination_vector[]; // World coordinates

    private double vantageAzimuth;
    private double vantageElevation;

    private double vantageDistance;
    private double beta;

    private double worldToVantageRotation[][];

    private Color waterColor;
    private double water_vrtx_world[][];

    private int water_poly_x[];
    private int water_poly_y[];

    private Color vehicleLineColor;
    private double[] vehiclePos;
    private double[] centerOfBuoyancy;

    private double bodyToWorldRotation[][];

    private double veh_vrtx_body[][];
    private double veh_vrtx_world[][];
    private int    veh_vrtx_screen[][];

    private int veh_edges[][];
    private int veh_triangles[][];

    private double veh_unit_normals_body[][];
    private double veh_unit_normals_world[][];
    private double veh_unit_normals_vantage[][];

    public CrewModuleView() {

        ViewListener viewListener = new ViewListener();
        addMouseListener(viewListener);
        addMouseMotionListener(viewListener);

        // Direction of light.
        illumination_vector = new double[] {-0.707, 0.0, -0.707};

        // Observer location, looking toward the world origin.
        vantageAzimuth   = Math.toRadians(45.0);
        vantageElevation = Math.toRadians(20.0);
        vantageDistance = 12.0;

        // Half field of view angle
        beta = Math.toRadians(40.0);

        worldToVantageRotation = new double[3][3];
        setAzElRotation(worldToVantageRotation, vantageAzimuth, vantageElevation);

        waterColor  = new  Color(220,220,250,180);
        water_vrtx_world = new double[][]
          {
            { 5.000, 5.000, 0.000},
            { 5.000,-5.000, 0.000},
            {-5.000,-5.000, 0.000},
            {-5.000, 5.000, 0.000}
          };

        water_poly_x = new int[water_vrtx_world.length];
        water_poly_y = new int[water_vrtx_world.length];

        vehicleLineColor = Color.GRAY;

        vehiclePos  = new double[] {0.0, 0.0, 0.0};

        centerOfBuoyancy  = new double[] {0.0, 0.0, 0.0};

        bodyToWorldRotation = new double[][] {{1.0, 0.0, 0.0},
                                              {0.0, 1.0, 0.0},
                                              {0.0, 0.0, 1.0}};

        veh_vrtx_body = new double[][]
          { { 0.000, 0.000, 2.400}, /*   0 top point*/
            { 0.900, 0.000, 2.400}, /*   1 Upper ring */
            { 0.779, 0.450, 2.400}, /*   2 */
            { 0.450, 0.779, 2.400}, /*   3 */
            { 0.000, 0.900, 2.400}, /*   4 */
            {-0.450, 0.779, 2.400}, /*   5 */
            {-0.779, 0.450, 2.400}, /*   6 */
            {-0.900, 0.000, 2.400}, /*   7 */
            {-0.779,-0.450, 2.400}, /*   8 */
            {-0.450,-0.779, 2.400}, /*   9 */
            { 0.000,-0.900, 2.400}, /*  10 */
            { 0.450,-0.779, 2.400}, /*  11 */
            { 0.779,-0.450, 2.400}, /*  12 */
            { 2.500, 0.000,-0.100}, /*  13 Middle ring */
            { 2.166, 1.250,-0.100}, /*  14 */
            { 1.250, 2.166,-0.100}, /*  15 */
            { 0.000, 2.500,-0.100}, /*  16 */
            {-1.250, 2.166,-0.100}, /*  17 */
            {-2.166, 1.250,-0.100}, /*  18 */
            {-2.500, 0.000,-0.100}, /*  19 */
            {-2.166,-1.250,-0.100}, /*  20 */
            {-1.250,-2.166,-0.100}, /*  21 */
            { 0.000,-2.500,-0.100}, /*  22 */
            { 1.250,-2.166,-0.100}, /*  23 */
            { 2.166,-1.250,-0.100}, /*  24 */
            { 2.500, 0.000,-0.300}, /*  25 Lower ring */
            { 2.166, 1.250,-0.300}, /*  26 */
            { 1.250, 2.166,-0.300}, /*  27 */
            { 0.000, 2.500,-0.300}, /*  28 */
            {-1.250, 2.166,-0.300}, /*  29 */
            {-2.166, 1.250,-0.300}, /*  30 */
            {-2.500, 0.000,-0.300}, /*  31 */
            {-2.166,-1.250,-0.300}, /*  32 */
            {-1.250,-2.166,-0.300}, /*  33 */
            { 0.000,-2.500,-0.300}, /*  34 */
            { 1.250,-2.166,-0.300}, /*  35 */
            { 2.166,-1.250,-0.300}, /*  36 */
            { 0.000, 0.000,-0.900}  /*  37 bottom point */
          };

        veh_vrtx_world = new double[veh_vrtx_body.length][3];
        veh_vrtx_screen = new int[veh_vrtx_body.length][2];

        veh_edges = new int[][]
        { /* connect top-center and upper ring */
          { 0, 1},{ 0, 2},{ 0, 3},{ 0, 4},{ 0, 5},{ 0, 6},{ 0, 7},{ 0, 8},{ 0, 9},{ 0,10},{ 0,11},{ 0,12},
          /* connect upper ring points */
          { 1, 2},{ 2, 3},{ 3, 4},{ 4, 5},{ 5, 6},{ 6, 7},{ 7, 8},{ 8, 9},{ 9,10},{10,11},{11,12},{12, 1},
          /* connect upper and middle rings */
          { 1,13},{ 2,14},{ 3,15},{ 4,16},{ 5,17},{ 6,18},{ 7,19},{ 8,20},{ 9,21},{10,22},{11,23},{12,24},
          /* connect middle ring points */
          {13,14},{14,15},{15,16},{16,17},{17,18},{18,19},{19,20},{20,21},{21,22},{22,23},{23,24},{24,13},
          /* connect middle and lower rings */
          {13,25},{14,26},{15,27},{16,28},{17,29},{18,30},{19,31},{20,32},{21,33},{22,34},{23,35},{24,36},
          /* connect lower ring points */
          {25,26},{26,27},{27,28},{28,29},{29,30},{30,31},{31,32},{32,33},{33,34},{34,35},{35,36},{36,25},
          /* connect lower points to bottom-center */
          {25,37},{26,37},{27,37},{28,37},{29,37},{30,37},{31,37},{32,37},{33,37},{34,37},{35,37},{36,37}
        };

        // The order of the vertices in each of the triangles is significant. The points should follow
        // a path counter clock wise about the triangle's normal vector. The position normal side of
        // the triangle is the visable side.

        veh_triangles = new int[][]
        {
          { 0, 1, 2},{ 0, 2, 3},{ 0, 3, 4},{ 0, 4, 5},{ 0, 5, 6},{ 0, 6, 7},{ 0, 7, 8},{ 0, 8, 9},{ 0, 9,10},{ 0,10,11},{ 0,11,12},{ 0,12, 1},
          { 2, 1,13},{ 3, 2,14},{ 4, 3,15},{ 5, 4,16},{ 6, 5,17},{ 7, 6,18},{ 8, 7,19},{ 9, 8,20},{10, 9,21},{11,10,22},{12,11,23},{ 1,12,24},
          { 2,13,14},{ 3,14,15},{ 4,15,16},{ 5,16,17},{ 6,17,18},{ 7,18,19},{ 8,19,20},{ 9,20,21},{10,21,22},{11,22,23},{12,23,24},{1,24,13},
          {14,13,25},{15,14,26},{16,15,27},{17,16,28},{18,17,29},{19,18,30},{20,19,31},{21,20,32},{22,21,33},{23,22,34},{24,23,35},{13,24,36},
          {14,25,26},{15,26,27},{16,27,28},{17,28,29},{18,29,30},{19,30,31},{20,31,32},{21,32,33},{22,33,34},{23,34,35},{24,35,36},{13,36,25},
          {25,37,26},{26,37,27},{27,37,28},{28,37,29},{29,37,30},{30,37,31},{31,37,32},{32,37,33},{33,37,34},{34,37,35},{35,37,36},{36,37,25}
        };

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

    public void setVantageRange( double range) {
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
      vantageElevation += (dy * Math.PI) / getHeight();
      if (vantageElevation >  Math.toRadians( 89.0)) vantageElevation = Math.toRadians( 89.0);
      if (vantageElevation <  Math.toRadians(-89.0)) vantageElevation = Math.toRadians(-89.0);
      setAzElRotation(worldToVantageRotation, vantageAzimuth, vantageElevation);
      repaint();
    }

    public void setAzElRotation(double RotationMatrix[][], double azimuth, double elevation) {
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

    public void setVehPos( double x, double y, double z) {
        vehiclePos[0] = x;
        vehiclePos[1] = y;
        vehiclePos[2] = z;
    }

    public void setCB(double CBx, double CBy, double CBz) {
        centerOfBuoyancy[0] = CBx;
        centerOfBuoyancy[1] = CBy;
        centerOfBuoyancy[2] = CBz;
    }

    private void doDrawing( Graphics g) {
        Graphics2D g2d = (Graphics2D) g;

        int width  = getWidth();
        int height = getHeight();

        screen_half_width = (width/2);
        screen_half_height = (height/2);

        g2d.setPaint(Color.WHITE);
        g2d.fillRect(0, 0, width, height);

        // ======================
        //       Draw Water

        g2d.setPaint( waterColor);
        int pt[] = {0,0};
        for (int i=0; i<water_vrtx_world.length ; i++) {
          worldToScreenPoint( pt, water_vrtx_world[i]);
          water_poly_x[i] = pt[0];
          water_poly_y[i] = pt[1];
        }
        g2d.fillPolygon(water_poly_x, water_poly_y, 4);
        g2d.setPaint( Color.BLUE);
        g2d.drawPolygon(water_poly_x, water_poly_y, 4);

        // ========================
        //       Draw Vehicle

        // Transform the vehicle vertices from body -> world, apply the vehicle position offset, and then to 2D screen points.
        for (int i=0; i<veh_vrtx_body.length ; i++) {
          MatrixOps.MtimesV(veh_vrtx_world[i], bodyToWorldRotation, veh_vrtx_body[i]);
          MatrixOps.VplusV(veh_vrtx_world[i], veh_vrtx_world[i], vehiclePos);

          worldToScreenPoint (veh_vrtx_screen[i], veh_vrtx_world[i]);
        }

        // Draw Solid Model

        for (int i=0; i<veh_triangles.length ; i++) {
            double LOS_vantage[] = {1.0, 0.0, 0.0};
            double veh_RGB[] = {255.0, 255.0, 250.0};

            // Proportion of the total light due to ambiant light, the remainder is proportion of reflected light.
            double ambiant = 0.8; // Must be between 0.0 and 1.0.

            // Transform the vehicle triangle normals from Body -> World -> Vantage
            MatrixOps.MtimesV(veh_unit_normals_world[i], bodyToWorldRotation, veh_unit_normals_body[i]);
            MatrixOps.MtimesV(veh_unit_normals_vantage[i], worldToVantageRotation, veh_unit_normals_world[i]);

            // Render the triangle only if it's facing us.
            double facing_angle = MatrixOps.VdotV(veh_unit_normals_vantage[i], LOS_vantage);
            if ( (facing_angle > 0.0) && (facing_angle < Math.toRadians(90))) {

                // Calculate the (diffuse) reflection intensity.
                double neg_illumination_vector[] = {0.0, 0.0, 0.0};
                MatrixOps.Vscale(neg_illumination_vector, illumination_vector, -1.0);
                double diffuse_intensity = MatrixOps.VdotV(neg_illumination_vector, veh_unit_normals_world[i]);
                if (diffuse_intensity < 0.0) diffuse_intensity = 0.0;

                // The color intensity is a combination of ambiant light intensity,
                // and diffuse reflection intensity.
                double color_intensity = (ambiant + (1.0 - ambiant) * diffuse_intensity);
                int CCR = (int) (veh_RGB[0] * color_intensity);
                int CCG = (int) (veh_RGB[1] * color_intensity);
                int CCB = (int) (veh_RGB[2] * color_intensity);
                g2d.setPaint( new Color(CCR, CCG, CCB));

                // Draw the triangle.
                int triangle_poly_x[] = {0, 0, 0};
                int triangle_poly_y[] = {0, 0, 0};
                for (int j=0; j < 3; j++) {
                  triangle_poly_x[j] = veh_vrtx_screen[ veh_triangles[i][j] ][0];
                  triangle_poly_y[j] = veh_vrtx_screen[ veh_triangles[i][j] ][1];
                }
                g2d.fillPolygon(triangle_poly_x, triangle_poly_y, 3);
            }
        }

        // Draw Wireframe Model

        // g2d.setPaint( vehicleLineColor );
        for (int i=0; i<veh_edges.length; i++) {
          int point0[] = veh_vrtx_screen[ veh_edges[i][0] ];
          int point1[] = veh_vrtx_screen[ veh_edges[i][1] ];
          g2d.drawLine( point0[0], point0[1], point1[0], point1[1]);
        }

        // =============================
        // Draw Center of Buoyancy Point

        int CB_screen[] = {0, 0};
        int CB_symbol_size = 15;
        worldToScreenPoint( CB_screen, centerOfBuoyancy);
        g2d.setPaint( Color.WHITE);
        g2d.fillOval(CB_screen[0]-CB_symbol_size/2, CB_screen[1]-CB_symbol_size/2, CB_symbol_size, CB_symbol_size);
        g2d.setPaint( Color.BLUE);
        g2d.fillArc( CB_screen[0]-CB_symbol_size/2, CB_screen[1]-CB_symbol_size/2, CB_symbol_size, CB_symbol_size,   0, 90 );
        g2d.fillArc( CB_screen[0]-CB_symbol_size/2, CB_screen[1]-CB_symbol_size/2, CB_symbol_size, CB_symbol_size, 180, 90);

        // ============================
        // Draw Center of Gravity Point

        int CG_screen[] = {0, 0};
        int CG_symbol_size = 15;
        worldToScreenPoint( CG_screen, vehiclePos);
        g2d.setPaint( Color.WHITE);
        g2d.fillOval(CG_screen[0]-CG_symbol_size/2, CG_screen[1]-CG_symbol_size/2, CG_symbol_size, CG_symbol_size);
        g2d.setPaint( Color.BLACK);
        g2d.fillArc( CG_screen[0]-CG_symbol_size/2, CG_screen[1]-CG_symbol_size/2, CG_symbol_size, CG_symbol_size,   0, 90 );
        g2d.fillArc( CG_screen[0]-CG_symbol_size/2, CG_screen[1]-CG_symbol_size/2, CG_symbol_size, CG_symbol_size, 180, 90);

        // ==========================
        // Draw World Coordinate Axes

        double origin_world[] = {0.0, 0.0, 0.0};
        int origin_screen[] = {0, 0};
        worldToScreenPoint( origin_screen, origin_world);

        g2d.setPaint(Color.RED);
        double x_axis_world[] = {5.0, 0.0, 0.0};
        int x_axis_screen[] = {0, 0};
        worldToScreenPoint( x_axis_screen, x_axis_world);
        g2d.drawLine( origin_screen[0], origin_screen[1], x_axis_screen[0], x_axis_screen[1]);
        g2d.drawString ( "X", x_axis_screen[0],x_axis_screen[1]);

        g2d.setPaint(Color.GREEN);
        double y_axis_world[] = {0.0, 5.0, 0.0};
        int y_axis_screen[] = {0, 0};
        worldToScreenPoint( y_axis_screen, y_axis_world);
        g2d.drawLine( origin_screen[0], origin_screen[1], y_axis_screen[0], y_axis_screen[1]);
        g2d.drawString ( "Y", y_axis_screen[0],y_axis_screen[1]);

        g2d.setPaint(Color.BLUE);
        double z_axis_world[] = {0.0, 0.0, 5.0};
        int z_axis_screen[] = {0, 0};
        worldToScreenPoint( z_axis_screen, z_axis_world);
        g2d.drawLine( origin_screen[0], origin_screen[1], z_axis_screen[0], z_axis_screen[1]);
        g2d.drawString ( "Z", z_axis_screen[0],z_axis_screen[1]);

    }

    @Override
    public void paintComponent( Graphics g) {
        super.paintComponent(g);
        doDrawing(g);
    }
}

public class CMDisplay extends JFrame {

    private CrewModuleView crewModuleView;
    private BufferedReader in;
    private DataOutputStream out;

    public CMDisplay( CrewModuleView cmv) {
        crewModuleView = cmv;
        add( crewModuleView);
        setTitle("CM Display");
        setSize(1597, 987);
        setLocationRelativeTo(null);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    }

    public void connectToServer( String host, int port ) throws IOException {
        Socket socket = new Socket(host, port);
        in = new BufferedReader( new InputStreamReader( socket.getInputStream()));
        out = new DataOutputStream(new BufferedOutputStream(socket.getOutputStream()));
    }

    public void drawCrewModuleView() {
        crewModuleView.repaint();
    }

    private static void  printHelpText() {
        System.out.println(
            "----------------------------------------------------------------------\n"
          + "usage: java jar CMDisplay.jar <port-number>\n"
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

        CrewModuleView crewModuleView = new CrewModuleView();

        CMDisplay sd = new CMDisplay(crewModuleView);
        sd.setVisible(true);

        double vehX = 0.0;
        double vehY = 0.0;
        double vehZ = 0.0;

        double Rxx  = 0.0;
        double Rxy  = 0.0;
        double Rxz  = 0.0;
        double Ryx  = 0.0;
        double Ryy  = 0.0;
        double Ryz  = 0.0;
        double Rzx  = 0.0;
        double Rzy  = 0.0;
        double Rzz  = 0.0;

        double CBx = 0.0;
        double CBy = 0.0;
        double CBz = 0.0;

        System.out.println("Connecting to: " + host + ":" + port);
        sd.connectToServer(host, port);

        sd.out.writeBytes("trick.var_set_client_tag(\"CMDisplay\") \n" +
                          "trick.var_pause() \n" +
                          "trick.var_add(\"crewModule.dyn.position[0]\") \n" +
                          "trick.var_add(\"crewModule.dyn.position[1]\") \n" +
                          "trick.var_add(\"crewModule.dyn.position[2]\") \n" +
                          "trick.var_add(\"crewModule.dyn.R[0][0]\") \n" +
                          "trick.var_add(\"crewModule.dyn.R[0][1]\") \n" +
                          "trick.var_add(\"crewModule.dyn.R[0][2]\") \n" +
                          "trick.var_add(\"crewModule.dyn.R[1][0]\") \n" +
                          "trick.var_add(\"crewModule.dyn.R[1][1]\") \n" +
                          "trick.var_add(\"crewModule.dyn.R[1][2]\") \n" +
                          "trick.var_add(\"crewModule.dyn.R[2][0]\") \n" +
                          "trick.var_add(\"crewModule.dyn.R[2][1]\") \n" +
                          "trick.var_add(\"crewModule.dyn.R[2][2]\") \n" +
                          "trick.var_add(\"crewModule.dyn.center_of_buoyancy[0]\") \n" +
                          "trick.var_add(\"crewModule.dyn.center_of_buoyancy[1]\") \n" +
                          "trick.var_add(\"crewModule.dyn.center_of_buoyancy[2]\") \n" +
                          "trick.var_ascii() \n" +
                          "trick.var_cycle(0.1) \n" +
                          "trick.var_unpause()\n" );
        sd.out.flush();

        sd.drawCrewModuleView();

        Boolean go = true;

        while (go) {
            String field[];
            try {
                String line;
                line = sd.in.readLine();
                field = line.split("\t");

                vehX = Double.parseDouble( field[1] );
                vehY = Double.parseDouble( field[2] );
                vehZ = Double.parseDouble( field[3] );

                Rxx  = Double.parseDouble( field[4] );
                Rxy  = Double.parseDouble( field[5] );
                Rxz  = Double.parseDouble( field[6] );
                Ryx  = Double.parseDouble( field[7] );
                Ryy  = Double.parseDouble( field[8] );
                Ryz  = Double.parseDouble( field[9] );
                Rzx  = Double.parseDouble( field[10] );
                Rzy  = Double.parseDouble( field[11] );
                Rzz  = Double.parseDouble( field[12] );

                CBx  = Double.parseDouble( field[13] );
                CBy  = Double.parseDouble( field[14] );
                CBz  = Double.parseDouble( field[15] );

                // Set the Vehicle position
                crewModuleView.setVehPos(vehX, vehY, vehZ);

                crewModuleView.setBodyToWorldRotation( Rxx, Rxy, Rxz,
                                                       Ryx, Ryy, Ryz,
                                                       Rzx, Rzy, Rzz );
                crewModuleView.setCB(CBx, CBy, CBz);

            } catch (IOException | NullPointerException e ) {
                go = false;
            }
            sd.drawCrewModuleView();
        }
    }
}
