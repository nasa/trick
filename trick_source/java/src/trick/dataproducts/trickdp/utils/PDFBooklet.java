
//========================================
//	Package
//========================================
package trick.dataproducts.trickdp.utils;

//========================================
//	Imports
//========================================

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Desktop;
import java.awt.Frame;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.border.Border;
import javax.swing.border.EtchedBorder;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

import org.jdesktop.swingx.JXTitledPanel;

import trick.common.ui.TrickFileFilter;
import trick.common.ui.UIUtils;
import trick.common.ui.panels.ListPanel;
import trick.dataproducts.utils.FileTreeNode;


/**
 * Defines a dialog for creating a PDF file from a number of postcript files .
 *
 * @author Hong Chen
 * @since Trick 10
 */
public class PDFBooklet extends JDialog implements ActionListener, ListSelectionListener {

	
	//========================================
	//	Public data
	//========================================
	
	
	//========================================
	//	Protected data
	//========================================
	
	
	//========================================
	//	Private Data
	//========================================
	
	private final static String BROWSE_OPTION   = "Browse";
	private final static String CLOSE_OPTION	= "Close";	
	private final static String PS_VIEW_OPTION	= "PS View";
	private final static String PS_MERGE_OPTION	= "PS Merge";
	private final static String PDF_BOOKLET_OPTION	= "PDF Booklet";
	
	private Frame parentFrame;
	
	private JTextField dirField;
	
	private JLabel msgLabel;
	
	// The directory that has postscript files for converting to a PDF
	private File fileDir;
	private JButton browseButton;
	private JButton okButton;
	private JButton closeButton;	
	private JButton psViewButton;
	private JButton psMergeButton;
	private JButton pdfBookletButton;
	private ListPanel selectedPSFileList;
		
	//========================================
	//	Constructors
	//========================================
	/**
	 *	Custructor with specified owner.
	 */
	private PDFBooklet(Frame parent, String title, File dir) {
		super(parent, title, true);
		parentFrame = parent;
		fileDir = dir;
		buildGUI();
		setLocationRelativeTo(parent);
	}
	
	//========================================
	//	Set/Get methods
	//========================================
	
	
	
	
	//========================================
	//	Methods
	//========================================
	/**
	 * Handles clicks on the buttons or check box selections.
	 * 
	 * Required by {@link ActionListener}.
	 */
	public void actionPerformed(ActionEvent e) {		
		if (CLOSE_OPTION.equals(e.getActionCommand())) {
			setVisible(false);	
			dispose();
		} else if (BROWSE_OPTION.equals(e.getActionCommand())) {
			File f = UIUtils.chooseDir("Choose a Directory", fileDir.getPath(), parentFrame);
			if (f != null) {
				fileDir = f;				
				dirField.setText(f.getPath());
				updateDirectory();
			}
		} else if (PS_VIEW_OPTION.equals(e.getActionCommand())) {
			psView();
		} else if (PS_MERGE_OPTION.equals(e.getActionCommand())) {
			psMerge();
		} else if (PDF_BOOKLET_OPTION.equals(e.getActionCommand())) {
			booklet();
		} 
	}
	
	/**
	 * Handles selections on the list.
	 * 
	 * Required by {@link ListSelectionListener}.
	 */
	public void valueChanged(ListSelectionEvent e) {
		if (e.getValueIsAdjusting() == false) {
			updateButtons();
		}
	}

	
	/**
     * Set up and show the booklet PDF dialog. 
     * 
     * @param parent	The parent component for the dialog.
     * @param title		The title of the dialog.
     * @param dir		The dir that has postscript files.
     */
    public static void showDialog(Component parent, String title, File dir) {
        Frame frame = JOptionPane.getFrameForComponent(parent);
        File trueDir = null;
        if (!dir.exists() || dir.isFile()) {
        	trueDir = dir.getParentFile();
        } else {
        	trueDir = dir;
        }
        PDFBooklet dialog = new PDFBooklet(frame, title, trueDir);      
        dialog.setVisible(true);  
    }
        

	/**
	 * Helper method for building the gui.
	 * 
	 */
    //for Java 7, the type of elements of JComboBox needs to be specified to avoid the warning and it's not supported in Java 6
    @SuppressWarnings("unchecked")
	private void buildGUI() {   	
    	JPanel dirPanel = new JPanel();
    	dirPanel.setLayout(new BoxLayout(dirPanel, BoxLayout.X_AXIS));
    	
    	dirField = new JTextField(fileDir.getPath());		
		dirField.setColumns(30);
		dirPanel.add(dirField);
		browseButton = new JButton(BROWSE_OPTION);
		browseButton.addActionListener(this);
		dirPanel.add(browseButton);
		
    	JXTitledPanel titledPanel = new org.jdesktop.swingx.JXTitledPanel();
    	Border loweredetched = BorderFactory.createEtchedBorder(EtchedBorder.LOWERED);
        titledPanel.setBorder(BorderFactory.createTitledBorder(loweredetched, "Directory Selection"));
		titledPanel.add(dirPanel, BorderLayout.NORTH);
		
		msgLabel = new JLabel();	
		titledPanel.add(msgLabel, BorderLayout.CENTER);
		
		selectedPSFileList = new ListPanel();
		selectedPSFileList.getJList().addListSelectionListener(this);		
		resetList();
	    loweredetched = BorderFactory.createEtchedBorder(EtchedBorder.LOWERED);
		selectedPSFileList.setBorder(BorderFactory.createTitledBorder(loweredetched, "All .ps files (select one or more):"));
		selectedPSFileList.getJList().setBackground(Color.white);       
		
		JPanel buttonPanel = new JPanel();
        
		psViewButton = new JButton(PS_VIEW_OPTION);
		psViewButton.addActionListener(this);
		
		psMergeButton = new JButton(PS_MERGE_OPTION);
		psMergeButton.addActionListener(this);
		
		pdfBookletButton = new JButton(PDF_BOOKLET_OPTION);
		pdfBookletButton.addActionListener(this);
		
		if (selectedPSFileList.getJList().getSelectedIndex() == -1) {
			psViewButton.setEnabled(false);
			psMergeButton.setEnabled(false);
			pdfBookletButton.setEnabled(false);
		}
		
		closeButton = new JButton(CLOSE_OPTION);
		closeButton.addActionListener(this);
		buttonPanel.setLayout(new BoxLayout(buttonPanel, BoxLayout.X_AXIS));
		buttonPanel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
		
		buttonPanel.add(psViewButton);
		buttonPanel.add(Box.createHorizontalStrut(10));
		buttonPanel.add(psMergeButton);
		buttonPanel.add(Box.createHorizontalStrut(10));
		buttonPanel.add(pdfBookletButton);
		buttonPanel.add(Box.createHorizontalStrut(10));
		buttonPanel.add(closeButton);
		
		getContentPane().setLayout(new BoxLayout(getContentPane(), BoxLayout.Y_AXIS));
		getContentPane().add(Box.createVerticalStrut(10));
		getContentPane().add(titledPanel);
		getContentPane().add(Box.createVerticalStrut(15));
		getContentPane().add(selectedPSFileList);
		getContentPane().add(buttonPanel);
		getContentPane().add(Box.createVerticalStrut(5));
		       
		pack();
	}
    
    /**
     * Helper method for showing not found message.
     */
    private void setNotFoundMsg() {
    	if (msgLabel != null) {
    		msgLabel.setText("No .ps file found. Please select another directory!");
    		msgLabel.setForeground(Color.red);
    	}
    }
    
	/**
	 * Helper method for updating the file list if the directory is changed.
	 */
	private void updateDirectory() {		
		resetList();
	}
	
	private void updateButtons() {
		if (selectedPSFileList != null) {
			if (selectedPSFileList.getJList().getModel().getSize() == 0 || selectedPSFileList.getJList().getSelectedIndex() == -1) {
				psViewButton.setEnabled(false);
				psMergeButton.setEnabled(false);
				pdfBookletButton.setEnabled(false);
			} else {
				psViewButton.setEnabled(true);
				psMergeButton.setEnabled(true);
				pdfBookletButton.setEnabled(true);
			}			
		}
	}
	
	// Action code for Toss button
	private void toss() {
		Object[] selectedData = selectedPSFileList.getSelectedData();
		if (selectedData != null && selectedData.length > 0) {
			selectedPSFileList.removeSelectedData();
			selectedPSFileList.getJList().setSelectedIndex(0);
		}
		updateButtons();
	}
	
	private void resetList() {
		selectedPSFileList.removeAllData();
		File[] psFiles = UIUtils.getListFiles(fileDir, TrickFileFilter.PS);
		if (psFiles != null && psFiles.length > 0) {
			for (File eachFile : psFiles) {
				selectedPSFileList.addData(new FileTreeNode(eachFile, false));				
			}			
			if (msgLabel != null) {
				msgLabel.setText("");
			}			
		} else {
			setNotFoundMsg();			
		}
		// at this moment, the buttons could be unavailable yet and if one is available, all should be ready
		if (psViewButton != null) {
			updateButtons();
		}
	}
	
	// For PS view 
	private void psView() {
		File selectedFirstFile = new File(fileDir + java.io.File.separator + selectedPSFileList.getSelectedFirstData().toString());
		try {
			if (selectedFirstFile.exists() && Desktop.isDesktopSupported()) {
				Desktop.getDesktop().open(selectedFirstFile);
			}
		} catch (java.io.IOException ioe) {
			UIUtils.showOkCancelDialog("Error", ioe.getMessage(), this);
		}
	}
	
	// For PS merge 
	private void psMerge() {
		File psFile = UIUtils.chooseSaveFile(fileDir.getPath(), "merge", "ps", parentFrame);
    	if (psFile == null) {   		
    		return;
    	}
    	
		StringBuffer commandBuf = new StringBuffer();
		commandBuf.append("gs -dNOPAUSE -dQUIET -sDEVICE=pswrite -dBATCH -sOutputFile=");
		commandBuf.append(psFile.getPath());
		Object[] selectedData = selectedPSFileList.getSelectedData();
		for (Object obj : selectedData) {
			commandBuf.append(" ");
			commandBuf.append(fileDir);
			commandBuf.append(java.io.File.separator);
			commandBuf.append(obj.toString());
		}		
		
		int ret = launchProgram(commandBuf.toString());	
		
		// Cancel
		if (ret == UIUtils.CANCEL_OPTION) {
			return;
		}
		try {
			if (psFile.exists() && Desktop.isDesktopSupported()) {
				Desktop.getDesktop().open(psFile);
			}
		} catch (java.io.IOException ioe) {
			UIUtils.showOkCancelDialog("Error", ioe.getMessage(), this);
		}
	} 
	
	// For creating booklet
	private void booklet() {
		File pdfFile = UIUtils.chooseSaveFile(fileDir.getPath(), "booklet", "pdf", parentFrame);
    	if (pdfFile == null) {   		
    		return;
    	}
    	
		StringBuffer commandBuf = new StringBuffer();
		commandBuf.append("gs -dNOPAUSE -dQUIET -sDEVICE=pdfwrite -dBATCH -sOutputFile=");
		commandBuf.append(pdfFile.getPath());
		Object[] selectedData = selectedPSFileList.getSelectedData();		
		for (Object obj : selectedData) {
			commandBuf.append(" ");
			commandBuf.append(fileDir);
			commandBuf.append(java.io.File.separator);
			commandBuf.append(obj.toString());
		}		
		
		int ret = launchProgram(commandBuf.toString());	
		
		// Cancel
		if (ret == UIUtils.CANCEL_OPTION) {
			return;
		}
		try {
			if (pdfFile.exists() && Desktop.isDesktopSupported()) {
				Desktop.getDesktop().open(pdfFile);
			}
		} catch (java.io.IOException ioe) {
			UIUtils.showOkCancelDialog("Error", ioe.getMessage(), this);
		}
	}
	
	private int launchProgram(String command) {		
		try {
            Process p = Runtime.getRuntime().exec(command);
            p.waitFor();
            // Ok
            return UIUtils.OK_OPTION;
        } catch (java.io.IOException ioe) {
        	return (UIUtils.showOkCancelDialog("Error", ioe.getMessage(), this));
        } catch (InterruptedException ie) {
        	return (UIUtils.showOkCancelDialog("Error", ie.getMessage(), this));
        }
	}
		
	//========================================
	//	Inner Classes
	//========================================
	
}
