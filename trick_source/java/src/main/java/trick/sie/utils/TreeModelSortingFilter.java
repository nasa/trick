package trick.sie.utils;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

import javax.swing.event.TreeModelEvent;
import javax.swing.event.TreeModelListener;
import javax.swing.tree.TreePath;
import javax.swing.tree.TreeModel;

/**
 * enables sorting of a TreeModel
 *
 * @author Derek Bankieris
 */
public class TreeModelSortingFilter extends TreeModelFilter {
   
    /** the comparator to use for sorting */
    private Comparator<Object> comparator;

    /** a comparator to use for ascending alphabetic sorting */
    public static final Comparator<Object> alphabeticAscendingComparator =
      new Comparator<Object>() {
          @Override
          public int compare(Object object1, Object object2) {
              return object1.toString().compareTo(object2.toString());
          }
      };

    /** a comparator to use for descending alphabetic sorting */
    public static final Comparator<Object> alphabeticDescendingComparator =
      new Comparator<Object>() {
          @Override
          public int compare(Object object1, Object object2) {
              return -alphabeticAscendingComparator.compare(object1, object2);
          }
      };

    /**
     * for convenience, an enumeration of pre-defined comparators for use
     * with <code>setSorting</code>
     */
    public enum Sorting {

        None(null, "None"),
        AscendingAlphabetical(TreeModelSortingFilter.alphabeticAscendingComparator, "Ascending Alphabetical"),
        DescendingAlphabetical(TreeModelSortingFilter.alphabeticDescendingComparator, "Descending Alphabetical");

        public final Comparator<Object> comparator;
        public final String description;

        Sorting(Comparator<Object> comparator, String description) {
            this.comparator = comparator;
            this.description = description;
        }

        @Override
        public String toString() {
            return description;
        }

    };
    
    /**
     * constructor
     *
     * @param sourceModel the target model the be sorted
     */
    public TreeModelSortingFilter(TreeModel sourceModel) {
        super(sourceModel);
    }

    /**
     * creates an entry in the cache for <code>node</code> if it does not already exist
     *
     * @param node the node to cache
     */
    protected void updateCache(Object node) {
        if (!cache.containsKey(node)) {
            ArrayList<Object> children = getSourceChildren(node);
            Collections.sort(children, comparator);
            cache.put(node, children);
        }
    }

    @Override
    public Object getChild(Object parent, int index) {
        if (comparator != null) {
            updateCache(parent);
            return cache.get(parent).get(index);
        }
        return sourceModel.getChild(parent, index);
    }

    @Override
    public int getChildCount(Object parent) {
        return sourceModel.getChildCount(parent);
    }

    @Override
    public int getIndexOfChild(Object parent, Object child) {
        if (comparator != null) {
            updateCache(parent);
            return cache.get(parent).indexOf(child);
        }
        return sourceModel.getIndexOfChild(parent, child);
    }

    /**
     * sets the comparator to use for sorting. Pass <code>null</code> to disable sorting.
     * 
     * @param comparator the sorting comparator, or <code>null</code>
     */
    public void setComparator(Comparator<Object> comparator) {
        if (this.comparator != comparator) {
            this.comparator = comparator;
            fireRootChangedEvent();
        }
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
     * returns the comparator being used for sorting
     *
     * @return the sorting comparator
     */
    public Comparator<Object> getComparator() {
        return comparator;
    }

}
