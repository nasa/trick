package trick.dataproducts.plot.utils;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.HeadlessException;
import java.awt.Image;
import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import java.io.File;

import javax.swing.AbstractAction;
import javax.swing.ImageIcon;
import javax.swing.JMenuItem;
import javax.swing.JScrollPane;
import javax.swing.JSeparator;
import javax.swing.JTabbedPane;
import javax.swing.JTextPane;
import javax.swing.JViewport;
import javax.swing.text.AttributeSet;
import javax.swing.text.BadLocationException;
import javax.swing.text.SimpleAttributeSet;
import javax.swing.text.StyleConstants;
import javax.swing.text.StyleContext;
import javax.swing.text.TabSet;
import javax.swing.text.TabStop;
import javax.swing.text.html.HTMLDocument;

import trick.common.TrickApplication;
import trick.common.ui.UIUtils;
import trick.dataproducts.trickqp.utils.ProductTable;
import trick.dataproducts.utils.Session;
import trick.dataproducts.utils.SessionRun;


public class TrickTableFrame extends TrickFrame {

	//========================================
    //    Public data
    //========================================
    
    

    //========================================
    //    Protected data
    //========================================
	
	
	
	//========================================
    //    Private data
    //========================================
	
	private static final long serialVersionUID = -3151139797020281386L;
	
	/** The pane that holds tables. One tab for one RUN. */
	private JTabbedPane framePane;
	
	
    
    //========================================
    //   Constructors
    //========================================	
	/**
     * Creates a new, initially invisible <code>Frame</code> with the
     * specified title and frame icon image if necessary.
     * <p>
     * This constructor sets the component's locale property to the value
     * returned by <code>JComponent.getDefaultLocale</code>.
     *
     * @param title the title for the frame
     * @param iconImage icon image
     * @param sessionObject the {@link Session} object that contains where the table data is from
     * @param theTable the {@link ProductTable} instance that contains the table specs
     * @throws HeadlessException HeadlessException
     * 
     */
    public TrickTableFrame(String title, Image iconImage, Session sessionObject, ProductTable theTable) throws HeadlessException {
        super(title, iconImage);
        setMenuBar();
        if (fileMenu != null) {       
        	fileMenu.add(new SaveAction("Save", new ImageIcon(TrickApplication.class.getResource("resources/filesave.gif")), "Save Table", KeyEvent.VK_S));
        
        	fileMenu.add(new JSeparator());
        	JMenuItem closeMenuItem = new JMenuItem(new CloseAction("Close", "Close Table", KeyEvent.VK_L));
        
        	fileMenu.add(closeMenuItem);
        }
        setupTable(sessionObject, theTable);
    }

	   
    
    //========================================
    //   Methods
    //========================================
    /**
     * Helper method for setting up the table.
     */
    private void setupTable(Session sessionObject, ProductTable theTable) {
    	framePane = new JTabbedPane();
    	framePane.setTabLayoutPolicy(JTabbedPane.SCROLL_TAB_LAYOUT);
        
        for (SessionRun eachRun : sessionObject.getRuns()) {          
            JTextPane eachTablePane = new JTextPane();
            eachTablePane.setEditable(false);
            eachTablePane.setPreferredSize(new Dimension(400, 600));
            
            // using HTMLDocument so that the table pane does not performing line wrapping
            eachTablePane.setContentType("text/html");
            HTMLDocument doc = (HTMLDocument)eachTablePane.getDocument();
            
            // sets tabs setting for the table pane so that all values separated by tabs
            // can be aligned as table columns
			TabStop[] tabs = new TabStop[theTable.getColumnList().size()];
			int tabPixels = 200;
			for (int i = 0; i < theTable.getColumnList().size(); i++) {
				tabs[i] = new TabStop(tabPixels, TabStop.ALIGN_RIGHT, TabStop.LEAD_EQUALS);
				tabPixels += 200;
			}			
			TabSet tabset = new TabSet(tabs);			
			StyleContext context = StyleContext.getDefaultStyleContext();
		    AttributeSet attrSet = context.addAttribute(SimpleAttributeSet.EMPTY, StyleConstants.TabSet, tabset);
		    eachTablePane.setParagraphAttributes(attrSet, false);
		    
			PlotUtils.insertTableData(eachRun, theTable, doc);
			// scroll to top of the pane
			eachTablePane.setCaretPosition(0);		
            framePane.add(eachRun.getDirName(), new JScrollPane(eachTablePane));
        }
        
        add(framePane, BorderLayout.CENTER);
	}
    
    /**
     * Helper method for saving the table data to a text file.
     */
    private void saveTable() {
        File file = UIUtils.chooseSaveFile(null, "table_", null, this);
        if (file != null) {   
        	JViewport viewPort = ((JScrollPane)framePane.getSelectedComponent()).getViewport();
        	
        	JTextPane selectedPane = (JTextPane)viewPort.getView();
            try {
				UIUtils.saveTextFile(selectedPane.getDocument().getText(0, selectedPane.getDocument().getLength()), file);				
			} catch (BadLocationException e) {					
			    e.printStackTrace();
			}
        }       
    }
    
    /**
     * Sets this frame invisible.
     */
    private void closeTable() {
    	setVisible(false);
    }
    
    
    
    //========================================
    //   Inner Classes
    //========================================
	public class SaveAction extends AbstractAction {
      		
		private static final long serialVersionUID = -2005694863569791565L;
		
		public SaveAction(String text, ImageIcon icon,
                          String desc, Integer mnemonic) {
            super(text, icon);
            putValue(SHORT_DESCRIPTION, desc);
            putValue(MNEMONIC_KEY, mnemonic);
        }
        public void actionPerformed(ActionEvent e) {
            saveTable();
        }
    }
	
	public class CloseAction extends AbstractAction {
  				
		private static final long serialVersionUID = 8766852963567400594L;
		
		public CloseAction(String text, String desc, Integer mnemonic) {
            super(text);
            putValue(SHORT_DESCRIPTION, desc);
            putValue(MNEMONIC_KEY, mnemonic);
        }
        public void actionPerformed(ActionEvent e) {
            closeTable();
        }
    }
}
