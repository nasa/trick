/*
 * Trick
 * 2016 (c) National Aeronautics and Space Administration (NASA)
 */

/* package trick;*/

import java.awt.Graphics2D;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.geom.AffineTransform;
import java.awt.image.AffineTransformOp;
import java.awt.image.BufferedImage;
import java.awt.event.ActionListener; 
import java.awt.event.ActionEvent;
import java.awt.Toolkit;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.Component;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.FileReader;
import java.io.FileNotFoundException;
import java.lang.Math;
import java.net.Socket;
import java.util.*;
import java.util.Hashtable;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JButton;
import javax.swing.JCheckBoxMenuItem;
import javax.swing.JToggleButton;
import javax.swing.BorderFactory; 
import javax.swing.border.EtchedBorder;
import javax.swing.BoxLayout;
import javax.swing.Icon;

import java.awt.Color;

import javax.imageio.ImageIO;
import java.io.File;

/**
 *
 * @author penn
 */

class Waypoint {
    public double north, west;
    public BufferedImage icon;
    
    public Waypoint(double n, double w, BufferedImage i) {
        north = n;
        west = w;
        icon = i;
    }
}

class ScenePoly {
    public Color color;
    public int n;
    public double[] x;
    public double[] y;
}

class SimulationMenuBar extends JMenuBar {
    private JMenu _file;
    private JMenu _edit;
    private JMenu _tools;
    private JMenu _help;
    private ViewMenu _view;

    private SkyView skyView;

    public SimulationMenuBar(SkyView sv) {
        skyView = sv;
        _view = new ViewMenu("View");

        add(_view);
    }

    public void setEnabled_DisabledViewCB(boolean s) {
        _view.disabledView.setEnabled(s);
    }
    private void initHelpMenu() {
        _help = new JMenu("Help");
        add(_help);
    }

    private void initToolsMenu() {
        _tools = new JMenu("Tools");
        add(_tools);
    }

    private void initEditMenu() {
        _edit = new JMenu("Edit");      // Undo, Redo, Cut, Copy, Paste
        JMenuItem mi;
        mi = new JMenuItem("Undo");
        _edit.add(mi);
        mi = new JMenuItem("Redo");
        _edit.add(mi);
        mi = new JMenuItem("Cut");
        _edit.add(mi);
        mi = new JMenuItem("Copy");
        _edit.add(mi);
        mi = new JMenuItem("Paste");
        _edit.add(mi);
        add(_edit);
    }

    private void initFileMenu() {
        _file = new JMenu("File");      // New, Open, Save, Save As, Exit
        JMenuItem mi;
        mi = new JMenuItem("New");
        _file.add(mi);
        mi = new JMenuItem("Open");
        _file.add(mi);
        mi = new JMenuItem("Save");
        _file.add(mi);
        mi = new JMenuItem("Save As");
        _file.add(mi);
        mi = new JMenuItem("Exit");
        _file.add(mi);
        
        add(_file);
    }

    private class ViewMenu extends JMenu {
        public JCheckBoxMenuItem posView, velView, scaleView, controlView, disabledView;
        ViewMenu(String name) {
            super(name);
            
            initPosViewCB();
            initVelViewCB();
            initScaleViewCB();
            initControlViewCB();
            initDisabledViewCB();

            add(posView);
            add(velView);
            add(scaleView);
            add(controlView);
            add(disabledView);
        }

        private void initPosViewCB() {
            posView = new JCheckBoxMenuItem("Aircraft Position", skyView.getPosView());
            posView.addItemListener(e -> skyView.setPosView(((JCheckBoxMenuItem) e.getItem()).isSelected()));
        }

        private void initVelViewCB() {
            velView = new JCheckBoxMenuItem("Aircraft Velocity", skyView.getVelView());
            velView.addItemListener(e -> skyView.setVelView(((JCheckBoxMenuItem) e.getItem()).isSelected()));
        }

        private void initScaleViewCB() {
            scaleView = new JCheckBoxMenuItem("Map Scale", skyView.getScaleView());
            scaleView.addItemListener(e -> skyView.setScaleView(((JCheckBoxMenuItem) e.getItem()).isSelected()));
        }

        private void initControlViewCB() {
            controlView = new JCheckBoxMenuItem("Control Mode", skyView.getCtrlView());
            controlView.addItemListener(e -> skyView.setCtrlView(((JCheckBoxMenuItem) e.getItem()).isSelected()));
        }

        private void initDisabledViewCB() {
            disabledView = new JCheckBoxMenuItem("Disabled Controls' Data", skyView.getDisabledView());
            disabledView.addItemListener(e -> skyView.setDisabledView(((JCheckBoxMenuItem) e.getItem()).isSelected()));
        }
    }
}

class SkyView extends JPanel {

    private double scale; // Pixels per meter

    private Color groundColor;

    private double[] aircraftPos;
    private double[] aircraftVel;
    private ScenePoly aircraft;
    private ScenePoly wpmarker;
    private int[] workPolyX, workPolyY;

    private double heading;
    private double actual_speed;
    private double desired_speed;
    private double desired_heading;
    private Boolean autopilot;

    private ArrayList<Waypoint> waypoints;

    // Origin of world coordinates in jpanel coordinates.
    private int worldOriginX;
    private int worldOriginY;

    // The data that is or isn't displayed on the map
    private boolean posView, velView, scaleView, ctrlView, disabledView;

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

        waypoints = new ArrayList<Waypoint>();

        setAllView(true);
    }

    public void addWaypoint( double n, double w, String fp) {
        BufferedImage img;
        try {
            img = ImageIO.read(new File(fp));
            waypoints.add(new Waypoint(n,w,img));
        } catch(Exception e) {
            System.out.printf("Waypoint (%.1f,%.1f) not added to map.");
        }

    }

    public void setAircraftPos( double n, double w) {
        aircraftPos[0] = n;
        aircraftPos[1] = w;
    }

    public void setAircraftVel( double n, double w) {
        aircraftVel[0] = n;
        aircraftVel[1] = w;
        heading = Math.atan2(w,n);
        actual_speed = Math.sqrt(n*n + w*w);
    }

    public void setScale (double mapScale) {
        if (mapScale < 0.00005) {
            scale = 0.00005;
        } else if (mapScale < 0.01){
            scale = 0.01;
        }
         else {
            scale = mapScale;
        }
    }

    public double getScale(){
        return scale;
    }

    public void setAutoPilot(Boolean ap){
        autopilot = ap;
    }

    public Boolean getAutoPilot(){
        return autopilot;
    }

    public void setDesiredSpeed(double n){
        desired_speed = n;
    }

    public int getDesiredSpeed(){
        return (int) Math.round(desired_speed);
    }

    public void setDesiredHeading(double n){
        desired_heading = Math.toRadians(n);
    }

    public double getDesiredHeading(){
       return desired_heading;
    }
    
    public void setInputDesiredHeading(double n){
        desired_heading = n;
    }

    // Getters and setters for all the 'View' variables
    public boolean getPosView()             {  return posView;  }
    public void setPosView(boolean v)       {  posView = v;  }

    public boolean getVelView()             {  return velView;  }
    public void setVelView(boolean v)       {  velView = v;  }

    public boolean getScaleView()           {  return scaleView;  }
    public void setScaleView(boolean v)     {  scaleView = v;  }

    public boolean getCtrlView()            {  return ctrlView;  }
    public void setCtrlView(boolean v)      {  ctrlView = v;  }

    public boolean getDisabledView()        {  return disabledView;  }
    public void setDisabledView(boolean v)  {  disabledView = v;  }

    public void setAllView(boolean v) {
        setPosView(v);
        setVelView(v);
        setScaleView(v);
        setCtrlView(v);
        setDisabledView(v);
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

    public void drawWaypoint(Graphics2D g, Waypoint wp) {
        int x = 0, y = 0;
        x = (int)((worldOriginX - scale * wp.west) - (wp.icon.getWidth()/2));
        y = (int)((worldOriginY - scale * wp.north) - (wp.icon.getHeight()/2));
        g.drawImage(wp.icon, x, y, null);
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
        for(int i = 0; i < waypoints.size(); i++) {
            Waypoint wp = waypoints.get(i);
            drawWaypoint(g2d, wp);
        }

        //  Draw Aircraft
        drawScenePoly(g2d, aircraft, heading, aircraftPos[0], aircraftPos[1] );

        // Display State Data
        g2d.setPaint(Color.BLACK);
        int textCursor = 40;
        
        if(posView) {
            g2d.drawString ( String.format("Aircraft Pos: [%.2f, %.2f]", aircraftPos[0], aircraftPos[1]), 20,textCursor);
            textCursor += 20;
        }

        if(velView) {
            g2d.drawString ( String.format("Aircraft Vel: [%.2f, %.2f]", aircraftVel[0], aircraftVel[1]), 20,textCursor);
            textCursor += 20;
        }
            
        if(scaleView){
            g2d.drawString ( String.format("SCALE: %f pixels/meter",scale), 20,textCursor);
            textCursor += 20;
        }
        
        if(ctrlView){
            g2d.drawString ( String.format("Control Mode: [%s]", autopilot ? "Auto-Pilot" : "Manual"), 20,textCursor);
            textCursor += 20;
        }
        
        if (autopilot == true) {
            g2d.drawString ( String.format("Aircraft Actual Heading:  [%.2f]", heading),(width - 240) ,40);
            g2d.drawString ( String.format("Aircraft Actual Speed: [%.2f m/s]", actual_speed), (width - 240),60);
            if (disabledView) {
                g2d.setPaint(Color.GRAY);
                g2d.drawString ( "-------Controls disabled-------", (width - 240),80);
                g2d.drawString ( String.format("Aircraft Desired Heading:  [%.2f]", desired_heading), (width - 240),100);
                g2d.drawString ( String.format("Aircraft Desired Speed: [%.2f m/s]", desired_speed), (width - 240),120);
                g2d.setPaint(Color.BLACK);
            }
        } else {
            g2d.drawString ( String.format("Aircraft Actual Heading:  [%.2f]", heading),(width - 240) ,100);
            g2d.drawString ( String.format("Aircraft Desired Heading:  [%.2f]", desired_heading), (width - 240),80);
            g2d.drawString ( String.format("Aircraft Actual Speed: [%.2f m/s]", actual_speed), (width - 240),60);
            g2d.drawString ( String.format("Aircraft Desired Speed: [%.2f m/s]", desired_speed), (width - 240),40);
        }
    }

    @Override
    public void paintComponent(Graphics g) {
        super.paintComponent(g);
        doDrawing(g);
    }
}

class ControlPanel extends JPanel implements ActionListener {
    private SkyView skyView;
    private JButton zoomOutButton, zoomInButton;
    private SpeedCtrlPanel speedCtrlPanel;
    private HeadingCtrlPanel headingCtrlPanel;
    private AutoPilotCtrlPanel autoPilotCtrlPanel;   
  


    public ControlPanel(SkyView skyView){
        skyView = skyView;
        setLayout(new BoxLayout(this, BoxLayout.X_AXIS));


        JPanel labeledSpeedCtrlPanel = new JPanel();
        labeledSpeedCtrlPanel.setLayout(new BoxLayout(labeledSpeedCtrlPanel, BoxLayout.Y_AXIS));
        JLabel speedCtrlLabel = new JLabel("Desired Speed");
        speedCtrlLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
        labeledSpeedCtrlPanel.add(speedCtrlLabel);
        speedCtrlPanel = new SpeedCtrlPanel(skyView);
        labeledSpeedCtrlPanel.add(speedCtrlPanel);
        add(labeledSpeedCtrlPanel);

        zoomInButton = new JButton("Zoom In");
        zoomInButton.addActionListener(this);
        zoomInButton.setActionCommand("zoomin");
        zoomInButton.setToolTipText("Zoom In");

        zoomOutButton = new JButton("Zoom Out");
        zoomOutButton.addActionListener(this);
        zoomOutButton.setActionCommand("zoomout");
        zoomOutButton.setToolTipText("Zoom Out");
       

        JPanel zoomCtrlPanel = new JPanel();
        zoomCtrlPanel.setLayout(new BoxLayout(zoomCtrlPanel, BoxLayout.Y_AXIS));
        zoomCtrlPanel.add(zoomInButton);
        zoomCtrlPanel.add(zoomOutButton);
        add(zoomCtrlPanel);


        JPanel labeledAutoPilotCtrlPanel = new JPanel();
        labeledAutoPilotCtrlPanel.setLayout(new BoxLayout(labeledAutoPilotCtrlPanel, BoxLayout.Y_AXIS));
        autoPilotCtrlPanel = new AutoPilotCtrlPanel(skyView);
        labeledAutoPilotCtrlPanel.add(autoPilotCtrlPanel);
        add(labeledAutoPilotCtrlPanel); 

        JPanel labeledHeadingCtrlPanel = new JPanel();
        labeledHeadingCtrlPanel.setLayout(new BoxLayout(labeledHeadingCtrlPanel, BoxLayout.Y_AXIS));
        JLabel headingCtrlLabel = new JLabel("Desired Heading");
        headingCtrlLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
        labeledHeadingCtrlPanel.add(headingCtrlLabel);
        headingCtrlPanel = new HeadingCtrlPanel(skyView);
        labeledHeadingCtrlPanel.add(headingCtrlPanel);
        add(labeledHeadingCtrlPanel);
    }

    public void actionPerformed (ActionEvent e){
        String s = e.getActionCommand();
        switch (s) {
            case "zoomout":
                skyView.setScale( skyView.getScale() / 2 );
                break;
            case "zoomin":
                skyView.setScale( skyView.getScale() * 2 );
                break;
            default:
                System.out.println("Unknown Action Command:" + s);
                break;
        }
    }
}

class SpeedCtrlPanel extends JPanel implements ChangeListener {
    private SkyView skyView;
    private JSlider speedSlider;
   
    public SpeedCtrlPanel(SkyView view){
        skyView = view;
        setLayout(new BoxLayout(this, BoxLayout.X_AXIS));
        setBorder( BorderFactory.createEtchedBorder(EtchedBorder.RAISED));

        speedSlider = new JSlider(JSlider.HORIZONTAL, 0, 250, 0);
        speedSlider.setMajorTickSpacing(50);
        speedSlider.setPaintTrack(true);
        speedSlider.setPaintLabels(true);
        speedSlider.addChangeListener(this);
        add(speedSlider);
    }

    public void stateChanged(ChangeEvent e){
        if (e.getSource() == speedSlider){
            skyView.setDesiredSpeed(speedSlider.getValue());
        }
    }
}

class HeadingCtrlPanel extends JPanel implements ChangeListener {
    private SkyView skyView;
    private JSlider headingSlider;

    public HeadingCtrlPanel (SkyView view) {
        skyView = view;
        setLayout(new BoxLayout(this, BoxLayout.X_AXIS));
        setBorder( BorderFactory.createEtchedBorder(EtchedBorder.RAISED));

        headingSlider = new JSlider(JSlider.HORIZONTAL, -180, 180, 0);

        
        headingSlider.setMajorTickSpacing(90);
        headingSlider.setPaintTrack(true);

        Hashtable<Integer, JLabel> labels = new Hashtable<>();
        labels.put(-180, new JLabel(" -\u03C0"));
        labels.put(180, new JLabel("\u03C0"));
        headingSlider.setLabelTable(labels);
        headingSlider.setPaintLabels(true);
        headingSlider.addChangeListener(this);
        add(headingSlider);
    }  

    public void stateChanged(ChangeEvent e){
        if (e.getSource() == headingSlider){
            skyView.setDesiredHeading(headingSlider.getValue());
        }
    }
}

class AutoPilotCtrlPanel extends JPanel implements ItemListener {
    private SkyView skyView;
    private JToggleButton autoPilotButton;

    public AutoPilotCtrlPanel(SkyView view){
        skyView = view;
        setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

        autoPilotButton = new JToggleButton("Autopilot OFF", false);
        skyView.setAutoPilot(false);
        autoPilotButton.addItemListener(this);
        add(autoPilotButton);
    }
    public void itemStateChanged(ItemEvent e){
        if (e.getStateChange() == ItemEvent.SELECTED){
            skyView.setAutoPilot(true);
            autoPilotButton.setText("Autopilot ON");
        } else {
            skyView.setAutoPilot(false);
            autoPilotButton.setText("Autopilot OFF");
        }
    }       
}

public class AircraftDisplay extends JFrame {

    private SkyView skyView;
    private SimulationMenuBar simMenu;
    private BufferedReader in;
    private DataOutputStream out;
    private JPanel panelGroup0, panelGroup1;
    private ControlPanel controlPanel;

    public AircraftDisplay(SkyView sky) {
        skyView = sky;
        simMenu = new SimulationMenuBar(skyView);
        setJMenuBar(simMenu);
        add( skyView);
        setTitle("Aircraft Display");
        setSize(800, 800);
        setLocationRelativeTo(null);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        panelGroup1 = new JPanel();
        panelGroup1.setLayout(new BoxLayout(panelGroup1, BoxLayout.X_AXIS));
        panelGroup1.add(skyView);
        
        controlPanel = new ControlPanel(skyView);

        panelGroup0 = new JPanel();
        panelGroup0.setLayout(new BoxLayout(panelGroup0, BoxLayout.Y_AXIS));
        panelGroup0.add(panelGroup1);
        panelGroup0.add(controlPanel);

        add(panelGroup0);
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
        String waypointInputFile = null;

        int ii = 0;
        while (ii < args.length) {
            switch (args[ii]) {
                case "-help" :
                case "--help" : {
                    printHelpText();
                    System.exit(0);
                } break;
                case "-w" :
                case "--waypoints" : {
                    waypointInputFile = args[++ii];
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

        if(waypointInputFile == null) {
            System.out.println("No waypoint file specified");
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

        double desired_speed = 0.0;

        Boolean autopilot = false;
        double desired_heading = 0.0;

        try {
            BufferedReader br = new BufferedReader(new FileReader(waypointInputFile));
            String line;
            while((line = br.readLine()) != null) {
                String[] parsedLine = line.split(",");
                sd.skyView.addWaypoint(Double.parseDouble(parsedLine[0]), Double.parseDouble(parsedLine[1]), parsedLine[2]);
            }
        } catch(FileNotFoundException e) {
            System.out.printf("'%s' not found", args[ii+1]);
            System.exit(0);
        }

        System.out.println("Connecting to: " + host + ":" + port);
        sd.connectToServer(host, port);

        sd.out.writeBytes("trick.var_set_client_tag(\"AircraftDisplay\") \n" +
                          "trick.var_pause() \n" +
                          "trick.var_add(\"dyn.aircraft.pos[0]\") \n" +
                          "trick.var_add(\"dyn.aircraft.pos[1]\") \n" +
                          "trick.var_add(\"dyn.aircraft.vel[0]\") \n" +
                          "trick.var_add(\"dyn.aircraft.vel[1]\") \n" +
                          "trick.var_add(\"dyn.aircraft.desired_speed\") \n" +
                          "trick.var_add(\"dyn.aircraft.desired_heading\") \n" +
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
                desired_speed = Double.parseDouble( field[5]);
                desired_heading = Double.parseDouble(field[6]);

                // Set the Aircraft position
                skyview.setAircraftPos(posNorth, posWest);
                skyview.setAircraftVel(velNorth, velWest);

                desired_speed = skyview.getDesiredSpeed();
                sd.out.writeBytes(String.format("dyn.aircraft.desired_speed = %.2f ;\n", desired_speed));

                desired_heading = skyview.getDesiredHeading();
                sd.out.writeBytes(String.format("dyn.aircraft.desired_heading= %.2f ;\n", desired_heading));

                autopilot = skyview.getAutoPilot();
            
                if (autopilot == true){
                    sd.out.writeBytes("dyn.aircraft.autoPilot = True ;\n");
                } else {
                  sd.out.writeBytes("dyn.aircraft.autoPilot = False ;\n");
                } 

                ((SimulationMenuBar) sd.getJMenuBar()).setEnabled_DisabledViewCB(autopilot);

            } catch (IOException | NullPointerException e ) {
                go = false;
            }
            sd.drawSkyView();
        }
    }
}
