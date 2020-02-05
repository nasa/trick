package trick.sie.utils;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;

import javax.swing.event.TreeModelEvent;
import javax.swing.event.TreeModelListener;
import javax.swing.tree.TreePath;
import javax.swing.tree.TreeModel;

/**
 * Models the variable hierarchy information from an S_sie.resource file in a
 * manner appropriate for driving a JTree.
 *
 * @author Derek Bankieris
 */
public class SieTreeModel implements TreeModel {
   
    /* the root */
    private final SieTemplate root = new SieTemplate("", "", 0); 

    /* listeners to be notified of events */
    private final ArrayList<TreeModelListener> treeModelListeners =
      new ArrayList<TreeModelListener>();
    
    /** 
     * returns the dot-delimited variable path name, excluding the final
     * component, represented by <code>treePath</code>
     *
     * @param treePath the path
     *
     * @return the dot-delimited variable path name
     */
    public static String getPathName(TreePath treePath) {
        Object[] path = treePath.getPath();
        String result = "";
        for (int i = 0;  i < path.length - 1; ++i) {
            String pathName = path[i].toString();
            if (!pathName.equals("") &&
              !pathName.equals("<Trick Variables>") &&
              !pathName.equals("<Dynamic Allocations>")) {
                result += path[i] + ".";
            }
        }
        return result;
    }

    public TreePath getPath(String variableName) {
        if (variableName == null || isEmpty()) {
            return null;
        }

        SieTemplate node = root;
        TreePath treePath = new TreePath(node);
        String[] elements = variableName.split("\\.");

        if (variableName.startsWith("trick_")) {
            node = root.children.get(0);
            treePath = treePath.pathByAddingChild(node);
        }

        outer:
        for (int i = 0; i< elements.length - 1; ) {
            String element = trimIndices(elements[i]);
            for (int j = 0; j < getChildCount(node); ++j) {
                SieTemplate child = getChild(node, j); 
                if (element.equals(trimIndices(child.toString()))) {
                    node = child;
                    treePath = treePath.pathByAddingChild(node);
                    ++i;
                    continue outer;
                }   
            }   
            break;
        }

        return treePath;
    }

    /**
     * trims trailing indices, if any, from <code>variableName</code>
     *
     * @param variableName the name of the variable to trim
     *
     * @return the trimmed name
     */
    private String trimIndices(String variableName) {
        int end = variableName.indexOf('[');
        return end == -1 ? variableName : variableName.substring(0, end);
    }
    
    @Override
    public SieTemplate getChild(Object parent, int index) {
        return ((SieTemplate)parent).children.get(index);
    }

    @Override
    public int getChildCount(Object parent) {
        return ((SieTemplate)parent).children.size();
    }

    @Override
    public int getIndexOfChild(Object parent, Object child) {
        return ((SieTemplate)parent).children.indexOf(child);
    }

    /** 
     *  sets the root instances
     *  
     *  @param instances root instances
     *
     */
    public void setRootInstances(final Collection<SieTemplate> instances) {
        root.children.clear();

        if (instances != null && !instances.isEmpty()) {
            SieTemplate trickInstances = new SieTemplate("<Trick Variables>", "", 0);
            ArrayList<SieTemplate> staticAllocations = new ArrayList<SieTemplate>();
            SieTemplate dynamicAllocations = new SieTemplate("<Dynamic Allocations>", "", 0);
            root.children.add(trickInstances);

            for (SieTemplate instance : instances) {
                if (instance.parameter.startsWith("trick_")) {
                    trickInstances.children.add(instance);
                }
                else if (instance.dynamicAllocation) {
                    dynamicAllocations.children.add(instance);
                }
                else {
                    staticAllocations.add(instance);
                }
            }

            if (!dynamicAllocations.children.isEmpty()) {
                root.children.add(dynamicAllocations);
            }
            root.children.addAll(staticAllocations);
        }

        fireRootChangedEvent();
    }

    @Override
    public SieTemplate getRoot() {
        return root;
    }

    @Override
    public boolean isLeaf(Object node) {
        return ((SieTemplate)node).children.isEmpty();
    }

    /**
     * returns whether or not this tree has any variables
     *
     * @return <code>true</code> if this tree contains variables
     */
    public boolean isEmpty() {
        return root.children.isEmpty();
    }

    /**
     * notifies all listeners that the entire tree's structure has changed
     */
    protected void fireRootChangedEvent() {
        TreeModelEvent treeModelEvent = new TreeModelEvent(this, new Object[] {root});
        for (TreeModelListener treeModelListener : treeModelListeners) {
            treeModelListener.treeStructureChanged(treeModelEvent);
        }
    }
    
    @Override
    public void addTreeModelListener(TreeModelListener treeModelListener) {
        treeModelListeners.add(treeModelListener);
    }
      
    @Override
    public void removeTreeModelListener(TreeModelListener treeModelListener) {
        treeModelListeners.remove(treeModelListener);
    }

    @Override
    public void valueForPathChanged(TreePath treePath, Object newValue) {
        throw new UnsupportedOperationException();
    }
      
}
