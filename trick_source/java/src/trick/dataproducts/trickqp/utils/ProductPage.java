
//========================================
//  Package
//========================================
package trick.dataproducts.trickqp.utils;

//========================================
//  Imports
//========================================
import java.util.ArrayList;
import java.util.List;

/**
 * Defines page data for Trick QP GUI. 
 * The page data would be mapping to the 
 * <code>page</code> element in XML file. 
 * 
 * 
 * @since Trick 10
 */
public class ProductPage extends CommonProduct {
	
	
    //========================================
    //  Public data
    //========================================
	public static final int TITLE_INDEX 	   = 0;
	public static final int START_INDEX        = 1;
	public static final int STOP_INDEX         = 2;
	public static final int FREQUENCY_INDEX    = 3;
	public static final int FOREGROUND_INDEX   = 4;
	public static final int BACKGROUND_INDEX   = 5;
	public static final int HCELLS_INDEX	   = 6;
	public static final int VCELLS_INDEX	   = 7;
	
    public static final String[] GUI_FIELD_LABELS	= new String[] {"Title", "Start", "Stop", "Frequency", "Foreground", "Background", "Horizontal Cells", "Vertical Cells"};
    
    //========================================
    //  Protected data
    //========================================
	
	
    //========================================
    //  Private Data
    //========================================

	private List<ProductPlot> plotList;
	private Integer hcells = new Integer(0); // default to 0
	private Integer vcells = new Integer(0); // default to 0
	
    //========================================
    //  Constructors
    //========================================
	/**
	 * Default constructor.
	 * 
	 */
    public ProductPage() {
    	setTitle("Page");
    }
	
    
    //========================================
    //  Set/Get methods
    //========================================
    /**
     * Sets the number shown plots horizontally.
     * @param hcellsStr number of horizontal cells
     */
    public void setHcells(String hcellsStr) {
    	try {
			hcells = new Integer(hcellsStr);
		} catch (NumberFormatException e) {
			hcells = null;
		} catch (NullPointerException e) {
			hcells = null;
		}
    }
    
    /**
     * Sets the number shown plots vertically.
     * @param vcellsStr number of vertical cells
     */
    public void setVcells(String vcellsStr) {
    	try {
			vcells = new Integer(vcellsStr);
		} catch (NumberFormatException e) {
			vcells = null;
		} catch (NullPointerException e) {
			vcells = null;
		}
    }
    
    /**
     * Gets the desired plots numbers shown horizontally.
     * @return number of horizontal cells
     */
    public Integer getHcells() {
    	return hcells;
    }
    
    /** 
     * Gets the desired plots numbers shown vertically.
     * @return number of vertical cells
     */
    public Integer getVcells() {
    	return vcells;
    }
    
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
    	if (label.equals(GUI_FIELD_LABELS[TITLE_INDEX])) {
    		type = ProductDataPanel.DataRepresentationType.PLAIN_TEXT_FIELD;
    	} else if (label.equals(GUI_FIELD_LABELS[START_INDEX]) || 
    			   label.equals(GUI_FIELD_LABELS[STOP_INDEX])  || 
    			   label.equals(GUI_FIELD_LABELS[FREQUENCY_INDEX])) {
    		type = ProductDataPanel.DataRepresentationType.NUMBER_FIELD;
    	} else if (label.equals(GUI_FIELD_LABELS[HCELLS_INDEX]) ||
 			       label.equals(GUI_FIELD_LABELS[VCELLS_INDEX])) {
    		type = ProductDataPanel.DataRepresentationType.WHOLE_NUMBER_FIELD;
    	} else {
    		type = ProductDataPanel.DataRepresentationType.COLOR_LABEL;
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
	    	case FREQUENCY_INDEX:
	    		setFrequency(valueStr);
	    		break;
	    	case FOREGROUND_INDEX:
	    		setForegroundColorCode(valueStr);
	    		break;
	    	case BACKGROUND_INDEX:
	    		setBackgroundColorCode(valueStr);
	    		break;
	    	case HCELLS_INDEX:
	    		setHcells(valueStr);
	    		break;
	    	case VCELLS_INDEX:
	    		setVcells(valueStr);
	    		break;
    	}
    }
    
    /**
     * Gets data for GUI display for the specified order number.
     * @param index index into list of objects
     * @return the object pointed to by index
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
	    	case FREQUENCY_INDEX:
	    		ret = getFrequency();
	    		break;
	    	case FOREGROUND_INDEX:
	    		ret = getForegroundColorCode();
	    		break;
	    	case BACKGROUND_INDEX:
	    		ret = getBackgroundColorCode();
	    		break;
	    	case HCELLS_INDEX:
	    		ret = getHcells();
	    		break;
	    	case VCELLS_INDEX:
	    		ret = getVcells();
	    		break;
    	}
    	return ret;
    }
    

    /**
     * Gets all of the plot data of the table.
     * 
     * @return	A list of {@link ProductPlot} of the page, 
     *          <code>null</code> if none plot is defined.
     * 
     */
    public List<ProductPlot> getPlotList() {
    	return plotList;
    }
  
    //========================================
    //  Methods
    //========================================
    /**
     * Removes a specified child from its child list.
     * 
     * @param child	The child object that needs removing.
     */
	@Override
	public void removeChild(Object child) {
    	if (plotList != null && child != null) {
    		plotList.remove(child);
    	}
    }
	
	/**
     * Removes a child at specified index from its child list.
     * 
     * @param idx	The index at which the child needs removing.
     */
    @Override
	public void removeChild(int idx) {
    	try {
    		if (plotList != null) {
        		plotList.remove(idx);
        	}
    	} catch (Exception e) {
    		// do nothing
    	}
    }
    
    /**
     * Adds a {@link ProductPlot} to the page.
     * 
     * @param plot	An instance of {@link ProductPlot} that needs adding to the page.
     */
    public void addPlot(ProductPlot plot) {
    	if (plotList == null) {
    		plotList = new ArrayList<ProductPlot>();
    	}
    	plotList.add(plot);
    }
    
    /**
     * Returns the desired text for any {@link ProductPage} instance.
     * It is for GUI displaying.
     * @return title of the page
     */
    @Override
	public String toString() {
    	if (getTitle() == null || getTitle() == "") {
    		return "Page";
    	} else {
    		return getTitle();
    	}
    }
    
    /**
     * Checks to see if the page is valid. A page is not considered to be valid if it doesn't
     * have at least one valid plot.
     * @return true or false
     */
    public boolean isValid() {
        boolean allValid = false;
        if (plotList != null && plotList.size() > 0) {
            for (ProductPlot aPlot : plotList) {
                if (aPlot.isValid()) {
                    allValid = true;
                    break;
                }
            }          
        }
        return allValid;
    }
    
}
