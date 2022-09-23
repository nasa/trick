package trick.dataproducts.plot.utils;

import java.awt.Component;
import java.awt.HeadlessException;
import java.awt.Image;
import java.awt.event.KeyEvent;

import javax.swing.Box;
import javax.swing.JComponent;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.WindowConstants;

import trick.common.ui.UIUtils;


public class TrickFrame extends JFrame {

    //========================================
    //    Public data
    //========================================



    //========================================
    //    Protected data
    //========================================
    protected JMenu fileMenu = null;

    //========================================
    //    Private Data
    //========================================

    private static final long serialVersionUID = -2771397837741213286L;


    //========================================
    //   Constructors
    //========================================
    /**
     * Creates a new, initially invisible <code>Frame</code> with the
     * specified title and frame icon if necessary.
     * <p>
     * This constructor sets the component's locale property to the value
     * returned by <code>JComponent.getDefaultLocale</code>.
     *
     * @param title the title for the frame
     * @param iconImage the icon image for the frame and has to set it to null for Mac
     *
     * @exception HeadlessException if GraphicsEnvironment.isHeadless()
     * returns true.
     * @see java.awt.GraphicsEnvironment#isHeadless
     * @see Component#setSize
     * @see Component#setVisible
     * @see JComponent#getDefaultLocale
     */
    public TrickFrame(String title, Image iconImage) throws HeadlessException {
        super(title);
        setIconImage(iconImage);
        setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);

        // Set chartFrame icon to null for Mac to avoid invalid context 0x0 error message
        String theOS = System.getProperty("os.name").toLowerCase();
        if (theOS.indexOf("mac") != -1) {
            setIconImage(null);
        }
    }


    //========================================
    //   Methods
    //========================================
    /**
     * Helper method for setting up the menu bar if needed.
     */
    protected void setMenuBar() {
         JMenuBar menuBar = new JMenuBar();
         fileMenu = new JMenu("File");
         fileMenu.setMnemonic(KeyEvent.VK_F);
         menuBar.add(fileMenu);
         menuBar.add(Box.createHorizontalGlue());
         menuBar.add(UIUtils.getSmallTrickIconLabel());
         setJMenuBar(menuBar);
    }


    @Override
    public String toString() {
        return getTitle();
    }

}
