
//========================================
//  Package
//========================================
package trick.dataproducts.trickqp.utils;

//========================================
//  Imports
//========================================

/**
 * Defines measurement data for Trick QP GUI.
 * The measurement data would be mapping to the
 *  <code>measurement</code> in the XML file.
 *
 * @since Trick 10
 */
public class ProductMeasurement {
    //========================================
    //  Public data
    //========================================
	
	// all field labels for vars in a measurement
    public static String[] GUI_FIELD_LABELS = new String[] {"Units"};
    
    //========================================
    //  Protected data
    //========================================


    //========================================
    //  Private Data
    //========================================
    private String name;
    private String units;

    private static final int UNITS_INDEX = 0;
    
    //========================================
    //  Constructors
    //========================================
    /**
     * Constructor with specified name for the measurement.
     *
     * @param name    The name of the measurement.
     *
     */
    public ProductMeasurement(String name) {
        this.name = name;
        this.setUnits( "--" );
    }

    //========================================
    //  Set/Get methods
    //========================================
    /**
     * Sets the measurement units.
     *
     * @param units        The variable units.
     */
    public void setUnits(String units) {
        this.units = units;
    }

    /**
     * Gets the name of the measurement.
     *
     * @return The name of the measurement.
     */
    public String getName() {
        return name;
    }

    /**
     * Gets the measurement units.
     *
     * @return The measurement units.
     */
    public String getUnits() {
        return units;
    }

    /**
     * Sets data attribute for the specified order number for GUI display.
     *
     * @param index        The order number of the data fields as shown in GUI.
     * @param value        The value of the GUI component represents.
     *
     */
    public void setData(int index, Object value) {
        // can't 100% trust str == "", so using "".equals(str) instead !!!
        String valueStr = null;
        if (value != null && !("".equals(value.toString().trim()))) {
            valueStr = value.toString();
        }

        switch (index) {            
            case UNITS_INDEX:
                setUnits(valueStr);
                break;            
        }
    }

    /**
     * Gets data for GUI display for the specified order number.
     * @param index index into the list
     * @return the object pointed to by index
     */
    public Object getData(int index) {
        Object ret = null;
        switch (index) {          
            case UNITS_INDEX:
                ret = getUnits();
                break;           
        }

        return ret;
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
    	return ProductDataPanel.DataRepresentationType.PLAIN_TEXT_FIELD;
    }
    
    
    //========================================
    //  Methods
    //========================================
    /**
     * Returns the name of this measurement. This is for GUI displaying.
     * @return the string value
     */
    @Override
	public String toString() {
    	return name;
    }
    
    /**
     * Check to see if the measurement is valid. A measurement is not considered to be valid if it doesn't 
     * have defined var.
     * @return true or false
     */
    public boolean isValid() {
        return (!"".equals(name));
    }
}
