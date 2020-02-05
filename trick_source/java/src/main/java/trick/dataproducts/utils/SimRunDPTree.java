
//========================================
//  Package
//========================================
package trick.dataproducts.utils;

//========================================
//Imports
//========================================
import java.io.File;
import java.util.Vector;

import javax.swing.JComponent;
import javax.swing.JPanel;
import javax.swing.JTree;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;

import org.jdesktop.swingx.JXTree;

import trick.common.ui.TrickFileFilter;


/**
 * Defines a {@link JPanel} that displays a file system using {@link JTree} for
 * either SIM directory or DP files.
 *
 * @since Trick 10
 */
public abstract class SimRunDPTree extends FileTreePanel{
	
	//========================================
	//	Public data
	//========================================
	
	
	//========================================
	//	Protected data
	//========================================
	
	
	//========================================
	//	Private Data
	//========================================
	
    private static final long serialVersionUID = 1286524498651614284L;
    

    //========================================
	//	Constructors
	//========================================
	/**
	 *	Constructs a {@link SimRunDPTree} with specified file directory.
         * @param dir directory to find run directories in.
         * @param filter wildcard filter to match to list directories.
	 */
	public SimRunDPTree(File dir, TrickFileFilter filter) {	
		super(dir, filter);
	}
	
	/**
	 * Constructs a {@link SimRunDPTree} with specified file directory and number of popup menus.
         * @param dir directory to find run directories in.
         * @param filter wildcard filter to match to list directories.
         * @param popupNum not sure
	 */
	public SimRunDPTree(File dir, TrickFileFilter filter, int popupNum) {
		super(dir, filter, popupNum);
	}

	//========================================
	//	Set/Get methods
	//========================================
	
	
	//========================================
	//	Methods
	//========================================
	
	
	
	/**
	 * Builds the tree required by {@link FileTreePanel}.
	 */
	@Override
	protected void buildTree() {
		if (observerList == null) {
			observerList = new Vector<JComponent>();
		} 
		
		// Make an invisible tree root
        rootNode = new DefaultMutableTreeNode(new FileTreeNode(null, true));
        
        fileTreeModel = new DefaultTreeModel(rootNode);
        fileTree = new JXTree(fileTreeModel);
        
        fileTree.setRootVisible(false);
        
        // Add listeners
        fileTree.addTreeSelectionListener(this);
        fileTree.addTreeExpansionListener(this);
        fileTree.addMouseListener(new TreeMouseListener()); 		
	}
	

	
    //========================================
    //    Inner classes
    //======================================== 
	
    
}
