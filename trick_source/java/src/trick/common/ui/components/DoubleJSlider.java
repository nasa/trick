
//========================================
//	Package
//========================================
package trick.common.ui.components;

import java.awt.Rectangle;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;

import javax.swing.JMenuItem;
import javax.swing.JPopupMenu;
import javax.swing.JSlider;
import javax.swing.border.EtchedBorder;
import javax.swing.border.TitledBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.plaf.basic.BasicSliderUI;

/**
 * An extension of JSlider that uses double for its values.
 * 
 * @since Trick 10
 */
public class DoubleJSlider extends JSlider implements ChangeListener{

	
	//========================================
	//	Public data
	//========================================
	//public final static double DOUBLE_FACTOR = 100.0;
	public static double DOUBLE_FACTOR ;
	
	//========================================
	//	Protected data
	//========================================
	
	
	//========================================
	//	Private Data
	//========================================
	private static final long serialVersionUID = 8104332457463381081L;
	
	/** This border show the current value of the slider as its title. */
	private TitledBorder border = new TitledBorder(new EtchedBorder());

	//========================================
	//	Constructors
	//========================================
	/**
	 * Default constructor - initializes with 0.0,100.0,50.0
	 */
	public DoubleJSlider(){
		super();
              DOUBLE_FACTOR = 100.0 ;
		setDoubleMinimum(0.0);
		setDoubleMaximum(100.0);
		setDoubleValue(50.0);
		addChangeListener(this);
	}
	
	/**
	 * Constructor with specified minimum, maximum and init value.
	 * 
	 * @param min	The minimum double value for the slider.
	 * @param max	The maximum double value for the slider.
	 * @param val	The init value for the slider.
	 */
    public DoubleJSlider(double min, double max, double val) {
	    super();
           DOUBLE_FACTOR = 100.0 ;
	    setUI(new DoubleJSliderUI(this));
	    setBorder(border);
	    setDoubleMinimum(min);
	    setDoubleMaximum(max);
	    setDoubleValue(val);
	    setLabel("Current Value: " + val);
	    addChangeListener(this);
    }
    
    public DoubleJSlider(double min, double max, double val, double factor ) {
	    super();
           DOUBLE_FACTOR = factor ;
	    setUI(new DoubleJSliderUI(this));
	    setBorder(border);
	    setDoubleMinimum(min);
	    setDoubleMaximum(max);
	    setDoubleValue(val);
	    setLabel("Current Value: " + val);
	    addChangeListener(this);
    }
    /**
     * Sets the title for the slider.
     * 
     * @param s		The specified title for the slider.
     */
    public void setLabel(String s) {
    	border.setTitle(s);
    }
    
	//========================================
	//	Set/Get methods
	//========================================	
    /**
     * Returns the maximum value for the slider in double.
     * 
     * @return The maximum value for the slider in double.
     */
    public double getDoubleMaximum() {
    	return( getMaximum()/DOUBLE_FACTOR );
    }

    /**
     * Returns the minimum value for the slider in double.
     * 
     * @return The minimum value for the slider in double.
     */
    public double getDoubleMinimum() {
    	return( getMinimum()/DOUBLE_FACTOR );
    }

    /**
     * Returns the current value for the slider in double.
     * 
     * @return The current value of the slider in double.
     */
    public double getDoubleValue() {
    	return( getValue()/DOUBLE_FACTOR );
    }
    
  
    /**
     * Sets maximum value for the slider in double.
     * 
     * @param max The value which needs to be set as maximum value for the slider.
     */
    public void setDoubleMaximum(double max) {
    	setMaximum((int)(max*DOUBLE_FACTOR));
    }
  
    /**
     * Sets minimum value for the slider in double.
     * 
     * @param min The value which needs to be set as minimum value for the slider. 
     */
    public void setDoubleMinimum(double min) {
    	setMinimum((int)(min*DOUBLE_FACTOR));
    }
  
    /**
     * Sets current value for the slider in double.
     * 
     * @param val The value which needs to be set as current value for the slider.
     */
    public void setDoubleValue(double val) {
    	setValue((int)(val*DOUBLE_FACTOR));
    	setToolTipText(Double.toString(val));
    }
    
    
  
	//========================================
	//	Methods
	//========================================
    /**
     * Required by ChangeListener.
     */
    public void stateChanged(ChangeEvent e) {
    	DoubleJSlider slider = (DoubleJSlider)e.getSource();
    	if (!slider.getValueIsAdjusting()) {
    		setToolTipText(Double.toString(slider.getDoubleValue()));   		
    		slider.setLabel("Current Value: "+slider.getDoubleValue());
    	}
    }
    
    
    //========================================
	//	Inner classes
	//========================================
    /**
     * The class for customized slider UI.
     */
    class DoubleJSliderUI extends BasicSliderUI implements MouseMotionListener, MouseListener {
        final JPopupMenu pop = new JPopupMenu();
        JMenuItem item = new JMenuItem();
       
        DoubleJSlider slider;
        
        /**
         * Constructor with specifed slider.
         * 
         * @param slider	The specified {@link DoubleJSlider}.
         */
        public DoubleJSliderUI(DoubleJSlider slider)
        {
            super(slider);
            slider.addMouseMotionListener(this);
            slider.addMouseListener(this);
            this.slider = slider;
            pop.add(item);           
            pop.setDoubleBuffered(true);
        }
    
        /**
         * Shows the tip text while mouse moving.
         * 
         * @param me	The mouse event of slider moving.
         */
        public void showToolTip(MouseEvent me) {
        	item.setText(""+slider.getDoubleValue());
            
            //limit the tooltip location relative to the slider
            Rectangle b = me.getComponent().getBounds();
            int x = me.getX();      
            x = (x > (b.x + b.width / 2) ? (b.x + b.width / 2) : (x < (b.x - b.width / 2) ? (b.x - b.width / 2) : x));
            
            pop.show( me.getComponent(), x - 5, -5 );
            
            item.setArmed( false );
        }

        /**
         * Required by {@link MouseMotionListener}.
         */
        public void mouseDragged(MouseEvent me) {       	
        	showToolTip(me);      	
        }
        /**
         * Required by {@link MouseMotionListener}.
         */
        public void mouseMoved(MouseEvent me) {       	
        }
        
        /**
         * Required by {@MouseListener}.
         */
        public void mousePressed(MouseEvent me) {       
        	showToolTip(me);
        }
        /**
         * Required by {@MouseListener}.
         */
        public void mouseClicked(MouseEvent me) {       	
        }
        /**
         * Required by {@MouseListener}.
         */
        public void mouseReleased(MouseEvent me) {
        	pop.setVisible(false);
        }
        /**
         * Required by {@MouseListener}.
         */
        public void mouseEntered(MouseEvent me) {	
        }
        /**
         * Required by {@MouseListener}.
         */
        public void mouseExited(MouseEvent me) {       	
        }
    } 
    
}
