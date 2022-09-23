
//========================================
//	Package
//========================================
package trick.sie.utils;

import java.awt.Color;
import java.util.Vector;

import javax.swing.ListModel;

import org.jdesktop.swingx.JXList;
import org.jdesktop.swingx.decorator.ColorHighlighter;
import org.jdesktop.swingx.decorator.HighlightPredicate;
import org.jdesktop.swingx.decorator.HighlighterFactory;
import org.jdesktop.swingx.decorator.SearchPredicate;
import org.jdesktop.swingx.renderer.DefaultListRenderer;
import org.jdesktop.swingx.renderer.StringValue;
import org.jdesktop.swingx.search.AbstractSearchable;
import org.jdesktop.swingx.search.ListSearchable;


/**
 * An extension of JXList for showing variable list.
 *
 * @author hchen
 */
public class VariableList extends JXList {
	

	//========================================
	//	Public data
	//========================================
	
	
	//========================================
	//	Protected data
	//========================================
	
	
	//========================================
	//	Private Data
	//========================================
	
    private static final long serialVersionUID = 6671278771362966200L;
    
    /** The highlighter for matching text */
	private ColorHighlighter matchHighlighter;
	
	
	//========================================
	//	Constructors
	//========================================
	/**
	 * Default constructor.
         * @param  model  initial list of variables to use.
	 */
	@SuppressWarnings("rawtypes")
    public VariableList(ListModel model) {
        super(model);
        setup();
    }
	
	/**
     * Constructs a <code>JXList</code> that displays the elements in
     * the specified <code>Vector</code> and automatic creation of a RowSorter disabled.
     *
     * @param  listData  the <code>Vector</code> to be loaded into the data model
     * @throws IllegalArgumentException   if <code>listData</code> is <code>null</code>
     */
	public VariableList(Vector<?> listData ) {
		super(listData);
		setup();
	}
	
	/**
     * Constructs a <code>JXList</code> that displays the elements in
     * the specified array and automatic creation of a RowSorter disabled.
     *
     * @param  listData  the array to be loaded into the data model
     * @throws IllegalArgumentException   if <code>listData</code> is <code>null</code>
     */
	public VariableList(Object[] listData) {
		super(listData);
		setup();
	}

	
	//========================================
	//	Set/Get methods
	//========================================	
	
	
	
	

    //========================================
	//	Methods
	//========================================
	/**
	 * Helper method for list setting.
	 */
	private void setup() {		
		setRolloverEnabled(true);
	    // color for rollover
		addHighlighter(new ColorHighlighter(HighlightPredicate.ROLLOVER_ROW, null, Color.MAGENTA)); 
				
		matchHighlighter = new ColorHighlighter(null, Color.BLUE, HighlighterFactory.LINE_PRINTER, Color.BLUE);		
		
	    // for highlighting all matched list nodes
		ListSearchable s = new ListSearchable(this) {
			@Override
			protected HighlightPredicate createMatchPredicate() {
				return hasMatch() ? createSearchPredicate() : HighlightPredicate.NEVER;
			}

			private SearchPredicate createSearchPredicate() {
				SearchPredicate predicate = new SearchPredicate(lastSearchResult.getPattern(), -1, -1);				
				return predicate;
			}
			
			@Override
			protected void moveMatchByHighlighter() {
	            super.moveMatchByHighlighter();
	            getTarget().setSelectedIndex(lastSearchResult.getFoundRow());	            
	        }
		};
		
		s.setMatchHighlighter(matchHighlighter);
		setSearchable(s);	
		putClientProperty(AbstractSearchable.MATCH_HIGHLIGHTER, Boolean.TRUE);
		
		StringValue sv = new StringValue() {
        	
            private static final long serialVersionUID = 6512152435333817384L;

            public String getString(Object value) {
        		return value.toString();
        	}
        };
        setCellRenderer(new DefaultListRenderer(sv));   
	}
	
	
	//========================================
	//	Inner classes
	//========================================
	
}
