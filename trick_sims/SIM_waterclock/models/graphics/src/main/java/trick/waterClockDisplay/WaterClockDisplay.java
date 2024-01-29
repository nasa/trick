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
import java.awt.geom.Area;
import java.awt.geom.Rectangle2D;

import javax.swing.text.NumberFormatter;
import java.text.NumberFormat;
import javax.swing.JFormattedTextField;
import java.awt.Dimension;

import javax.swing.BorderFactory;
import javax.swing.border.EtchedBorder;
import java.awt.Component;

class RangeView extends JPanel {

    private int scale;
    private Color bucket_color;
    private Color spout_color;
    private Color water_color;
    private Color float_color;

    // Origin of world coordinates in jpanel coordinates.
    private int worldOriginX;
    private int worldOriginY;

	private double  spout_rate;
	private double  overflow_rate;
	private double  intake_depth;
	private double  timer_depth;
	private double  intake_lvl;
	private double  timer_lvl;
	private double  intake_vol;
	private double  timer_vol;
    private double  spout_height;
	private double  overflow_height;
	private double  input_flow;
	private int current_tick;

    // Controls
	private double input_flow_select;
	public boolean new_input_flow;

    /**
     * Class constructor.
     */
    public RangeView( int mapScale) {

        setScale(mapScale);
			
		spout_rate = 0.0;
		overflow_rate = 0.0;
		intake_depth = 0.0;
		timer_depth = 0.0;
		intake_lvl = 0.0;
		timer_lvl = 0.0;
		intake_vol = 0.0;
		timer_vol = 0.0;
        spout_height = 0.0;
        overflow_height = 0.0;
        input_flow = 0.0;
		input_flow_select = 0.0;
		current_tick = 0;
		
		new_input_flow = false;

        bucket_color = new  Color(150,75,0);
        spout_color = new  Color(128,128,128);
        water_color = new  Color(0,0,128);
        float_color = new  Color(0,0,0);
    }

    public void fillSceneRect(Graphics2D g2d, Color color, double x, double y, double w, double h) {
        g2d.setPaint(color);
        g2d.fillRect( (int)(worldOriginX+scale*(x)), (int)(worldOriginY-scale*(y)), (int)(scale*w), (int)(scale*h));
    }

    public void setSpoutRate(double x) {
        spout_rate = x;
    }

    public void setOverflowRate(double x) {
        overflow_rate = x;
    }

    public void setIntakeDepth(double x) {
        intake_depth = x;
    }

    public void setTimerDepth(double x) {
        timer_depth = x;
    }

    public void setIntakeWarerLevel(double x) {
        intake_lvl = x;
    }

    public void setTimerWarerLevel(double x) {
        timer_lvl = x;
    }

    public void setIntakeWaterVol(double x) {
        intake_vol = x;
    }

    public void setTimerWaterLevel(double x) {
        timer_vol = x;
    }

    public void setSpoutHeight(double x) {
        spout_height = x;
    }

    public void setOverflowHeight(double x) {
        overflow_height = x;
    }

    public void setInputFlow(double x) {
        input_flow = x;
    }

    public void setInputFlowSelect(double x) {
        input_flow_select = x;
    }

    public double getInputFlowSelect() {
        return input_flow_select;
    }

    public void setCurrentTick(int x) {
        current_tick = x;
    }

    public void setScale (int mapScale) {
        if (mapScale < 2) {
            scale = 2;
        } else if (mapScale > 128) {
            scale = 128;
        } else {
            scale = mapScale;
        }
        repaint();
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

        worldOriginX = (width/2)  - (int)(scale * 0);
        worldOriginY = (height/2) + (int)(scale * 0);
		
		
        // Draw intake Bucket
		double bucket_length = 2.0;
		double bucket_width = 0.15;
		double intake_bucket_origin_x = 4;
		double intake_bucket_origin_y = 1.7;
		double intake_bucket_left_wall_x = intake_bucket_origin_x - bucket_length - bucket_width;
		double intake_bucket_bottom_x = intake_bucket_origin_x - bucket_length - bucket_width;
		double intake_bucket_bottom_y = intake_bucket_origin_y - bucket_length;
		double spout_width = 0.175;
		double spout_length = 1.0;
		double spout_pipe_x = intake_bucket_left_wall_x - spout_length + bucket_width;
		double spout_pipe_y = intake_bucket_origin_y - bucket_length + spout_width + ((spout_height / intake_depth) * bucket_length) - spout_width/2;
		double overflow_pipe_x = intake_bucket_origin_x;
		double overflow_pipe_y = intake_bucket_origin_y - bucket_length + spout_width + ((overflow_height / intake_depth) * bucket_length) - spout_width/2;
        fillSceneRect(	g2d, 
						bucket_color, 
						intake_bucket_origin_x, 
						intake_bucket_origin_y, 
						bucket_width, 
						bucket_length);
        fillSceneRect(	g2d, 
						bucket_color, 
						intake_bucket_bottom_x,
						intake_bucket_bottom_y, 
						bucket_length + (2 * bucket_width), 
						bucket_width);
        fillSceneRect(	g2d, 
						bucket_color,
						intake_bucket_left_wall_x, 
						intake_bucket_origin_y, 
						bucket_width, 
						bucket_length);
        fillSceneRect(	g2d, 
						spout_color, 
						overflow_pipe_x, 
						overflow_pipe_y, 
						spout_length, 
						spout_width);
        fillSceneRect(	g2d, 
						spout_color, 
						spout_pipe_x,
						spout_pipe_y, 
						spout_length, 
						spout_width);
		
		//Draw timer bucket
		double timer_bucket_origin_x = 1.375;
		double timer_bucket_origin_y = -0.5;
		double timer_bucket_left_wall_x = timer_bucket_origin_x - bucket_length - bucket_width;
		double timer_bucket_bottom_x = timer_bucket_origin_x - bucket_length - bucket_width;
		double timer_bucket_bottom_y = timer_bucket_origin_y - bucket_length;
        fillSceneRect(	g2d, 
						bucket_color, 
						timer_bucket_origin_x, 
						timer_bucket_origin_y, 
						bucket_width, 
						bucket_length);
        fillSceneRect(	g2d, 
						bucket_color, 
						timer_bucket_bottom_x,
						timer_bucket_bottom_y, 
						bucket_length + (2 * bucket_width), 
						bucket_width);
        fillSceneRect(	g2d, 
						bucket_color,
						timer_bucket_left_wall_x, 
						timer_bucket_origin_y, 
						bucket_width, 
						bucket_length);
						
		
		//Draw water flow
		double flow_width = 0.15;
		double spout_flow_height = intake_bucket_origin_y - timer_bucket_origin_y + (bucket_length - (intake_bucket_origin_y - spout_pipe_y)) - spout_width/2;
		double overflow_height = 20;
		if (spout_rate != 0.0) {
			fillSceneRect(	g2d, 
							water_color, 
							intake_bucket_left_wall_x - spout_length + bucket_width - flow_width + 0.015,
							spout_pipe_y - spout_width/2,
							flow_width, 
							spout_flow_height);
		}
		
		if (overflow_rate != 0.0) {
			fillSceneRect(	g2d, 
							water_color, 
							intake_bucket_origin_x + spout_length, 
							overflow_pipe_y - spout_width/2,
							flow_width, 
							overflow_height);
		}
						
		
		//Draw water level
		double intake_capacity = intake_lvl / intake_depth;
		double intake_offset = ((1 - intake_capacity) * bucket_length) ;
		double timer_capacity = timer_lvl / timer_depth;
		double timer_offset = ((1 - timer_capacity) * bucket_length) ;
		fillSceneRect(	g2d, 
						water_color, 
						intake_bucket_origin_x - bucket_length - 0.01,
						intake_bucket_origin_y - intake_offset, 
						bucket_length + 0.03,
						intake_capacity * bucket_length);
		//fill in pixel gaps due to rounding error
		if(intake_lvl > 0.0) {
			fillSceneRect(	g2d, 
							water_color, 
							intake_bucket_origin_x - bucket_length - 0.01,
							intake_bucket_origin_y - bucket_length + 0.05, 
							bucket_length + 0.03,
							0.05);
		}
						
		
		fillSceneRect(	g2d, 
						water_color, 
						timer_bucket_origin_x - bucket_length - 0.01,
						timer_bucket_origin_y - timer_offset, 
						bucket_length + 0.03,
						timer_capacity * bucket_length);
		//fill in pixel gaps due to rounding error
		if(timer_lvl > 0.0) {
			fillSceneRect(	g2d, 
							water_color, 
							timer_bucket_origin_x - bucket_length - 0.01,
							timer_bucket_origin_y - bucket_length + 0.05, 
							bucket_length + 0.03,
							0.075);
		}
		
		//Draw external source pipe
        fillSceneRect(	g2d, 
						spout_color, 
						intake_bucket_origin_x - bucket_length/4, 
						intake_bucket_origin_y + 0.8, 
						7, 
						spout_width);
						
		if(input_flow > 0) {
			fillSceneRect(	g2d, 
							water_color, 
							intake_bucket_origin_x - bucket_length/4 - flow_width + 0.03,
							intake_bucket_origin_y + 0.8 - spout_width/2,
							flow_width, 
							0.815 + bucket_length - spout_width/2);
		}
		
		//Draw float
		fillSceneRect(	g2d, 
						float_color, 
						timer_bucket_origin_x - bucket_length + bucket_length/4,
						timer_bucket_origin_y - timer_offset + 0.25, 
						bucket_length/2,
						0.25);
						
		fillSceneRect(	g2d, 
						float_color, 
						timer_bucket_origin_x - bucket_length + bucket_length*0.45,
						timer_bucket_origin_y - timer_offset + 3, 
						bucket_length*0.1,
						3);
						
		fillSceneRect(	g2d, 
						float_color, 
						timer_bucket_origin_x - bucket_length + bucket_length*0.1,
						timer_bucket_origin_y - timer_offset + 3, 
						bucket_length*0.8,
						0.05);

        // Draw Information
        g2d.setPaint(Color.BLACK);
		g2d.drawString ( String.format("Input Flow Rate:   [%.4f] m^3/s",input_flow), 20,20);
		g2d.drawString ( String.format("Intake Water Level:   [%.4f] m",intake_lvl), 20,40);
		g2d.drawString ( String.format("Intake Water Volume:   [%.4f] m^3",intake_vol), 20,60);
		g2d.drawString ( String.format("Intake Overflow Rate:   [%.4f] m^3/s",overflow_rate), 20,80);
		g2d.drawString ( String.format("Intake Spout Rate:   [%.4f] m^3/s",spout_rate), 20,100);
		g2d.drawString ( String.format("Timer Water Level:   [%.4f] m",timer_lvl), 20,120);
		g2d.drawString ( String.format("Timer Water Volume:   [%.4f] m^3",timer_vol), 20,140);
		g2d.drawString ( String.format("Current Time Tick:   [%d]",current_tick), 20,160);
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

class InputFlowCtrlPanel extends JPanel implements ChangeListener {

    static final int FLOW_MIN   =  0;
    static final int FLOW_MAX   =  10000;
    static final int FLOW_INIT  =  1000;

    private RangeView rangeView;
    private JSlider inputFlowSlider;

    public InputFlowCtrlPanel (RangeView view) {
        rangeView = view;
        setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

        JLabel sliderLabel = new JLabel("", JLabel.CENTER);
        sliderLabel.setAlignmentX(CENTER_ALIGNMENT);

        inputFlowSlider = new JSlider(JSlider.HORIZONTAL, FLOW_MIN, FLOW_MAX, FLOW_INIT);
        inputFlowSlider.addChangeListener(this);
        inputFlowSlider.setToolTipText("Input Flow Rate (mm^3/s).");
        inputFlowSlider.setMajorTickSpacing(1000);
        inputFlowSlider.setMinorTickSpacing(100);
        inputFlowSlider.setPaintTicks(true);
        inputFlowSlider.setPaintLabels(true);

        add(sliderLabel);
        add(inputFlowSlider);
    }

    @Override
    public void stateChanged(ChangeEvent e) {
        JSlider source = (JSlider)e.getSource();
        if (!source.getValueIsAdjusting()) {
            rangeView.setInputFlowSelect( source.getValue() / 1000.0);
			rangeView.new_input_flow = true;
        }
    }

    public void setValue(int value) {
        if (value > FLOW_MAX) value = FLOW_MAX;
        if (value < FLOW_MIN) value = FLOW_MIN;
        inputFlowSlider.setValue(value);
    }
}

class ControlPanel extends JPanel implements ActionListener {

    private RangeView rangeView;
    private InputFlowCtrlPanel inputFlowCtrlPanel;

    public ControlPanel(RangeView view) {

        rangeView = view;
        setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

        JPanel labeledInputFlowCtrlPanel = new JPanel();
        labeledInputFlowCtrlPanel.setLayout(new BoxLayout(labeledInputFlowCtrlPanel, BoxLayout.Y_AXIS));
        JLabel inputFlowCtrlLabel = new JLabel("Input Flow Rate (mm^3/s)");
        inputFlowCtrlLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
        labeledInputFlowCtrlPanel.add(inputFlowCtrlLabel);
        inputFlowCtrlPanel = new InputFlowCtrlPanel(rangeView);
        labeledInputFlowCtrlPanel.add( inputFlowCtrlPanel );
        add(labeledInputFlowCtrlPanel);

    }

    public void actionPerformed(ActionEvent e) {
        String s = e.getActionCommand();
        switch (s) {
            default:
                System.out.println("Unknown Action Command:" + s);
                break;
        }
    }
} // class ControlPanel

public class WaterClockDisplay extends JFrame {

    private RangeView rangeView;
    private BufferedReader in;
    private DataOutputStream out;
    private JPanel panelGroup0;
    private JPanel panelGroup1;
    private ControlPanel controlPanel;

    public WaterClockDisplay(RangeView arena) {
        setTitle("Water Clock");

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

        rangeView.setScale(64);

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
          + "usage: java jar WaterClockDisplay.jar <port-number>\n"
          + "----------------------------------------------------------------------\n"
          );
    }

    public enum ModelState { INACTIVE, READY, ACTIVE }

    public static void main(String[] args) throws IOException, InterruptedException {

        String host = "localHost";
        int port = 0;
        boolean boom = false;
		
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
        double  dt 					= 0.100; // Time between updates (seconds).
        double  spout_rate    		= 0.0;
        double  overflow_rate   	= 0.0;
        double  intake_depth   		= 0.0;
        double  timer_depth    		= 0.0;
        double  intake_lvl    		= 0.0;
        double  timer_lvl   		= 0.0;
        double  intake_vol   		= 0.0;
        double  timer_vol 			= 0.0;
        double  spout_height 		= 0.0;
        double  overflow_height 	= 0.0;
        double  input_flow 			= 0.0;
        double  flow_select 		= 0.0;
        int  tick					= 0;

        // Outbound command variables
        int simMode = 0;
        boolean standalone = false;

        int mapScale = 32 ; // pixels per meter.

        RangeView rangeView = new RangeView( mapScale);
        WaterClockDisplay waterClockDisplay = new WaterClockDisplay( rangeView);
        waterClockDisplay.setVisible(true);
        waterClockDisplay.drawRangeView();

        if (go) {
            if (port == 0) {
                System.out.println("No variable server port specified.");
                printHelpText();
                System.exit(0);
            }

            // Connect to the Trick simulation's variable server
            System.out.println("Connecting to: " + host + ":" + port);
            waterClockDisplay.connectToServer(host, port);

            waterClockDisplay.out.writeBytes("trick.var_set_client_tag(\"WaterClockDisplay\") \n");
            waterClockDisplay.out.flush();

            // Have the Variable Server send us the simulation mode ONCE.
            waterClockDisplay.out.writeBytes( "trick.var_add(\"trick_sys.sched.mode\")\n" +
                                "trick.var_send() \n" +
                                "trick.var_clear() \n");
            waterClockDisplay.out.flush();

            // Read the response and extract the simulation mode.
            try {
                String line;
                String field[];
                line = waterClockDisplay.in.readLine();
                field = line.split("\t");
                simMode = Integer.parseInt( field[1]);
            } catch (IOException | NullPointerException e ) {
                go = false;
            }

            // Configure the Variable Server to cyclically send us the following varibales.
            // Tell the variable server:
            //  1) We want the values of the following variables:
            waterClockDisplay.out.writeBytes( "trick.var_pause() \n" +
                                "trick.var_add(\"dyn.waterclock.intake_clock_spout_flowrate\")\n" +
                                "trick.var_add(\"dyn.waterclock.intake_overflow_flowrate\")\n" +
                                "trick.var_add(\"dyn.waterclock.intake_bucket_depth\")\n" +
                                "trick.var_add(\"dyn.waterclock.timer_bucket_depth\")\n" +
                                "trick.var_add(\"dyn.waterclock.intake_water_level\")\n" +
                                "trick.var_add(\"dyn.waterclock.timer_water_level\")\n" +
                                "trick.var_add(\"dyn.waterclock.intake_bucket_vol\")\n" +
                                "trick.var_add(\"dyn.waterclock.timer_bucket_vol\")\n" +
                                "trick.var_add(\"dyn.waterclock.intake_clock_spout_height\")\n" +
                                "trick.var_add(\"dyn.waterclock.intake_overflow_height\")\n" +
                                "trick.var_add(\"dyn.waterclock.input_flow\")\n" +
                                "trick.var_add(\"dyn.waterclock.current_tick\")\n" +
                                "trick.var_add(\"trick_sys.sched.mode\")\n" +
								//  2) We want the responses in ASCII:
                                "trick.var_ascii() \n" +
								//  3) We want values to be updated at the specified rate:
                                String.format("trick.var_cycle(%.3f)\n", dt) +
								//  4) Start sending values as specified.
                                "trick.var_unpause() \n" );
            waterClockDisplay.out.flush();
        } // if go

        while (go) {

            // Recieve and parse periodic data response from the variable server.
            try {
                String line;
                String field[];
                line 			= waterClockDisplay.in.readLine();
                field   		= line.split("\t");
                spout_rate    	= Double.parseDouble( field[1]);
                overflow_rate   = Double.parseDouble( field[2]);
                intake_depth    = Double.parseDouble( field[3]);
                timer_depth    	= Double.parseDouble( field[4]);
                intake_lvl    	= Double.parseDouble( field[5]);
                timer_lvl   	= Double.parseDouble( field[6]);
                intake_vol   	= Double.parseDouble( field[7]);
                timer_vol 		= Double.parseDouble( field[8]);
                spout_height 	= Double.parseDouble( field[9]);
                overflow_height = Double.parseDouble( field[10]);
                input_flow 		= Double.parseDouble( field[11]);
                tick 			= Integer.parseInt( field[12]);
                simMode    		= Integer.parseInt( field[13]);
            } catch (IOException | NullPointerException e ) {
                go = false;
            }

            // Update the display data.
			rangeView.setSpoutRate(spout_rate);
			rangeView.setOverflowRate(overflow_rate);
			rangeView.setIntakeDepth(intake_depth); 
			rangeView.setTimerDepth(timer_depth); 
			rangeView.setIntakeWarerLevel(intake_lvl);
			rangeView.setTimerWarerLevel(timer_lvl); 
			rangeView.setIntakeWaterVol(intake_vol);
			rangeView.setTimerWaterLevel(timer_vol);
			rangeView.setSpoutHeight(spout_height);
			rangeView.setOverflowHeight(overflow_height);
			rangeView.setInputFlow(input_flow);
			rangeView.setCurrentTick(tick);

			if(rangeView.new_input_flow) {
				flow_select = rangeView.getInputFlowSelect();
				waterClockDisplay.out.writeBytes( String.format("dyn.waterclock.input_flow = %f ;\n", flow_select ));
				rangeView.new_input_flow = false;
			}

            waterClockDisplay.out.flush();

            //  Update the scene.
            waterClockDisplay.drawRangeView();

        } // while
    } // main
} // class
