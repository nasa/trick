package trick;

import java.awt.*; /* library for design and graphics */
import javax.swing.*; /* library for GUI window */
import java.awt.event.ActionListener; /*  */
import java.awt.event.ActionEvent;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import java.util.*;
import java.io.*;
import javax.swing.BorderFactory; /* */
import javax.swing.border.EtchedBorder;
/* 
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.FileReader;
import java.net.Socket;
*/

/* CLASS RECAP: user-defined blueprint/template that allows us to create objects */
/* class for entire GUI window */
public class JetDisplay extends JFrame implements ChangeListener {
    private RangeView rangeView;
    private ControlPanel controlPanel;
    private JPanel viewPanelGroup, controlPanelGroup;

    public JetDisplay(RangeView arena){
        setTitle("Jet Simulation");
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setSize(500,500);
        setFocusable(true);

        viewPanelGroup = new JPanel();
        viewPanelGroup.setLayout(new BoxLayout(viewPanelGroup, BoxLayout.X_AXIS));
        viewPanelGroup.add(arena);

        controlPanel = new ControlPanel(rangeView);

        controlPanelGroup = new JPanel();
        controlPanelGroup.setLayout(new BoxLayout(controlPanelGroup, BoxLayout.Y_AXIS));
        controlPanelGroup.add(viewPanelGroup);
        controlPanelGroup.add(controlPanel);

        add(controlPanelGroup);

        //rangeView.setScale(8);

    }
    /*public void createGUI(int mapScale){
        rangeView = new RangeView(mapScale);
        JPanel viewPanel = new JPanel();
        viewPanel.setLayout(new BoxLayout(viewPanel, BoxLayout.X_AXIS));
        viewPanel.add(rangeView);

        controlPanel = new ControlPanel(rangeView);
        JPanel controlPanel = new JPanel();
        controlPanel.setLayout(new BoxLayout(controlPanel, BoxLayout.Y_AXIS));
        controlPanel.add(viewPanel);
        controlPanel.add(controlPanel);
        add(controlPanel);

        setVisible(true);
    

    }*/
    public void drawRangeView(){
        rangeView.repaint();
    }


    public static void main (String[] args) throws IOException{
        System.out.println("-------------Opening Java GUI now-------------");
        /* creating display object aka GUI window */
        int mapScale = 32;
      
        RangeView rangeView = new RangeView(mapScale);
        JetDisplay jetDisplay = new JetDisplay(rangeView); 
        ControlPanel listener = new ControlPanel();
       // jetDisplay.drawRangeView();
       
        jetDisplay.setVisible(true);
       

    }
}
/* class for creating graphic objects */
class ScenePoly {  
    public Color color; 
    public int n; 
    /* arrays which hold x and y screen coordinates of objects */
    public double[] x; 
    public double[] y;
}
/* class for display */
class RangeView extends JPanel { 

    private int scale;
    private int worldOriginX;   
    private int worldOriginY;

    private double[] jetPos;
    private double[] jetVel;

    private int speed;
    private ScenePoly jet;
    public RangeView(int mapScale){
      //  setScale(mapScale);
    }
    /*  public void setScale(int mapScale){
        if (mapScale < 2) {
            scale = 2;
        } else if (mapScale > 128) {
            scale = 128;
        } else {
            scale = mapScale;
        }
        repaint();
    } */
    public int getScale(){
        return scale;
    }
    private void doDrawing(Graphics g){
        Graphics2D g2d = (Graphics2D) g;
    }

    @Override
    public void paintComponent(Graphics g){
        super.paintComponent(g);
        doDrawing(g);
    }

}
/* class for control buttons */
class ControlPanel extends JPanel implements ActionListener{
    //private RangeView display;

    private RangeView view;
    /* must implement speed, bearing, and scale */
    private JSlider speedSlider, bearingSlider, scaleSlider;
    private JLabel speedLabel, bearingLabel, scaleLabel;

    public ControlPanel (RangeView display){
        /* display = display;  why do we do this? */
        setLayout(new BoxLayout(this, BoxLayout.X_AXIS));
        setBorder( BorderFactory.createEtchedBorder(EtchedBorder.RAISED));

        


        
        speedSlider = new JSlider(JSlider.HORIZONTAL, 0, 2000, 0);
        speedSlider.setMajorTickSpacing(500);
        speedSlider.setMinorTickSpacing(100);
        speedSlider.setPaintTicks(true);
        speedSlider.setPaintTrack(true);
        speedSlider.addChangeListener(listener);
        speedLabel.setText("Current speed "+ speedSlider.getValue());
        add(speedSlider);

        //speedLabel = new JLabel("Current Value: 0");
        //speedSlider.addActionListener(this);
       // speedLabel.setText("Current value: " + speedSlider.getValue());

    }
    /* Should I use change event or action event? */
    public void stateChanged(ChangeEvent e){

    }
    public void actionPerformed(ActionEvent e){
        
    }

}
