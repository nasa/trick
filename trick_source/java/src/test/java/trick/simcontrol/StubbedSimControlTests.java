package trick.simcontrol;

import trick.simcontrol.SimControlApplication;
import trick.simcontrol.StubbedSimControlApplication;

import java.awt.Frame;
import java.awt.Dimension;
import java.awt.Toolkit;
import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.awt.event.KeyEvent;
import java.beans.Transient;
import java.util.ArrayList;

import javax.swing.JButton;
import javax.swing.JTextField;
import javax.swing.JToggleButton;
import javax.swing.text.JTextComponent;

import org.assertj.swing.core.GenericTypeMatcher;
import org.assertj.swing.core.KeyPressInfo;
import org.assertj.swing.exception.ComponentLookupException;
import org.assertj.swing.fixture.ComponentContainerFixture;
import org.assertj.swing.fixture.JButtonFixture;
import org.assertj.swing.fixture.JPanelFixture;
import org.assertj.swing.fixture.JTextComponentFixture;
import org.assertj.swing.fixture.JToggleButtonFixture;
import org.assertj.swing.fixture.FrameFixture;
import org.assertj.swing.fixture.MouseInputSimulationFixture;
import org.assertj.swing.junit.testcase.AssertJSwingJUnitTestCase;

import org.jdesktop.swingx.JXFindBar;
import org.jdesktop.swingx.JXEditorPane;

import org.junit.BeforeClass;
import org.junit.Test;

import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assumptions.assumeThat;
import static org.assertj.swing.finder.WindowFinder.findFrame;
import static org.assertj.swing.launcher.ApplicationLauncher.application;

public class StubbedSimControlTests extends AssertJSwingJUnitTestCase {
    private FrameFixture mainFrame;
    private JPanelFixture findPanel;
    private StubbedSimControlApplication app = null;

    @BeforeClass
    public static void onSetUpBeforeClass() {

    }

    @Override
    protected void onSetUp() {       
        application(StubbedSimControlApplication.class).start();
        app = StubbedSimControlApplication.getInstance();
        mainFrame = getFrameByTitle("Sim Control");
        findPanel = mainFrame.panel(
            new GenericTypeMatcher<JXFindBar>(JXFindBar.class) {
                @Override
                protected boolean isMatching(JXFindBar pane) {
                    return true;
                }
            }
        );

        sleep(500);
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

    @Test
    public void testDumpChkpntButton() {
        testButtonToggleTwice("Dump Chkpnt", ActionID.DUMP_CHKPNT);
    }

    @Test
    public void testLoadChkpntButton() {
        testButtonToggleTwice("Load Chkpnt", ActionID.LOAD_CHKPNT);
    }

    @Test
    public void testLiteToggle() {
        Frame frame = mainFrame.target();
        Dimension toggle1, toggle2; 

        testButtonToggleOnce("Lite", "Full", ActionID.LITE);
        toggle1 = frame.getSize();
        testButtonToggleOnce("Full", "Lite", ActionID.LITE);
        toggle2 = frame.getSize();

        assertThat(toggle1).isEqualTo(SimControlApplication.LITE_SIZE);
        assertThat(toggle2).isEqualTo(SimControlApplication.FULL_SIZE);
    }

    @Test
    public void testDataRecToggle() {
        testButtonToggleTwice("Data Rec On", "Data Rec Off", ActionID.RECORDING);
    }

    @Test
    public void testRealtimeToggle() {
        testButtonToggleTwice("RealTime On", ActionID.REALTIME);
    }

    @Test
    public void testStatusMsgPane() {
        // ARRANGE
        String message = "I must not fear.\n" + //
                         "Fear is the mind-killer.\n" + //
                         "Fear is the little-death that brings total obliteration.\n" + //
                         "I will face my fear.\n" + //
                         "I will permit it to pass over me and through me.\n" + //
                         "And when it has gone past, I will turn the inner eye to see its path.\n" + //
                         "Where the fear has gone there will be nothing. Only I will remain";
        final String outStencil = "%s@%d";
        final String[] expOutput = {"null@304", "fear@11",
                                    "Fear@17" , "Fear@42",
                                    "fear@114", "fear@249"};
        String[] queryResults = new String[10];

        JTextComponentFixture editorFixture = setStatusMessage(message);
        JXEditorPane editorPane = (JXEditorPane) editorFixture.target();
        JButtonFixture findNextButton = getButtonByText(findPanel, "Find Next");
        assumeThat(findNextButton).isNotNull();
                              
        // ACT
        setFindText("Hello");
        queryResults[0] = String.format(outStencil, 
                                        editorPane.getSelectedText(), 
                                        editorPane.getSelectionStart());

        setFindText("fear");
        queryResults[1] = String.format(outStencil, 
                                        editorPane.getSelectedText(), 
                                        editorPane.getSelectionStart());

        for(int i = 2; i < 6; i++) {
            findNextButton.click();
            queryResults[i] = String.format(outStencil, 
                                            editorPane.getSelectedText(), 
                                            editorPane.getSelectionStart());
            sleep(100);
        }

        // ASSERT
        for(int i = 0; i < 6; i++) {
            assertThat(queryResults[i]).isEqualTo(expOutput[i]);
        }
    }

	public static void sleep(long ms) {
		try {Thread.sleep(ms);} catch(Exception ignored) {}
	}

    private JTextComponentFixture setStatusMessage(String text) {
        JTextComponentFixture editorFixture = mainFrame.textBox(
            new GenericTypeMatcher<JXEditorPane>(JXEditorPane.class) {
                @Override
                protected boolean isMatching(JXEditorPane pane) {
                    return true;
                }
            }
        );

        editorFixture.deleteText()
                     .setText(text);

        return editorFixture;
    }

    private void setFindText(String text) {
        JTextComponentFixture findTextField = findPanel.textBox(
            new GenericTypeMatcher<JTextField>(JTextField.class) {
                @Override
                protected boolean isMatching(JTextField pane) {
                    return true;
                }
            }
        );
        
        try {
            findTextField.deleteText()
                         .setText(text);
        } catch(Exception ignored) {}
        
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

    private void testButtonToggleOnce(String text, ActionID action) { testButtonToggleOnce(text, text, action); }

    private void testButtonToggleOnce(String initText, String toggleText, ActionID initAction) {
        // ARRANGE
        JToggleButtonFixture button = getToggleButtonByText(initText);
        String newButtonText;
        assumeThat(button).withFailMessage("Toggle Button with text\"%s\" not found\n", initText)
                          .isNotNull();

        // ACT
        testConnectedAction(button, initAction); // Toggle
        newButtonText = button.target().getText();

        // ASSERT
        assertThat(newButtonText).isEqualTo(toggleText);
    }

    private void testButtonToggleTwice(String text, ActionID action) { testButtonToggleTwice(text, action, text, action); }

    private void testButtonToggleTwice(String initText, String toggleText, ActionID action) { 
        testButtonToggleTwice(initText, action, toggleText, action); 
    }

    private void testButtonToggleTwice(String initText, ActionID initAction, String toggleText, ActionID toggleAction) {
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
