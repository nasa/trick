
//========================================
//  Package
//========================================
package trick.common.utils;

//========================================
//Imports
//========================================
import java.util.Arrays;
import java.util.Collection;
import java.util.Iterator;
import java.util.SortedSet;
import java.util.TreeSet;

import javax.swing.AbstractListModel;

import org.jdesktop.swingx.JXList;


/**
 * A class that extends {@link AbstractListModel} for providing a {@link JXList} with 
 * sorted contents with uniqueness.
 *
 * @author Hong Chen
 * @since Trick 10
 */
public class SortedListModel extends AbstractListModel {
	
	//========================================
    //    Public data
    //========================================


    //========================================
    //    Protected data
    //========================================

		
	//========================================
    //    Private Data
    //========================================	
	
	private static final long serialVersionUID = -4554077303172728923L;
	
	/** For holding the data of the list. */
    private SortedSet<Object> set;
    
    //========================================
    //    Constructors
    //========================================
    /**
     * Constructs a SortedListModel.
     */
    public SortedListModel() {
        set = new TreeSet<Object>();
    }

    //========================================
    //    Set/Get methods
    //========================================
    /**
     * Gets the size of the list.
     *
     * @return The size of the list.
     */
    public int getSize() {
        return set.size();
    }
    
    /**
     * Gets an element from the list based the index number.
     *
     * @param index        The index number of the data from the list.
     *
     * @return An element at the specified index number from the list if possible, otherwise <code>null</code> is returned.
     */
    public Object getElementAt(int index) {      
    	// in case, index is out of bound
    	if (index < getSize()) {
    		return set.toArray()[index];
    	}
    	return null;
    }
    
    /**
     * Gets object from list with same name as the given element.
     *
     * @param element      The element to get.
     *
     * @return The object with same name as element, or null if not in list.
     */
    public Object getElement(Object element) {
        //if (set.contains(element)) {
            Object[] objs = set.toArray();
            String element_string;
            for (int ii=0; ii<objs.length; ii++) {
                if (element instanceof LogVar) {
                    // ignore variable units
                    element_string = ((LogVar)element).getName();
                } else {
                    element_string = element.toString();
                }
                if (objs[ii].toString().startsWith(element_string)) {
                    return objs[ii];
                }
            }
        //}
        return null;
    }


    //========================================
    //    Methods
    //========================================
    /**
     * Returns the list as an array.
     *
     * @return The array of the data.
     */
    public Object[] toArray() {
        return set.toArray();
    }
    
    /**
     * Adds one element to the list if it is not already present.
     * @param element        The element to add to the list
     */
    public void add(Object element) {
        if (set.add(element)) {
          fireContentsChanged(this, 0, getSize());
        }
    }

    /**
     * Adds all of the elements to the list.
     * @param elements       All of the elements to add to the list
     */
    public void addAll(Object elements[]) {
        Collection<Object> c = Arrays.asList(elements);
        set.addAll(c);
        fireContentsChanged(this, 0, getSize());
    }

    /**
     * Removes all of the elements from the list.
     */
    public void clear() {
        set.clear();
        fireContentsChanged(this, 0, getSize());
    }
    /**
     * Redraw all elements of list -- need this when hiding rows.
     */
    public void refresh() {
        fireContentsChanged(this, 0, getSize());
    }
    
    /**
     * Checks to see if the list contains the specified element.
     *
     * @param element    Element whose presence in this list is to be tested.
     *
     * @return    <code>true</code> if the list contains the specified element, <code>false</code> otherwise.
     */
    public boolean contains(Object element) {
        return set.contains(element);
    }

    /**
     * Returns the first element from the list.
     *
     * @return    The first element of the list.
     */
    public Object firstElement() {
        return set.first();
    }

    /**
     * Returns the iterator over the elements in this list.
     *
     * @return    An iterator over the elements in this list.
     */
    public Iterator iterator() {
        return set.iterator();
    }

    /**
     * Returns the last element from the list.
     *
     * @return    The last element of the list.
     */
    public Object lastElement() {
        return set.last();
    }

    /**
     * Removes the specified element from this list if it is present.
     *
     * @param element    Element to be removed from this list, if present.
     *
     * @return    <code>true</code> if this list contains the specified element, <code>false</code> otherwise.
     */
    public boolean removeElement(Object element) {
    	if (getSize() < 0) {
    		return false;
    	}
        boolean removed = set.remove(element);
        if (removed) {
          fireContentsChanged(this, 0, getSize());
        }
        return removed;
    }

    //========================================
    //    Inner classes
    //========================================
    
    
}
