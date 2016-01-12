
//========================================
//  Package
//========================================
package trick.dataproducts.trickqp.utils;

//========================================
//Imports
//========================================

import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

import javax.swing.JOptionPane;
import javax.swing.event.TreeModelEvent;
import javax.swing.event.TreeModelListener;
import javax.swing.tree.TreePath;

import trick.common.ui.UIUtils;
import trick.common.ui.components.CommonTreeNode;
import trick.common.ui.panels.DynamicTree;
import trick.dataproducts.trickqp.TrickQPApplication;

/**
 * Defines a {@link DynamicTree} that displays data for product GUI.
 *
 * @author Hong Chen
 * @since Trick 10
 */
public class ProductTree extends DynamicTree {
	
	//========================================
	//	Public data
	//========================================
	public final static int PLOTS_POP 	= 0;
	public final static int PAGE_POP 	= 1;
	public final static int PLOT_POP 	= 2;
	public final static int CURVE_POP 	= 3;
	public final static int VARCASE_POP = 4;
	public final static int VAR_POP 	= 5;
	public final static int TABLES_POP 	= 6;
	public final static int TABLE_POP	= 7;
	public final static int COLUMN_POP	= 8;
	public final static int PROGRAMS_POP = 9;
	public final static int PROGRAM_POP = 10;
	public final static int OUTPUT_POP  = 11;
	public final static int INPUT_POP   = 12;
	
	
	//========================================
	//	Protected data
	//========================================
	
	
	//========================================
	//	Private Data
	//========================================
	private static final long serialVersionUID = 3790097973650649163L;
	
	private CommonTreeNode clickedNode = null;
	
	// whether the tree is changed
	private boolean isChanged = false;
	
	//========================================
	//	Constructors
	//========================================
	/**
	 *	Default constructor.
	 */
	public ProductTree() {	
		// This is total popup menus needed for the tree
		super(13);
		tree.addMouseListener(new TreeMouseListener());
		treeModel.addTreeModelListener(new LocalTreeModelListener());
	}
	
	//========================================
	//	Set/Get methods
	//========================================
	/**
	 * Gets the popup menu index for a specified node type. 
	 * 
	 * @param nodeType	The tree node type which we need to get popup menu index for.
	 * 
	 * @return 	The popup menu index, -1 if no popup menu is corresponded to this type of node.
	 * 
	 * @see #PLOTS_POP
	 * @see #PAGE_POP
	 * @see #PLOT_POP
	 * @see #CURVE_POP
	 * @see #VAR_POP
	 * @see #TABLES_POP
	 * @see #TABLE_POP
	 * @see #COLUMN_POP
	 */
	public static int getPopupIndexForNode(int nodeType) {
		int popIndex = -1;
		switch (nodeType) {
		case CommonTreeNode.PLOTS_NODE:
			popIndex = PLOTS_POP;
			break;
		case CommonTreeNode.PAGE_NODE:
			popIndex = PAGE_POP;
			break;
		case CommonTreeNode.PLOT_NODE:
			popIndex = PLOT_POP;
			break;
		case CommonTreeNode.CURVE_NODE:
			popIndex = CURVE_POP;
			break;
		case CommonTreeNode.VARCASE_NODE:
			popIndex = VARCASE_POP;
			break;
		case CommonTreeNode.TABLES_NODE:
			popIndex = TABLES_POP;
			break;
		case CommonTreeNode.TABLE_NODE:
			popIndex = TABLE_POP;
			break;
		case CommonTreeNode.COLUMN_NODE:
			popIndex = COLUMN_POP;
			break;
		case CommonTreeNode.PROGRAMS_NODE:
			popIndex = PROGRAMS_POP;
			break;
		case CommonTreeNode.PROGRAM_NODE:
			popIndex = PROGRAM_POP;
			break;
		case CommonTreeNode.OUTPUT_NODE:
			popIndex = OUTPUT_POP;
			break;
		case CommonTreeNode.INPUT_NODE:
			popIndex = INPUT_POP;
			break;
		case CommonTreeNode.VAR_NODE:
		case CommonTreeNode.X_NODE:
		case CommonTreeNode.Y_NODE:
		case CommonTreeNode.Z_NODE:
		case CommonTreeNode.INPUT_VAR_NODE:
		case CommonTreeNode.OUTPUT_VAR_NODE:
			popIndex = VAR_POP;
			break;
		}
    	return popIndex;
	}
	
	
	//========================================
	//	Methods
	//========================================
	/**
	 * Removes the right-clicked node.
	 */
	public void removeClickedNode() {		
		if (clickedNode != null) {
			int nodeType = clickedNode.getNodeType();
			if (nodeType == CommonTreeNode.X_NODE) {
				JOptionPane.showMessageDialog(this, 
	  					  					  "Can't delete X Var!!!", 
	  					  					  "Deleting X Var",						  	
	  					                      JOptionPane.WARNING_MESSAGE);
			} else {	
				CommonTreeNode parent = (CommonTreeNode)clickedNode.getParent();
				treeModel.removeNodeFromParent(clickedNode);
				
				switch (nodeType) {
					case CommonTreeNode.Y_NODE:
						if (parent.getNodeType() == CommonTreeNode.CURVE_NODE) {
							((ProductCurve)parent.getUserObject()).setY(null);
						} else if (parent.getNodeType() == CommonTreeNode.VARCASE_NODE) {
							((ProductVarcase)parent.getUserObject()).setY(null);
						}
						break;
					case CommonTreeNode.CURVE_NODE:	
					case CommonTreeNode.PLOT_NODE:
					case CommonTreeNode.COLUMN_NODE:
						((CommonProduct)parent.getUserObject()).removeChild(clickedNode.getUserObject());
						break;
					case CommonTreeNode.VARCASE_NODE:
						((ProductCurve)parent.getUserObject()).removeChild(clickedNode.getUserObject());
						break;
					case CommonTreeNode.VAR_NODE:
						if (parent.getNodeType() == CommonTreeNode.COLUMN_NODE) {
							((ProductColumn)parent.getUserObject()).setVar(null);
						}
						CommonTreeNode tableNode = (CommonTreeNode)parent.getParent();
						
						// after the var is removed, the column has to be removed since
						// each single column only has one var
						treeModel.removeNodeFromParent(parent);
						((CommonProduct)tableNode.getUserObject()).removeChild(parent.getUserObject());
						break;
					case CommonTreeNode.INPUT_VAR_NODE:						
						if (parent.getNodeType() == CommonTreeNode.INPUT_NODE) {							
							CommonTreeNode programNode = (CommonTreeNode)parent.getParent();												
							((ProductExternalFunction)programNode.getUserObject()).removeInput(clickedNode.getUserObject());
						}
						break;
					case CommonTreeNode.OUTPUT_VAR_NODE:
						if (parent.getNodeType() == CommonTreeNode.OUTPUT_NODE) {
							CommonTreeNode programNode = (CommonTreeNode)parent.getParent();
							((ProductExternalFunction)programNode.getUserObject()).removeOutput(clickedNode.getUserObject());
							// TODO: remove the output var from the var list ?
						}
						break;
					// TODO: more cases if necessary
				}
				
			}
		}
	}
	
	/**
	 * Checks to see if the tree gets changed.
         * @return true or false
	 */
	public boolean isChanged() {
		return isChanged;
	}
    
	/**
	 * Sets the tree changing status.
         * @param b true or false
	 */
	public void setChanged(boolean b) {
		isChanged = b;
	}
	
	//========================================
	//	Inner classes
	//========================================	
	/**
	 * Using an inner class to define MouseListener to help organize code better.
	 * The goal of this class is to handle mouse calls and forward them
	 * to the interested parties.
	 */
	private class TreeMouseListener extends MouseAdapter {
		/**
		 * Default constructor.
		 */
		public TreeMouseListener() {
		}

		//========================================
		//	MouseListener methods
		//========================================
		/**
		 * Invoked when the mouse button has been clicked (pressed
		 * and released) on a component.
		 *
		 * @param e MouseEvent sent from system.
		 */
		@Override
		public void mouseClicked(MouseEvent e) 
		{
			TreePath clickedPath = null;
			// when nothing is viewable, do nothing and return immediately 
			if (tree.getClosestPathForLocation(e.getX(), e.getY()) == null) {
				return;
			}
			clickedPath = tree.getClosestPathForLocation(e.getX(), e.getY());
			
			if (clickedPath.getLastPathComponent() instanceof CommonTreeNode) {
				clickedNode = (CommonTreeNode)clickedPath.getLastPathComponent();
			}
			
			if (clickedNode == null) {
				return;
			}
			
			// Highlight this selected path
			tree.setSelectionPath(clickedPath);
			
			int nodeType = clickedNode.getNodeType();
			int popIndex = getPopupIndexForNode(nodeType);
			TrickQPApplication.QPTabType tabType = TrickQPApplication.getTabTypeForNode(nodeType);
			
			// Determining which popup menu needs displaying
			if( UIUtils.isRightMouseClick(e) ) {
				if (popIndex>-1 && getPopup(popIndex)!= null && !getPopup(popIndex).isVisible()) {	
					getPopup(popIndex).show(e.getComponent(), e.getX(), e.getY());
				}				
			} else {
				try {
					if (tabType != null) {
						(TrickQPApplication.getInstance(TrickQPApplication.class)).setSelectedTab(tabType.ordinal(), clickedNode);
					} else {
						(TrickQPApplication.getInstance(TrickQPApplication.class)).setSelectedTab(-1);
					}
				} catch (ArrayIndexOutOfBoundsException ae) {
					// do nothing, if this happens, no tab is selected.
				}
			}
		}
	}
	
	/**
	 * Defines an inner class for implementing {@link TreeModelListener}.
	 */
	private class LocalTreeModelListener implements TreeModelListener {
		public void treeNodesChanged(TreeModelEvent e) {
			isChanged = true;
		}
		public void treeNodesInserted(TreeModelEvent e) {
			isChanged = true;
		}
		public void treeNodesRemoved(TreeModelEvent e) {
			isChanged = true;
		}
		public void treeStructureChanged(TreeModelEvent e) {
			isChanged = true;
		}
	}
	
}
