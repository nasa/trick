
//========================================
//  Package
//========================================
package trick.common.ui.panels;

//========================================
//Imports
//========================================

import java.awt.Color;
import java.awt.Component;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import javax.swing.SwingConstants;

import org.jdesktop.swingx.AbstractPatternPanel;
import org.jdesktop.swingx.JXFindBar;
import org.jdesktop.swingx.JXFindPanel;
import org.jdesktop.swingx.search.Searchable;



/**
 * A {@link JXFindBar} that is for allowing users to input search text.
 *
 * @since Trick 10
 */
public class FindBar extends JXFindBar implements ActionListener{
	
	//========================================
	//	Public data
	//========================================
	
	
	//========================================
	//	Protected data
	//========================================
		

	//========================================
	//	Private Data
	//=======================================
	
	private static final long serialVersionUID = 9092192049485321408L;
	
	/** The initial search text for this find bar */
	private String initialSearchText = null;
	
	/** By default, this would not have search options as JXFindPanel, true otherwise. */
	private boolean hasOptions = false;
	
	// TODO: add pattern support
	//private JCheckBox anchorCheck;
	
	//========================================
	//	Constructors
	//========================================
	/**
	 * Default constructor.
	 */
	public FindBar() {
		super();
		setNotFoundForegroundColor();
	}

	/**
	 * The constructor that specifies total number of popup menus for the panel.
	 * 
	 * @param searchable 	An instance of {@link Searchable} from a gui component which
	 *                      this search bar is for.
	 */
	public FindBar(Searchable searchable) {
		super(searchable);		
		setNotFoundForegroundColor();
	}
	
	//========================================
	//	Set/Get methods
	//========================================
	/**
	 * Gets to see if case sensitive is checked. 
         * @return true or false
	 */
	public boolean isCaseSensitive() {
		return getPatternModel().isCaseSensitive();
	}
	/**
	 * Sets whether to have the options as {@link JXFindPanel}.
         * @param b true or false
	 */
	public void setOptions(boolean b) {
		hasOptions = b;
	}
	
	/**
	 * Updates the searchField that is defined in parent class {@link AbstractPatternPanel}
	 * if there is initial search text is defined.
         * @param searchText searchText
	 */
	public void updateSearchField(String searchText) {
		initialSearchText = searchText;
		if (searchField != null) {			
			searchField.setText(searchText);
		}
	}
	
	/** 
	 * Gets the current search text shown in searchField.
         * @return the text
	 */
	public String getSearchText() {
		if (searchField != null) {
			return searchField.getText();
		}
		return null;
	}
	
	/**
	 * Helper method for changing the forground color when the text is not found.
	 * Since notFoundForegroundColor is proteced in {@link JXFindBar}, extending
	 * it is the only way to be able to change it.
	 * 
	 */
	private void setNotFoundForegroundColor() {
		notFoundForegroundColor = Color.red;
	}
	
	
	//========================================
	//	Methods
	//========================================
	@Override
	protected void build() {
		if (hasOptions) {
			buildBarWithOption();
		} else {
			buildBar();
		}
		if (initialSearchText != null) {						
			updateSearchField(initialSearchText);
		}
	}
		
	private void buildBar() {
		setLayout(new FlowLayout(SwingConstants.LEADING));
        add(searchLabel);
        add(new JLabel(":"));
        add(new JLabel("  "));
        add(searchField);
        add(findNext);
        add(findPrevious);
	}
	
	private void buildBarWithOption() {
		//anchorCheck = new JCheckBox("Anchor");
		//anchorCheck.addActionListener(this);
		
		wrapCheck = new JCheckBox();
        backCheck = new JCheckBox();
		Box lBox = new Box(BoxLayout.LINE_AXIS);
		//lBox.add(anchorCheck);		
		lBox.add(matchCheck);
        lBox.add(wrapCheck);
        lBox.add(backCheck);
        lBox.setAlignmentY(Component.TOP_ALIGNMENT);
        
        Box mBox = new Box(BoxLayout.LINE_AXIS);
        mBox.add(searchLabel);
        mBox.add(new JLabel(":  "));
        mBox.add(searchField);
        mBox.add(findNext);
        mBox.add(findPrevious);       
        mBox.setAlignmentY(Component.TOP_ALIGNMENT);

        setLayout(new BoxLayout(this, BoxLayout.PAGE_AXIS));
       
        add(lBox);
        add(mBox);
	}
	
	public void actionPerformed(ActionEvent e) {
		/*if (e.getSource() == anchorCheck) {
			if (anchorCheck.isSelected()) {
				getPatternModel().setRegexCreatorKey(PatternModel.REGEX_ANCHORED);
			} else {
				getPatternModel().setMatchRule(PatternModel.REGEX_MATCH_RULES);
			}
		}*/
	}
}
