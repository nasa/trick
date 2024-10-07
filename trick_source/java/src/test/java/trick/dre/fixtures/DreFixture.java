package trick.dre.fixtures;

import java.awt.Font;
import java.awt.GraphicsEnvironment;

import org.assertj.swing.core.Robot;
import org.assertj.swing.fixture.ComponentContainerFixture;
import org.assertj.swing.fixture.DialogFixture;
import org.assertj.swing.fixture.FrameFixture;
import org.assertj.swing.fixture.JCheckBoxFixture;
import org.assertj.swing.fixture.JLabelFixture;
import org.assertj.swing.fixture.JListFixture;
import org.assertj.swing.fixture.JPanelFixture;

import org.jdesktop.swingx.JXTitledPanel;

import trick.dre.DreApplication;
import trick.common.ui.components.FontChooser;

public class DreFixture extends FrameFixture {
    private JPanelFixture varTreePanel,
                          varSelectedPanel,
                          varSearchPanel;

    public DreFixture(Robot robot, DreApplication target) {
        super(robot, target.getMainFrame());

        varTreePanel = panel(
            new GenericTypeMatcher<JXTitledPanel>(JXTitledPanel.class) {
                @Override
                protected boolean isMatching(JXTitledPanel pane) {
                    return pane.getTitle().equals("Variables");
                }
            }
        );
    }

}