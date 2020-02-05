
//========================================
//  Package
//========================================
package trick.common.ui.panels;

//========================================
//Imports
//========================================

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.util.ArrayList;
import java.util.Enumeration;

import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTree;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeCellRenderer;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreePath;

import org.jdesktop.swingx.JXTree;
import org.jdesktop.swingx.decorator.ColorHighlighter;
import org.jdesktop.swingx.decorator.HighlightPredicate;

import trick.common.ui.UIUtils;
import trick.common.ui.components.CommonTreeNode;


/**
 * A {@link JPanel} that displays a dynamic {@link JTree}.
 *
 * @author Hong Chen
 * @since Trick 10
 */
public class DynamicTree extends DataPanel {

	//========================================
	//	Public data
	//========================================
	
	
	//========================================
	//	Protected data
	//========================================
	/** An instance of JXTree. */
	protected JXTree tree;
	
	/** The tree model for a JTree. */
	protected DefaultTreeModel treeModel;
	
	/** The root for a JTree. */
	protected DefaultMutableTreeNode rootNode;
	
	
	//========================================
	//	Private Data
	//========================================
	private static final long serialVersionUID = 7238523759496892652L;
	
	//========================================
	//	Constructors
	//========================================
	/**
	 *	Default constructor.
	 */
	public DynamicTree() {	
		super();
		rootNode = new DefaultMutableTreeNode("Root Node");
        treeModel = new DefaultTreeModel(rootNode);    
        tree = new JXTree(treeModel);
        tree.setRolloverEnabled(true);
		tree.addHighlighter(new ColorHighlighter(HighlightPredicate.ROLLOVER_ROW, null, Color.MAGENTA));
        tree.putClientProperty("JTree.lineStyle", "Angled");
        //tree.setEditable(true);
        //tree.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);
        tree.setShowsRootHandles(true);
        tree.setRootVisible(false); 
        tree.putClientProperty("JTree.lineStyle", "Horizontal");
        DynamicTreeCellRenderer renderer = new DynamicTreeCellRenderer();
    
        tree.setCellRenderer(renderer);
        
        setup();
	}
	
	/**
	 * Constructor with specified the number of pop up menu.
	 * 
	 * @param popNum	The number of pop up menu.
	 */
	public DynamicTree(int popNum) {
		super(popNum);
		rootNode = new DefaultMutableTreeNode("Root Node");
        treeModel = new DefaultTreeModel(rootNode);    
        tree = new JXTree(treeModel);
        tree.setRolloverEnabled(true);
		tree.addHighlighter(new ColorHighlighter(HighlightPredicate.ROLLOVER_ROW, null, Color.MAGENTA));
        tree.putClientProperty("JTree.lineStyle", "Angled");
        //tree.setEditable(true);
        //tree.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);
        tree.setShowsRootHandles(true);
        tree.setRootVisible(false); 
        tree.putClientProperty("JTree.lineStyle", "Horizontal");
        DynamicTreeCellRenderer renderer = new DynamicTreeCellRenderer();
    
        tree.setCellRenderer(renderer);
        
        setup();
	}
	
	//========================================
	//	Set/Get methods
	//========================================
	/**
	 * Gets the instance of the {@link JXTree}.
	 * 
	 * @return The instance of the tree.
	 */
	public JXTree getTree() {
		return tree;
	}
	
	/**
	 * Gets the root of the tree.
	 * 
	 * @return	The root {@link DefaultMutableTreeNode} of the tree.
	 */
	public DefaultMutableTreeNode getRoot() {
		return rootNode;
	}
	
	/**
	 * Gets the default tree model for the tree.
	 * 
	 * @return The instance of {@link DefaultTreeModel} for the tree.
	 */
	public DefaultTreeModel getDefaultModel() {
		return treeModel;
	}
	

	//========================================
	//	Methods
	//========================================
	/**
	 * Helper method.
	 */
	private void setup() {
		setLayout(new BorderLayout());
		JScrollPane scrollpane = new JScrollPane();
	    scrollpane.getViewport().add(tree);
	    
	    add(BorderLayout.CENTER, scrollpane);
	}
	
	/**
	 * Removes all nodes except the root node.
	 */
	public void clear() {
		rootNode.removeAllChildren();
		treeModel.reload();
	}
	
	
	/**
	 * Adds a child node to a parent node.
	 * 
	 * @param parent	The parent {@link DefaultMutableTreeNode}.
	 * @param child		The child object.
	 * @return the top of the tree
	 */
	public DefaultMutableTreeNode addObject(DefaultMutableTreeNode parent, Object child) {
		return addObject(parent, child, false);
	}

	/**
	 * Adds a child node object to a parent node by specifying if it should be visible.
	 * 
	 * @param parent			The parent {@link DefaultMutableTreeNode}.
	 * @param child				The child object.
	 * @param shouldBeVisible	<code>true</code> the node should be visible, <code>false</code> otherwise.
	 * @return the top of the tree
	 */
	public DefaultMutableTreeNode addObject(DefaultMutableTreeNode parent, Object child, boolean shouldBeVisible) {
		DefaultMutableTreeNode childNode = new DefaultMutableTreeNode(child);
		
		if (parent == null) {
			parent = rootNode;
		}
		
		//It is key to invoke this on the TreeModel, and NOT DefaultMutableTreeNode
		treeModel.insertNodeInto(childNode, parent, parent.getChildCount());
		
		//Make sure the user can see the lovely new node.
		if (shouldBeVisible) {
			tree.scrollPathToVisible(new TreePath(childNode.getPath()));
		}
		return childNode;
	}

	/**
	 * Adds a child node to a parent by specifying if it should be visible.
	 * 
	 * @param parent			The parent {@link DefaultMutableTreeNode}.
	 * @param child				The child {@link DefaultMutableTreeNode}.
	 * @param shouldBeVisible	<code>true</code> the node should be visible, <code>false</code> otherwise.
	 */
	public void addNode(DefaultMutableTreeNode parent, DefaultMutableTreeNode child, boolean shouldBeVisible) {
		if (parent == null) {
			parent = rootNode;
		}
		addNode(parent, child, parent.getChildCount(), shouldBeVisible);
	}
 
	/**
	 * Adds a child node to a parent by specifying if it should be visible and its index location.
	 * 
	 * @param parent			The parent {@link DefaultMutableTreeNode}.
	 * @param child				The child {@link DefaultMutableTreeNode}.
	 * @param index				The child location index in parent children nodes.
	 * @param shouldBeVisible	<code>true</code> the node should be visible, <code>false</code> otherwise.
	 * 
	 */
	public void addNode(DefaultMutableTreeNode parent, DefaultMutableTreeNode child, int index, boolean shouldBeVisible) {
		
		
		if (index < 0 || index > parent.getChildCount()) {
			//It is key to invoke this on the TreeModel, and NOT DefaultMutableTreeNode
			treeModel.insertNodeInto(child, parent, parent.getChildCount());
		} else {
			treeModel.insertNodeInto(child, parent, index);
		}
		
		//Make sure the user can see the lovely new node.
		if (shouldBeVisible) {
			tree.scrollPathToVisible(new TreePath(child.getPath()));
		}
	}
	
	/** 
	 * Gets all child node of a specified node.
	 * 
	 * @param parent	A parent {@link DefaultMutableTreeNode}.
	 * 
	 * @return A list of {@link DefaultMutableTreeNode} of the specified parent node.
	 */
	public ArrayList<DefaultMutableTreeNode> getAllChildren(DefaultMutableTreeNode parent) {
		ArrayList<DefaultMutableTreeNode> childNodes = null;
		int count = parent.getChildCount();
		if (count > 0) {
			childNodes = new ArrayList<DefaultMutableTreeNode>();		
			for (int i = 0; i < count; i++) {
				DefaultMutableTreeNode eachChild = (DefaultMutableTreeNode)treeModel.getChild(parent, i);
				childNodes.add(eachChild);
			}
		}
		return childNodes;
	}
	
	/**
	 * Gets all selected nodes. 
	 * @return list of selected nodes.
	 */
	public ArrayList<DefaultMutableTreeNode> getSelectedNodes() {
		ArrayList<DefaultMutableTreeNode> selectedNodes = null;
		TreePath[] selectedPaths = tree.getSelectionPaths();
		
		if (selectedPaths != null && selectedPaths.length > 0) {
			selectedNodes = new ArrayList<DefaultMutableTreeNode>();
		
			for (int i = 0; i < selectedPaths.length; i ++) {
				if (selectedPaths[i].getLastPathComponent() instanceof DefaultMutableTreeNode) {
					DefaultMutableTreeNode treeNode = (DefaultMutableTreeNode)selectedPaths[i].getLastPathComponent();
					selectedNodes.add(treeNode);
				}
			}
		}		
		return selectedNodes;
	}
	
	/**
	 * Removes all selected tree nodes.
	 */
	@Override
	public void removeSelectedData() {
		// TODO:
	}
	
	/**
	 * Expands the specified tree node.
	 * 
	 * @param node	A {@link DefaultMutableTreeNode} that needs expanding.
	 */
	@SuppressWarnings("rawtypes")
	public void expandNode(DefaultMutableTreeNode node) {
		Enumeration children = node.depthFirstEnumeration();
		while (children.hasMoreElements()) {			
			DefaultMutableTreeNode child = (DefaultMutableTreeNode)children.nextElement();
		    if (child.isLeaf()) {
		    	tree.scrollPathToVisible(new TreePath(child.getPath()));
		    }
		}
	}
	
	
	//========================================
	//	Inner classes
	//========================================
	/**
	 * Class that extends {@link DefaultTreeCellRenderer}.
	 *
	 */
	public class DynamicTreeCellRenderer extends DefaultTreeCellRenderer  {  
		
		private static final long serialVersionUID = 6474954896113266472L;

		/**
		 * Configures the renderer based on the passed in components. 
		 */
		@Override
		public Component getTreeCellRendererComponent(JTree tree, Object value, boolean sel,   
	                                                 boolean expanded, boolean leaf, int row, boolean hasFocus) { 
			
			super.getTreeCellRendererComponent(tree, value, sel, expanded, leaf, row, hasFocus);  
			
			if (value instanceof CommonTreeNode) {
				CommonTreeNode node = (CommonTreeNode)value;
				switch (node.getNodeType()) {
					case CommonTreeNode.PAGE_NODE:
						setIcon(UIUtils.createImageIcon("page2.gif"));
						break;
					case CommonTreeNode.PLOT_NODE:
					case CommonTreeNode.CURVE_NODE:
					case CommonTreeNode.VARCASE_NODE:
						setIcon(UIUtils.createImageIcon("plot.gif"));
						break;
					case CommonTreeNode.X_NODE:
						setIcon(UIUtils.createImageIcon("x_variable.gif"));
						break;
					case CommonTreeNode.Y_NODE:
						setIcon(UIUtils.createImageIcon("y_variable.gif"));
						break;
					case CommonTreeNode.VAR_NODE:
					case CommonTreeNode.INPUT_VAR_NODE:
					case CommonTreeNode.OUTPUT_VAR_NODE:
						setIcon(UIUtils.createImageIcon("variable.gif"));
						break;
					case CommonTreeNode.TABLE_NODE:
						setIcon(UIUtils.createImageIcon("table_small.gif"));
						break;
					case CommonTreeNode.PROGRAM_NODE:
						setIcon(UIUtils.createImageIcon("program.gif"));
						break;
					case CommonTreeNode.INPUT_NODE:
						setIcon(UIUtils.createImageIcon("program_in.gif"));
						break;
					case CommonTreeNode.OUTPUT_NODE:
						setIcon(UIUtils.createImageIcon("program_out.gif"));
						break;
				}
			}
			return this;  
		}  
	}
	
}
