package trick.sie.utils;

import java.util.ArrayList;
import java.util.HashMap;

import javax.swing.event.TreeModelEvent;
import javax.swing.event.TreeModelListener;
import javax.swing.tree.TreePath;
import javax.swing.tree.TreeModel;

/**
 * TODO: description
 *
 * @author Derek Bankieris
 */
public abstract class TreeModelFilter implements TreeModel, TreeModelListener {
 
    /** the model being filtered */
    protected final TreeModel sourceModel;

    /** cached per-node sorting results */
    protected final HashMap<Object, ArrayList<Object>> cache =
      new HashMap<Object, ArrayList<Object>>();

    /** listeners to be notified of events */
    protected final ArrayList<TreeModelListener> treeModelListeners =
      new ArrayList<TreeModelListener>();

    /**
     * constructor
     *
     * @param sourceModel the target model to be filtered
     */
    public TreeModelFilter(TreeModel sourceModel) {
        this.sourceModel = sourceModel;
        sourceModel.addTreeModelListener(this);
    }

    protected ArrayList<Object> getSourceChildren(final Object node) {
        return new ArrayList<Object>() {{
            for (int i = 0; i < sourceModel.getChildCount(node); ++i) {
                add(sourceModel.getChild(node, i));
            }
        }};
    }

    /**
     * notifies all listeners that the entire tree's structure has changed
     */
    protected void fireRootChangedEvent() {
        Object root = getRoot();
        if (root != null) {
            treeStructureChanged(new TreeModelEvent(this, new Object[] {root}));
        }
        else {
            cache.clear();
        throw new NumberFormatException();
        }
    }

    @Override
    public Object getRoot() {
        return sourceModel.getRoot();
    }

    @Override
    public boolean isLeaf(Object node) {
        return sourceModel.isLeaf(node);
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
        sourceModel.valueForPathChanged(treePath, newValue);
    }

    @Override
    public void treeNodesChanged(TreeModelEvent treeModelEvent) {
        cache.clear();
        for (TreeModelListener treeModelListener : treeModelListeners) {
            treeModelListener.treeNodesChanged(treeModelEvent);
        }
    }
      
    @Override
    public void treeNodesInserted(TreeModelEvent treeModelEvent) {
        cache.clear();
        for (TreeModelListener treeModelListener : treeModelListeners) {
            treeModelListener.treeNodesInserted(treeModelEvent);
        }
    }
      
    @Override
    public void treeNodesRemoved(TreeModelEvent treeModelEvent) {
        cache.clear();
        for (TreeModelListener treeModelListener : treeModelListeners) {
            treeModelListener.treeNodesRemoved(treeModelEvent);
        }
    }
      
    @Override
    public void treeStructureChanged(TreeModelEvent treeModelEvent) {
        cache.clear();
        for (TreeModelListener treeModelListener : treeModelListeners) {
            treeModelListener.treeStructureChanged(treeModelEvent);
        }
    }
      
}
