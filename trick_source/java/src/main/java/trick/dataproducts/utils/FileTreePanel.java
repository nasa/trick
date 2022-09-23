
//========================================
//  Package
//========================================
package trick.dataproducts.utils;

//========================================
//Imports
//========================================

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.File;
import java.util.Collections;
import java.util.Enumeration;
import java.util.Vector;

import javax.swing.JComponent;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTree;
import javax.swing.event.TreeExpansionEvent;
import javax.swing.event.TreeExpansionListener;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreeNode;
import javax.swing.tree.TreePath;

import org.jdesktop.swingx.JXTree;
import org.jdesktop.swingx.decorator.ColorHighlighter;
import org.jdesktop.swingx.decorator.HighlightPredicate;

import trick.common.TrickApplication;
import trick.common.ui.TrickFileFilter;
import trick.common.ui.UIUtils;
import trick.common.ui.panels.DataPanel;
import trick.common.ui.panels.ListPanel;

/**
 * A {@link JPanel} that displays a file system using {@link JTree}.
 *
 * @since Trick 10
 */
public abstract class FileTreePanel extends DataPanel implements TreeSelectionListener, TreeExpansionListener{
    
    //========================================
    //    Public data
    //========================================

		

    //========================================
    //    Protected data
    //========================================
    /** The file filter for showing specific dirs/files. */
    protected TrickFileFilter fileFilter;
    
    /** The file dirs shown at beginning. */
    protected File fileDir;
    
    protected Vector<File> fileDirs;

    /** An instance of JXTree. */
    protected JXTree fileTree;
    
    /** The tree model for a JTree. */
    protected DefaultTreeModel fileTreeModel;
    
    /** The root for a JTree. */
    protected DefaultMutableTreeNode rootNode;
    
    /** A list of observers that are interested in changes of the FileTree. */
    protected Vector<JComponent> observerList = null;
    
    /**
     * All paths in this this is for keeping uniqueness of the tree. 
     */
    protected Vector<String> mainPaths = null;
    
    protected JScrollPane treePane;
    
    //========================================
    //    Private Data
    //========================================
    
    private static final long serialVersionUID = 9200081096110536799L;
    
    /** All selected items for the JTree. */
    private Vector<String> selectedItems;
    
    /** Whether or not add the selected items to the observer. */
    private boolean needAddToObserver = true;
    
    /** The last dir for the displayed path if necessary. */ 
    private String finalPath;
    
    /** Right clicked tree node. */
    protected FileTreeNode rightClickedTreeUserObj;
    
    final private static int EXPAND_NODE = 1;
    final private static int COLLAPSE_NODE = 2;
    final private static int REMOVE_NODE = 3;
    
    //========================================
    //    Constructors
    //========================================
    /**
     *    Constructs a {@link FileTreePanel} with specified file directory.
     * @param dir directory to list
     * @param filter the filter to use for displaying files
     */
    public FileTreePanel(File dir, TrickFileFilter filter) {    
        super();
        fileFilter = filter;
        fileDir = dir;        
        setup();
    }
    
    /**
     * Constructs a {@link FileTreePanel} with specified file directory and number of popup menus.
     * @param dir directory to list
     * @param filter the filter to use for displaying files
     * @param popupNum not sure
     */
    public FileTreePanel(File dir, TrickFileFilter filter, int popupNum) {
        super(popupNum);
        fileFilter = filter;
        fileDir = dir;    
        setup();
    }

    /**
     * Constructs a {@link FileTreePanel} with specified file directory list and number of popup menus.
     * @param dirs list of dirs.
     * @param filter the filter to use for displaying files
     * @param popupNum not sure
     */
    public FileTreePanel(Vector<File> dirs, TrickFileFilter filter, int popupNum) {
        super(popupNum);
        fileFilter = filter;
        fileDirs = dirs;
        setup();
    }
    
    //========================================
    //    Set/Get methods
    //========================================
    /**
     * Gets the instance of the {@link JXTree}.
     * 
     * @return The instance of the tree.
     */
    public JXTree getTree() {
        return fileTree;
    }
    
    /**
     * Gets the root of the tree.
     * 
     * @return    The root {@link DefaultMutableTreeNode} of the tree.
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
        return fileTreeModel;
    }
    
    /**
     * Gets the {@link TrickFileFilter} for the tree.
     * 
     * @return The instance of TrickFileFilter.
     */
    public TrickFileFilter getFilter() {
        return fileFilter;
    }
    
    /**
     * Sets the last sub-dir for the tree.
     * 
     * @param path    The sub-dir for the tree.
     */
    public void setFinalPath(String path) {
        finalPath = path;
    }
    
    /**
     * Gets the last sub-dir for the tree.
     * 
     * @return The last sub-dir for the tree.
     */
    public String getFinalPath() {
        return finalPath;
    }
    
    /**
     * Gets the path in the tree as specified by the array of names. 
     * The names[0] is the root and names[i] is a child of names[i-1].
     * 
     * @param names list of names to get the path of.
     * @return The {@link TreePath} if found, null otherwise.
     */
    public TreePath getPathByName(Object[] names) {
    	if (names != null && names.length > 0) {
    		if (!(names[0] instanceof java.lang.String)) {
    			String[] strNames = new String[names.length];		
				for (int i = 0; i < names.length; i++) {
					strNames[i] = names[i].toString();
				}
				return findPath(new TreePath(rootNode), strNames, 0, true);
    		} else {
    			return findPath(new TreePath(rootNode), names, 0, true);
    		}
    	} else {
    		return null;
    	}
    }
    
    /**
     * Gets an instance of {@link FileTreeNode} which gets right clicked on.
     * 
     * @return  An instance of {link FileTreeNode}.
     */
    public FileTreeNode getRightClickedTreeUserObj() {
        return rightClickedTreeUserObj;
    }
    
    /**
     * Helper method for finding the path by node objects.
     */
    private TreePath findPath(TreePath parent, Object[] nodes, int depth, boolean byName) {
    	TreeNode node = (TreeNode)parent.getLastPathComponent();
    	Object obj = node;

    	// If by name, convert node to a string
    	if (byName) {
    		obj = obj.toString();
    	}

    	// If equal, go down the branch
    	if (obj.equals(nodes[depth])) {
    		// If at end, return match
    		if (depth == nodes.length-1) {
    			return parent;
    		}

    		// Traverse children
    		if (node.getChildCount() >= 0) {
    			for (Enumeration e=node.children(); e.hasMoreElements(); ) {
    				TreeNode child = (TreeNode)e.nextElement();
    				TreePath path = parent.pathByAddingChild(child);
    				TreePath result = findPath(path, nodes, depth+1, byName);
    				// Found a match
    				if (result != null) {
    					return result;
    				}
    			}
    		}
    	}

    	// No match at this branch
    	return null;
    }

    
    //========================================
    //    Methods
    //========================================
    /**
     * Helper method.
     */
    private void setup() {
        selectedItems = new Vector<String>();
        buildTree();
        setLayout(new BorderLayout());
        treePane = new JScrollPane();
        treePane.getViewport().add(fileTree);
        fileTree.setRolloverEnabled(true);
		fileTree.addHighlighter(new ColorHighlighter(HighlightPredicate.ROLLOVER_ROW, null, Color.MAGENTA));
        add(BorderLayout.CENTER, treePane);
    }
    
    /**
     * Clears what needs clearing.
     */
    public void cleanUp() {
        selectedItems.clear();
        
        if (mainPaths != null) {
            mainPaths.clear();
        }        
        rootNode.removeAllChildren();
        fileTreeModel.reload();
    }
    
    
    /**
     * Abstract method for building the tree.
     */
    protected abstract void buildTree(); 
    
    /**
     * Abstract method for refreshing the tree. 
     */
    public abstract void rebuild();
    
    
    /**
     * Scrolls to the top of the tree.
     */
    public void scrollToTreeTop() {
    	treePane.getVerticalScrollBar().setValue(0);
    }
    
    
    /**
     * Refreshes the currently selected nodes.
     */
    public void refreshSelectedNodes() {
        // backup needAddToObserver flag
        boolean tempFlag = needAddToObserver;
        // when refreshing, needAddToObserver is always false since any tree expansion could 
        // cause its observer chaning and we don't want it happen while refreshing.
        needAddToObserver= false;
    
        Vector<DefaultMutableTreeNode> selectedNodes = getSelectedFirstGenerationNodes();
        if (selectedNodes != null) {    
            for (DefaultMutableTreeNode eachNode: selectedNodes) {
                refreshNode(eachNode);
            }
        }
        // restore needAddToObserver flag
        needAddToObserver = tempFlag;
    }
    
    /**
     * Refreshed the specified nodes.
     * 
     * @param nodes    A list of {@link DefaultMutableTreeNode} that need refreshing.
     */
    public void refreshNodes(Vector<DefaultMutableTreeNode> nodes) {
        boolean tempFlag = needAddToObserver;
        needAddToObserver= false;
        if (nodes != null){
            for (DefaultMutableTreeNode eachNode: nodes) {
                refreshNode(eachNode);
            }
        }
        needAddToObserver = tempFlag;
    }
    
    /**
     * Helper method for refreshing a specified tree node.
     * 
     * @param node    The specified {@link DefaultMutableTreeNode} that need refreshing.
     */
    public void refreshNode(DefaultMutableTreeNode node) {
        if (node.isRoot()) {
            int count = rootNode.getChildCount();
            for (int i = 0; i < count; i++) {
                DefaultMutableTreeNode eachChild = (DefaultMutableTreeNode)fileTreeModel.getChild(rootNode, i);
                refreshNode(eachChild);
            }
        } else {       	
            if (node.getUserObject() instanceof FileTreeNode && !node.isLeaf()) {                     
                node.removeAllChildren();                
                addNodes(node, fileFilter, null);
                fileTreeModel.nodeStructureChanged(node);
            }
        }
    }
    
    /**
     * Imports a dir to the tree. Don't import it if it's already there.
     * 
     * @param dir	The directory that needs importing for displaying.
     * @return		<code>null</code> if sucessful, otherwise a message to describe the reason why it can't be imported. 
     */
    public String importDir(File dir) {
        if (rootNode != null && fileFilter != null) {
            // if the dir is already in the tree, ignore it.                       
            Vector<DefaultMutableTreeNode> rootChildren = getAllChildren(rootNode);
            if (rootChildren != null) {
                for (DefaultMutableTreeNode eachChild : rootChildren) {
                    if (eachChild.getUserObject() instanceof FileTreeNode) {
                        FileTreeNode childNode = (FileTreeNode)eachChild.getUserObject();
                        if (childNode.getFile().equals(dir)) {
                            // found it already in list, do nothing
                            return "Directory " + dir.toString() + "\nalready imported.";
                        }
                        if (dir.toString().startsWith(childNode.getFile().toString())) {
                        	Enumeration enums = eachChild.breadthFirstEnumeration();
                        	boolean found = false;
                        	if (enums != null) {
                        		while (enums.hasMoreElements()) {
                        			DefaultMutableTreeNode tempNode = (DefaultMutableTreeNode)enums.nextElement();
                        			if (!tempNode.equals(childNode) && ((FileTreeNode)tempNode.getUserObject()).getFile().equals(dir)) {
                        				found = true;
                        				break;
                        			}
                        		}
                        	}
                        	if (found) {
                        		// adding a dir that is nested beneath this node, do nothing
                        		return "Directory already imported under\n" + childNode.getFile().toString();
                        	}
                        }
                    }
                }
            }
         
            DefaultMutableTreeNode lastNode = addNodes(rootNode, fileFilter, dir);
            if (lastNode.getChildCount()==0) {
                // if no nodes added, it contains no SIM dirs or RUN dirs, so remove it and alert user
                if (mainPaths != null) {
                    mainPaths.remove(dir.getPath());
                }
                if (lastNode.getParent() != null) {
                	fileTreeModel.removeNodeFromParent(lastNode);
                }
                return "Directory " + dir.toString() + "\ncontains no SIM or RUN dirs.";
            }

            TreePath path = new TreePath(lastNode.getPath());
            //fileTree.expandPath(path);
            fileTree.scrollPathToVisible(path);
            
            // Also need to add the imported dir to the related observer.
            /***DANNY this is handled in treeExpanded()
            if (observerList != null) {
                for (int i = 0; i < observerList.size(); i ++) {            
                    JComponent key = (JComponent)observerList.elementAt(i);            
                    
                    if (newNode.getUserObject() instanceof FileTreeNode) {
                        String parentPath = ((FileTreeNode)newNode.getUserObject()).getFile().getPath();
                        
                        if (key instanceof FileTree) {
                            FileTree observer = (FileTree)key;
                            DefaultTreeModel model = observer.getDefaultModel();
                            model.insertNodeInto(newNode, observer.getRoot(), observer.getRoot().getChildCount());                                
                        } 
                    }
                }
            }***/
        }
        return null;
    }
    
    /**
     * Adds an observer to the observer list. An observer is a component that
     * is interested in certain tree events.
     * 
     * @param comp    A {@link JComponent} that wants to get notified when certain
     *                 interested event happens.
     */
    public void addObserver(JComponent comp) {
        observerList.addElement(comp);
    }
    
    /** 
     * Gets all child node of a specified node.
     * 
     * @param parent    A parent {@link DefaultMutableTreeNode}.
     * 
     * @return A list of {@link DefaultMutableTreeNode} of the specified parent node.
     */
    public Vector<DefaultMutableTreeNode> getAllChildren(DefaultMutableTreeNode parent) {
        Vector<DefaultMutableTreeNode> childNodes = null;
        int count = parent.getChildCount();
        if (count > 0) {
            childNodes = new Vector<DefaultMutableTreeNode>();        
            for (int i = 0; i < count; i++) {
                DefaultMutableTreeNode eachChild = (DefaultMutableTreeNode)fileTreeModel.getChild(parent, i);
                childNodes.add(eachChild);
            }
        }
        return childNodes;
    }
    
    /**
     * Gets all nodes that don't have any direct parent nodes.
     * In the process of refreshing nodes, if a parent node is in the selected list, there is
     * no need to refresh the child node since the child node gets refreshed when the parent
     * node is refreshed.
     * 
     * @return All selected nodes that don't have any parents.
     */
    public Vector<DefaultMutableTreeNode> getSelectedFirstGenerationNodes() {
    	Vector<DefaultMutableTreeNode> wantedNodes = null;
    	Vector<DefaultMutableTreeNode> selectedNodes = getSelectedNodes();
    	if (selectedNodes != null) {
    		wantedNodes = new Vector<DefaultMutableTreeNode>();
    		for (DefaultMutableTreeNode node : selectedNodes) {
    			if (!selectedNodes.contains(node.getParent())) {
    				wantedNodes.add(node);
    			}
    		}
    	}
    	return wantedNodes;   	
    }
    
    /**
     * Gets all selected nodes. 
     * 
     * @return All selected nodes in a {@link Vector}.
     */
    public Vector<DefaultMutableTreeNode> getSelectedNodes() {
        Vector<DefaultMutableTreeNode> selectedNodes = null;
        TreePath[] selectedPaths = fileTree.getSelectionPaths();
        
        if (selectedPaths != null && selectedPaths.length > 0) {
            selectedNodes = new Vector<DefaultMutableTreeNode>();       
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
     * Gets all selected items. Each item is a string and composed in a particular format.
     * @return vector of selected items
     */
    public Vector<String> getSelectedItems() {
        TreePath[] selectedPaths = fileTree.getSelectionPaths();
        
        if (selectedPaths != null && selectedPaths.length > 0) {
            selectedItems.clear();
            for (int i = 0; i < selectedPaths.length; i ++) {
                if (selectedPaths[i].getLastPathComponent() instanceof DefaultMutableTreeNode) {
                    DefaultMutableTreeNode treeNode = (DefaultMutableTreeNode)selectedPaths[i].getLastPathComponent();
                                       
                    for (Enumeration e = treeNode.depthFirstEnumeration(); e.hasMoreElements();) {
                        DefaultMutableTreeNode theNode = (DefaultMutableTreeNode)e.nextElement();
                        FileTreeNode dir = null;
                        if (theNode.getUserObject() instanceof FileTreeNode) {
                            dir = (FileTreeNode)theNode.getUserObject();
                        }
                        if (theNode.isLeaf() && dir.isEnabled()) {                      	
                            TreeNode[] thePath = theNode.getPath();
                            StringBuffer buffer = new StringBuffer();
                            String eachDir = null;
                            for (int j = 0; j < thePath.length; j ++) {
                                DefaultMutableTreeNode tmpNode = (DefaultMutableTreeNode)thePath[j];
                                if (tmpNode.getLevel() == 1) {
                                    eachDir = tmpNode.toString();
                                    
                                    //TODO: ok?
                                    buffer.append(eachDir);
                                    buffer.append(File.separator);
                                }
                                // The level 0 is the invisible root, the level 1 is the home of sim
                                // Each item shown on run list in old Tcl gui was like : 
                                // <relative path>    <(path of sim home)>
                                // Thought, we may just display the the full path for the dir. 
                                // TODO: make sure this change is ok?
                                if (tmpNode.getLevel()>1) {
                                    buffer.append(tmpNode.toString());
                                    if (j<thePath.length-1) {
                                        buffer.append(File.separator);
                                    }
                                }
                            }
                            /* TODO:ok? buffer.append("    ");
                            buffer.append("(");
                            buffer.append(eachDir);
                            buffer.append(")");*/
                            selectedItems.addElement(buffer.toString());
                        }
                    }
                }
            }
        }
        // 08/03/10 Hong - commented out this
        //expandSelectedNodes();
        return selectedItems;
    }
    
    /**
     * Expands all selected tree nodes.
     */
    public void expandSelectedNodes() {   	
        actionToSelected(EXPAND_NODE);
    }
    
    /**
     * Collapses all selected tree nodes.
     */
    public void collapseSelectedNodes() {
        actionToSelected(COLLAPSE_NODE);
    }
    
    /**
     * Removes all selected tree nodes.
     */
    @Override
	public void removeSelectedData() {
        actionToSelected(REMOVE_NODE);
    }
    
    /**
     * Required by {@link TreeSelectionListener} interface.
     */
    public void valueChanged(TreeSelectionEvent e) {
        e.getPath().getLastPathComponent();
        
    }
    /**
     * Required by {@link TreeExpansionListener} interface.
     */
    public void treeExpanded(TreeExpansionEvent e) {
        DefaultMutableTreeNode node = (DefaultMutableTreeNode)e.getPath().getLastPathComponent();    
        if (needAddToObserver) {
            addNodeToObserverList(node);
        } 
    }
    
    /**
     * Required by {@link TreeExpansionListener} interface.
     */
    public void treeCollapsed(TreeExpansionEvent e) {
        // do nothing
    }
    
    /**
     * Helper method. It either expands or collapses all selected tree nodes.
     * 
     * @param flag    If <code>EXPAND_NODE</code>, expands all selected tree nodes; 
     *                If <code>COLLAPSE_NODE</code>, collapses all selected tree nodes.
     *                if <code>REMOVE_NODE</code>, removes all selected tree nodes.
     */
    private void actionToSelected(int flag) {
        TreePath[] selectedPaths = fileTree.getSelectionPaths();
        if (selectedPaths != null) {
            for (int i = 0; i < selectedPaths.length; i ++) {            
                switch (flag) {
                    case EXPAND_NODE:              
                    	// never add nodes to the observer if just expanding tree
                        needAddToObserver = false;
                        expand(fileTree, selectedPaths[i]);
                        needAddToObserver = true;
                        break;
                    case COLLAPSE_NODE:
                        fileTree.collapsePath(selectedPaths[i]);
                        break;
                    case REMOVE_NODE:
                        DefaultMutableTreeNode node = (DefaultMutableTreeNode)selectedPaths[i].getLastPathComponent();
                        String path = ((FileTreeNode)node.getUserObject()).getFile().getPath();
                        
                        // if the node that needs removing is a child node of the invisible root,
                        // all items starts with the path of the node need removing from the list.
                        // otherwise, just remove the node path from the list
                        if (node.getParent().equals(node.getRoot())) {                        
                            if (mainPaths != null) {
                                Vector<String> removalList = new Vector<String>();
                                for (int j = 0; j < mainPaths.size(); j++) {
                                    String thePath = mainPaths.elementAt(j);
                                    if (thePath.startsWith(path)) {
                                        removalList.addElement(thePath);
                                    }
                                }
                                if (removalList.size()>0) {
                                    mainPaths.removeAll(removalList);
                                }
                            }                            
                        } else {
                            if (mainPaths != null) {
                                mainPaths.remove(path);
                            }
                        }
                        fileTreeModel.removeNodeFromParent(node);
                        break;
                    default:
                }
            }
        }
    }
    
    /**
     * Helper method to expand the tree totally from the specified path.
     */
    private void expand(JTree tree, TreePath path) {
        TreeNode node = (TreeNode)path.getLastPathComponent();
        if (node.getChildCount() > 0) {
            Enumeration e = node.children();
            while(e.hasMoreElements()) {
                TreeNode n = (TreeNode)e.nextElement();
                expand(tree, path.pathByAddingChild(n));
            }
        }
        tree.expandPath(path);
    }

    
    /**
     * Adds a node to the observers in the observer map.
     * 
     * @param treeNode	The node that needs adding. 
     */
    private void addNodeToObserverList(DefaultMutableTreeNode treeNode) {
        if (observerList == null) {
            return;
        }
        for (int i = 0; i < observerList.size(); i++) {
            JComponent key = observerList.elementAt(i);
            
            if (treeNode.getUserObject() instanceof FileTreeNode) {
                
                if (key instanceof FileTreePanel) {
                    addNodeToObserver(treeNode, (FileTreePanel)key);
                } else if  (key instanceof ListPanel) {
                    if ( treeNode.isLeaf() ) {
                        ListPanel lp = (ListPanel)key ;
                        // temporary fix for adding SessionRun object to run list instead of String
                        // TODO: need to move specific thing to subclasses.
                        if (lp.getType() == DataPanel.RUN_LIST) {
                        	lp.addData(new SessionRun(((FileTreeNode)treeNode.getUserObject()).getFile().getPath()));
                        } else {
                        	lp.addData(((FileTreeNode)treeNode.getUserObject()).getFile().getPath());
                        }
                    }
                }
            }
        }
    }
    
    /**
     * Adds a new node to the tree if the node path doesn't exist.
     * 
     * @param newTreeNode 	The node that needs adding.
     * @param observer		A {@link FileTreePanel} which needs adding new node for.
     * 
     * @return	the newly created node for the observer.
     */
    public DefaultMutableTreeNode addNodeToObserver(DefaultMutableTreeNode newTreeNode, FileTreePanel observer) {        
        FileTreeNode newNode = null;
        File newFile;
        if (newTreeNode==rootNode) {
            // ignore Root Node
            return null;
        }
        if (newTreeNode.getUserObject() instanceof FileTreeNode) {
            String newTreeNodePath = ((FileTreeNode)newTreeNode.getUserObject()).getFile().getPath();

            // if the path is already added to the observer, ignore it.
            if (observer.mainPaths.contains(newTreeNodePath)) {
                return null;
            }
            
            String newPath = newTreeNodePath + File.separator + observer.getFinalPath();
            newFile = new File(newPath);
            if (newFile.exists()) {            
                if (observer.mainPaths != null ) {
                    observer.mainPaths.add(newTreeNodePath);                    
                }
                newNode = (FileTreeNode)newTreeNode.getUserObject();
                
            } else {
                return null;
            }
        } else {
            return null;
        }
        
        DefaultMutableTreeNode childNode = new DefaultMutableTreeNode(newNode);
        
        DefaultMutableTreeNode parent = null;
        boolean found = false;
        
        int count = observer.rootNode.getChildCount();
        // Going through all child nodes of the observer's root
        for (int i = 0; i < count; i++) {
            parent = (DefaultMutableTreeNode)observer.fileTreeModel.getChild(observer.rootNode, i);
            if (((DefaultMutableTreeNode)newTreeNode.getParent()).getUserObject().toString().equals(parent.toString())) {
                found = true;
                break;
            }
        }
        // The parent node for the new node is found, add the new node to the parent.
        // Otherwise, create a new parent node for the observer and add the new node to it.           
        if (!found) {
            parent = (DefaultMutableTreeNode)newTreeNode.getParent();
            if (newTreeNode.getParent().toString().equals("Root Node")) {
                // root is the parent
                parent = observer.rootNode;
            } else {
                // create and insert a new parent
                parent = new DefaultMutableTreeNode(parent.getUserObject());
                observer.fileTreeModel.insertNodeInto(parent, observer.getRoot(), observer.getRoot().getChildCount());
            }
        }
        // insert the new node under its parent
        observer.fileTreeModel.insertNodeInto(childNode, parent, parent.getChildCount());
        
        DefaultMutableTreeNode lastNode = addNodes(childNode, observer.getFilter(), newFile);
        TreePath lastPath = null;

        if (lastNode != null) {
            lastPath = new TreePath(lastNode.getPath());
        }
        
        //Make sure the user can see the lovely new node.
        if (lastPath != null) {
            observer.fileTree.expandPath(lastPath);
            observer.fileTree.scrollPathToVisible(lastPath);
        }

        return childNode;
    }
    
    
    /** 
     * Add nodes from under "dir" into curTop cursively.
     * 
     * @param curTop    The current node which needs adding child nodes to.
     * @param filter    A {@link TrickFileFilter} for only showing interested dirs/files.
     * @param dir       The {@link File} object for the node.
     * @return not sure
     */
    protected DefaultMutableTreeNode addNodes(DefaultMutableTreeNode curTop, TrickFileFilter filter, File dir) {
        String curPath = null;
        DefaultMutableTreeNode nodeForDir = null;

        if (dir != null && dir.exists()) {
            curPath = dir.getPath();
            if (curTop == null) {
                nodeForDir = new DefaultMutableTreeNode(new FileTreeNode(dir, false));
            } else {
                if (curTop.isRoot()) {
                    nodeForDir = new DefaultMutableTreeNode(new FileTreeNode(dir, true));
                } else {
                    nodeForDir = new DefaultMutableTreeNode(new FileTreeNode(dir, false));
                } 
            }
            
            if (curTop.isRoot()) {
                fileTreeModel.insertNodeInto(nodeForDir, curTop, curTop.getChildCount());           
            } else {
                curTop.add(nodeForDir);
            }
        } else {
            // DANNY what is this block for?
            // Look for cases where this method is called as addNodes(xxx, xxx, null) for more details.
            nodeForDir = curTop;
            if (nodeForDir.getUserObject() instanceof FileTreeNode) {
                if ( ((FileTreeNode)nodeForDir.getUserObject()).getFile() != null ) {
                    curPath = ((FileTreeNode)nodeForDir.getUserObject()).getFile().getPath();
                    dir = ((FileTreeNode)nodeForDir.getUserObject()).getFile();
                } else {
                    curPath = ".";
                    dir = new File(curPath) ;
                }
            } else {
                curPath = ".";
                dir = new File(curPath) ;
            }
        }

        Vector<String> fileNames = new Vector<String>();
        File[] tmp = null;
        if (filter != null) {
            tmp = dir.listFiles(filter);
        } else {
            tmp = dir.listFiles();
        }
        
        if (tmp == null) {
        	return nodeForDir;
        }
        
        for (int i=0; i<tmp.length; i++) {        
            fileNames.addElement(tmp[i].getName());
        }
        
        Collections.sort(fileNames, String.CASE_INSENSITIVE_ORDER);
        File f;
        Vector<File> files = new Vector<File>();
        // Make two passes, one for all directories.
        for (int i=0; i<fileNames.size(); i++) {
            String fName = fileNames.elementAt(i);
            String newPath;
            if (curPath.equals(".")) {
                newPath = fName;
            } else {
                newPath = curPath + File.separator + fName;
            }
            
            if ((f = new File(newPath)).isDirectory()) {
                addNodes(nodeForDir, filter, f);
            } else {
                files.addElement(f);
            }
        }
        
        // Pass two: for files.
        for (int fnum=0; fnum<files.size(); fnum++) {
            nodeForDir.add(new DefaultMutableTreeNode(new FileTreeNode(files.elementAt(fnum), false)));
        }
        return nodeForDir;
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
		public void mouseClicked(MouseEvent e) 
        {
            if( UIUtils.isRightMouseClick(e) ) {
                // when nothing is viewable, do nothing and return immediately 
                if (fileTree.getClosestPathForLocation(e.getX(), e.getY()) == null) {
                    rightClickedTreeUserObj = null;
                    return;
                }
                TreePath rightClickedPath = fileTree.getClosestPathForLocation(e.getX(), e.getY());
                
                // Highlight this selected path
                fileTree.addSelectionPath(rightClickedPath);
                
                DefaultMutableTreeNode rightClickedNode = null;
                
                if (rightClickedPath.getLastPathComponent() instanceof DefaultMutableTreeNode) {
                    rightClickedNode = (DefaultMutableTreeNode)rightClickedPath.getLastPathComponent();
                }
                
                rightClickedTreeUserObj = (FileTreeNode)rightClickedNode.getUserObject();
                
                if (rightClickedNode != null && rightClickedTreeUserObj.isEnabled()) {    
                    int popIndex = -1;
                    
                    if (rightClickedNode.isLeaf() && getPopupNum() > 0) {
                        popIndex = getPopupNum() - 1;
                    } else {
                        popIndex = rightClickedNode.getLevel() - 1;
                    }
                    
                    if (popIndex>-1 && getPopup(popIndex)!= null && !getPopup(popIndex).isVisible()) {    
                        getPopup(popIndex).show(e.getComponent(), e.getX(), e.getY());
                    }
                    TrickApplication.setPopupInvoker(getType());
                }
            } else if (UIUtils.isDoubleClick(e)) {

                TreePath doubleClickedPath = fileTree.getClosestPathForLocation(e.getX(), e.getY());
                
                if ( doubleClickedPath != null ) {
                    DefaultMutableTreeNode doubleClickedNode = null;
                
                    if (doubleClickedPath.getLastPathComponent() instanceof DefaultMutableTreeNode) {
                        doubleClickedNode = (DefaultMutableTreeNode)doubleClickedPath.getLastPathComponent();
                    }
                    FileTreeNode dir = (FileTreeNode)doubleClickedNode.getUserObject();
                    if (dir.isEnabled()) {
                    	addNodeToObserverList(doubleClickedNode);
                    }
                }
                
            }

        }

    }

}
