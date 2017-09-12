/*
 * Trick
 * 2016 (c) National Aeronautics and Space Administration (NASA)
 */

package sundisplay;

import java.awt.Color;
import java.awt.Font;
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

/**
 *
 * @author penn
 */

class NWUPoint {

    // NWUPoint is a point in North-West-Up coordinates.
    // We use unit vectors in this coordinate system to point to locations on the
    // celestial sphere.
    double n; // Positive to the north.
    double w; // Positive to the west.
    double u; // Positive up.
    
    // In the the following constructor, az and el are the azimuth and elevation
    // of an object on the celestial sphere. az is measured from north, and is
    // positive clock-wise. el is measured from the horizon, and is positive up.

    public NWUPoint (double heading, double elevation) {

        // Create a NWUPoint from compass heading, and elevation.
        // Note that compass heading is positive clockwise.
        n =  Math.cos(heading) * Math.cos(elevation);
        w = -Math.sin(heading) * Math.cos(elevation);
        u =                      Math.sin(elevation);
    }
}

class CameraPoint {

    // CameraPoint is a point in Camera coordinates.
    double x; // Out along the camera line of sight.
    double y; // Positive to the left.
    double z; // Positive up.
    
    public CameraPoint (double X, double Y, double Z) {
        x = X;
        y = Y;
        z = Z;
    }
    
    public CameraPoint (NWUPoint nwu, double CameraElevation, double CameraHeading) {  

        // Create a CameraPoint from a NWUPoint.
        //     CameraHeading - Compass heading of the camera.
        //     CameraElevation - Elevation of the camera above the horizon.

        // NOTE that NWU is a right-hand coordinate system,
        // and that compass heading (like on a ship or airplane) is
        // in the opposite direction of a positive z-axis rotation.
        // Therefore, our z-axis rotation is the negative of CameraHeading.
        // CameraElevation is a positive rotation. 
        double yrot =  CameraElevation; 
        double zrot = -CameraHeading;

        // |camera_x|   | cos(yrot) 0 -sin(yrot) |   | cos(zrot)  sin(zrot) 0 |   |n|
        // |camera_y| = |    0      1     0      | * |-sin(zrot)  cos(zrot) 0 | * |w|
        // |camera_z|   | sin(yrot) 0  cos(yrot) |   |    0          0      1 |   |u|
        //
        //              |  cos(zrot)* cos(yrot)  cos(yrot)* sin(zrot) -sin(yrot) |   |n|
        //            = | -sin(zrot)             cos(zrot)             0         | * |w|
        //              |  cos(zrot)* sin(yrot)  sin(zrot)* sin(yrot)  cos(yrot) |   |u|

        x =  nwu.n *  Math.cos(zrot) * Math.cos(yrot) + nwu.w * Math.cos(yrot) * Math.sin(zrot) + nwu.u * -Math.sin(yrot);
        y =  nwu.n * -Math.sin(zrot)                  + nwu.w * Math.cos(zrot);
        z =  nwu.n *  Math.cos(zrot) * Math.sin(yrot) + nwu.w * Math.sin(zrot) * Math.sin(yrot) + nwu.u *  Math.cos(yrot);
    }
}

class ViewPoint {

    // Viewpoint is a point in JPanel-Graphics2D coordinates. The origin of the
    // JPanel-Graphics2D coordinates is the upper left hand corner of the JPanel.
    int x; // Positive to the right.
    int y; // Positive down.
    
    public ViewPoint (int X, int Y) {
        x = X;
        y = Y;
    }
    
    public ViewPoint (CameraPoint cp, double origin_x, double origin_y, double scale) {

    // Transform point in Camera Coordinates to ViewPoint (Jpanel-Graphics2D) Coordinates
    //
    //                                              |camera_x|
    // |view_x| = |origin_x| + scale * | 0 -1  0| * |camera_y|
    // |view_y| = |origin_y|           | 0  0 -1|   |camera_z|

        x = (int)(origin_x + scale * -cp.y);
        y = (int)(origin_y + scale * -cp.z);
    }
}

class CalendarDate {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    double second;
}
    
class SkyMap extends JPanel {

    double sun_azimuth;
    double sun_elevation;
    double observer_lat;
    double observer_lon;
    double observer_el;
    double observer_az;
    double sun_right_ascension;
    double sun_declination;
    double JD;
    double hour_angle;
    
    CalendarDate local_time;
    CalendarDate utc_time;
    
    Color sunColor;
    Color skyColor;
    Color groundColor;
    Color textColor;
    
    public SkyMap() {
        sun_azimuth = 0.0;
        sun_elevation = 0.0;
        observer_el = 20.0;
        observer_az = 180.0;
        JD = 0.0;
        
        local_time = new CalendarDate();
        utc_time = new CalendarDate();
        
        sunColor = new Color(255, 255, 50);
        skyColor = new Color(150, 150, 255);
        groundColor = new Color(70,150,70);
        textColor = new Color(0,0,50);
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

        g2d.setRenderingHints(rh);
        
        int width = getWidth();
        int height = getHeight();
        
        g2d.setPaint(Color.GRAY);
        g2d.fillRect(0, 0, width, height);
        
        int d =  Math.min(width, height);

        g2d.setPaint(skyColor);
        g2d.fillOval(0, 0, d, d);
        
        double obs_el_rad = Math.toRadians(observer_el);
        double obs_az_rad = Math.toRadians(observer_az);
        
        g2d.setPaint(Color.BLACK);
        g2d.fillArc(0, 0, d, d, 0,-180);
        
        g2d.setPaint(groundColor);
        g2d.fillOval(0, (int)(d/2 * (1.0 - Math.sin(obs_el_rad))),d,(int)(d * Math.sin(obs_el_rad)));
        
        double sun_az_rad = Math.toRadians(sun_azimuth);
        double sun_el_rad = Math.toRadians(sun_elevation);

        ViewPoint centerPoint = new ViewPoint( new CameraPoint( 0.0, 0.0, 0.0 ), d/2, d/2, d/2 );
        
        ViewPoint p1 = new ViewPoint( new CameraPoint( new NWUPoint( sun_az_rad, 0.0), obs_el_rad, obs_az_rad), d/2, d/2, d/2 );
        
        ViewPoint p2 = new ViewPoint( new CameraPoint( new NWUPoint( Math.toRadians(  0.0), 0.0), obs_el_rad, obs_az_rad), d/2, d/2, d/2 );
        ViewPoint p3 = new ViewPoint( new CameraPoint( new NWUPoint( Math.toRadians(180.0), 0.0), obs_el_rad, obs_az_rad), d/2, d/2, d/2 );
        ViewPoint p4 = new ViewPoint( new CameraPoint( new NWUPoint( Math.toRadians( 90.0), 0.0), obs_el_rad, obs_az_rad), d/2, d/2, d/2 );
        ViewPoint p5 = new ViewPoint( new CameraPoint( new NWUPoint( Math.toRadians(270.0), 0.0), obs_el_rad, obs_az_rad), d/2, d/2, d/2 );

        g2d.setPaint(Color.WHITE);
        g2d.drawString("North", p2.x, p2.y);
        g2d.drawString("South", p3.x, p3.y);
        g2d.drawString("East", p4.x, p4.y);
        g2d.drawString("West", p5.x, p5.y);
        
        g2d.setPaint(Color.BLACK);
        g2d.drawLine(p2.x, p2.y, p3.x, p3.y);      
        g2d.drawLine(p4.x, p4.y, p5.x, p5.y);
        
        g2d.setPaint(Color.WHITE);
        g2d.drawLine(centerPoint.x, centerPoint.y, p1.x, p1.y);
        
        // Draw the Sun.
        ViewPoint sun_point = new ViewPoint( new CameraPoint( new NWUPoint( sun_az_rad, sun_el_rad), obs_el_rad, obs_az_rad), d/2, d/2, d/2 );
        g2d.setPaint(sunColor);
        drawCenteredCircle(g2d, sun_point.x, sun_point.y, 20);
        
        g2d.setPaint(Color.WHITE);
        g2d.drawString("Local Time: " + String.format("%d/%d/%d - %d:%d:%.0f",
                local_time.month, local_time.day, local_time.year, local_time.hour, local_time.minute, local_time.second), centerPoint.x + 20, centerPoint.y + 20);
        
        g2d.drawString("UTC Time: " + String.format("%d/%d/%d - %d:%d:%.0f",
                utc_time.month, utc_time.day, utc_time.year, utc_time.hour, utc_time.minute, utc_time.second), centerPoint.x + 20, centerPoint.y + 40);
        
        g2d.drawString("JD " + String.format("%.5f",JD), centerPoint.x + 20, centerPoint.y + 60);
        
        g2d.drawString("Solar AZ " + String.format("%.3f",sun_azimuth) + "\u00b0", sun_point.x, sun_point.y + 20);
        g2d.drawString("Solar EL " + String.format("%.3f",sun_elevation) + "\u00b0", sun_point.x, sun_point.y + 40);
        g2d.drawString("Solar RA " + String.format("%.3f",sun_right_ascension) + "\u00b0", sun_point.x, sun_point.y + 60);
        g2d.drawString("Solar DEC " + String.format("%.3f",sun_declination) + "\u00b0", sun_point.x, sun_point.y + 80);
        
        g2d.setPaint(Color.WHITE);
        g2d.drawString("Observer Lat " + String.format("%.5f",observer_lat) + "\u00b0", 10, 20);
        g2d.drawString("Observer Lon " + String.format("%.5f",observer_lon) + "\u00b0", 10, 40);
        
        g2d.drawString("Camera AZ " + String.format("%.0f",observer_az) + "\u00b0", 10, 100);
        g2d.drawString("Camera EL " + String.format("%.0f",observer_el) + "\u00b0", 10, 120);

    }
    
    @Override
    public void paintComponent(Graphics g) {
        super.paintComponent(g);
        doDrawing(g);
    }
}

public class SunDisplay extends JFrame implements ActionListener {

    private SkyMap skyMap;
    private BufferedReader in;
    private DataOutputStream out;

    private SunDisplay(SkyMap sky) {
        
        skyMap = sky;
        setTitle("Sky Map");

        JPanel mainPane = new JPanel();
        mainPane.setLayout(new BoxLayout(mainPane, BoxLayout.Y_AXIS));
        mainPane.add(skyMap);
        
        JPanel buttonPane = new JPanel();
        buttonPane.setLayout(new BoxLayout(buttonPane, BoxLayout.X_AXIS));
        mainPane.add(buttonPane);
        
        JButton b1 = new JButton("\u25c0");
        b1.addActionListener(this);
        b1.setActionCommand("left");
        b1.setToolTipText("Rotate Azimuth Left");
        buttonPane.add(b1);
        
        JButton b2 = new JButton("\u25b6");
        b2.addActionListener(this);
        b2.setActionCommand("right");
        b2.setToolTipText("Rotate Azimuth Right");
        buttonPane.add(b2);
        
        JButton b3 = new JButton("\u25ce");
        b3.addActionListener(this);
        b3.setActionCommand("reset");
        b3.setToolTipText("Camera Reset");
        buttonPane.add(b3);
        
        JButton b4 = new JButton("\u25b2");
        b4.addActionListener(this);
        b4.setActionCommand("up");
        b4.setToolTipText("Increase Observer Elevation");
        buttonPane.add(b4);
        
        JButton b5 = new JButton("\u25bc");
        b5.addActionListener(this);
        b5.setActionCommand("down");
        b5.setToolTipText("Decrease Observer Elevation");
        buttonPane.add(b5);
        add(mainPane);
        
        setSize(800, 800);
        
        setLocationRelativeTo(null);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    }
    
    // This function acts on messages recieved from the buttons. 
    public void actionPerformed(ActionEvent e) {
        
        String s = e.getActionCommand();
        switch (s) {
            case  "left":
                skyMap.observer_az -= 10.0;
                skyMap.observer_az = (skyMap.observer_az + 360.0) % 360.0;
                break;
            case  "right":
                skyMap.observer_az += 10.0;
                skyMap.observer_az %= 360.0;
                break;
            case "reset" :
                skyMap.observer_az = 180.0;
                skyMap.observer_el =  20.0;
                break;
            case  "up":
                skyMap.observer_el += 10.0;
                if (skyMap.observer_el > 90.0) {
                    skyMap.observer_el = 90.0;
                }
                break;
            case  "down":
                skyMap.observer_el -= 10.0;
                if (skyMap.observer_el < 0.0) {
                    skyMap.observer_el = 0.0;
                }
                break;
            default:
                System.out.println("Unknown Action Command:" + s);
                break;
        }
    }
    
    public void connectToServer(String host, int port ) throws IOException {
        Socket socket = new Socket(host, port);
        in = new BufferedReader( new InputStreamReader( socket.getInputStream()));
        out = new DataOutputStream(new BufferedOutputStream(socket.getOutputStream()));
    }

    public void drawArenaMap() {
        skyMap.repaint();
    }

    private static void  printHelpText() {
        System.out.println(
            "----------------------------------------------------------------------\n"
          + "usage: java jar SunDisplay.jar <port-number>\n"
          + "----------------------------------------------------------------------\n"
          );
    }

    public static void main(String[] args) throws IOException {

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

        if (port == 0) {
            System.out.println("No variable server port specified.");
            printHelpText();
            System.exit(0);
        } 
        
        SkyMap skyMap = new SkyMap();
        
        SunDisplay evd = new SunDisplay(skyMap);
        evd.setVisible(true);

        System.out.println("Connecting to: " + host + ":" + port);
        evd.connectToServer(host, port);

        evd.out.writeBytes("trick.var_set_client_tag(\"SunDisplay\") \n" +
                           "trick.var_pause() \n" +
                           "trick.var_add(\"sun_predictor.sun.JD\") \n" +
                           "trick.var_add(\"sun_predictor.sun.observer_latitude\") \n" +
                           "trick.var_add(\"sun_predictor.sun.observer_longitude\") \n" +
                           "trick.var_add(\"sun_predictor.sun.observer_offset_from_UTC\") \n" +
                           "trick.var_add(\"sun_predictor.sun.right_ascension\") \n" +
                           "trick.var_add(\"sun_predictor.sun.declination\") \n" +
                           "trick.var_add(\"sun_predictor.sun.hour_angle\") \n" +
                           "trick.var_add(\"sun_predictor.sun.local_sidereal_time\") \n" +
                           "trick.var_add(\"sun_predictor.sun.sidereal_time_at_Greenwich\") \n" +
                           "trick.var_add(\"sun_predictor.sun.solar_azimuth\") \n" +
                           "trick.var_add(\"sun_predictor.sun.solar_elevation\") \n" +
                           "trick.var_add(\"sun_predictor.sun.local_time.year\") \n" +
                           "trick.var_add(\"sun_predictor.sun.local_time.month\") \n" +
                           "trick.var_add(\"sun_predictor.sun.local_time.day\") \n" +
                           "trick.var_add(\"sun_predictor.sun.local_time.hour\") \n" +
                           "trick.var_add(\"sun_predictor.sun.local_time.min\") \n" +
                           "trick.var_add(\"sun_predictor.sun.local_time.sec\") \n" +
                           "trick.var_add(\"sun_predictor.sun.utc.year\") \n" +
                           "trick.var_add(\"sun_predictor.sun.utc.month\") \n" +
                           "trick.var_add(\"sun_predictor.sun.utc.day\") \n" +
                           "trick.var_add(\"sun_predictor.sun.utc.hour\") \n" +
                           "trick.var_add(\"sun_predictor.sun.utc.min\") \n" +
                           "trick.var_add(\"sun_predictor.sun.utc.sec\") \n" +
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
                //System.out.println("line:" + line);
                field = line.split("\t");

                double observer_offset_from_UTC = Integer.parseInt( field[4] );
                double hour_angle = Double.parseDouble( field[7] );
                double local_sidereal_time = Double.parseDouble( field[8] );
                double sidereal_time_at_Greenwich = Double.parseDouble( field[9] );
                
                skyMap.JD                  = Double.parseDouble( field[1] );
                skyMap.observer_lat        = Double.parseDouble( field[2] );
                skyMap.observer_lon        = Double.parseDouble( field[3] );
                skyMap.sun_right_ascension = Double.parseDouble( field[5] );
                skyMap.sun_declination     = Double.parseDouble( field[6] );
                skyMap.sun_azimuth         = Double.parseDouble( field[10] );
                skyMap.sun_elevation       = Double.parseDouble( field[11] );
       
                skyMap.local_time.year     = Integer.parseInt( field[12] );
                skyMap.local_time.month    = Integer.parseInt( field[13] );
                skyMap.local_time.day      = Integer.parseInt( field[14] );
                skyMap.local_time.hour     = Integer.parseInt( field[15] );
                skyMap.local_time.minute   = Integer.parseInt( field[16] );
                skyMap.local_time.second   = Double.parseDouble( field[17] );

                skyMap.utc_time.year       = Integer.parseInt( field[18] );
                skyMap.utc_time.month      = Integer.parseInt( field[19] );
                skyMap.utc_time.day        = Integer.parseInt( field[20] );
                skyMap.utc_time.hour       = Integer.parseInt( field[21] );
                skyMap.utc_time.minute     = Integer.parseInt( field[22] );
                skyMap.utc_time.second     = Double.parseDouble( field[23] );
                               
            } catch (IOException | NullPointerException e ) {
                go = false;
            }
            evd.drawArenaMap();
        }
    }
}
