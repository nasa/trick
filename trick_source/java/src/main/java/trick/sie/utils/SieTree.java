package trick.sie.utils;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Comparator;
import java.util.Enumeration;

import javax.swing.JComponent;
import javax.swing.JTree;
import javax.swing.ToolTipManager;
import javax.swing.tree.DefaultTreeCellRenderer;
import javax.swing.tree.DefaultTreeSelectionModel;
import javax.swing.tree.TreePath;

import org.jdesktop.swingx.JXTree;

import trick.sie.utils.TreeModelSortingFilter.Sorting;

/**
 * displays a variable hierarchy as described by an S_sie.resource file
 *
 * @author Derek Bankieris
 */
public class SieTree extends JXTree {

    /** the tree model */
    private SieTreeModel sieTreeModel = new SieTreeModel();

    /** the exclusion filter */
    private TreeModelExclusionFilter treeModelExclusionFilter = new TreeModelExclusionFilter(sieTreeModel);

    /** the sorting filter */
    private TreeModelSortingFilter treeModelSortingFilter = new TreeModelSortingFilter(treeModelExclusionFilter);

    /**
     * constructor
     */
    public SieTree() {
        setModel(treeModelSortingFilter);
        
        setRootVisible(false);
        setShowsRootHandles(true);

        // Add a custom TreeSelectionModel to prevent selection of variables with no Trick-managed members.
        setSelectionModel(new DefaultTreeSelectionModel() {
                       
            private boolean canPathBeAdded(TreePath path) {
                return path != null && ((SieTemplate)path.getLastPathComponent()).isTrickManaged();
            }
            
            @Override
            public void addSelectionPath(TreePath path) {
                if (canPathBeAdded(path)) {
                    super.addSelectionPath(path);
                }
            }
            @Override
            public void addSelectionPaths(TreePath[] paths) {
                if (paths != null) {
                    ArrayList<TreePath> selectablePaths = new ArrayList<TreePath>(paths.length);
                    for (TreePath path : paths) {
                        if (canPathBeAdded(path)) {
                            selectablePaths.add(path);
                        }
                    }
                    super.addSelectionPaths(selectablePaths.toArray(new TreePath[selectablePaths.size()]));
                }
            }
            @Override
            public void setSelectionPath(TreePath path) {
                if (canPathBeAdded(path)) {
                    super.setSelectionPath(path);
                }
            }
            @Override
            public void setSelectionPaths(TreePath[] paths) {
                if (paths != null) {
                    ArrayList<TreePath> selectablePaths = new ArrayList<TreePath>(paths.length);
                    for (TreePath path : paths) {
                        if (canPathBeAdded(path)) {
                            selectablePaths.add(path);
                        }
                    }
                    super.setSelectionPaths(selectablePaths.toArray(new TreePath[selectablePaths.size()]));
                }
            }
        });
       
        // Add tool tips to cells.
        ToolTipManager.sharedInstance().registerComponent(this);
        setCellRenderer(new DefaultTreeCellRenderer() {
            @Override
            public JComponent getTreeCellRendererComponent(
              final JTree tree, final Object value, final boolean selected, final boolean expanded,
              final boolean leaf, final int row, final boolean hasFocus) {

                JComponent component = (JComponent)super.getTreeCellRendererComponent(
                  tree, value, selected, expanded, leaf, row, hasFocus);

                component.setToolTipText(((SieTemplate)value).getToolTipText());
    
                return component;
            }
        });
    }
   
    /**
     *  sets root instances
     *  
     *  @param instances root instances
     *
     */
    public void setRootInstances(Collection<SieTemplate> instances) {
        sieTreeModel.setRootInstances(instances);
    }

    /**
     * returns whether or not the root has no children
     *
     * @return <code>true</code> if the root is empty
     */
    public boolean isEmpty() {
        return sieTreeModel.isEmpty();
    }
    
    /**
     * returns the comparator being used for sorting
     *
     * @return the sorting comparator
     */
    public Comparator<Object> getComparator() {
        return treeModelSortingFilter.getComparator();
    }

    /**
     * a convenience method for <code>setComparator</code>, using a predefined
     * set of options
     *
     * @param sorting the sorting to apply
     */
    public void setSorting(Sorting sorting) {
        setComparator(sorting.comparator);
    }

    /**
     * sets the comparator used for sorting. Pass <code>null</code>to disable sorting.
     *
     * @param comparator the sorting comparator, or <code>null</code>
     */
    public void setComparator(Comparator<Object> comparator) {
        if (treeModelSortingFilter.getComparator() != comparator) {
            if (isEmpty()) {
                treeModelSortingFilter.setComparator(comparator);
            }
            else {
                TreePath[] selectedPaths = getSelectionPaths();
                Enumeration<TreePath> expandedPaths = getExpandedDescendants(new TreePath(getModel().getRoot()));
                treeModelSortingFilter.setComparator(comparator);
                if (expandedPaths != null) {
                    while (expandedPaths.hasMoreElements()) {
                        expandPath(expandedPaths.nextElement());
                    }
                }
                if (selectedPaths != null) {
                    setSelectionPaths(selectedPaths);
                    if (selectedPaths.length > 0) {
                        scrollPathToVisible(selectedPaths[0]);
                    }
                }
            }
        }
    }

    /**
     * expands, selects, and scrolls to the deepest available parent of
     * <code>variableName</code>
     *
     * @param variableName the name of a variable whose parent to expand
     */
    public void expandParent(String variableName) {
        TreePath treePath = sieTreeModel.getPath(variableName);
        if (treePath != null) {
            expandPath(treePath);
            setSelectionPath(treePath);
            scrollPathToVisible(treePath);
        }
    }

}
