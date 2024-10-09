package trick.dre.fixtures;

import java.awt.Font;
import java.awt.GraphicsEnvironment;

import org.assertj.swing.core.GenericTypeMatcher;
import org.assertj.swing.core.Robot;
import org.assertj.swing.fixture.ComponentContainerFixture;
import org.assertj.swing.fixture.DialogFixture;
import org.assertj.swing.fixture.FrameFixture;
import org.assertj.swing.fixture.JCheckBoxFixture;
import org.assertj.swing.fixture.JLabelFixture;
import org.assertj.swing.fixture.JListFixture;
import org.assertj.swing.fixture.JPanelFixture;
import org.assertj.swing.fixture.JTreeFixture;

import org.jdesktop.swingx.JXTitledPanel;

import trick.dre.DreApplication;
import trick.common.ui.components.FontChooser;

public class DreFixture extends FrameFixture {
    private JPanelFixture varTreePanel,
                          varSelectedPanel,
                          varSearchPanel;
    
    private JTreeFixture varTree;

    public DreFixture(Robot robot, DreApplication target) {
        super(robot, target.getMainFrame());

        varTreePanel = panel(new JXTitledPanelMatcher("Variables"));
        varSelectedPanel = panel(new JXTitledPanelMatcher("Selected Variables"));

        varTree = varTreePanel.tree();
    }

    //---------------------------
    //      Fixture Methods      
    //---------------------------

    public void selectVar(String varPath) {
        varTree.doubleClickPath(varPath);
    }

    public void setOptions(int opts) {
        switch (opts & 0b1110000000) {
            case BINARY: selectFormat(0); break;
            case ASCII:  selectFormat(1); break;
            case HDF5:   selectFormat(2); break;
            default: break;
        }

        switch (opts & 0b0001110000) {
            case ALWAYS:  selectFrequency(0); break;
            case CHANGES: selectFrequency(1); break;
            case STEP:    selectFrequency(2); break;
            default: break;
        }

        switch (opts & 0b0000001110) {
            case BUFFER:      selectBuffer(0); break;
            case NO_BUFFER:   selectBuffer(1); break;
            case RING_BUFFER: selectBuffer(2); break;
            default: break;
        }
    }

    private void selectFormat(int index) {
        String menuItemName = "DR" + FORMATS[index];
        menuItem(menuItemName).click();
    }

    private void selectFrequency(int index) {
        String menuItemName = "DR_" + FREQUENCIES[index];
        menuItem(menuItemName).click();
    }

    private void selectBuffer(int index) {
        String menuItemName = "DR_" + BUFFERS[index];
        menuItem(menuItemName).click();
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

    //----------------------------
    //     Constant Variables     
    //----------------------------
    // 0x000 000 000 0

    public final static int BINARY      = 0b1000000000,
                            ASCII       = 0b0100000000,
                            HDF5        = 0b0010000000,
                            ALWAYS      = 0b0001000000,
                            CHANGES     = 0b0000100000,
                            STEP        = 0b0000010000,
                            BUFFER      = 0b0000001000,
                            NO_BUFFER   = 0b0000000100,
                            RING_BUFFER = 0b0000000010,
                            SINGLE_PREC = 0b0000000001;

    private final String[]  FORMATS = {"Binary", "Ascii", "HJDF5"},
                            BUFFERS = {"Buffer", "No_Buffer", "Ring_Buffer", "Thread_Buffer"},
                            FREQUENCIES = {"Always", "Changes", "Step_Changes"};
}