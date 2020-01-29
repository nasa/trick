
package trick.dataproducts.utils;

//========================================
//	Imports
//========================================
import java.io.File;

/**
 * This class defines a node for {@link File} related components such as
 * tree, list and etc.
 *
 * @author Hong Chen
 * @since Trick 10
 */
public class FileTreeNode implements Comparable<FileTreeNode> {
	//========================================
	//	Public data
	//========================================
	
	
	//========================================
	//	Protected data
	//========================================
	
	
	//========================================
	//	Private Data
	//========================================
	// an instance of File for this node
	private File file;
    // true when the node is root or the first child of the root
    private boolean isSpecial;
    // true when the node is enabled, false otherwise
    private boolean isEnabled = true;
	
	//========================================
	//	Constructors
	//========================================
    /**
     * A node of the tree with specified {@link File} object. 
     * It also specifies if it is a special node.
     * 
     * @param file        The {@link File} object.
     * @param isSpecial    True if it is a special node, false otherwise.
     */
    public FileTreeNode(File file, boolean isSpecial) {
        this.file = file;
        this.isSpecial = isSpecial;
    }
	
    
	//========================================
	//	Set/Get methods
	//========================================
    /**
     * Sets the status of the node about whether it is enabled or not.
     * 
     * @param b    <code>true</code> or <code>false</code>.
     */
    public void setEnabled(boolean b) {
        isEnabled = b;
    }
	
	//========================================
	//	Methods
	//========================================
    /**
     * Returns the {@link File} object for the tree node. 
     * @return the file
     */
    public File getFile() {
        return file;
    }
    
    /**
     * Returns true if the node is special.
     * @return true or false
     */
    public boolean isSpecial() {
        return isSpecial;
    }
    
    
    /**
     * Returns the status of the node about whether it is enabled or not.
     * 
     * @return <code>true</code> if is enabled, <code>false</code> otherwise.
     */
    public boolean isEnabled() {
        return isEnabled;
    }
    
    /**
     * Converts the tree node object to a {@link String}.
     * It returns the path of the {@link File} for the node if it is the root,
     * otherwise, it returns the name of the {@link File} for the node.
     */
    public String toString() {
        if (isSpecial) {
            // If file is null, it's the root. Otherwise, it's the first child of the root.
            // For the first child of the root, we show the file path.
            if (file != null) {
                return file.getPath();
            } else {
                return "Root Node";
            }
        } else {
            // Otherwise, we only show the dir/file name.
            return file.getName();
        }
    }
    
    /**
     * Required by Comparable interface. When two File objects compare, only
     * name gets compared.
     *
     * @param   o the object to be compared.
     * @return  a negative integer, zero, or a positive integer as this object
     *            is less than, equal to, or greater than the specified object.
     */
    public int compareTo(FileTreeNode o) {
        return file.compareTo(o.getFile());
    }

    @Override
    public boolean equals(Object o) {
        if (!(o instanceof FileTreeNode)) {
            return false;
        }
        return file.equals(((FileTreeNode)o).getFile());
    }
}
