
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
 * Defines table data for Trick QP GUI. 
 * The table data would be mapping to the 
 * <code>table</code> element in XML file. 
 * 
 * 
 * @since Trick 10
 */
public class ProductTable extends CommonProduct {
	
    //========================================
    //  Public data
    //========================================
	public static String[] GUI_FIELD_LABELS	= new String[] {"Title", "Start", "Stop", "Frequency", "Foreground", "Background"};
    
    //========================================
    //  Protected data
    //========================================
	
	
    //========================================
    //  Private Data
    //========================================
	private static final int TITLE_INDEX 	    	= 0;
	private static final int START_INDEX        	= 1;
	private static final int STOP_INDEX         	= 2;
	private static final int FREQUENCY_INDEX        = 3;
	private static final int FOREGROUND_INDEX  		= 4;
	private static final int BACKGROUND_INDEX 		= 5;
	
	private List<ProductColumn> columnList;

    //========================================
    //  Constructors
    //========================================
	/**
	 * Default constructor.
	 * 
	 */
    public ProductTable() {
    	setTitle("Table");
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
    	if (label.equals(GUI_FIELD_LABELS[FOREGROUND_INDEX]) || 
        	label.equals(GUI_FIELD_LABELS[BACKGROUND_INDEX])) {
        	type = ProductDataPanel.DataRepresentationType.COLOR_LABEL;
        } else if (label.equals(GUI_FIELD_LABELS[START_INDEX]) || 
     		label.equals(GUI_FIELD_LABELS[STOP_INDEX])  || 
    		label.equals(GUI_FIELD_LABELS[FREQUENCY_INDEX])) {
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
	    	case FOREGROUND_INDEX:
	    		setForegroundColorCode(valueStr);
	    		break;
	    	case BACKGROUND_INDEX:
	    		setBackgroundColorCode(valueStr);
	    		break;
    	}
    }
    
    /**
     * Gets data for GUI display for the specified order number.
     * @param index the index into the array
     * @return the object pointed to by the index
     */
    public Object getData(int index) {    	
    	Object ret = null;
    	switch (index) {
	    	case TITLE_INDEX:    	
	    		ret = getTitle();
	    		break;
	    	case START_INDEX:
	    		break;
	    	case STOP_INDEX:
	    		break;
	    	case FOREGROUND_INDEX:
	    		ret = getForegroundColorCode();
	    		break;
	    	case BACKGROUND_INDEX:
	    		ret = getBackgroundColorCode();
	    		break;
    	}
    	return ret;
    }
    
    /**
     * Gets all of the columns of the table.
     * 
     * @return	A list of {@link ProductColumn} of the table, 
     *          <code>null</code>if none column is defined.
     * 
     */
    public List<ProductColumn> getColumnList() {
    	return columnList;
    }
  
    //========================================
    //  Methods
    //========================================
    /**
     * Adds a {@link ProductColumn} to the table.
     * 
     * @param column	An instance of {@link ProductColumn} that needs adding to the table.
     */
    public void addColumn(ProductColumn column) {
    	if (columnList == null) {
    		columnList = new ArrayList<ProductColumn>();
    	}
    	columnList.add(column);
    }
    
    /**
     * Adds a {@link ProductColumn} to the table at the specified position.
     * 
     * @param column	An instance of {@link ProductColumn} that needs adding to the table.
     * @param idx       The index at which the specified column is to be inserted.
     */
    public void addColumn(ProductColumn column, int idx) {
    	if (columnList == null) {
    		columnList = new ArrayList<ProductColumn>();
    	}
    	try {
    		columnList.add(idx, column);
    	} catch (IndexOutOfBoundsException e) {
    		columnList.add(column);
    	}   	
    }
    
    /**
     * Removes a specified child from its child list.
     * 
     * @param child	The child object that needs removing.
     */
    @Override
	public void removeChild(Object child) {
    	if (columnList != null && child != null) {
    		columnList.remove(child);
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
    		if (columnList != null) {
    			columnList.remove(idx);
        	}
    	} catch (Exception e) {
    		// do nothing
    	}
    }
    
    /**
     * Returns the desired text for any {@link ProductTable} instance.
     * It is for GUI displaying.
     */
    @Override
	public String toString() {
    	if (getTitle() == null || getTitle() == "") {
    		return "Table";
    	} else {
    		return getTitle();
    	}
    }
    
    /**
     * Check to see if the table is valid. A table is not considered to be valid if it doesn't 
     * have at least one valid column.
     * @return true or false
     */
    public boolean isValid() {
        boolean allValid = false;
        if (columnList != null && columnList.size() > 0) {
            for (ProductColumn aColumn : columnList) {
                if (aColumn.isValid()) {
                    allValid = true;
                    break;
                }
            }
        }
        return allValid;
    }
}
