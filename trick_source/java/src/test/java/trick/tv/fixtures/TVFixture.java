package trick.tv.fixtures;

import javax.swing.JOptionPane;

import org.assertj.swing.core.GenericTypeMatcher;
import org.assertj.swing.core.Robot;
import org.assertj.swing.fixture.FrameFixture;
import org.assertj.swing.fixture.JCheckBoxFixture;
import org.assertj.swing.fixture.JOptionPaneFixture;
import org.assertj.swing.fixture.JPanelFixture;
import org.assertj.swing.fixture.JScrollPaneFixture;
import org.assertj.swing.fixture.JTableFixture;
import org.assertj.swing.fixture.JTextComponentFixture;
import org.assertj.swing.fixture.JTreeFixture;
import static org.assertj.swing.timing.Timeout.timeout;
import org.jdesktop.swingx.JXTitledPanel;

import trick.sie.utils.SearchPanel;
import trick.tv.TVApplication;

public class TVFixture extends FrameFixture {
    private JPanelFixture varSearchPanel;
    private JTableFixture varSelectedPanel;                          

    private JTextComponentFixture searchBar;

    private JCheckBoxFixture caseSCheckBox,
                             regexCheckBox,
                             greedCheckBox;

    private JScrollPaneFixture searchResults;
    private JTreeFixture varTree;

    public TVFixture(Robot robot, TVApplication target) {
        super(robot, target.getMainFrame());

        // varTreePanel = panel(new JXTitledPanelMatcher("Variables"));
        varSelectedPanel = table();
        varSearchPanel = panel(
            new GenericTypeMatcher<SearchPanel>(SearchPanel.class) {
                @Override
                protected boolean isMatching(SearchPanel panel) {
                    return true;
                }
            });

        varTree = tree();

        // searchBar     = varSearchPanel.textBox();
        // searchResults = varSearchPanel.scrollPane();
        caseSCheckBox = varSearchPanel.checkBox("caseSensitiveCheckBox");
        regexCheckBox = varSearchPanel.checkBox("regularExpressionCheckBox");
        greedCheckBox = varSearchPanel.checkBox("greedySearchCheckBox");
    }

    //---------------------------
    //      Fixture Methods      
    //---------------------------

    public void selectVar(String varPath) {
        varPath = varPath.replace(".", "/");
        varTree.doubleClickPath(varPath);
    }

    public void saveMenuItem(String path) {
        
    }

    public void openMenuItem(String path) {
        
    }    

    public void enterQuery(String query) {
        varSearchPanel.textBox()
                      .deleteText()
                      .enterText(query);
    }

    public void setSearchOptions(int opts) {
        boolean senseSel = (opts & CASE_SENSITIVE_ON) == CASE_SENSITIVE_ON,
                regexSel = (opts & REG_EXPRESSION_ON) == REG_EXPRESSION_ON,
                greedSel = (opts & GREEDY_SEARCH_ON ) == GREEDY_SEARCH_ON ;

        caseSCheckBox.check(senseSel);
        regexCheckBox.check(regexSel);
        greedCheckBox.check(greedSel);
    }

    public JOptionPaneFixture getErrorPopupFixture() {
        JOptionPaneFixture opt = optionPane(timeout(1500));
        if(opt.target().getMessageType() == JOptionPane.ERROR_MESSAGE)
            return opt;
        else return null;

    }

    public String[][] getSelectedVars() {
        return varSelectedPanel.contents();
    }

    public String[] getSearchResults() {
        String[] list = varSearchPanel.list().contents();
        return list.length > 0 ? list : null;
    }

    //---------------------------
    //       Inner Classes       
    //---------------------------

    private class JXTitledPanelMatcher extends GenericTypeMatcher<JXTitledPanel> {
        private final String title;

        public JXTitledPanelMatcher(String title) {
            super(JXTitledPanel.class);

            this.title = title;
        }

        @Override
        protected boolean isMatching(JXTitledPanel pane) {
            return pane.getTitle().equals(title);
        }
    }

    public static final int CASE_SENSITIVE_ON = 0b100,
                            REG_EXPRESSION_ON = 0b010,
                            GREEDY_SEARCH_ON  = 0b001;

    public static final int CASE_SENSITIVE_OFF = 0b000,
                            REG_EXPRESSION_OFF = 0b000,
                            GREEDY_SEARCH_OFF  = 0b000;
}