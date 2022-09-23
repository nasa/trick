
//========================================
//  Package
//========================================
package trick.common.ui.panels;

//========================================
//Imports
//========================================

import javax.swing.JPanel;
import javax.swing.JPopupMenu;


/**
 * A {@link JPanel} that is for displaying data.
 *
 * @since Trick 10
 */
public abstract class DataPanel extends JPanel {
		
	//========================================
	//	Public data
	//========================================
	public static final int SIM_RUN_TREE     = 1;
	public static final int RUN_LIST         = 2;
	public static final int SIM_DP_TREE      = 3;
	public static final int DP_LIST          = 4;
	public static final int VAR_LIST         = 5;
	
	//========================================
	//	Protected data
	//========================================
	/** The pop-up menu for the data panel. */
	protected JPopupMenu popup = null;
	
	//========================================
	//	Private Data
	//=======================================
	
	private static final long serialVersionUID = -8211915811050384782L;
		
	/** The panel could have more than one popup menu. */
	private JPopupMenu[] popupList;
	
	/** The type of the panel so we know what kind of panel invokes the popup. */
	private int panelType;
	
	//========================================
	//	Constructors
	//========================================
	/**
	 * Default constructor.
	 */
	public DataPanel() {
		popupList = new JPopupMenu[1];
	}

	/**
	 * The constructor that specifies total number of popup menus for the panel.
	 * 
	 * @param popupNum	The total popup menus for the panel.
	 */
	public DataPanel(int popupNum) {
		if (popupNum > 0) {
			popupList = new JPopupMenu[popupNum];
		} else {
			popupList = new JPopupMenu[1];
		}
	}
	
	//========================================
	//	Set/Get methods
	//========================================
	/**
	 * Sets the panel type.
	 * 
	 * @param type	The type in integer for the panel.
	 */
	public void setType(int type) {
		panelType = type;
	}
	
	/**
	 * Gets the panel type.
	 * 
	 * @return 	The panel type in integer.
	 */
	public int getType() {
		return panelType;
	}
	
	/**
	 * Gets the total number of popup menus.
	 * 
	 * @return 	The number of popup menus.
	 */
	public int getPopupNum() {
		if (popupList != null) {
			return popupList.length;
		} 
		return 0;
	}
	
	/**
	 * Sets the popup menu with specified index number.
	 * 
	 * @param popup		An instance of {@link JPopupMenu}.
	 * @param index		The index number for the popup menu.
	 */
	public void setPopup(JPopupMenu popup, int index) {
		if (index < popupList.length) {
			popupList[index] = popup;
		}
	}
	
	/**
	 * Gets the popup menu at specified index number.
	 * 
	 * @param index		The index number of the popup menu.
	 * 
	 * @return	An instance of {@link JPopupMenu}.
	 */
	public JPopupMenu getPopup(int index) {
		if (index < popupList.length) {
			return popupList[index];
		} 
		return popupList[0];
	}
	
	
	//========================================
	//	Methods
	//========================================
	public abstract void removeSelectedData();
}
