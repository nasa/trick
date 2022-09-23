
//========================================
//	Package
//========================================
package trick.common.ui.components;

import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.TreeNode;


/**
 * This class that extends {@link DefaultMutableTreeNode}, which
 * has needed information associated with tree in Trick GUI.
 *
 * @author Hong Chen
 * @since Trick 7
 */
public class CommonTreeNode extends DefaultMutableTreeNode {
	
	
	//========================================
	//	Public data
	//========================================
	public static final int NORMAL_NODE 	= 0;
	public static final int X_NODE 			= 1;
	public static final int Y_NODE 			= 2;
	public static final int Z_NODE			= 3;
	public static final int PLOT_NODE 		= 4;
	public static final int CURVE_NODE		= 5;
	public static final int VARCASE_NODE	= 6;
	public static final int PAGE_NODE 		= 7;
	public static final int TABLE_NODE 		= 8;
	public static final int COLUMN_NODE		= 9;
	public static final int VAR_NODE		= 10;
	public static final int PROGRAM_NODE	= 11;
	public static final int INPUT_NODE		= 12;
	public static final int OUTPUT_NODE		= 13;
	public static final int PLOTS_NODE 		= 14;
	public static final int TABLES_NODE		= 15;
	public static final int PROGRAMS_NODE 	= 16;
	public static final int INPUT_VAR_NODE  = 17;
	public static final int OUTPUT_VAR_NODE = 18;
	
	
	//========================================
	//	Protected data
	//========================================
	
	
	//========================================
	//	Private Data
	//========================================
	private static final long serialVersionUID = 7211981327477779671L;
	
	// By default, this tree node is a normal node.
	private int nodeType = 0;

	//========================================
	//	Constructors
	//========================================
	/**
	 * Default constructor.
	 */
	public CommonTreeNode()
	{
		super();
	}
	
	/**
	 * Constructor that takes argument {@link Object} for the tree node.
	 * 
	 * @param obj 	An instance of {@link Object} that is used for the tree node.
	 */
	public CommonTreeNode(Object obj) {
		super(obj);
	}
	
	/**
	 * Constructor that takes argument {@link Object} for the tree node.
	 * 
	 * @param obj 	An instance of {@link Object} that is used for the tree node.
	 * @param type	The type of the node.
	 */
	public CommonTreeNode(Object obj, int type) {
		super(obj);
		nodeType = type;
	}
	
	//========================================
	//	Set/Get methods
	//========================================
	/**
	 * Sets the type for this node.
	 * 
	 * @param type	The type of the node.
	 */
	public void setNodeType(int type) {
		nodeType = type;
	}
	
	/**
	 * Gets this node type.
	 * 
	 * @return 	The type of the node.
	 */
	public int getNodeType() {
		return nodeType;
	}
	
	/**
	 * Gets the path for this tree node in String format. Each tree node in 
	 * the path is separated by ".".
	 * 
	 * @return a String that has all tree node names separated by ".".
	 * 
	 */
	public String getPathString() {
		StringBuffer buf = new StringBuffer();
		TreeNode[] path = getPath();
	
		if (path != null && path.length > 0) {
			for ( int j = 1; j < path.length; j ++ ) {
				buf.append(path[j].toString());
				if (j < path.length - 1) {
					buf.append(".");
				}
			}			
		}
		return buf.toString();
	}
	
	//========================================
	//	Methods
	//========================================
}
