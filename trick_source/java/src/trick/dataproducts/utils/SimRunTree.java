
package trick.dataproducts.utils;

import java.awt.Component;
import java.io.File;

import javax.swing.JPanel;
import javax.swing.JTree;
import javax.swing.SwingWorker;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeCellRenderer;
import javax.swing.tree.TreePath;

import trick.common.ui.TrickFileFilter;
import trick.common.ui.UIUtils;

/**
 * Defines a {@link JPanel} that displays a file system using {@link JTree} for
 * SIM directory.
 *
 * @author Hong Chen
 */
public class SimRunTree extends SimRunDPTree {

  //========================================
    //  Public data
    //========================================
    
    
    //========================================
    //  Protected data
    //========================================
    
    
    //========================================
    //  Private Data
    //========================================
    
    private static final long serialVersionUID = 8233545929962887167L;
    

    //========================================
    //  Constructors
    //========================================
    /**
     *  Constructs a {@link SimRunTree} with specified file directory.
     * @param dir directory to find run directories in.
     * @param filter wildcard filter to match to list directories.
     */
    public SimRunTree(File dir, TrickFileFilter filter) { 
        super(dir, filter);
    }
    
    /**
     * Constructs a {@link SimRunTree} with specified file directory and number of popup menus.
     * @param dir directory to find run directories in.
     * @param filter wildcard filter to match to list directories.
     * @param popupNum not sure
     */
    public SimRunTree(File dir, TrickFileFilter filter, int popupNum) {
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
        addNodes(rootNode, fileFilter, fileDir);                
        fileTree.revalidate();
        fileTreeModel.reload();
    }
    
    
    /**
     * Builds the tree required by {@link FileTreePanel}.
     */
    @Override
    protected void buildTree() {
        super.buildTree();
        (new AddNodesTask()).execute();
    }
    

    
    //========================================
    //    Inner classes
    //======================================== 
    /**
     * Class that extends {@link DefaultTreeCellRenderer}.
     *
     */
    public class FileTreeCellRenderer extends DefaultTreeCellRenderer  {  
               
        private static final long serialVersionUID = -7973388358163785178L;

        /**
         * Configures the renderer based on the passed in components. 
         */
        @Override
        public Component getTreeCellRendererComponent(JTree tree, Object value, boolean sel,   
                                                     boolean expanded, boolean leaf, int row, boolean hasFocus) { 
            
            super.getTreeCellRendererComponent(tree, value, sel, expanded, leaf, row, hasFocus);  
            DefaultMutableTreeNode node = (DefaultMutableTreeNode)value;  
            FileTreeNode dir = (FileTreeNode)node.getUserObject();          
            if (!node.isRoot() && node.isLeaf() && dir != null && !dir.isSpecial() && fileFilter.getType() == TrickFileFilter.SIM_RUN) {                
                // If a leaf node of the tree does not have any files with names starting with "log",
                // disable it that turns to grey.
                if (!UIUtils.hasLogFile(dir.getFile())) {
                    this.setEnabled(false);
                    this.setDisabledIcon(this.getClosedIcon());
                    dir.setEnabled(false);                 
                }
            }           
            return this;  
        }  
    }  
    
    /**
     * Inner class for loading file system for specified directories filtered by specified file filter.
     * 
     */
    private class AddNodesTask extends SwingWorker<Void, Void> {
        
        @Override
        public Void doInBackground() {            
            addNodes(rootNode, fileFilter, fileDir);            
            return null;
        }
        
        @Override
        public void done() {
            // refreshing this node in case it is not entirely loaded for display
            if (rootNode.getChildCount() > 0 && rootNode.getFirstChild() != null) {
                refreshNode((DefaultMutableTreeNode)rootNode.getFirstChild());
            }
            fileTree.expandPath(new TreePath(rootNode.getPath()));
            fileTree.setCellRenderer(new FileTreeCellRenderer());
        }
    }
    
}
