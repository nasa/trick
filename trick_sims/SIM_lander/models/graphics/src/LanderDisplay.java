/*
 * Trick
 * 2020 (c) National Aeronautics and Space Administration (NASA)
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

class ScenePoly {
    public Color color;
    public int n;
    public double[] x;
    public double[] y;
}


class RangeView extends JPanel {

    private int scale;
    private Color skyColor;
    private Color groundColor;

    private Color landerColor;

    // Origin of world coordinates in jpanel coordinates.
    private int worldOriginX;
    private int worldOriginY;

    private double[] landerPos;
    private double[] landerVel;
    private double   landerAngle;
    private double   landerAngleRate;
    private double   nozzleAngle;
    private int      landerThrottle;    /* percent */
    private int      rcs_state;

    private ScenePoly left_pad, right_pad;
    private ScenePoly left_L1,  right_L1;
    private ScenePoly left_L2,  right_L2;
    private ScenePoly left_L3,  right_L3;
    private ScenePoly left_L4,  right_L4;
    private ScenePoly fuselage;
    private ScenePoly nozzle;
    private ScenePoly left_rcs_pod, right_rcs_pod;
    private ScenePoly left_top_rcs_nozzle, right_top_rcs_nozzle, left_bottom_rcs_nozzle, right_bottom_rcs_nozzle;
    private ScenePoly left_top_rcs_plume, right_top_rcs_plume, left_bottom_rcs_plume, right_bottom_rcs_plume;
    private ScenePoly docking_ring;
    private ScenePoly flame;

    private int[] workPolyX, workPolyY;

    // Controls
    private int throttleChange ;
    private int rcs_state_chg_cmd;
    private boolean altitudeCtrlEnabled;
    private double altitudeSetPoint;
    private boolean downRangeCtrlEnabled;
    private double downRangeSetPoint;


    /**
     * Class constructor.
     */
    public RangeView( int mapScale) {

        setScale(mapScale);

        throttleChange = 0;
        rcs_state = 0;
        altitudeCtrlEnabled = false;
        downRangeCtrlEnabled = false;

        skyColor    = new Color(0,0,20);
        groundColor = new Color(150,150,150);
        landerColor = new Color(150,10,10);

        landerAngle = 0.0;
        nozzleAngle = 0.0;
        landerPos  = new double[]
            {5.0, 0.0};
        landerVel  = new double[]
            {0.0, 0.0};
        landerThrottle = 0;

        left_pad = new ScenePoly();
        left_pad.color = new Color(200,200,150);
        left_pad.x = new double[] { -2.0, -1.6, -1.7, -1.8, -1.9, -2.0, -2.1, -2.2, -2.3, -2.4 };
        left_pad.y  = new double[] { -1.7, -1.7, -1.75, -1.77, -1.79, -1.80, -1.79, -1.77, -1.75, -1.70 };
        left_pad.n = 10;

        right_pad = new ScenePoly();
        right_pad.color = new Color(200,200,150);
        right_pad.x = new double[] { 2.0, 1.6, 1.7, 1.8, 1.9, 2.0, 2.1, 2.2, 2.3, 2.4 };
        right_pad.y = new double[] { -1.7, -1.7, -1.75, -1.77, -1.79, -1.80, -1.79, -1.77, -1.75, -1.70 };
        right_pad.n = 10;

        left_L1 = new ScenePoly();
        left_L1.color = new Color(100,100,100);
        left_L1.x = new double[]  { -1.46, -1.0,  -1.0, -1.48 };
        left_L1.y  = new double[] { -0.51, -0.95, -1.0, -0.55};
        left_L1.n = 4;

        right_L1 = new ScenePoly();
        right_L1.color = new Color(100,100,100);
        right_L1.x = new double[]  { 1.46, 1.0,  1.0, 1.48 };
        right_L1.y  = new double[] { -0.51, -0.95, -1.0, -0.55};
        right_L1.n = 4;

        left_L2 = new ScenePoly();
        left_L2.color = new Color(100,100,100);
        left_L2.x = new double[]  { -1.0, -1.0, -1.75, -1.84};
        left_L2.y  = new double[] { -0.95, -1.0, -1.28,-1.25};
        left_L2.n = 4;

        right_L2 = new ScenePoly();
        right_L2.color = new Color(100,100,100);
        right_L2.x = new double[]  { 1.0, 1.0, 1.75, 1.84};
        right_L2.y  = new double[] { -0.95, -1.0, -1.28,-1.25};
        right_L2.n = 4;

        left_L3 = new ScenePoly();
        left_L3.color = new Color(50,50,50);
        left_L3.x = new double[]  { -1.0,  -1.0,  -1.47, -1.75, -1.84, -1.54};
        left_L3.y  = new double[] { -0.15, -0.25, -0.53, -1.28, -1.25, -0.46};
        left_L3.n = 6;

        right_L3 = new ScenePoly();
        right_L3.color = new Color(50,50,50);
        right_L3.x = new double[] {   1.0,   1.0,  1.47,  1.75,  1.84,  1.54};
        right_L3.y = new double[] { -0.15, -0.25, -0.53, -1.28, -1.25, -0.46};
        right_L3.n = 6;

        left_L4 = new ScenePoly();
        left_L4.color = new Color(100,100,100);
        left_L4.x = new double[]  { -1.78, -1.94, -1.98, -1.81};
        left_L4.y  = new double[] { -1.27, -1.7, -1.7, -1.26};
        left_L4.n = 4;

        right_L4 = new ScenePoly();
        right_L4.color = new Color(100,100,100);
        right_L4.x = new double[]  { 1.78, 1.94, 1.98, 1.81};
        right_L4.y  = new double[] { -1.27, -1.7, -1.7, -1.26};
        right_L4.n = 4;

        fuselage = new ScenePoly();
        fuselage.color = new Color(200,200,220);
        fuselage.x = new double[]  { 0.40, 0.65, 0.90, 1.00,  1.00, -1.00, -1.00, -0.90, -0.65, -0.40};
        fuselage.y  = new double[] { 1.75, 1.60, 1.30, 0.85, -1.00, -1.00,  0.85,  1.30,  1.60,  1.75};
        fuselage.n = 10;

        nozzle = new ScenePoly();
        nozzle.color = new Color(150,150,150);
        nozzle.x = new double[]  { -0.3, -0.4, 0.4, 0.3};
        nozzle.y  = new double[] { -1.0, -1.3, -1.3, -1.0};
        nozzle.n = 4;

        left_rcs_pod = new ScenePoly();
        left_rcs_pod.color = new Color(100,100,100);
        left_rcs_pod.x = new double[]  { -1.0, -1.25, -1.25, -1.0};
        left_rcs_pod.y  = new double[] { 0.1, 0.1, -0.1, -0.1};
        left_rcs_pod.n = 4;

        right_rcs_pod = new ScenePoly();
        right_rcs_pod.color = new Color(100,100,100);
        right_rcs_pod.x = new double[]  { 1.0, 1.25, 1.25, 1.0};
        right_rcs_pod.y  = new double[] { 0.1, 0.1, -0.1, -0.1};
        right_rcs_pod.n = 4;

        left_top_rcs_nozzle = new ScenePoly();
        left_top_rcs_nozzle.color = new Color(255,100,100);
        left_top_rcs_nozzle.x = new double[]  { -1.12, -1.18, -1.2, -1.1 };
        left_top_rcs_nozzle.y  = new double[] { 0.1, 0.1, 0.2, 0.2 };
        left_top_rcs_nozzle.n = 4;

        left_top_rcs_plume = new ScenePoly();
        left_top_rcs_plume.color = new Color(255,255,255);
        left_top_rcs_plume.x = new double[]  { -1.1, -1.2, -1.24, -1.06 };
        left_top_rcs_plume.y  = new double[] { 0.2, 0.2, 0.4, 0.4 };
        left_top_rcs_plume.n = 4;

        right_top_rcs_nozzle = new ScenePoly();
        right_top_rcs_nozzle.color = new Color(255,100,100);
        right_top_rcs_nozzle.x = new double[]  { 1.12, 1.18, 1.2, 1.1 };
        right_top_rcs_nozzle.y  = new double[] { 0.1, 0.1, 0.2, 0.2 };
        right_top_rcs_nozzle.n = 4;

        right_top_rcs_plume = new ScenePoly();
        right_top_rcs_plume.color = new Color(255,255,255);
        right_top_rcs_plume.x = new double[]  { 1.1, 1.2, 1.24, 1.06 };
        right_top_rcs_plume.y  = new double[] { 0.2, 0.2, 0.4, 0.4 };
        right_top_rcs_plume.n = 4;

        left_bottom_rcs_nozzle = new ScenePoly();
        left_bottom_rcs_nozzle.color = new Color(255,100,100);
        left_bottom_rcs_nozzle.x = new double[]  { -1.12, -1.18, -1.2, -1.1 };
        left_bottom_rcs_nozzle.y  = new double[] { -0.1, -0.1, -0.2, -0.2 };
        left_bottom_rcs_nozzle.n = 4;

        left_bottom_rcs_plume = new ScenePoly();
        left_bottom_rcs_plume.color = new Color(255,255,255);
        left_bottom_rcs_plume.x = new double[]  { -1.1, -1.2, -1.24, -1.06 };
        left_bottom_rcs_plume.y  = new double[] { -0.2, -0.2, -0.4, -0.4 };
        left_bottom_rcs_plume.n = 4;

        right_bottom_rcs_nozzle = new ScenePoly();
        right_bottom_rcs_nozzle.color = new Color(255,100,100);
        right_bottom_rcs_nozzle.x = new double[]  { 1.12, 1.18, 1.2, 1.1 };
        right_bottom_rcs_nozzle.y  = new double[] { -0.1, -0.1, -0.2, -0.2 };
        right_bottom_rcs_nozzle.n = 4;

        right_bottom_rcs_plume = new ScenePoly();
        right_bottom_rcs_plume.color = new Color(255,255,255);
        right_bottom_rcs_plume.x = new double[]  {  1.1,  1.2,  1.24, 1.06 };
        right_bottom_rcs_plume.y  = new double[] { -0.2, -0.2, -0.4, -0.4 };
        right_bottom_rcs_plume.n = 4;

        docking_ring = new ScenePoly();
        docking_ring.color = new Color(100,100,200);
        docking_ring.x = new double[]  { 0.4, 0.4, -0.4, -0.4 };
        docking_ring.y  = new double[] { 2.0, 1.75, 1.75, 2.0 };
        docking_ring.n = 4;

        flame = new ScenePoly();
        flame.color = new Color(200,150,100);
        flame.x = new double[]  { 0.0, -0.4, 0.4 };
        flame.y  = new double[] { -6.3, -1.3, -1.3 };
        flame.n = 3;

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

    public void rcs_ccw()         { rcs_state_chg_cmd = 1; }
    public void rcs_cw()          { rcs_state_chg_cmd = -1; }
    public void rcs_zero_change() { rcs_state_chg_cmd = 0; }

    public int  get_rcs_state_chg() { return rcs_state_chg_cmd; }
    public void set_rcs_state(int state) { rcs_state = state; }

    public void throttleUp()         { throttleChange =    1; }
    public void throttleDown()       { throttleChange =   -1; }
    public void throttleZeroChange() { throttleChange =    0; }
    public void throttleMaxDown()    { throttleChange = -100; }
    public int  getThrottleDelta()   { return throttleChange; }

    public boolean getAltitudeCtrlState()  { return altitudeCtrlEnabled; }
    public void toggleAltitudeCtrlState()  { altitudeCtrlEnabled = !altitudeCtrlEnabled; }
    public void disableAltitudeCtrl()      { altitudeCtrlEnabled = false; }

    public void setAltitudeSetPoint(double setpoint) { altitudeSetPoint = setpoint; }
    public double getAltitudeSetPoint()    { return altitudeSetPoint; }

    public boolean getDownRangeCtrlState() { return downRangeCtrlEnabled; }
    public void toggleDownRangeCtrlState() { downRangeCtrlEnabled = !downRangeCtrlEnabled; }
    public void disableDownRangeCtrl()     { downRangeCtrlEnabled = false; }

    public void setDownRangeSetPoint(double setpoint) { downRangeSetPoint = setpoint; }
    public double getDownRangeSetPoint()    { return downRangeSetPoint; }

    public void setLanderAngle(double angle) {
        landerAngle = angle;
    }
    public void setLanderAngleRate(double angle_rate) {
        landerAngleRate = angle_rate;
    }
    public void setNozzleAngle(double angle) {
        nozzleAngle = angle;
    }
    public void setLanderPos(double x, double y) {
        landerPos[0] = x;
        landerPos[1] = y;
    }
    public void setLanderVel(double vx, double vy) {
        landerVel[0] = vx;
        landerVel[1] = vy;
    }
    public void setLanderThrottle(int percent) {
        landerThrottle = percent;
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

        int ii, jj;
        int width  = getWidth();
        int height = getHeight();

        worldOriginX = (width/2)  - (int)(scale * landerPos[0]);
        worldOriginY = (height/2) + (int)(scale * landerPos[1]);

        // Draw Sky
        g2d.setPaint(skyColor);
        g2d.fillRect(0, 0, width, worldOriginY);

        // |jpanel_x| = |origin_x| + |scale    0  | * |cos(angle) -sin(angle)| * |world_x|
        // |jpanel_y|   |origin_y|   |  0   -scale|   |sin(angle)  cos(angle)|   |world_y|

        // Draw ground.
        g2d.setPaint(groundColor);
        g2d.fillRect(0, worldOriginY, width, height);

        // ===============================================================================
        //  Draw lander
        // ===============================================================================
        drawScenePoly(g2d, left_pad, landerAngle, landerPos[0], landerPos[1]);
        drawScenePoly(g2d, right_pad, landerAngle, landerPos[0], landerPos[1]);
        drawScenePoly(g2d, left_L1, landerAngle, landerPos[0], landerPos[1]);
        drawScenePoly(g2d, right_L1, landerAngle, landerPos[0], landerPos[1]);
        drawScenePoly(g2d, left_L2, landerAngle, landerPos[0], landerPos[1]);
        drawScenePoly(g2d, right_L2, landerAngle, landerPos[0], landerPos[1]);
        drawScenePoly(g2d, left_L3, landerAngle, landerPos[0], landerPos[1]);
        drawScenePoly(g2d, right_L3, landerAngle, landerPos[0], landerPos[1]);
        drawScenePoly(g2d, left_L4, landerAngle, landerPos[0], landerPos[1]);
        drawScenePoly(g2d, right_L4, landerAngle, landerPos[0], landerPos[1]);
        drawScenePoly(g2d, fuselage, landerAngle, landerPos[0], landerPos[1]);
        drawScenePoly(g2d, nozzle, (landerAngle + nozzleAngle), landerPos[0], landerPos[1]);
        drawScenePoly(g2d, left_rcs_pod, landerAngle, landerPos[0], landerPos[1]);
        drawScenePoly(g2d, right_rcs_pod, landerAngle, landerPos[0], landerPos[1]);

        drawScenePoly(g2d, left_top_rcs_nozzle, landerAngle, landerPos[0], landerPos[1]);
        drawScenePoly(g2d, right_top_rcs_nozzle, landerAngle, landerPos[0], landerPos[1]);
        drawScenePoly(g2d, left_bottom_rcs_nozzle, landerAngle, landerPos[0], landerPos[1]);
        drawScenePoly(g2d, right_bottom_rcs_nozzle, landerAngle, landerPos[0], landerPos[1]);
        drawScenePoly(g2d, docking_ring, landerAngle, landerPos[0], landerPos[1]);
        drawScenePoly(g2d, flame, landerAngle, landerPos[0], landerPos[1]);

        if ( rcs_state > 0) {
            drawScenePoly(g2d, left_top_rcs_plume, landerAngle, landerPos[0], landerPos[1]);
            drawScenePoly(g2d, right_bottom_rcs_plume, landerAngle, landerPos[0], landerPos[1]);
        } else if ( rcs_state < 0) {
            drawScenePoly(g2d, right_top_rcs_plume, landerAngle, landerPos[0], landerPos[1]);
            drawScenePoly(g2d, left_bottom_rcs_plume, landerAngle, landerPos[0], landerPos[1]);
        }

        // ===============================================================================
        // Move the first vertex of the triangular polygon that represents the main-engine
        // plume to make the triangle bigger or smaller depending on the throttle setting.
        flame.y[0] = - (landerThrottle / 100.0) * 5.0 - 1.3;

        for (ii = 0; ii < flame.n; ii++) {
            workPolyX[ii] = (int)(worldOriginX + scale *
                ( Math.cos(landerAngle+nozzleAngle) * flame.x[ii] - Math.sin(landerAngle+nozzleAngle) * flame.y[ii] + landerPos[0] ));
            workPolyY[ii] = (int)(worldOriginY - scale *
                ( Math.sin(landerAngle+nozzleAngle) * flame.x[ii] + Math.cos(landerAngle+nozzleAngle) * flame.y[ii] + landerPos[1] ));
        }
        g2d.setPaint(flame.color);
        g2d.fillPolygon(workPolyX, workPolyY, flame.n);

        // Draw range markers.
        int tickRange = 50;
        if (scale >=  8) tickRange = 20;
        if (scale >= 16) tickRange = 10;
        if (scale >= 32) tickRange =  5;
        if (scale >= 64) tickRange =  1;

        int lower = ((int)((      - worldOriginX)/(scale * tickRange)) + 1) * tickRange;
        int upper = ((int)((width - worldOriginX)/(scale * tickRange)) + 1) * tickRange;

        g2d.setPaint(Color.WHITE);

        for (ii = lower ; ii < upper ; ii += tickRange) {
            int mx = (int)(worldOriginX + scale * ii);
            g2d.drawLine( mx, worldOriginY, mx, worldOriginY + 20);
            g2d.drawString ( String.format("%d",ii), mx, worldOriginY + 15);
        }
        double angle_d =  landerAngle * 57.29577;
        double angle_rate_d = landerAngleRate * 57.29577;
        g2d.drawString ( String.format("SCALE: %d pixels/meter",scale), 20,20);
        g2d.drawString ( String.format("Throttle : [%d]", landerThrottle), 20,40);
        g2d.drawString ( String.format("Lander Pitch (Deg): [%.2f]", angle_d), 20,60);
        g2d.drawString ( String.format("Lander PitchRate (Deg/s): [%.2f]", angle_rate_d), 20,80);
        g2d.drawString ( String.format("Lander Pos: [%.2f, %.2f]", landerPos[0], landerPos[1]), 20,100);
        g2d.drawString ( String.format("Lander Vel: [%.2f, %.2f]", landerVel[0], landerVel[1]), 20,120);

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

class AltitudeCtrlPanel extends JPanel implements ActionListener {

    private RangeView rangeView;
    private JButton altCtrlButton;
    private JButton setButton;
    private JFormattedTextField altitudeField;

    public AltitudeCtrlPanel(RangeView view) {

        rangeView = view;
        setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

        altCtrlButton = new JButton("V-Ctrl");
        altCtrlButton.addActionListener(this);
        altCtrlButton.setActionCommand("altitude-ctrl");
        altCtrlButton.setToolTipText("Altitude Control");
        add(altCtrlButton);

        NumberFormat format = NumberFormat.getInstance();
        format.setGroupingUsed(false);

        NumberFormatter formatter = new NumberFormatter(format);
        formatter.setValueClass(Integer.class);
        formatter.setMinimum(0);
        formatter.setMaximum(99999);
        formatter.setAllowsInvalid(true);
        formatter.setCommitsOnValidEdit(true);

        altitudeField = new JFormattedTextField(formatter);
        altitudeField.setMaximumSize( new Dimension(80, 20) );
        altitudeField.setText("0");
        add(altitudeField);

        setButton = new JButton("Set");
        setButton.addActionListener(this);
        setButton.setActionCommand("set");
        setButton.setToolTipText("Set Desired Altitude.");
        add(setButton);
    }

    public void actionPerformed(ActionEvent e) {
        String s = e.getActionCommand();
        switch (s) {
            case "altitude-ctrl":
                rangeView.toggleAltitudeCtrlState();
                if (rangeView.getAltitudeCtrlState())
                    altCtrlButton.setForeground(Color.GREEN);
                else
                    altCtrlButton.setForeground(Color.BLACK);
                break;
            case "set":
                double setpoint = ((Number)altitudeField.getValue()).doubleValue();
                rangeView.setAltitudeSetPoint(setpoint);
                break;
            default:
                System.out.println("Unknown Action Command:" + s);
                break;
        }
    }

    public void update() {
        if (rangeView.getAltitudeCtrlState()) {
            altCtrlButton.setForeground(Color.GREEN);
        } else {
            altCtrlButton.setForeground(Color.BLACK);
        }
    }

}

class DownRangeCtrlPanel extends JPanel implements ActionListener {

    private RangeView rangeView;
    private JButton drngCtrlButton;
    private JButton setButton;
    private JFormattedTextField drngField;

    public DownRangeCtrlPanel(RangeView view) {

        rangeView = view;
        setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

        drngCtrlButton = new JButton("H-Ctrl");
        drngCtrlButton.addActionListener(this);
        drngCtrlButton.setActionCommand("downrange-ctrl");
        drngCtrlButton.setToolTipText("Down-Range Control");
        add(drngCtrlButton);

        NumberFormat format = NumberFormat.getInstance();
        format.setGroupingUsed(false);

        NumberFormatter formatter = new NumberFormatter(format);
        formatter.setValueClass(Integer.class);
        formatter.setMinimum(0);
        formatter.setMaximum(99999);
        formatter.setAllowsInvalid(true);
        formatter.setCommitsOnValidEdit(true);

        drngField = new JFormattedTextField(formatter);
        drngField.setMaximumSize( new Dimension(80, 20) );
        drngField.setText("0");
        add(drngField);

        setButton = new JButton("Set");
        setButton.addActionListener(this);
        setButton.setActionCommand("set");
        setButton.setToolTipText("Set Desired Horizontal Position.");
        add(setButton);

    }

    public void actionPerformed(ActionEvent e) {
        String s = e.getActionCommand();
        switch (s) {
            case "downrange-ctrl":
                rangeView.toggleDownRangeCtrlState();
                if (rangeView.getDownRangeCtrlState())
                    drngCtrlButton.setForeground(Color.GREEN);
                else
                    drngCtrlButton.setForeground(Color.BLACK);
                break;
            case "set":
                double setpoint = ((Number)drngField.getValue()).doubleValue();
                rangeView.setDownRangeSetPoint(setpoint);
                break;
            default:
                System.out.println("Unknown Action Command:" + s);
                break;
        }
    }

    public void update() {
        if (rangeView.getDownRangeCtrlState()) {
            drngCtrlButton.setForeground(Color.GREEN);
        } else {
            drngCtrlButton.setForeground(Color.BLACK);
        }
    }

}


class ManualCtrlPanel extends JPanel implements ActionListener {
    private RangeView rangeView;
    private JButton rollLeftButton, rollRightButton;
    private JButton throttleUpButton, throttleDownButton;

    public ManualCtrlPanel(RangeView view) {
        rangeView = view;
        setLayout(new BoxLayout(this, BoxLayout.X_AXIS));
        setBorder( BorderFactory.createEtchedBorder(EtchedBorder.RAISED));

        rollLeftButton = new JButton("\u21ba");
        rollLeftButton.addActionListener(this);
        rollLeftButton.setActionCommand("rollLeft");
        rollLeftButton.setToolTipText("Roll Left");

        throttleUpButton = new JButton("\u25b2");
        throttleUpButton.addActionListener(this);
        throttleUpButton.setActionCommand("throttleUp");
        throttleUpButton.setToolTipText("Throttle Up");

        throttleDownButton = new JButton("\u25bc");
        throttleDownButton.addActionListener(this);
        throttleDownButton.setActionCommand("throttleDown");
        throttleDownButton.setToolTipText("Throttle Down");

        rollRightButton = new JButton("\u21bb");
        rollRightButton.addActionListener(this);
        rollRightButton.setActionCommand("rollRight");
        rollRightButton.setToolTipText("Roll Right");

        add(rollLeftButton);
        add(throttleUpButton);
        add(throttleDownButton);
        add(rollRightButton);
    }

    public void actionPerformed(ActionEvent e) {
        String s = e.getActionCommand();
        switch (s) {
            case "rollLeft":
                rangeView.rcs_ccw();
                break;
            case "rollRight":
                rangeView.rcs_cw();
                break;
            case "throttleUp":
                rangeView.throttleUp();
                break;
            case "throttleDown":
                rangeView.throttleDown();
                break;
            default:
                System.out.println("Unknown Action Command:" + s);
                break;
        }
    }
}

class ControlPanel extends JPanel implements ActionListener {

    private RangeView rangeView;
    private JButton zoomOutButton, zoomInButton;
    private JButton shutDownButton;
    private ManualCtrlPanel manualCtrlPanel;
    private AltitudeCtrlPanel altitudeCtrlPanel;
    private DownRangeCtrlPanel downRangeCtrlPanel;

    public ControlPanel(RangeView view) {

        rangeView = view;
        setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

        JPanel labeledManualCtrlPanel = new JPanel();
        labeledManualCtrlPanel.setLayout(new BoxLayout(labeledManualCtrlPanel, BoxLayout.Y_AXIS));
        JLabel manualControlLabel = new JLabel("Manual Control");
        manualControlLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
        labeledManualCtrlPanel.add(manualControlLabel);
        manualCtrlPanel = new ManualCtrlPanel(rangeView);
        labeledManualCtrlPanel.add( manualCtrlPanel );

        shutDownButton = new JButton("Shut Down");
        shutDownButton.addActionListener(this);
        shutDownButton.setActionCommand("shutdown");
        shutDownButton.setToolTipText("Set Throttle to zero and turn off automatic control.");
        shutDownButton.setAlignmentX(Component.CENTER_ALIGNMENT);
        labeledManualCtrlPanel.add( shutDownButton );
        add(labeledManualCtrlPanel);

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

        JPanel labeledAutoCtrlPanel = new JPanel();
        labeledAutoCtrlPanel.setLayout(new BoxLayout(labeledAutoCtrlPanel, BoxLayout.Y_AXIS));
        JLabel automaticControlLabel = new JLabel("Automatic Control");
        automaticControlLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
        labeledAutoCtrlPanel.add(automaticControlLabel);
        JPanel autoCtrlPanel = new JPanel();
        autoCtrlPanel.setBorder( BorderFactory.createEtchedBorder(EtchedBorder.RAISED));
        autoCtrlPanel.setLayout(new BoxLayout(autoCtrlPanel, BoxLayout.Y_AXIS));
        altitudeCtrlPanel = new AltitudeCtrlPanel(rangeView);
        autoCtrlPanel.add(altitudeCtrlPanel);
        downRangeCtrlPanel = new DownRangeCtrlPanel(rangeView);
        autoCtrlPanel.add(downRangeCtrlPanel);
        labeledAutoCtrlPanel.add(autoCtrlPanel);
        add (labeledAutoCtrlPanel);

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
            case "shutdown":
                rangeView.disableAltitudeCtrl();
                altitudeCtrlPanel.update();
                rangeView.disableDownRangeCtrl();
                downRangeCtrlPanel.update();
                rangeView.throttleMaxDown();
                break;
            default:
                System.out.println("Unknown Action Command:" + s);
                break;
        }
    }
} // class ControlPanel

public class LanderDisplay extends JFrame {

    private RangeView rangeView;
    private BufferedReader in;
    private DataOutputStream out;
    private JPanel panelGroup0;
    private JPanel panelGroup1;
    private ControlPanel controlPanel;

    public LanderDisplay(RangeView arena) {
        setTitle("Lander Range");

        rangeView  = arena;

        panelGroup1 = new JPanel();
        panelGroup1.setLayout(new BoxLayout(panelGroup1, BoxLayout.X_AXIS));
        panelGroup1.add(rangeView);

        controlPanel = new ControlPanel(rangeView);

        panelGroup0 = new JPanel();
        panelGroup0.setLayout(new BoxLayout(panelGroup0, BoxLayout.Y_AXIS));
        panelGroup0.add(panelGroup1);
        panelGroup0.add(controlPanel);

        add(panelGroup0);

        rangeView.setScale(32);
        rangeView.setLanderPos (0.0, 1.8);

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

    public void drawRangeView() {
        rangeView.repaint();
    }

    private static void  printHelpText() {
        System.out.println(
            "----------------------------------------------------------------------\n"
          + "usage: java jar LanderDisplay.jar <port-number>\n"
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
        double dt = 0.100; // Time between updates (seconds).
        double posx = 0.0;
        double posy = 0.0;
        double angle = 0.0;
        double angle_rate = 0.0;
        double nozzle_angle = 0.0;
        double velx = 0.0;
        double vely = 0.0;
        int throttle = 0;
        int rcs_state = 0;

        // Outbound command variables
        int throttle_delta;
        int rcs_state_chg_cmd;

        int simMode = 0;
        boolean standalone = false;

        int mapScale = 32 ; // pixels per meter.

        RangeView rangeView = new RangeView( mapScale);
        LanderDisplay landerDisplay = new LanderDisplay( rangeView);
        landerDisplay.setVisible(true);
        landerDisplay.drawRangeView();

        if (port == 0) {
            System.out.println("No variable server port specified.");
            printHelpText();
            System.exit(0);
        }

        // Connect to the Trick simulation's variable server
        System.out.println("Connecting to: " + host + ":" + port);
        landerDisplay.connectToServer(host, port);

        landerDisplay.out.writeBytes("trick.var_set_client_tag(\"LanderDisplay\") \n");
        landerDisplay.out.flush();

        // Have the Variable Server send us the simulation mode ONCE.
        landerDisplay.out.writeBytes( "trick.var_add(\"trick_sys.sched.mode\")\n" +
                            "trick.var_send() \n" +
                            "trick.var_clear() \n");
        landerDisplay.out.flush();

        // Read the response and extract the simulation mode.
        try {
            String line;
            String field[];
            line = landerDisplay.in.readLine();
            field = line.split("\t");
            simMode = Integer.parseInt( field[1]);
        } catch (IOException | NullPointerException e ) {
            go = false;
        }

        // Configure the Variable Server to cyclically send us the following varibales.
        // Tell the variable server:
        //  1) We want the values of the following variables:
        landerDisplay.out.writeBytes( "trick.var_pause() \n" +
                            "trick.var_add(\"dyn.lander.pos[0]\")\n" +
                            "trick.var_add(\"dyn.lander.pos[1]\")\n" +
                            "trick.var_add(\"dyn.lander.pitch\")\n" +
                            "trick.var_add(\"dyn.lander.vel[0]\")\n" +
                            "trick.var_add(\"dyn.lander.vel[1]\")\n" +
                            "trick.var_add(\"dyn.lander.pitchDot\")\n" +
                            "trick.var_add(\"dyn.lander.nozzle_angle\")\n" +
                            "trick.var_add(\"dyn.lander.throttle\")\n" +
                            "trick.var_add(\"dyn.lander.rcs_command\")\n" +
                            "trick.var_add(\"trick_sys.sched.mode\")\n" +
        //  2) We want the responses in ASCII:
                            "trick.var_ascii() \n" +
        //  3) We want values to be updated at the specified rate:
                            String.format("trick.var_cycle(%.3f)\n", dt) +
        //  4) Start sending values as specified.
                            "trick.var_unpause() \n" );
        landerDisplay.out.flush();

        while (go) {

            // Recieve and parse periodic data response from the variable server.
            try {
                String line;
                String field[];
                line = landerDisplay.in.readLine();
                field = line.split("\t");
                posx   = Double.parseDouble( field[1]);
                posy   = Double.parseDouble( field[2]);
                angle  = Double.parseDouble( field[3]);
                velx   = Double.parseDouble( field[4]);
                vely   = Double.parseDouble( field[5]);
                angle_rate = Double.parseDouble( field[6]);
                nozzle_angle = Double.parseDouble( field[7]);
                throttle = Integer.parseInt( field[8]);
                rcs_state = Integer.parseInt( field[9]);
                simMode = Integer.parseInt( field[10]);
            } catch (IOException | NullPointerException e ) {
                go = false;
            }

            // Update the display data.
            rangeView.setLanderPos(posx, posy);
            rangeView.setLanderVel(velx, vely);
            rangeView.setLanderAngle(angle);
            rangeView.setNozzleAngle(nozzle_angle);
            rangeView.setLanderAngleRate(angle_rate);
            rangeView.setLanderThrottle(throttle);
            rangeView.set_rcs_state(rcs_state);

            throttle_delta = rangeView.getThrottleDelta();
            landerDisplay.out.writeBytes( String.format("dyn.lander.manual_throttle_change_command = %d ;\n", throttle_delta ));
            rangeView.throttleZeroChange();

            rcs_state_chg_cmd = rangeView.get_rcs_state_chg();
            landerDisplay.out.writeBytes( String.format("dyn.lander.manual_rcs_command = %d ;\n", rcs_state_chg_cmd ));
            rangeView.rcs_zero_change();

            if (rangeView.getAltitudeCtrlState()) {
                landerDisplay.out.writeBytes("dyn.lander.altitudeCtrlEnabled = 1 ;\n");
                landerDisplay.out.writeBytes( String.format("dyn.lander.y_cmd = %.2f ;\n", rangeView.getAltitudeSetPoint()));
            } else {
                landerDisplay.out.writeBytes("dyn.lander.altitudeCtrlEnabled = 0 ;\n");
            }

            if (rangeView.getDownRangeCtrlState()) {
                landerDisplay.out.writeBytes("dyn.lander.downRangeCtrlEnabled = 1 ;\n");
                double foo = rangeView.getDownRangeSetPoint();
                landerDisplay.out.writeBytes( String.format("dyn.lander.x_cmd = %.2f ;\n", rangeView.getDownRangeSetPoint()));
            } else {
                landerDisplay.out.writeBytes("dyn.lander.downRangeCtrlEnabled = 0 ;\n");
            }

            landerDisplay.out.flush();

            //  Update the scene.
            landerDisplay.drawRangeView();

        } // while
    } // main
} // class
