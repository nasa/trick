
//========================================
//  Package
//========================================
package trick.dataproducts.trickqp.utils;

//========================================
//  Imports
//========================================

/**
 * Defines varcase data for Trick QP GUI. 
 * The varcase data would be mapping to the
 * <code>varcase</code> in the XML file. 
 * 
 * @author Hong Chen
 * @since Trick 10
 */
public class ProductVarcase {
    //========================================
    //  Public data
    //========================================
    
    
    //========================================
    //  Protected data
    //========================================
	
	
    //========================================
    //  Private Data
    //========================================
    private ProductVar varX;
    private ProductVar varY;
    private ProductVar varZ;
    
	
    //========================================
    //  Constructors
    //========================================
    /**
     * Default constructor.
     */
    public ProductVarcase(){
    }	
   
    /**
	 * Constructor with specified X variable.
	 * 
	 * @param varX	An instance of {@link ProductVar} for X variable.
	 *
	 */
    public ProductVarcase(ProductVar varX) {
    	this.varX = varX;
    }
    
    //========================================
    //  Set/Get methods
    //======================================== 
    /**
     * Sets the X variable for the varcase.
     * 
     * @param varX	An instance of {@link ProductVar} for X variable.
     */
    public void setX(ProductVar varX) {
    	this.varX = varX;
    }
    
    /**
     * Sets the Y variable for the varcase.
     * 
     * @param varY	An instance of {@link ProductVar} for Y variable.
     */
    public void setY(ProductVar varY) {
    	this.varY = varY;
    }
    
    /**
     * Sets the Z variable for the varcase.
     * 
     * @param varZ	An instance of {@link ProductVar} for Z variable.
     */
    public void setZ(ProductVar varZ) {
    	this.varZ = varZ;
    }
    
    /**
     * Gets the X variable for the varcase.
     * 
     * @return	An instance of {@link ProductVar} for X variable.
     */
    public ProductVar getX() {
    	return varX;
    }
    
    /**
     * Gets the Y variable for the varcase.
     * 
     * @return	An instance of {@link ProductVar} for Y variable,
     * 			<code>null</code> if Y variable is never set.
     */
    public ProductVar getY() {
    	return varY;
    }
 
    /**
     * Gets the Z variable for the varcase.
     * 
     * @return	An instance of {@link ProductVar} for Z variable,
     * 			<code>null</code> if Z variable is never set.
     */
    public ProductVar getZ() {
    	return varZ;
    }
    
    //========================================
    //  Methods
    //========================================
    /**
     * Returns the desired text for any {@link ProductVarcase} instance.
     * It is for GUI displaying.
     * @return "Varcase"
     */
    public String toString() {
    	return "Varcase";
    }
    
    /**
     * Check to see if the varcase is valid. A varcase is not considered to be valid if it doesn't 
     * have at least varX and varY both defined.
     * @return true or false
     */
    public boolean isValid() {
        return (varX != null && varY != null);
    }
}
