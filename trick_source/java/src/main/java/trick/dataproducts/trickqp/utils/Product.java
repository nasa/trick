
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
 * Defines Product data for Trick QP GUI. 
 * The Product data would be mapping to the 
 * <code>product</code> element in XML file. 
 * 
 * @since Trick 10
 */
public class Product extends CommonProduct {
    //========================================
    //  Public data
    //========================================
    
    
    //========================================
    //  Protected data
    //========================================
	
	
    //========================================
    //  Private Data
    //========================================
	private String version;
	
	private List<ProductExternalFunction> functionList;
	private List<ProductPage> pageList;
	private List<ProductTable> tableList;

	
    //========================================
    //  Constructors
    //========================================
	/**
	 * Constructor with specified version for the data product.
	 * 
	 * @param version		The product version.
	 *
	 */
    public Product(String version) {
    	this.version = version;
    }
    
    //========================================
    //  Set/Get methods
    //======================================== 
    /**
     * Gets the list external function data.
     * 
     * @return A list of {@link ProductExternalFunction},
     *         <code>null</code> if none external function is defined.
     */
    public List<ProductExternalFunction> getFunctionList() {
    	return functionList;
    }
    
    /**
     * Gets the list of page data.
     * 
     * @return A list of {@link ProductPage}, <code>null</code> if
     *         none page data is defined.
     *   
     */
    public List<ProductPage> getPageList() {
    	return pageList;
    }
    
    /**
     * Sets the product version.
     * 
     * @param version	The specified version.
     */
    public void setVersion(String version) {
    	this.version = version;
    }
    
    /** 
     * Gets the product version.
     * 
     * @return The product version.
     */
    public String getVersion() {
    	return version;
    }
    
    /**
     * Gets the list of table data.
     * 
     * @return A list of {@link ProductTable}, <code>null</code> if
     *         none table data is defined.
     */
    public List<ProductTable> getTableList() {
    	return tableList;
    }
    
    //========================================
    //  Methods
    //========================================
    /**
     * Adds a {@link ProductExternalFunction} to the product.
     * 
     * @param function	An instance of {@link ProductExternalFunction}.
     */
    public void addExternalFunction(ProductExternalFunction function) {
    	if (functionList == null) {
    		functionList = new ArrayList<ProductExternalFunction>();
    	}
    	functionList.add(function);
    }
    
    /**
     * Adds a {@link ProductPage} to the product.
     * 
     * @param page	An instance of {@link ProductPage}.
     */
    public void addPage(ProductPage page) {
    	if (pageList == null) {
    		pageList = new ArrayList<ProductPage>();
    	}
    	pageList.add(page);
    }
    
    /**
     * Adds a {@link ProductTable} to the product.
     * 
     * @param table	An instance of {@link ProductTable}.
     */
    public void addTable(ProductTable table) {
    	if (tableList == null) {
    		tableList = new ArrayList<ProductTable>();
    	}
    	tableList.add(table);
    }
    
    /**
     * Removes a specified child from its child list.
     * 
     * @param child	The child object that needs removing.
     */
    public void removeChild(Object child) {
    	if (child != null) {
    		if (child instanceof ProductPage && pageList != null) {
    			pageList.remove(child);
    		} else if (child instanceof ProductTable && tableList != null) {
    			tableList.remove(child);
    		} else if (child instanceof ProductExternalFunction && functionList != null) {
    			functionList.remove(child);
    		}
    	}
    }
    
    /**
     * Removes a child at specified index from its child list.
     * 
     * @param idx	The index at which the child needs removing.
     */
    public void removeChild(int idx) {
    	// do nothing as there are different lists in this class
    }
    
    /**
     * Checks to see if the product is valid. A product node is considered to be valid if
     * it has at least one valid page or table.
     * @return true or false
     */
    public boolean isValid() {
        boolean allValid = false;
        if (pageList != null && pageList.size() > 0) {
            for (ProductPage aPage : pageList) {
                if (aPage.isValid()) {
                    allValid = true;
                    return allValid;
                }
            }
        }
        if (tableList != null && tableList.size() > 0) {
            for (ProductTable aTable : tableList) {
                if (aTable.isValid()) {
                    allValid = true;
                    break;
                }
            }
        }
        return allValid;
    }
}
