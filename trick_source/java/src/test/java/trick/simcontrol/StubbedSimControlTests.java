package trick.simcontrol;

import trick.simcontrol.SimControlApplication;
import trick.simcontrol.StubbedSimControlApplication;

import java.awt.Frame;

import javax.swing.JButton;
import javax.swing.JToggleButton;

import org.assertj.swing.core.GenericTypeMatcher;
import org.assertj.swing.exception.ComponentLookupException;
import org.assertj.swing.fixture.ComponentContainerFixture;
import org.assertj.swing.fixture.MouseInputSimulationFixture;
import org.assertj.swing.fixture.JButtonFixture;
import org.assertj.swing.fixture.JToggleButtonFixture;
import org.assertj.swing.fixture.FrameFixture;
import org.assertj.swing.junit.testcase.AssertJSwingJUnitTestCase;

import org.junit.BeforeClass;
import org.junit.Test;

import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assumptions.assumeThat;
import static org.assertj.swing.finder.WindowFinder.findFrame;
import static org.assertj.swing.launcher.ApplicationLauncher.application;

public class StubbedSimControlTests extends AssertJSwingJUnitTestCase {
    private FrameFixture mainFrame;
    private StubbedSimControlApplication app = null;

    @BeforeClass
    public static void onSetUpBeforeClass() {

    }

    @Override
    protected void onSetUp() {       
        application(StubbedSimControlApplication.class).start();
        app = StubbedSimControlApplication.getInstance();
        mainFrame = getFrameByTitle("Sim Control");

        sleep(1000);
        updateState();
    }

    //--------------------
    // JButton Tests
    //--------------------

    @Test
    public void testConnectButton() {
        testJButton("Connect", ActionID.CONNECT);
    }

    @Test
    public void testStartButton() {
        testJButton("Start", ActionID.START);
    }

    @Test
    public void testFreezeButton() {
        testJButton("Freeze", ActionID.FREEZE);
    }

    @Test
    public void testStepButton() {
        testJButton("Step", ActionID.STEP);
    }

    @Test
    public void testShutdownButton() {
        testJButton("Shutdown", ActionID.SHUTDOWN);
    }

    @Test
    public void testExitButton() {
        testJButton("Exit", ActionID.EXIT);
    }

    //--------------------
    // JToggleButton Tests
    //--------------------

    // @Test
    // public void testLiteButton() {
    //     testJToggleButton("Lite", ActionID.LITE);
    // }

    @Test
    public void testDumpChkpntButton() {
        testJToggleButton("Dump Chkpnt", ActionID.DUMP_CHKPNT);
    }

    @Test
    public void testLoadChkpntButton() {
        testJToggleButton("Load Chkpnt", ActionID.LOAD_CHKPNT);
    }

	public static void sleep(long ms) {
		try {Thread.sleep(ms);} catch(Exception ignored) {}
	}

    private void testJButton(String text, ActionID action) {
        JButtonFixture button = getButtonByText(text);
        assumeThat(button).withFailMessage("Button with text\"%s\" not found\n", text)
                          .isNotNull();

        testConnectedAction(button, action);
    }

    private void updateState() {
        JToggleButtonFixture liteFixture, recordFixture, realFixture;
        
        liteFixture = getToggleButtonByText("Lite");
        if (liteFixture == null)
            liteFixture = getToggleButtonByText("Full");
        
    }

    private void testJToggleButton(String text, ActionID action) { testJToggleButton(text, action, text, action); }

    private void testJToggleButton(String initText, ActionID initAction, String toggleText, ActionID toggleAction) {
        // ARRANGE
        JToggleButtonFixture button = getToggleButtonByText(initText);
        String buttonText, newButtonText;
        assumeThat(button).withFailMessage("Toggle Button with text\"%s\" not found\n", initText)
                          .isNotNull();

        // ACT
        testConnectedAction(button, initAction); // Toggle On
        newButtonText = button.target().getText();

        testConnectedAction(button, toggleAction); // Toggle Off
        buttonText = button.target().getText();

        // ASSERT
        assertThat(newButtonText).isEqualTo(toggleText);
        assertThat(buttonText).isEqualTo(initText);
    }

    private void testConnectedAction(MouseInputSimulationFixture clickable, ActionID expected) {
        // ARRANGE
        ActionID actual;
        int initialLogSize, logSize;

        // ACT
        initialLogSize = StubbedSimControlApplication.ActionRecord.size();
        clickable.click();

        assumeThat(StubbedSimControlApplication.ActionRecord.size() > 0)
                    .withFailMessage("\"%s\" Action ID Not Registered", expected.name())
                    .isTrue();
        actual = StubbedSimControlApplication.ActionRecord.peek();
        logSize = StubbedSimControlApplication.ActionRecord.size();

        // ASSERT
        assertThat(actual).isEqualTo(expected);
        assertThat(logSize).isEqualTo(initialLogSize + 1);
    }

    protected FrameFixture getFrameByTitle(String title) {
        FrameFixture frame = findFrame(new GenericTypeMatcher<Frame>(Frame.class) {
            protected boolean isMatching(Frame frame) {
                return title.equals(frame.getTitle()) && frame.isShowing();
            }
        }).using(robot());

        return frame;
    }

    protected JButtonFixture getButtonByText(ComponentContainerFixture container, String text) {
        JButtonFixture button;

        try {
            button = container.button(new GenericTypeMatcher<JButton>(JButton.class) {
                @Override
                protected boolean isMatching(JButton button) {
                    return text.equals(button.getText());
                }
            });
        } catch (ComponentLookupException e) {
            return null;
        }

        return button;
    }

    protected JToggleButtonFixture getToggleButtonByText(String text) {
        return getToggleButtonByText(mainFrame, text);
    }

    protected JToggleButtonFixture getToggleButtonByText(ComponentContainerFixture container, String text) {
        JToggleButtonFixture button;

        try {
            button = container.toggleButton(new GenericTypeMatcher<JToggleButton>(JToggleButton.class) {
                @Override
                protected boolean isMatching(JToggleButton button) {
                    return text.equals(button.getText());
                }
            });
        } catch (ComponentLookupException e) {
            return null;
        }

        return button;
    }

    protected JButtonFixture getButtonByText(String text) {
        return getButtonByText(mainFrame, text);
    }
}
