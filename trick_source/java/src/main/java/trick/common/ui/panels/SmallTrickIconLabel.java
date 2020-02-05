//========================================
//    Package
//========================================
package trick.common.ui.panels;

//========================================
//    Package
//========================================
import java.net.URL;

import javax.swing.ImageIcon;

import org.jdesktop.swingx.JXLabel;

import trick.common.TrickApplication;

public class SmallTrickIconLabel extends JXLabel {

	private static final long serialVersionUID = -1272978741978959753L;

	/**
     * creates a label displaying a small Trick icon with version information tool tip
     */ 
    public SmallTrickIconLabel() {
        URL imageURL =  TrickApplication.class.getResource("resources/trick_small.gif");
        if (imageURL != null) {
            String description = "Trick Version " + System.getenv("TRICK_VER");
            setIcon(new ImageIcon(imageURL, description));
            setToolTipText(description);
        }
    }

}
