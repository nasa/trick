
//========================================
//  Package
//========================================
package trick.dataproducts.trickqp.utils;

//========================================
//  Imports
//========================================
import java.awt.Color;
import java.awt.SystemColor;
import java.util.ArrayList;
import java.util.List;

import trick.common.ui.UIUtils;

/**
 * Defines plot data for Trick QP GUI. 
 * The plot data would be mapping to the <code>plot</code> element in XML file. 
 * 
 * 
 * @author Hong Chen
 * @since Trick 10
 */
public class ProductPlot extends CommonProduct {
	
	
    //========================================
    //  Public data
    //========================================
	public static enum PlotScaleType {
		SCALAR("Scalar"),
		LOG("Log");
		
		private final String typeText;
		
		private PlotScaleType(String s) {
			typeText = s;
		}
		
		@Override
		public String toString() {
			return typeText;
		}
	}
	public static String[] GUI_FIELD_LABELS	= new String[] {"Title", "Start", "Stop", 
    	                                                    "X Axis Label", "X Min", "X Max", "X Scale",  	                            
    	                                                    "Y Axis Label", "Y Min", "Y Max", "Y Scale", "Y Axis Format",
    	                                                    "Frequency", "Foreground", "Background",
    	                                                    "Grid", "Grid Color", "Font"};
    
    
    //========================================
    //  Protected data
    //========================================
	
	
    //========================================
    //  Private Data
    //========================================
	private static final int TITLE_INDEX 	     = 0;
	private static final int START_INDEX         = 1;
	private static final int STOP_INDEX          = 2;
	private static final int X_AXIS_LABEL_INDEX  = 3;
	private static final int X_MIN_INDEX		 = 4;
	private static final int X_MAX_INDEX		 = 5;
	private static final int X_SCALE_INDEX		 = 6;
	private static final int Y_AXIS_LABEL_INDEX  = 7;
	private static final int Y_MIN_INDEX		 = 8;
	private static final int Y_MAX_INDEX		 = 9;
	private static final int Y_SCALE_INDEX       = 10;
	private static final int Y_AXIS_FORMAT_INDEX = 11;
	private static final int FREQUENCY_INDEX     = 12;
	private static final int FOREGROUND_INDEX    = 13;
	private static final int BACKGROUND_INDEX    = 14;
	private static final int GRID_INDEX			 = 15;
	private static final int GRID_COLOR_INDEX	 = 16;
	private static final int FONT_INDEX 		 = 17;
	
	
	private ProductAxis xaxis;
	private ProductAxis yaxis;
	private List<ProductCurve> curveList;
	
	private Double xmin;
	private Double xmax;
	private String xscale;
	private Double ymin;
	private Double ymax;
	private String yscale;
	private String grid = ProductDataPanel.YesNoChoice.YES.toString();
	//private String gridColor = UIUtils.colorToHTMLHex(UIManager.getColor("Label.foreground"));
	private String gridColorCode = UIUtils.colorToHTMLHex(SystemColor.controlLtHighlight.darker());
	
	private String font;

    //========================================
    //  Constructors
    //========================================
	/**
	 * Default constructor.
	 */
    public ProductPlot() {
    	setTitle("Plot");
    }
	
	/**
	 * Constructor with specified X axis for the plot.
	 * 
	 * @param xaxis	An instance of {@link ProductAxis} for the X axis of the plot.
	 *
	 */
    public ProductPlot(ProductAxis xaxis) {
    	this.xaxis = xaxis;
    }
    
    
    //========================================
    //  Set/Get methods
    //========================================
    /**
     * Gets the data field representation type {@link ProductDataPanel.DataRepresentationType}
     * for the specified label. A desired GUI component will be created based on the type.
     *
     * @param label    The label for the GUI data field.
     *
     * @return The GUI data field type in {@link ProductDataPanel.DataRepresentationType}.
     *
     */
    public static ProductDataPanel.DataRepresentationType getDataFieldType(String label) {
    	ProductDataPanel.DataRepresentationType type = ProductDataPanel.DataRepresentationType.PLAIN_TEXT_FIELD;
    	
    	if (label.equals(GUI_FIELD_LABELS[FONT_INDEX])) {
    		type = ProductDataPanel.DataRepresentationType.FONT_FIELD;
    	} else if  (label.equals(GUI_FIELD_LABELS[GRID_INDEX])) {
    		type = ProductDataPanel.DataRepresentationType.COMBO_BOX;
    		type.setComboBoxItems(ProductDataPanel.YesNoChoice.values());
    	} else if (label.equals(GUI_FIELD_LABELS[FOREGROUND_INDEX]) || 
    			   label.equals(GUI_FIELD_LABELS[BACKGROUND_INDEX]) ||
    			   label.equals(GUI_FIELD_LABELS[GRID_COLOR_INDEX])) {
    		type = ProductDataPanel.DataRepresentationType.COLOR_LABEL;
    	} else if (label.equals(GUI_FIELD_LABELS[X_SCALE_INDEX]) ||
    			   label.equals(GUI_FIELD_LABELS[Y_SCALE_INDEX])) {
    		type = ProductDataPanel.DataRepresentationType.COMBO_BOX;
    		type.setComboBoxItems(PlotScaleType.values());
    	} else if (label.equals(GUI_FIELD_LABELS[START_INDEX])     ||
    			   label.equals(GUI_FIELD_LABELS[STOP_INDEX])      ||
    			   label.equals(GUI_FIELD_LABELS[FREQUENCY_INDEX]) ||
    			   label.equals(GUI_FIELD_LABELS[X_MIN_INDEX])     ||
    			   label.equals(GUI_FIELD_LABELS[X_MAX_INDEX])     ||
    			   label.equals(GUI_FIELD_LABELS[Y_MIN_INDEX])     ||
    			   label.equals(GUI_FIELD_LABELS[Y_MAX_INDEX])
    			   ) {
    		type = ProductDataPanel.DataRepresentationType.NUMBER_FIELD;
    	} 
    	return type;
    }
    
    /**
     * Sets data attribute for the specified order number.
     * 
     * @param index		The order number of the data fields as shown in GUI.
     * @param value		The value of the GUI component represents.
     * 
     */
    public void setData(int index, Object value) {
    	// can't 100% trust str == "", so using "".equals(str) instead !!!
    	String valueStr = null;
    	if (value != null && !("".equals(value.toString().trim()))) {
    		valueStr = value.toString();
    	}
    	
    	switch (index) {
	    	case TITLE_INDEX:
	    		setTitle(valueStr);
	    		break;
	    	case START_INDEX:
	    		setTStart(valueStr);
	    		break;
	    	case STOP_INDEX:
	    		setTStop(valueStr);
	    		break;
	    	case X_AXIS_LABEL_INDEX:	    		
	    		if (xaxis == null) {	    				
	    			xaxis = new ProductAxis(valueStr);
	    			xaxis.setType(ProductAxis.X_AXIS);
	    		} else {
	    			xaxis.setLabel(valueStr);
	    		}
	    		break;
	    	case X_MIN_INDEX:
	    		setXMin(valueStr);	    		
	    		break;
	    	case X_MAX_INDEX:
	    		setXMax(valueStr);	    		
	    		break;
	    	case X_SCALE_INDEX:
	    		setXScale(valueStr);
	    		break;
	    	case Y_AXIS_LABEL_INDEX:
	    		if (yaxis == null) {
	    			yaxis = new ProductAxis(valueStr);
	    			yaxis.setType(ProductAxis.Y_AXIS);
	    		} else {
	    			yaxis.setLabel(valueStr);
	    		}
	    		break;
	    	case Y_MIN_INDEX:
	    		setYMin(valueStr);
	    		break;	    		
	    	case Y_MAX_INDEX:
	    		setYMax(valueStr);
	    		break;
	    	case Y_SCALE_INDEX:
	    		setYScale(valueStr);
	    		break;
	    	case Y_AXIS_FORMAT_INDEX:
	    		if (yaxis == null) {
	    			yaxis = new ProductAxis();
	    			yaxis.setType(ProductAxis.Y_AXIS);	    			
	    		} 
	    		yaxis.setFormat(valueStr);	    		
	    		break;
	    	case FREQUENCY_INDEX:
	    		setFrequency(valueStr);
	    		break;
	    	case FOREGROUND_INDEX:
	    		setForegroundColorCode(valueStr);
	    		break;
	    	case BACKGROUND_INDEX:
	    		setBackgroundColorCode(valueStr);
	    		break;
	    	case GRID_INDEX:
	    		setGrid(valueStr);
	    		break;
	    	case GRID_COLOR_INDEX:
	    		setGridColor(valueStr);
	    		break;
	    	case FONT_INDEX:
	    		setFont(valueStr);
	    		break;
    	}
    }
    
    /**
     * Gets data for GUI display for the specified order number.
     * @param index index into the list
     * @return object pointed to by index
     */
    public Object getData(int index) {    	
    	Object ret = null;
    	switch (index) {
	    	case TITLE_INDEX:
	    		ret = getTitle();
	    		break;
	    	case START_INDEX:
	    		ret = getTStart();
	    		break;
	    	case STOP_INDEX:
	    		ret = getTStop();
	    		break;
	    	case X_AXIS_LABEL_INDEX:
	    		if (xaxis != null) {
	    			ret = xaxis.getLabel();
	    		} 
	    		break;
	    	case X_MIN_INDEX:
	    		ret = getXMin();
	    		break;
	    	case X_MAX_INDEX:
	    		ret = getXMax();
	    		break;
	    	case X_SCALE_INDEX:
	    		ret = getXScale();
	    		break;
	    	case Y_AXIS_LABEL_INDEX:
	    		if (yaxis != null) {
	    			ret = yaxis.getLabel();
	    		} 
	    		break;
	    	case Y_MIN_INDEX:
	    		ret = getYMin();
	    		break;	    	
	    	case Y_MAX_INDEX:
	    		ret = getYMax();
	    		break;
	    	case Y_SCALE_INDEX:
	    		ret = getYScale();
	    		break;
	    	case Y_AXIS_FORMAT_INDEX:
	    		if (yaxis != null) {
	    			ret = yaxis.getFormat();
	    		}
	    		break;
	    	case FREQUENCY_INDEX:
	    		ret = getFrequency();
	    		break;
	    	case FOREGROUND_INDEX:
	    		ret = getForegroundColorCode();
	    		break;
	    	case BACKGROUND_INDEX:
	    		ret = getBackgroundColorCode();
	    		break;
	    	case GRID_INDEX:
	    		ret = getGrid();
	    		break;
	    	case GRID_COLOR_INDEX:
	    		ret = getGridColorCode();
	    		break;
	    	case FONT_INDEX:
	    		ret = getFont();
	    		break;
    	}
    	return ret;
    }
    
    /**
     * Sets the X axis for the plot.
     * 
     * @param xaxis	An instance of {@link ProductAxis} for X axis.
     */
    public void setXAxis(ProductAxis xaxis) {
    	this.xaxis = xaxis;
    }   
    
    /**
     * Sets the Y axis for the plot.
     * 
     * @param yaxis	An instance of {@link ProductAxis} for Y axis.
     */
    public void setYAxis(ProductAxis yaxis) {
    	this.yaxis = yaxis;
    }
    
    /**
     * Sets the Z axis for the plot.
     * 
     * @param zaxis	An instance of {@link ProductAxis} for Z axis.
     */
    public void setZAxis(ProductAxis zaxis) {
    }
    
    /**
     * Sets the minimum value for X variable.
     * 
     * @param xmin	The minimum value for X variable.
     */
    public void setXMin(double xmin) {
    	this.xmin = new Double(xmin);
    }    
    
    /**
     * Sets the minimum value for X variable.
     * 
     * @param xminStr	The minimum value in text for X variable.
     */
    public void setXMin(String xminStr) {
    	try {
			xmin = new Double(xminStr);
		} catch (NumberFormatException e) {
			xmin = null;
		} catch (NullPointerException e) {
			xmin = null;
		}
    }
      
    /**
     * Sets the scale option for X variable.
     * 
     * @param xscaleStr	The scale option in text for X variable.
     */
    public void setXScale(String xscaleStr) {   	
    	xscale = xscaleStr;
    }
    
    /**
     * Sets the maximum value for X variable.
     * 
     * @param xmax	The maximum value for X variable.
     */
    public void setXMax(double xmax) {
    	this.xmax = new Double(xmax);
    }    
    
    /**
     * Sets the maximum value for X variable.
     * 
     * @param xmaxStr	The maximum value in text for X variable.
     */
    public void setXMax(String xmaxStr) {
    	try {
			xmax = new Double(xmaxStr);
		} catch (NumberFormatException e) {
			xmax = null;
		} catch (NullPointerException e) {
			xmax = null;
		}
    }
    
    /**
     * Sets the minimum value for Y variable.
     * 
     * @param ymin	The minimum value for Y variable.
     */
    public void setYMin(double ymin) {
    	this.ymin = new Double(ymin);
    }    
    
    /**
     * Sets the minimum value for Y variable.
     * 
     * @param yminStr	The minimum value in text for Y variable.
     */
    public void setYMin(String yminStr) {
    	try {
			ymin = new Double(yminStr);
		} catch (NumberFormatException e) {
			ymin = null;
		} catch (NullPointerException e) {
			ymin = null;
		}
    }    
      
    /**
     * Sets the scale option for Y variable.
     * 
     * @param yscaleStr	The scale option in text for Y variable.
     */
    public void setYScale(String yscaleStr) {    	
    	yscale = yscaleStr;
    }
    /**
     * Sets the maximum value for Y variable.
     * 
     * @param ymax	The maximum value for Y variable.
     */
    public void setYMax(double ymax) {
    	this.ymax = new Double(ymax);
    }    
    
    /**
     * Sets the maximum value for Y variable.
     * 
     * @param ymaxStr	The maximum value in text for Y variable.
     */
    public void setYMax(String ymaxStr) {
    	try {
			ymax = new Double(ymaxStr);
		} catch (NumberFormatException e) {
			ymax = null;
		} catch (NullPointerException e) {
			ymax = null;
		}
    }    
    
    /**
     * Sets whether to use grid for the plot.
     * 
     * @param grid	<code>Yes</code> if the plot has grid,
     * 				<code>No</code> otherwise.
     */
    public void setGrid(String grid) {
    	this.grid = grid;
    }
    
    /**
     * Sets the grid color for the plot.
     * 
     * @param gridColor	The plot grid color.
     */
    public void setGridColor(String gridColor) {
    	this.gridColorCode = gridColor;
    }
    
    /**
     * Sets the font for the plot.
     * 
     * @param font		The plot font.
     */
    public void setFont(String font) {
    	this.font = font;
    }
    
    /**
     * Gets the X axis for the plot.
     * 
     * @return	An instance of {@link ProductAxis} for X axis.
     */
    public ProductAxis getXAxis() {
    	return xaxis;
    }
    
    /**
     * Gets the Y axis for the plot.
     * 
     * @return	An instance of {@link ProductAxis} for Y axis.
     */
    public ProductAxis getYAxis() {
    	return yaxis;
    }
    
    /**
     * Gets the Z axis for the plot.
     * 
     * @return	An instance of {@link ProductAxis} for Z axis.
     */
    public ProductAxis getZAxis() {
    	// TODO: add zaxis when 3D is needed
    	return null;
    }
    
    /**
     * Gets the minimum value for X variable.
     * 
     * @return	The minimum value object for X variable.
     */
    public Double getXMin() {
    	return xmin;
    }
    
    /**
     * Gets the scale option for X variable.
     * 
     * @return	The scale option for X variable.
     */
    public String getXScale() {
    	return xscale;
    }
    
    /**
     * Gets the maximum value for X variable.
     * 
     * @return	The maximum value object for X variable.
     */
    public Double getXMax() {
    	return xmax;
    }    
    
    /**
     * Gets the minimum value for Y variable.
     * 
     * @return	The minimum value object for Y variable.
     */
    public Double getYMin() {
    	return ymin;
    }    
    
    /**
     * Gets scale option for Y variable.
     * 
     * @return	The scale option for Y variable.
     */
    public String getYScale() {
    	return yscale;
    }
    
    /**
     * Sets the maximum value for Y variable.
     * @return Y max
     */
    public Double getYMax() {
    	return ymax;
    }    
    
    
    /**
     * Gets whether the plot uses grid.
     * 
     * @return	<code>Yes</code> if the plot has grid,
     * 			<code>No</code> otherwise.
     */
    public String getGrid() {
    	return grid;
    }
    
    /**
     * Gets the grid color for the plot.
     * 
     * @return	The plot grid color.
     */
    public String getGridColorCode() {
    	return gridColorCode;
    }
    
    /**
     * Gets the grid color for the plot.
     * 
     * @return	The plot grid {@link Color} instance.
     */
    public Color getGridColor() {
    	return UIUtils.getColorFromHTMLHex(gridColorCode);
    }
    
    /**
     * Gets the font for the plot.
     * 
     * @return	The plot font.
     */
    public String getFont() {
    	return font;
    }
    
    /**
     * Gets the list of curve data of the plot.
     * 
     * @return	A list of {@link ProductCurve}, <code>null</code> if
     * 	        none curve data is defined.
     * 
     */
    public List<ProductCurve> getCurveList() {
    	return curveList;
    }
 
    //========================================
    //  Methods
    //========================================
    /**
     * Adds one curve to the end of the list.
     * 
     * @param curve	An instance of {@link ProductCurve} 
     *              that needs adding to the list.
     */
    public void addCurve(ProductCurve curve) {
    	if (curveList == null) {
    		curveList = new ArrayList<ProductCurve>();
    	}
    	curveList.add(curve);
    }
    
    /**
     * Inserts one curve at the specified poistion in the list.
     * 
     * @param curve	An instance of {@link ProductCurve} the needs adding.
     * @param idx	The index at which the specified curve is to be inserted.
     */
    public void addCurve(ProductCurve curve, int idx) {
    	if (curveList == null) {
    		curveList = new ArrayList<ProductCurve>();
    	}
    	try {
    		curveList.add(idx, curve);
    	} catch (IndexOutOfBoundsException e) {
    		curveList.add(curve);
    	}
    }
    
    /**
     * Removes a specified child from its child list.
     * 
     * @param child	The child object that needs removing.
     */
	@Override
	public void removeChild(Object child) {
    	if (curveList != null && child != null) {
    		curveList.remove(child);
    	}
    }
    
    /**
     * Removes a child at specified location from its child list.
     * 
     * @param idx	The index at which the child needs removing.
     */
    @Override
	public void removeChild(int idx) {
    	try {
    		if (curveList != null) {
    			curveList.remove(idx);
    		}
    	} catch (Exception e) {
    		// do nothing
    	}
    }
    
    /**
     * Returns the desired text for any {@link ProductPlot} instance.
     * It is for GUI displaying.
     */
    @Override
	public String toString() {
    	if (getTitle() == null || getTitle() == "") {
    		return "Plot";
    	} else {
    		return getTitle();
    	}
    }
    
    /**
     * Check to see if the plot is valid. A plot is not considered to be valid if it doesn't 
     * have at least one valid curve.
     * @return true or false
     */
    public boolean isValid() {
        boolean allValid = false;
        if (curveList != null && curveList.size() > 0) {
            for (ProductCurve aCurve : curveList) {
                if (aCurve.isValid()) {
                    allValid = true;
                    break;
                }
            }
        }
        return allValid;
    }
    
}
