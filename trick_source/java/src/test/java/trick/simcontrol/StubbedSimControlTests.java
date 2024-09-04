package trick.simcontrol;

import trick.common.fixtures.FontChooserFixture;
import trick.common.ui.components.FontChooser;
import trick.simcontrol.SimControlApplication;
import trick.simcontrol.StubbedSimControlApplication;

import java.awt.Font;
import java.awt.Frame;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Toolkit;
import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.awt.event.KeyEvent;
import java.beans.Transient;
import java.util.ArrayList;

import javax.swing.JButton;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JTextField;
import javax.swing.JToggleButton;
import javax.swing.text.JTextComponent;

import org.assertj.swing.core.GenericTypeMatcher;
import org.assertj.swing.core.KeyPressInfo;
import org.assertj.swing.exception.ComponentLookupException;
import org.assertj.swing.fixture.AbstractComponentFixture;
import org.assertj.swing.fixture.ComponentContainerFixture;
import org.assertj.swing.fixture.DialogFixture;
import org.assertj.swing.fixture.JButtonFixture;
import org.assertj.swing.fixture.JMenuItemFixture;
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
    private JTextComponentFixture editorFixture;
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

        editorFixture = mainFrame.textBox(
            new GenericTypeMatcher<JXEditorPane>(JXEditorPane.class) {
                @Override
                protected boolean isMatching(JXEditorPane pane) {
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

    //-----------------------
    // JToggleButton Tests
    //-----------------------

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

    //-----------------------------
    // Status Message Pane Tests
    //-----------------------------

    @Test
    public void testStatusMsgPane() {
        // ARRANGE
        final String msg = "How much wood can a woodchuck chuck if a woodchuck could chuck wood?";

        assumeThat(editorFixture).isNotNull();

        // ACT
        editorFixture.deleteText()
                     .enterText(msg);

        // ASSERT
        assertThat(editorFixture.target().getText());
    }

    @Test
    public void testFindPanelButtons() {
        // ARRANGE
        final String message  = "I must not fear.\n" + //
                                "Fear is the mind-killer.\n" + //
                                "Fear is the little-death that brings total obliteration.\n" + //
                                "I will face my fear.\n" + //
                                "I will permit it to pass over me and through me.\n" + //
                                "And when it has gone past, I will turn the inner eye to see its path.\n" + //
                                "Where the fear has gone there will be nothing. Only I will remain";
        final String outStencil = "%s@%d";
        final String[] expOutputNext = {"null@304", "fear@11",
                                        "Fear@17" , "Fear@42",
                                        "fear@114", "fear@249"},
                       expOutputPrev = {"fear@114", "Fear@42", 
                                        "Fear@17" , "fear@11"};
                                        
        String[] queryResults = new String[10];

        JXEditorPane editorPane = (JXEditorPane) editorFixture.target();
        JButtonFixture findNextButton = getButtonByText(findPanel, "Find Next"),
                       findPrevButton = getButtonByText(findPanel, "Find Previous");
        
        assumeThat(findNextButton).isNotNull();
        assumeThat(findPrevButton).isNotNull();
        
        setStatusMessage(message);
                              
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

        for(int i = 6; i < 10; i++) {
            findPrevButton.click();
            queryResults[i] = String.format(outStencil, 
                                            editorPane.getSelectedText(), 
                                            editorPane.getSelectionStart());
            sleep(100);
        }

        // ASSERT
        for(int i = 0; i < 6; i++) 
            assertThat(queryResults[i]).isEqualTo(expOutputNext[i]);

        for(int i = 0; i < 4; i++) 
            assertThat(queryResults[i + 6]).isEqualTo(expOutputPrev[i]);
    }

    //--------------------
    // JMenuBar Tests
    //--------------------

    @Test
    public void testFontChange() {
        // ARRANGE
        FontChooserFixture fontDialogFixt;
        JButtonFixture okButton;

        JMenuItemFixture fontMenu = getJMenuItemByName(mainFrame, "showStatusFontMenuItem");
        
        final int fontSize = 15;
        final String fontName = "Droid Sans",
                     expPreviewText = fontName + ":3:" + fontSize;
        final Font expFont = new Font(fontName, 3, fontSize);

        String actualPreviewText;
        Font actualFont, actualPreviewFont;
        
        fontMenu.click();
        fontDialogFixt = getFontChooserFixture();
        okButton = getButtonByText(fontDialogFixt, "Ok");

        assumeThat(fontDialogFixt).isNotNull();

        // ACT
        fontDialogFixt.selectFont(fontName);
        fontDialogFixt.selectSize(fontSize);
        fontDialogFixt.setBold(true);
        fontDialogFixt.setItalic(true);

        actualPreviewText = fontDialogFixt.getPreviewText();
        actualPreviewFont = fontDialogFixt.getPreviewFont();

        okButton.click();

        actualFont = editorFixture.font().target();

        // ASSERT
        assertThat(actualPreviewText).isEqualTo(expPreviewText);
        assertThat(actualPreviewFont).isEqualTo(expFont);
        assertThat(actualFont).isEqualTo(expFont);
    }

    //--------------------
    // Helper Methods
    //--------------------

	public static void sleep(long ms) {
		try {Thread.sleep(ms);} catch(Exception ignored) {}
	}

    private void setStatusMessage(String text) {
        editorFixture.deleteText()
                     .setText(text);
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

    protected FontChooserFixture getFontChooserFixture() {
        try {
            AbstractComponentFixture abst = mainFrame.with(FontChooserFixture.getExtension());

            if (abst instanceof FontChooserFixture)
                return (FontChooserFixture) abst;
            else
                return null;
        } catch(Exception e) {
            return null;
        }
    }

    protected FrameFixture getFrameByTitle(String title) {
        FrameFixture frame = findFrame(new GenericTypeMatcher<Frame>(Frame.class) {
            protected boolean isMatching(Frame frame) {
                return title.equals(frame.getTitle()) && frame.isShowing();
            }
        }).using(robot());

        return frame;
    }

    protected JMenuItemFixture getJMenuByName(ComponentContainerFixture container, String name) {
        JMenuItemFixture menu;

        try {
            menu = container.menuItem(new GenericTypeMatcher<JMenu>(JMenu.class) {
                @Override
                protected boolean isMatching(JMenu jmenu) {
                    return name.equals(jmenu.getName());
                }
            });
        } catch (ComponentLookupException e) {
            return null;
        }

        return menu;

    }

    protected JMenuItemFixture getJMenuItemByName(ComponentContainerFixture container, String name) {
        JMenuItemFixture menu;

        try {
            menu = container.menuItem(new GenericTypeMatcher<JMenuItem>(JMenuItem.class) {
                @Override
                protected boolean isMatching(JMenuItem jmenu) {
                    return name.equals(jmenu.getName());
                }
            });
        } catch (ComponentLookupException e) {
            return null;
        }

        return menu;

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
