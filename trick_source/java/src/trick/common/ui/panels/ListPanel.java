
//========================================
//  Package
//========================================
package trick.common.ui.panels;

//========================================
//Imports
//========================================

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;

import javax.swing.AbstractListModel;
import javax.swing.DefaultListModel;
import javax.swing.JList;
import javax.swing.JScrollPane;
import javax.swing.event.ListDataListener;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

import org.jdesktop.swingx.JXList;
import org.jdesktop.swingx.decorator.ColorHighlighter;
import org.jdesktop.swingx.decorator.HighlightPredicate;

import trick.common.TrickApplication;
import trick.common.ui.UIUtils;
import trick.common.utils.SortedListModel;


/**
 * A class that extends {@link DataPanel} for displaying a list of data using {@link JXList}.
 * All contents are unique.
 *
 * @author Hong Chen
 * @since Trick 10
 */
public class ListPanel extends DataPanel implements ListSelectionListener{

    //========================================
    //    Public data
    //========================================


	//========================================
    //    Protected data
    //========================================
	/** The model for the data list. */
    protected AbstractListModel dataListModel;
	
    
	//========================================
    //    Private Data
    //========================================
    
    private static final long serialVersionUID = 929091003093241018L;
	
    /** An instance of JXList to represent a list of data shown on the panel. */
    private JXList dataList;

    
    //========================================
    //    Constructors
    //========================================
    /**
     *    Default constructor.
     */
    public ListPanel() {
        super();        
        dataListModel = new DefaultListModel();        
        setup();
    }
    
    /**
     * Constructor with specified list model.
     * @param model not sure
     */
    public ListPanel(AbstractListModel model) {
    	super();
    	dataListModel = model;  	
        setup();
    }

    //========================================
    //    Set/Get methods
    //========================================
    /**
     * Gets the instance of the {@link JList}.
     *
     * @return The instance of the JList.
     */
    public JList getJList() {
        return dataList;
    }

    //========================================
    //    Methods
    //========================================
    /**
     * Sets up the list with a scroll pane.
     */
    protected void setup() {
    	if (dataListModel == null) {
    		dataListModel = new DefaultListModel();
    	}
    	dataList = new JXList(dataListModel);
        
    	setLayout(new BorderLayout());
        
        dataList.setRolloverEnabled(true);
		dataList.addHighlighter(new ColorHighlighter(HighlightPredicate.ROLLOVER_ROW, null, Color.MAGENTA)); 

        // Put the JList into a JScrollPane.
        JScrollPane scrollpane = new JScrollPane(dataList);

        dataList.addListSelectionListener(this);
        dataList.addMouseListener(new ListMouseListener());

        add(BorderLayout.CENTER, scrollpane);
    }
    
    /**
     * Adds a {@link MouseListener} to the data list.
     * 
     * @param listener	An instance of {@link MouseListener} needs adding the list.
     */
    public void addListMouseListener(MouseListener listener) {
    	if (dataList != null) {
    		dataList.addMouseListener(listener);
    	}
    }
    
    /**
     * Adds data array to the list.
     *
     * @param items    An array of {@link Object} that need adding to the list.
     */
    public void addData(Object[] items) {
        if (items == null) {
            return;
        }

        for (int i = 0; i < items.length; i++) {
            addData(items[i]);
        }
    }

    /**
     * Adds one data to the list if it doesn't exist.
     *
     * @param item    The {@link Object} that needs adding to the list.
     */
    @SuppressWarnings("unchecked")
    public void addData(Object item) {      
        if (item == null) {
        	return;
        }
        if (dataListModel instanceof SortedListModel) {
        	if (!((SortedListModel)dataListModel).contains(item)) {
        		((SortedListModel)dataListModel).add(item);
        	}
        } else {
        	if (!((DefaultListModel)dataListModel).contains(item)) {
        		((DefaultListModel)dataListModel).addElement(item);
        	}
        }
    }

    /**
     * Removes all of the selected data from the list.
     */
	public void removeSelectedData() {
        Object[] values = dataList.getSelectedValues();
        if (values != null) {
        	for (int i = 0; i < values.length; i++) {
        		if (dataListModel instanceof SortedListModel) {
        			((SortedListModel)dataListModel).removeElement(values[i]);
        		} else {       			
        			((DefaultListModel)dataListModel).removeElement(values[i]);
        		}
        	}
        }
    }

    /**
     * Removes all specified data from the list.
     *
     * @param dataArray    The specified data array that need removing from the list.
     */
    public void removeData(Object[] dataArray) {
        if (dataArray != null && dataArray.length > 0) {
            for (int i = 0; i < dataArray.length; i++) {
                removeData(dataArray[i]);
            }
        }
    }

    /**
     * Removes one data from the list.
     *
     * @param item    The specified data that needs removing from the list.
     */
    public void removeData(Object item) {
        if (item==null) {
            return;
        }
        if (dataListModel instanceof SortedListModel) {
        	((SortedListModel)dataListModel).removeElement(item);
        } else {        	
        	((DefaultListModel)dataListModel).removeElement(item);
        }
        
    }

    /**
     * Gets all selected data from the list.
     * @return list of selected data
     */
    public Object[] getSelectedData() {
    	return dataList.getSelectedValues();
    }

    /**
     * Gets the first selected value, or <code>null</code> if the selection is empty.
     * @return first selected data
     */
    public Object getSelectedFirstData() {
    	return dataList.getSelectedValue();
    }

    /**
     * Gets all data from the list.
     * @return list of all data
     */
    public Object[] getAllData() {
    	if (dataListModel instanceof SortedListModel) {
    		return ((SortedListModel)dataListModel).toArray();
    	}
    	return ((DefaultListModel)dataListModel).toArray();
    }

    /**
     * Removes all data from the list.
     */
    public void removeAllData() {
    	if (dataListModel instanceof SortedListModel) {
    		((SortedListModel)dataListModel).clear();
    	} else {
    		((DefaultListModel)dataListModel).clear();
    	}
    }

    /**
     * Required by {@link ListSelectionListener} interface.
     */
    public void valueChanged(ListSelectionEvent evt) {
        evt.getSource();
    }

    public void addListDataListener(ListDataListener listener) {
        dataListModel.addListDataListener(listener);
    }

    
    //========================================
    //    Inner classes
    //========================================
    /**
     * Using an inner class to define MouseListener to help organize code better.
     * The goal of this class is to handle mouse calls and forward them
     * to the interested parties.
     */
    private class ListMouseListener extends MouseAdapter {
        
        //========================================
        //    MouseListener methods
        //========================================
        /**
         * Invoked when the mouse button has been clicked (pressed
         * and released) on a component.
         *
         * @param e MouseEvent sent from system.
         */
        public void mouseClicked(MouseEvent e)
        {
            // there is nothing shown or the list is empty, return without doing anything.
            if (dataListModel.getSize() < 1) {
                return;
            }
            if( UIUtils.isRightMouseClick(e) ) {
                int index = dataList.locationToIndex(e.getPoint());
                if (index > -1) {
                    int[] selectedIndices = dataList.getSelectedIndices();
                    int[] extendedIndices = new int[selectedIndices.length+1];
                    System.arraycopy(selectedIndices, 0, extendedIndices, 0, selectedIndices.length);
                    // Adds the right-clicked one to the selected list
                    extendedIndices[extendedIndices.length-1] = index;
                    dataList.setSelectedIndices(extendedIndices);
                }
                if (getPopup(0) != null && !getPopup(0).isVisible()) {
                    TrickApplication.setPopupInvoker(getType());
                    getPopup(0).show(e.getComponent(), e.getX(), e.getY());
                }
            }
        }
        
    }
}
