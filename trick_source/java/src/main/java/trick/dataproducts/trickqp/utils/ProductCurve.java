
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
 * Defines curve data for Trick QP GUI. 
 * The curve data would be mapping to the
 * <code>curve</code> in the XML file. 
 * 
 * @author Hong Chen
 * @since Trick 10
 */
public class ProductCurve {
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
	 private List<ProductVarcase> varcaseList;
    
	
    //========================================
    //  Constructors
    //========================================
	/**
	 * Default constructor.
	 */
    public ProductCurve() {
    	varcaseList = new ArrayList<ProductVarcase>();
    }
    
	/**
	 * Constructor with specified X variable.
	 * 
	 * @param varX	An instance of {@link ProductVar} for X variable.
	 *
	 */
    public ProductCurve(ProductVar varX) {
    	this.varX = varX;
    }
    
    //========================================
    //  Set/Get methods
    //======================================== 
    /**
     * Sets the X variable for the curve.
     * 
     * @param varX	An instance of {@link ProductVar} for X variable.
     */
    public void setX(ProductVar varX) {
    	this.varX = varX;
    }
    
    /**
     * Sets the Y variable for the curve.
     * 
     * @param varY	An instance of {@link ProductVar} for Y variable.
     */
    public void setY(ProductVar varY) {
    	this.varY = varY;
    }
    
    /**
     * Sets the Z variable for the curve.
     * 
     * @param varZ	An instance of {@link ProductVar} for Z variable.
     */
    public void setZ(ProductVar varZ) {
    	this.varZ = varZ;
    }
    
    /**
     * Gets the X variable for the curve.
     * 
     * @return	An instance of {@link ProductVar} for X variable,
     * 			<code>null</code> if X variable is never set.
     */
    public ProductVar getX() {
    	return varX;
    }
    
    /**
     * Gets the Y variable for the curve.
     * 
     * @return	An instance of {@link ProductVar} for Y variable,
     * 			<code>null</code> if Y variable is never set.
     */
    public ProductVar getY() {
    	return varY;
    }
 
    /**
     * Gets the Z variable for the curve.
     * 
     * @return	An instance of {@link ProductVar} for Z variable,
     * 			<code>null</code> if Z variable is never set.
     */
    public ProductVar getZ() {
    	return varZ;
    }
    
    /**
     * Gets a list of varcase data.
     * 
     * @return	A list of {@link ProductVarcase}, 
     *          <code>null</code> if none varcase defined.
     */
    public List<ProductVarcase> getVarcaseList() {
    	return varcaseList;
    }
    
    /**
     * Checks to see if this {@link ProductCurve} has "varcase" or not.
     * 
     * @return <code>true</code> if it has one or more "varcase", <code>false</code> otherwise.
     */
    public boolean hasVarcase() {
    	if (varcaseList != null && varcaseList.size() > 0) {
    		return true;
    	}
    	return false;
    }
    
    //========================================
    //  Methods
    //========================================
    /**
     * Adds one varcase data to the list.
     * 
     * @param varcase	An instance of {@link ProductVarcase} 
     *                  that needs adding to the list.
     */
    public void addVarcase(ProductVarcase varcase) {
    	varcaseList.add(varcase);
    }
    
    /**
     * Removes a specified child from its child list.
     * 
     * @param child	The child object that needs removing.
     */
    public void removeChild(Object child) {
    	if (varcaseList != null && child != null) {
    		varcaseList.remove(child);
    	}
    }
    
    /**
     * Returns the desired text for any {@link ProductCurve} instance.
     * It is for GUI displaying.
     * @return "Curve"
     */
    public String toString() {
    	return "Curve";
    }
    
    /**
     * Checks to see if the curve is valid. A curve is not considered to be valid if it 
     * doesn't have either X and Y variables or varcase specs.
     * @return true or false
     */
    public boolean isValid() {
        boolean allValid = false;
        if (varX != null && varY != null) {
            allValid = true;
        } else if (varcaseList != null && varcaseList.size() > 0) {
            for (ProductVarcase aVarcase : varcaseList) {
                if (aVarcase.isValid()) {
                    allValid = true;
                    break;
                }
            }
        }
        return allValid;
    }
}
