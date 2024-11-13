package trick.tv.fixtures;

import javax.swing.JOptionPane;

import org.assertj.swing.core.GenericTypeMatcher;
import org.assertj.swing.core.Robot;
import org.assertj.swing.fixture.FrameFixture;
import org.assertj.swing.fixture.JCheckBoxFixture;
import org.assertj.swing.fixture.JOptionPaneFixture;
import org.assertj.swing.fixture.JPanelFixture;
import org.assertj.swing.fixture.JScrollPaneFixture;
import org.assertj.swing.fixture.JTextComponentFixture;
import org.assertj.swing.fixture.JTreeFixture;
import static org.assertj.swing.timing.Timeout.timeout;
import org.jdesktop.swingx.JXTitledPanel;

import trick.tv.TVApplication;

public class TVFixture extends FrameFixture {
    private JPanelFixture varSelectedPanel,
                          varSearchPanel;

    private JTextComponentFixture searchBar;

    private JCheckBoxFixture caseSCheckBox,
                             regexCheckBox;

    private JScrollPaneFixture searchResults;
    private JTreeFixture varTree;

    public TVFixture(Robot robot, TVApplication target) {
        super(robot, target.getMainFrame());

        // varTreePanel = panel(new JXTitledPanelMatcher("Variables"));
        // varSelectedPanel = panel(new JXTitledPanelMatcher("Selected Variables"));
        // varSearchPanel = panel(
        //     new GenericTypeMatcher<SearchPanel>(SearchPanel.class) {
        //         @Override
        //         protected boolean isMatching(SearchPanel panel) {
        //             return true;
        //         }
        //     });

        varTree = tree();

        // searchBar     = varSearchPanel.textBox();
        // searchResults = varSearchPanel.scrollPane();
        // caseSCheckBox = varSearchPanel.checkBox("caseSensitiveCheckBox");
        // regexCheckBox = varSearchPanel.checkBox("regularExpressionCheckBox");
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

    public JOptionPaneFixture getErrorPopupFixture() {
        JOptionPaneFixture opt = optionPane(timeout(1500));
        if(opt.target().getMessageType() == JOptionPane.ERROR_MESSAGE)
            return opt;
        else return null;

    }

    public String[] getSelectedVars() {
        return varSelectedPanel.list().contents();
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
}