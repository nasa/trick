
//========================================
//  Package
//========================================
package trick.dataproducts.trickqp.utils;

//========================================
//  Imports
//========================================

/**
 * Defines axis (X, Y, or Z) data for Trick QP GUI.
 * The axis data would be mapping to the
 * <code>xaxis</code>, <code>yaxis</code>, or <code>zaxis</code>
 * in the XML file.
 *
 * @since Trick 10
 */
public class ProductAxis {
    //========================================
    //  Public data
    //========================================
    public static final int X_AXIS = 1;
    public static final int Y_AXIS = 2;
    public static final int Z_AXIS = 3;

    //========================================
    //  Protected data
    //========================================


    //========================================
    //  Private Data
    //========================================
    // The label of the axis
    private String label;
    
    // The type of the axis (X, Y, or Z)
    private int type;

    // The format of the axix, currently, only y axis has this attribute
    private String format;
    
    //========================================
    //  Constructors
    //========================================
    /**
     * Default constructor.
     */
    public ProductAxis() {

    }

    /**
     * Constructor with variable name for the measurement.
     *
     * @param label    The function name.
     *
     */
    public ProductAxis(String label) {
        this.setLabel( label );
    }

    //========================================
    //  Set/Get methods
    //========================================
    /**
     * Sets the label of axis.
     *
     * @param label    The axis label.
     */
    public void setLabel(String label) {
        this.label = label;
    }

    /**
     * Sets type of axis.
     *
     * @param type    The type of axis.
     */
    public void setType(int type) {
        this.type = type;
    }

    /**
     * Gets the type of axis.
     *
     * @return    The type of the axis.
     */
    public int getType() {
        return type;
    }

    /**
     * Gets the axis label.
     *
     * @return The axis label.
     */
    public String getLabel() {
        return label;
    }
    
    /**
     * Sets the format of axis.
     * 
     * @param format	The axis print format.
     */
    public void setFormat(String format) {
    	this.format = format;
    }

    /**
     * Gets the print format of axis.
     * 
     * @return	The print format of this axis.
     */
    public String getFormat() {
    	return format;
    }
    //========================================
    //  Methods
    //========================================
    /**
     * Checks to see if this axis is empty. 
     * Meaning it doesn't have any attributes or elements XML wise.
     * @return true or false
     */
    public boolean isEmptyNode() {
    	if ((label == null || label.isEmpty()) && (format == null || format.isEmpty())) {
    		return true;
    	}
    	return false;
    }
}
