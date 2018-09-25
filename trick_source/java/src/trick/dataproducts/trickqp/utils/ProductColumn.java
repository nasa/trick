
//========================================
//  Package
//========================================
package trick.dataproducts.trickqp.utils;

//========================================
//  Imports
//========================================
import trick.common.utils.DataReader;

/**
 * Defines column data for Trick QP GUI.
 * The column data would be mapping to the <code>column</code>
 * in the XML file.
 *
 * @since Trick 10
 */
public class ProductColumn {
    //========================================
    //  Public data
    //========================================
    public static String[] GUI_FIELD_LABELS    = new String[] {"Format"};
    public DataReader dataReader;

    //========================================
    //  Protected data
    //========================================


    //========================================
    //  Private Data
    //========================================
    private static final int FORMAT_INDEX                 = 0;

    private String label;
    // The <units> element is OPTIONAL. It specifies the units
    // to which the user wants the recorded data to be converted.
    // The default value of units should be null, NOT "--".
    private String units;
    private ProductVar var;
    private String format;

    //========================================
    //  Constructors
    //========================================
    /**
     * Default constructor.
     */
    public ProductColumn() {
        this.setLabel( "Column" );
        this.setUnits(null);
        this.dataReader = null;
    }

    /**
     * Constructor with specified label and variable.
     *
     * @param var    The function name.
     *
     */
    public ProductColumn(ProductVar var) {
        this.setLabel( "Column" );
        this.setUnits(null);
        this.setVar( var );
        this.dataReader = null;
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
    	return ProductDataPanel.DataRepresentationType.PLAIN_TEXT_FIELD;
    }

    /**
     * Sets data attribute for the specified order number.
     *
     * @param index        The order number of the data fields as shown in GUI.
     * @param value        The value of the GUI component represents.
     *
     */
    public void setData(int index, Object value) {
        String valueStr = null;
        if (value != null && !("".equals(value.toString().trim()))) {
            valueStr = value.toString();
        }
        switch (index) {
            case FORMAT_INDEX:
                setFormat(valueStr);
                break;
        }
    }

    /**
     * Gets data for GUI display for the specified order number.
     * @param index index
     * @return object pointed to by index
     */
    public Object getData(int index) {
        Object ret = null;
        switch (index) {
            case FORMAT_INDEX:
                ret = getFormat();
        }
        return ret;
    }

    /**
     * Sets the var for the column.
     *
     * @param var    An instance of {@link ProductVar} for the column.
     */
    public void setVar(ProductVar var) {
        this.var = var;
    }

    /**
     * Sets the label for the column.
     *
     * @param label    The label for the column.
     */
    public void setLabel(String label) {
        this.label = label;
    }

    /**
     * Sets the format that is used for printing column data.
     * <p> e.g., "%18.6e"
     *
     * @param format The format for printing the column data..
     */
    public void setFormat(String format) {
        this.format = format;
    }

    /**
     * Sets the column units.
     *
     * @param units        The column units.
     */
    public void setUnits(String units) {
        this.units = units;
    }

    /**
     * Gets the column label.
     *
     * @return The axis label.
     */
    public String getLabel() {
        // TODO: it depends what we want to see on the table for each column
        if (var != null) {
            return var.getLabel();
        } else {
            return label;
        }
    }

    /**
     * Gets the column printing format.
     *
     * @return    The column printing format.
     */
    public String getFormat() {
        return format;
    }

    /**
     * Gets the column units.
     *
     * @return The column units.
     */
    public String getUnits() {
        return units;
    }

    /**
     * Gets the variable data for the column.
     *
     * @return An instance {@link ProductVar} for the column.
     */
    public ProductVar getVar() {
        return var;
    }

    //========================================
    //  Methods
    //========================================
    /**
     * Returns the desired text for any {@link ProductColumn} instance.
     * It is for GUI displaying.
     * @return "Column"
     */
    @Override
	public String toString() {
        return "Column";
    }
    
    /**
     * Check to see if the column is valid. A column is not considered to be valid if it doesn't 
     * have a valid var.
     * @return true or false
     */
    public boolean isValid() {
        return (var != null);
    }
}
