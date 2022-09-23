
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
 * Defines external function data for Trick QP GUI. 
 * The external function data would be mapping to the
 *  <code>extfn</code> in the XML file. 
 * 
 * @since Trick 10
 */
public class ProductExternalFunction {
    //========================================
    //  Public data
    //========================================
	// all field labels for vars in a program
    public static String[] GUI_FIELD_LABELS = new String[] {"Program Title"};
    
    
    //========================================
    //  Protected data
    //========================================
	
	
    //========================================
    //  Private Data
    //========================================
	// The name of the function
	private String name = "PROGRAM";
	// The input list of the function
	private List<ProductVar> inputs;
	// The output list of the function
	private List<ProductMeasurement> outputs;
	
	private static final int TITLE_INDEX = 0;
	
    //========================================
    //  Constructors
    //========================================
	
	/**
	 * Default constructor.
	 * 
	 */
    public ProductExternalFunction() {
    	inputs = new ArrayList<ProductVar>();
    	outputs = new ArrayList<ProductMeasurement>();
    }
	
	/**
	 * Constructor with name for the external function.
	 * 
	 * @param name	The function name.
	 *
	 */
    public ProductExternalFunction(String name) {
    	this();
    	this.name = name;
    }
    
    //========================================
    //  Set/Get methods
    //======================================== 
    /**
     * Gets the external function name.
     * 
     * @return The function name.
     */
    public String getName() {
    	return name;
    }
    
    public void setName(String name) {
    	this.name = name;
    }
   
    /**
     * Gets the list of inputs of the <code>measurement</code>.
     * 
     * @return A {@link List} of inputs of <code>measurement</code>.
     */
    public List<ProductVar> getInputs() {
    	return inputs;
    }
    
    /**
     * Gets the list of outputs of the <code>measurement</code>.
     * 
     * @return A {@link List} of outputs of <code>measurement</code>.
     */
    public List<ProductMeasurement> getOutputs() {
    	return outputs;
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
            case TITLE_INDEX:
                setName(valueStr);
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
            case TITLE_INDEX:
                ret = getName();
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
    	return ProductDataPanel.DataRepresentationType.PROGRAM_FIELD;
    }
    
    
    //========================================
    //  Methods
    //========================================
    /**
     * Adds an input to the external function input list.
     * 
     * @param input		An input for the external function.
     */
    public void addInput(ProductVar input) {
    	inputs.add(input);
    }
    
    /**
     * Adds an output to the external function output list.
     * 
     * @param output	An output for the external function.
     */
    public void addOutput(ProductMeasurement output) {
    	outputs.add(output);
    }
    
    /**
     * Removes an input from the external function input list.
     * 
     * @param input		An input for the external function that needs removing.
     */
    public void removeInput(Object input) {
    	if (inputs != null && input != null) {
    		inputs.remove(input);
    	}
    }
    
    /**
     * Removes an output from the external function output list.
     * 
     * @param output	An output for the external function that needs removing.
     */
    public void removeOutput(Object output) {
    	if (outputs != null && output != null) {
    		outputs.remove(output);
    	}
    }
    
    /**
     * Returns the desired text for any {@link ProductExternalFunction} instance.
     * It is for GUI displaying.
     */
    @Override
	public String toString() {
    	return name;
    }
    
    /**
     * Check to see if the external function is valid. An external function is not considered 
     * to be valid if it doesn't valid inputs and outputs.
     * @return true or false
     */
    public boolean isValid() {
        boolean allValid = false;
        if (inputs != null && inputs.size() > 0 && outputs != null && outputs.size() > 0) {
            boolean tempValid = false;
            for (ProductVar theInput : inputs) {
                if (theInput != null && !"".equals(theInput.getName())) {
                    tempValid = true;
                    break;
                }
            }
            if (tempValid) {
                for (ProductMeasurement theMeasurement : outputs) {
                    if (theMeasurement.isValid()) {
                        allValid = tempValid;  
                        break;
                    }
                }
            }
        }
        
        return allValid;
    }
}
