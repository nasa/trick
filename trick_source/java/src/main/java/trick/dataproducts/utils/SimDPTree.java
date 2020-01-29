
package trick.dataproducts.utils;

import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.File;
import java.util.Vector;

import javax.swing.JPanel;
import javax.swing.JTree;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.TreePath;

import trick.common.TrickApplication;
import trick.common.ui.TrickFileFilter;
import trick.common.ui.UIUtils;

/**
 * Defines a {@link JPanel} that displays a file system using {@link JTree} for
 * DP files.
 *
 * @author Hong Chen
 */
public class SimDPTree extends SimRunDPTree {

  //========================================
    //  Public data
    //========================================
    
    
    //========================================
    //  Protected data
    //========================================
    
    
    //========================================
    //  Private Data
    //========================================
    
    private static final long serialVersionUID = -8556781720871570214L;


    //========================================
    //  Constructors
    //========================================
    /**
     *  Constructs a {@link SimDPTree} with specified file directory.
     * @param dir directory to find run directories in.
     * @param filter wildcard filter to match to list directories.
     */
    public SimDPTree(File dir, TrickFileFilter filter) { 
        super(dir, filter);
    }
    
    /**
     * Constructs a {@link SimDPTree} with specified file directory and number of popup menus.
     * @param dir directory to find run directories in.
     * @param filter wildcard filter to match to list directories.
     * @param popupNum not sure
     */
    public SimDPTree(File dir, TrickFileFilter filter, int popupNum) {
        super(dir, filter, popupNum);
    }

    //========================================
    //  Set/Get methods
    //========================================
    
    
    //========================================
    //  Methods
    //========================================
    /**
     * Rebuilds the tree and the implementation is required by {@link FileTreePanel}
     * 
     * @see FileTreePanel#rebuild()
     */
    @Override
    public void rebuild() {
        DefaultMutableTreeNode nextNode = new DefaultMutableTreeNode(new FileTreeNode(fileDir, true));
        rootNode.add(nextNode);        
        fileTree.revalidate();
        fileTreeModel.reload();
    }
    
    
    /**
     * Builds the tree required by {@link FileTreePanel}.
     */
    @Override
    protected void buildTree() {
        super.buildTree();
        mainPaths = new Vector<String>();
        fileTree.addMouseListener(new TreeMouseListener()); 
    }
    

    
    //========================================
    //    Inner classes
    //========================================
    /**
     * Using an inner class to define MouseListener to help organize code better.
     * The goal of this class is to handle mouse calls and forward them
     * to the interested parties.
     */
    public class TreeMouseListener extends MouseAdapter {

        //========================================
        //    MouseListener methods
        //========================================
        /**
         * Invoked when the mouse button has been clicked (pressed
         * and released) on a component.
         *
         * @param e MouseEvent sent from system.
         */
        @Override
		public void mouseClicked(MouseEvent e) {
            if( UIUtils.isRightMouseClick(e) ) {
            	TreePath rightClickedPath = fileTree.getClosestPathForLocation(e.getX(), e.getY());
            	DefaultMutableTreeNode rightClickedNode = null;
                
                if (rightClickedPath.getLastPathComponent() instanceof DefaultMutableTreeNode) {
                    rightClickedNode = (DefaultMutableTreeNode)rightClickedPath.getLastPathComponent();
                }
            	
                if (rightClickedNode != null && rightClickedTreeUserObj.isEnabled()) {    
                    int popIndex = -1;
                    
                    if (rightClickedNode.isLeaf() && getPopupNum() > 0) {
                        popIndex = getPopupNum() - 1;
                    } else {
                    	if ((rightClickedNode.getLevel() >= getPopupNum() - 1) && rightClickedTreeUserObj.getFile().isDirectory()) {
                    		popIndex = getPopupNum() -2;
                    	} else {
                    		popIndex = rightClickedNode.getLevel() - 1;
                    	}
                    }
                    
                    if (popIndex>-1 && getPopup(popIndex)!= null && !getPopup(popIndex).isVisible()) {    
                        getPopup(popIndex).show(e.getComponent(), e.getX(), e.getY());
                    }
                    TrickApplication.setPopupInvoker(getType());
                }
            } 
        }
    }

}
