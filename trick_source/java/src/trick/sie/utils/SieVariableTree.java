
//========================================
//	Package
//========================================
package trick.sie.utils;

import java.awt.Color;

import org.jdesktop.swingx.decorator.ColorHighlighter;
import org.jdesktop.swingx.decorator.HighlightPredicate;
import org.jdesktop.swingx.decorator.HighlighterFactory;
import org.jdesktop.swingx.decorator.SearchPredicate;
import org.jdesktop.swingx.search.AbstractSearchable;
import org.jdesktop.swingx.search.TreeSearchable;


/**
 * An extension of JTree, which has more event handling.
 *
 * @since Trick 7
 */
public class SieVariableTree extends SieTree {
	

	//========================================
	//	Public data
	//========================================
	
	
	//========================================
	//	Protected data
	//========================================
	
	
	//========================================
	//	Private Data
	//========================================
       
    private static final long serialVersionUID = 5838083006873138347L;
    
	/** The highlighter for matching text */
	private ColorHighlighter matchHighlighter;
	
	
	//========================================
	//	Constructors
	//========================================
	/**
     * Default constructor.
     * 
     */
    public SieVariableTree() {
        super();
        setup();
    }
    
	//========================================
	//	Set/Get methods
	//========================================   
	
	
	
	//========================================
	//	Methods
	//========================================
	/**
	 * Helper method for tree setting.
	 */
	private void setup() {
	    
		setRolloverEnabled(true);
		// color for rollover
		addHighlighter(new ColorHighlighter(HighlightPredicate.ROLLOVER_ROW, null, Color.MAGENTA)); 
						
		matchHighlighter = new ColorHighlighter(HighlighterFactory.LINE_PRINTER, Color.BLUE, HighlighterFactory.LINE_PRINTER, Color.BLUE);
		
		// for highlighting all matched tree nodes
		TreeSearchable s = new TreeSearchable(this) {
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
	            getTarget().setSelectionPath(tree.getPathForRow(lastSearchResult.getFoundRow()));	            
	        }
		};
		
		s.setMatchHighlighter(matchHighlighter);
		setSearchable(s);	
		putClientProperty(AbstractSearchable.MATCH_HIGHLIGHTER, Boolean.TRUE);
	}
}
