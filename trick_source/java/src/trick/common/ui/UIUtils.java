
//========================================
//	Package
//========================================
package trick.common.ui;

//========================================
//	Imports
//========================================
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.HeadlessException;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.PrintWriter;
import java.net.URL;
import java.text.DecimalFormat;
import java.text.NumberFormat;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.swing.DefaultComboBoxModel;
import javax.swing.Icon;
import javax.swing.ImageIcon;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JDialog;
import javax.swing.JFileChooser;
import javax.swing.JList;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextField;
import javax.swing.JTextPane;
import javax.swing.ListSelectionModel;
import javax.swing.filechooser.FileNameExtensionFilter;
import javax.swing.text.Style;
import javax.swing.text.StyleConstants;
import javax.swing.text.StyleContext;
import javax.swing.text.StyledDocument;

import org.jdesktop.swingx.JXLabel;
import org.jdesktop.swingx.JXTitledPanel;

import trick.common.TrickApplication;
import trick.common.utils.TrickColors;


/**
 * Utilities class for UI.
 *
 * @author Hong Chen
 * @since Trick 7
 */
public class UIUtils {
	
	//========================================
	//	Public data
	//========================================
	public final static String DOCUMENT_STYLE_REGULAR 						= "regular";
	public final static String DOCUMENT_STYLE_REGULAR_ITALIC 				= "regularItalic";
	public final static String DOCUMENT_STYLE_REGULAR_BOLD_BLUE 			= "regularBoldBlue";
	public final static String DOCUMENT_STYLE_REGULAR_BOLD_BLUE_SHADE 		= "regularBoldBlueShade";
	public final static String DOCUMENT_STYLE_REGULAR_LARGE_BOLD_BLUE_SHADE = "regularLargeBoldBlueShade";
	public final static String DOCUMENT_STYLE_REGULAR_LARGE_BOLD_BLUE 		= "regularLargeBoldBlue";
	public final static String DOCUMENT_STYLE_REGULAR_SMALL 				= "regularSmall";
	
	// For numbers that only needs to 2 fraction points.
	public static final NumberFormat TWO_FRACTION_FORMATTER = new DecimalFormat("0.00");
	
	public final static int CANCEL_OPTION = 0;
	public final static int OK_OPTION = 1;
	
	
	//========================================
	//	Protected data
	//========================================
	
	
	//========================================
	//	Private Data
	//========================================
	// for env variables
	private final static int HOME				= 0;
	private final static int TRICK_HOME			= 1;
	private final static int TRICK_USER_HOME	= 2;
	private final static int TRICK_VER          = 3;
	private final static int TRICK_HOST_CPU  	= 4;
	private final static int TRICK_LOGO			= 5;
	
	
	//========================================
	//	Constructors
	//========================================
	
	
	//========================================
	//	Set/Get methods
	//========================================
	/**
	 * Gets the value of HOME environment variable.
	 * 
	 * @return The value of HOME.
	 */
	public static String getUserHome() {
		return getTrickEnv(HOME);
	}
	
	/**
	 * Gets the value of TRICK_USER_HOME environment variable.
	 * 
	 * @return The value of TRICK_USER_HOME. If it is not defined, return 
	 * 		   the value of HOME.
	 */
	public static String getTrickUserHome() {
		String ret = getTrickEnv(TRICK_USER_HOME);
		if (ret == null) {
			ret = getTrickEnv(HOME);
		}
		return ret;
	}
	
	/**
	 * Gets the value of TRICK_HOME environment variable.
	 * 
	 * @return The value of TRICK_HOME. If it is not defined, return 
	 * 		   the value of HOME. 
	 */
	public static String getTrickHome() {
		String ret = getTrickEnv(TRICK_HOME);
		if (ret == null) {
			ret = getTrickEnv(HOME);
		}
		return ret;
	}
	
	/**
     * Gets the text of $TRICK_HOME/bin.
     * 
     * @return The text of $TRICK_HOME/bin. If it is not defined, return 
     *         the text of $HOME/bin. 
     */
    public static String getTrickBin() {
        String ret = getTrickEnv(TRICK_HOME) ;
        if (ret == null) {
            ret = getTrickEnv(HOME);
        }
        ret = ret + File.separator + "bin";
        return ret;
    }
	
	/**
	 * Gets the value of TRICK_VER environment variable.
	 * 
	 * @return The value of TRICK_VER. 
	 */
	public static String getTrickVersion() {
		return getTrickEnv(TRICK_VER);
	}
	
	/**
	 * Gets the value of TRICK_HOST_CPU environment variable.
	 * 
	 * @return The value of TRICK_HOST_CPU.
	 */
	public static String getTrickHostCPU() {
		return getTrickEnv(TRICK_HOST_CPU);
	}
	
	/**
	 * Gets the value of TRICK_LOGO environment variable.
	 * 
	 * @return The value of TRICK_LOGO.
	 */
	public static String getTrickLogo() {
		return getTrickEnv(TRICK_LOGO);
	}
	
	//========================================
	//	Methods
	//========================================
	/**
	 * Helper method for getting Trick environment variable value.
	 * 
	 * @param type 	The type of the variable.
	 * 
	 * @return The environment variable value.
	 */
	private static String getTrickEnv(int type) {
		String value = null;
		switch (type) {
			case TRICK_HOME:
				value = System.getenv("TRICK_HOME");
				break;
			case TRICK_USER_HOME:
				value = System.getenv("TRICK_USER_HOME");
				break;
			case TRICK_VER:
				value = System.getenv("TRICK_VER");
				break;
			case HOME:
				value = System.getenv("HOME");
				break;
			case TRICK_HOST_CPU:
				value = System.getenv("TRICK_HOST_CPU");
				break;
			case TRICK_LOGO:
				value = System.getenv("TRICK_LOGO");
				break;
			default:
				break;
		}
		return value;
	}
	
	
	/**
	 * Gets html hex for a {@link Color}.
	 * 
	 * @param c	An instance of {@link Color} which the hex number is getting from.
	 * 
	 * @return "#" followed by exactly 6 hex digits.
	 * 
	 */
	public final static String colorToHTMLHex(Color c) {
		String s = Integer.toHexString( c.getRGB() & 0xffffff );
		    if ( s.length() < 6 ) { // pad on left with zeros
		        s = "000000".substring( 0, 6 - s.length() ) + s;
		    }
		return '#' + s;
	}
	
	/**
	 * Gets the color based on the 6-digit html hex string or color name.
	 * 
	 * @param colorStrOrCode	The 6-digit hex leading by "#" in string or a supported name of a color.
	 * 
	 * @return An instance of {@link Color}.
	 */
	public final static Color getColorFromHTMLHex(String colorStrOrCode) {
		if (colorStrOrCode == null || "".equals(colorStrOrCode)) {
			return null;
		}
		Color c = null;
		try {
			c = Color.decode(colorStrOrCode);
		} catch (NumberFormatException nfe) {
			c = TrickColors.getColor(colorStrOrCode);
		}
		return c;
	}
	
	/**
	 * Checks to see if a JComboBox contains an object.
         * @param model combo box to check
         * @param obj object to test for
         * @return true or false
	 */
	public static boolean comboBoxContains(DefaultComboBoxModel model, Object obj) {
		for (int i = 0; i < model.getSize(); i++) {
			if (obj.equals(model.getElementAt(i))) {
				return true;
			}
		}
		return false;
	}
	
	/**
	 * Returns whether the mouse click is a right click.
	 *
	 * @param e a {@link MouseEvent} to test.
	 *
	 * @return <code>true</code> if right button was pressed, <code>false</code> otherwise.
	 */
	public static boolean isRightMouseClick(MouseEvent e) {
		boolean	rval = false;
		if(e.getButton() == 3) {
			rval = true;
		}

		return rval;
	}
	
	/**
	 * Check to see if the mouse click is double click.
	 * 
	 * @param e a {@link MouseEvent} to test.
	 * 
	 * @return <code>true</code> if is double-clicking, <code>false</code> otherwise.
	 */
	public static boolean isDoubleClick(MouseEvent e) {
		boolean rval = false;
		
		if (e.getClickCount() == 2) {
			rval = true;
		}
		
		return rval;
	}
	
	/**
	 * Check to see if the specified directory has "log" files.
	 * 
	 * @param dir 	A file directory.
	 * 
	 * @return <code>true</code> if the specified directory has log files, <code>false</code> otherwise.
	 */
	public static boolean hasLogFile(File dir) {
		return hasSpecifiedFile(dir, TrickFileFilter.LOG);
	}
	
	/**
	 * Check to see if the specified directory has .header files.
	 * 
	 * @param dir 	A file directory.
	 * 
	 * @return <code>true</code> if the specified directory has .header files, <code>false</code> otherwise.
	 */
	public static boolean hasHeaderFile(File dir) {
		return hasSpecifiedFile(dir, TrickFileFilter.HEADER);
	}
	
	/**
	 * Check to see if the specified directory has .trk files.
	 * 
	 * @param dir 	A file directory.
	 * 
	 * @return <code>true</code> if the specified directory has .trk files, <code>false</code> otherwise.
	 */
	public static boolean hasTRKFile(File dir) {
		return hasSpecifiedFile(dir, TrickFileFilter.TRK);
	}
	
	/**
	 * Check to see if the specified directory has .csv files.
	 * 
	 * @param dir 	A file directory.
	 * 
	 * @return <code>true</code> if the specified directory has .csv files, <code>false</code> otherwise.
	 */
	public static boolean hasCSVFile(File dir) {
		return hasSpecifiedFile(dir, TrickFileFilter.CSV);
	}
	
	/**
	 * Helper method for checking to see if a particular directory has certain files.
	 * 
	 * @param dir	A directory.
	 * @param type	The Trick file type. See {@link TrickFileFilter} for all supported types.
	 * 
	 * @return <code>true</code> if the specified directory has specified files, <code>false</code> otherwise.
	 */
	private static boolean hasSpecifiedFile(File dir, int type) {
		File[] tmp = dir.listFiles(new TrickFileFilter(type));	
		if (tmp == null || tmp.length < 1) {
			return false;
		}
		return true;
	}
	
	/**
	 * Returns a file's name without its extension. If no extension found, return
	 * the original file name.
         * @param file file object to use.
         * @return file name
	 */
	public static String getFileNameWithoutExtension(File file) {
		String fileName = file.getName();
		int dotLoc = fileName.lastIndexOf('.');
		if (dotLoc > 0 && dotLoc <= fileName.length() - 2) {
			return fileName.substring(0, dotLoc);
		}
		return fileName;
	}
	
	/**
	 * Returns an array of abstract pathnames denoting the files that satisfy the specified filter.
	 * 
	 * @param dir	A directory.
	 * @param type	The Trick file type. See {@link TrickFileFilter} for all supported types.
	 * 
	 * @return	An array of abstract pathnames denoting the files, <code>null</code>
	 * 		    if the specified dir is null or is not a directory.
	 */
	public static File[] getListFiles(File dir, int type) {
		if (dir != null && dir.isDirectory()) {
			return dir.listFiles(new TrickFileFilter(type));
		}
		return null;
	}
	
	/**
	 * Returns an instance of {@link JXLabel} that displays trick small icon for different GUIs
	 * with version information tool tip.
	 * 
	 * @return	An instance of {@link JXLabel} with its icon set to trick small icon and 
	 *          tool tip set to the trick version.
	 */
	public static JXLabel getSmallTrickIconLabel() {
		JXLabel label = new JXLabel();	
		String desc = "Trick Version " + getTrickVersion();
		label.setToolTipText(desc);
		
		ImageIcon smallIcon = createImageIcon("trick_small.gif");
		if (smallIcon != null) {
			smallIcon.setDescription(desc);
			label.setIcon(smallIcon);
		}
		return label;
	}
	
	/**
	 * Search to see if an input string has the search string. 
	 * The search string can have "*". If there is no wildcard, 
	 * it simply checks if inputStr string contians the searchStr string.
	 * 
	 * @param inputStr - the input search string.
	 * @param searchStr - the search string which searching is based upon.
	 * 
	 * @return - true if found, false otherwise.
	 */
	public static boolean searchWithWildcard(String inputStr, String searchStr) {
		StringBuffer patternBuf = new StringBuffer();
		if (searchStr.contains("*")) {
			String[] parts = searchStr.split("\\*");
			for (String part : parts) {
				patternBuf.append(".*(");
				patternBuf.append(part);
				patternBuf.append(")");
			}
			patternBuf.append(".*");
			
			Pattern pattern = Pattern.compile(patternBuf.toString());
	    	Matcher matcher = pattern.matcher(inputStr);
	    	return matcher.find();
		} else {
			return (inputStr.contains(searchStr));
		}
	}
	
	/**
	 * Opens a directory only file chooser for a particular directory 
	 * with specified dialog title.
	 * 
	 * @param chooserTitle	The title for the dialog.
	 * @param dir			The directory for the file chooser.
	 * @param parent		The parent component for opening the file chooser.
	 * 
	 * @return	The selected directory.
	 */
	public static File chooseDir(String chooserTitle, String dir, Component parent) {
		JFileChooser chooser = new JFileChooser();
		if (dir == null) {
			chooser.setCurrentDirectory(new java.io.File("."));
		} else {
			File file = new java.io.File(dir);
			if (!file.exists()) {
				file = new java.io.File(".");
			}
			chooser.setCurrentDirectory(file);			
		}
		chooser.setDialogTitle(chooserTitle);
		chooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
		
		int chooserState = chooser.showDialog(parent, "Ok");
		if (chooserState == JFileChooser.APPROVE_OPTION) {
			return chooser.getSelectedFile();
		} else {
			return null;
		}
	}
	
	/**
	 * Opens a file chooser and returns the selected file if available. 
	 * 
	 * @param dir				The directory from which a file will be selected.
	 * @param fileName			The initial file name that is prompted in the file field.
	 * @param fileNameExtension	The desired file extension.
	 * @param parent The parent component that makes the call.
	 * 
	 * @return An instance of {@link File} if the selection is successful, <code>null</code> otherwise.
	 */
	public static File chooseOpenFile(String dir, String fileName, String fileNameExtension, Component parent) {
		JFileChooser chooser = new JFileChooser();
		File selectedFile = null;
		
		File fileDir = null;
		
		// if dir is not specified or the dir doesn't exist, use the current directory
		if (dir == null) {
			fileDir = new File(".");
		} else {
			fileDir = new File(dir);
		}
		
		if (!fileDir.exists()) {
			fileDir = new File(".");
		}
		
		chooser.setCurrentDirectory(fileDir);
		
		if (fileName != null) {
			chooser.setSelectedFile(new java.io.File(fileName));
		}
		
		if (fileNameExtension != null) {
			chooser.setFileFilter(new FileNameExtensionFilter(fileNameExtension+" file", fileNameExtension));
		}
		chooser.setDialogTitle("Open File");
		
		boolean isValid = true;
		do {
			int chooserState = chooser.showDialog(parent, "Ok");
			if (chooserState == JFileChooser.APPROVE_OPTION) {
				selectedFile = chooser.getSelectedFile();
				if (!selectedFile.exists()) {
					JOptionPane.showMessageDialog(parent, 
							  					  selectedFile.getName()+" does not exist. Please choose another file!", 
							  					  "File Not Found",						  	
							  					  JOptionPane.WARNING_MESSAGE);
				} else {
					isValid = false;
				}
			} else {
				return null;
			}
		} while (isValid);
				
		return selectedFile;
	}
	
	/**
	 * Saves text to a specified file.
	 * 
	 * @param contents	The text contents that need to be saved.
	 * @param file		The file which the text will be saved to.
	 */
	public static void saveTextFile(String contents, File file) {
		try {
			PrintWriter out = new PrintWriter(new FileWriter(file));
			out.print(contents);
			out.close();
		} catch (IOException ioe) {
			
		}
	}
	
	/**
	 * Opens a file chooser and let the user to either choose or specify a file for saving purpose.
	 * 
	 * @param dir				The directory which the file chooser works from initially.
	 * @param initialFile		The file that is set initially.
	 * @param fileNameExtension	The desired file extension.
	 * @param parent The parent component that wants to open the file chooser.
	 * 
	 * @return	The specified the file, <code>null</code> if canceled.
	 */
	public static File chooseSaveFile(String dir, String initialFile, String fileNameExtension, Component parent) {
		JFileChooser chooser = new JFileChooser();
		File selectedFile = null;
		String selectedFileName = null;
		
		File fileDir = null;
		
		// if dir is not specified or the dir doesn't exist, use the current directory
		if (dir == null) {
			fileDir = new File(".");
		} else {
			fileDir = new File(dir);
		}
		
		if (!fileDir.exists()) {
			fileDir = new File(".");
		}
		
		chooser.setCurrentDirectory(fileDir);
		
		if (initialFile != null) {
			chooser.setSelectedFile(new java.io.File(initialFile));
		}
		if (fileNameExtension != null) {
			chooser.setFileFilter(new FileNameExtensionFilter(fileNameExtension + " file", fileNameExtension));
		}
		chooser.setDialogTitle("Save File");
		boolean isValid = true;
		do {
			int chooserState = chooser.showDialog(parent, "Ok");
			if (chooserState == JFileChooser.APPROVE_OPTION) {
				selectedFile = chooser.getSelectedFile();
				selectedFileName = selectedFile.getName();
				if (fileNameExtension != null) {
					// 1.append the specified file extension if there is no extension.
					// 2.append the desired extension if there is an extension which is not the same as specified
					if (selectedFileName.indexOf('.') == -1 || 
						(selectedFileName.indexOf('.') != -1 && !selectedFileName.endsWith(fileNameExtension))) {
						selectedFileName += "." + fileNameExtension;
					}
					selectedFile = new File(selectedFile.getParentFile(), selectedFileName);
				}
				if (selectedFile.exists()) {					
					int choice = JOptionPane.showConfirmDialog(parent, 
															   selectedFileName + " already exists. Overwrite it?", 
															   "Overwrite File", 
															   JOptionPane.YES_NO_OPTION);
					// If "Yes" is selected
					if (choice == JOptionPane.YES_OPTION) {
						isValid = false;
					}
				} else {
					isValid = false;
				}
			} else {
				return null;
			}
		} while (isValid);
			
		return selectedFile;
	}
	
	
	/**
	 * Shows a warning dialog with specified message and title.
	 * 
	 * @param title		The title for the dialog window.
	 * @param msg		The message text for the dialog window.
	 * @param parent 	The parent component that wants to open the dialog.
	 * 
	 * @return	The selected choice.
	 */
	public static int showOkCancelDialog(String title, String msg, Component parent) {
		Object[] options = new Object[2];
		options[OK_OPTION] = "Ok";
		options[CANCEL_OPTION] = "Cancel";
		int choice = JOptionPane.showOptionDialog(parent,
				                                  msg,
				                                  title,
				                                  JOptionPane.DEFAULT_OPTION,
				                                  JOptionPane.WARNING_MESSAGE,
				                                  null,
				                                  options,
				                                  options[CANCEL_OPTION]);		
		return choice;
	}
	
	/**
	 * Shows a list input dialog. Forces to use a {@link JList} for user to input.
	 * 
	 * JOptionPane.showInputDialog with the same arguments as this method uses 
	 * a component for user to input and it is upt to the UI to decide how best to represent the 
	 * <code>selectionValues</code>. 
	 * Normally, it's a {@link JComboBox} if less than 20 <code>selectionValues</code>, 
	 * {@link JList} if equal or greater than 20 <code>selectionValues</code>, or 
	 * {@link JTextField} if null <code>selectionValues</code>. 
         * @param parentComponent parent to use for dialog box
         * @param message message to show
         * @param title title of window
         * @param messageType type of message to show
         * @param icon icon to use
         * @param selectionValues list of values to use as choices
         * @param initialSelectionValue sets initial value of selection values
         * @return value of the input selection
         * @throws HeadlessException HeadlessException
	 */
	public static Object showListInputDialog(Component parentComponent, Object message, String title, int messageType,
			                                 Icon icon, Object[] selectionValues, Object initialSelectionValue) 
	                                         throws HeadlessException {
		JOptionPane pane = new JOptionPane();
		JList list = getSingleSelectionList(pane, selectionValues, initialSelectionValue);
        JScrollPane sp = new JScrollPane(list);
        list.ensureIndexIsVisible(list.getSelectedIndex());
        
		pane.setMessage(new Object[] { message, sp});
		pane.setMessageType(JOptionPane.QUESTION_MESSAGE);
	    pane.setOptionType(JOptionPane.OK_CANCEL_OPTION);
		
		JDialog dialog = pane.createDialog(parentComponent, title);
		
		dialog.setVisible(true);
		dialog.dispose();
		
		Object value = pane.getInputValue();
		
		// this choice is either JOptionPane.OK_OPTION or JOptionPane.CANCEL_OPTION,
		// if null, meaning the user closed the window without choosing anything
		Object choice = pane.getValue();
		
		if (choice == null || ((Integer)choice).intValue() == JOptionPane.CANCEL_OPTION) {
			return initialSelectionValue;
		}
		
		if (value == JOptionPane.UNINITIALIZED_VALUE) {
			return null;
		}
		
		return value;
	}
	
	/**
	 * Helper method and only used by <code>showListInputDialog</code> method.
	 */
    @SuppressWarnings("unchecked")
	private static JList getSingleSelectionList(final JOptionPane optionPane, Object[] values, Object initialValue) {
		JList list = new JList(values);
		list.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        if(initialValue != null) {
            list.setSelectedValue(initialValue, true);
        }
        
        MouseListener mouseListener = new MouseAdapter() {
    		@Override
			public void mousePressed(MouseEvent e) {
    			JList list = (JList)e.getSource();
				int index = list.locationToIndex(e.getPoint());	
				optionPane.setInputValue(list.getModel().getElementAt(index));
				
    			if (isDoubleClick(e)) {  				
    				// close the pane once the user selects a value by double-clicking
    				optionPane.setValue(JOptionPane.CLOSED_OPTION);
    			} 
    		}
        };
        
        list.addMouseListener(mouseListener);
        
        return list;
	}
	
	/** 
	 * Returns an {@link ImageIcon} for the specified full file path or
	 * a file in common/resources.
	 * 
	 * @param fileName	The name or the full path of the image file.
	 * 
	 * @return	an instance of {@link ImageIcon} for the specified image, 
	 * 	        null if the image file can't be found.
	 */
    public static ImageIcon createImageIcon(String fileName) {	
    	ImageIcon imgIcon = null;
    	
    	// if the fileName is a full path
    	if (fileName.indexOf(java.io.File.separator) != -1) {
    		imgIcon = new ImageIcon(fileName);
    	} else {
    		// if only a file name specified, try to find it at common resources folder
    		URL imgURL = TrickApplication.class.getResource("resources" + java.io.File.separator + fileName);
    		if (imgURL != null) {
    			imgIcon = new ImageIcon(imgURL);
    		} 
    	}
    	
        if (imgIcon == null) {
            System.err.println("Couldn't find file: " + fileName);            
        }
    
    	return imgIcon;   	
    }

    /**
     * Returns a {@link InputStream} for the specified file path or a file
     * in common/resources.
     * 
     * @param fileName	The name of a file in common/resources or a full file path.
     * 
     * @return	an instance of {@link InputStream} for the specified file, 
     *          null if the specified file can't be found.
     */
    public static InputStream getInputStreamForFile(String fileName) {
    	try {		
    		InputStream ins = null;
    		// if the fileName is a full path
    		if (fileName.indexOf(java.io.File.separator) != -1) {
    			ins = new FileInputStream(fileName);
    		} else {
    		// if only a file name, then find it at common resources area
    			ins = TrickApplication.class.getResourceAsStream("resources" + java.io.File.separator + fileName);
    		}
    		return ins;
		} catch (NullPointerException npe) {
			return null;
		} catch (FileNotFoundException fnfe) {
			return null;
		}
    }
	
    /**
	 * Creates titled panel with content view plus search capability if applicable.
	 * 
	 * @param title				The title of the panel.
	 * @param contentComponent	The searchable component for the content. 
	 * @param findPanel			The panel for find/search function. 
	 *                          If <code>null</code>, then only a titled panel is created.
	 * 
	 * @return	A titled panel with find bar for search feature.
	 */
	public static JPanel createSearchableTitledPanel(String title, 
			                                         JComponent contentComponent, 
			                                         JPanel findPanel) {
		JXTitledPanel titledPanel = new JXTitledPanel();	    
		titledPanel.setBorder(javax.swing.BorderFactory.createTitledBorder(""));
		titledPanel.setTitle(title);
		titledPanel.add(new JScrollPane(contentComponent), BorderLayout.CENTER);
		titledPanel.setMinimumSize(new Dimension(300, 250));
		if (findPanel != null) {
			titledPanel.add(findPanel, BorderLayout.SOUTH);
		}
	    return titledPanel;
	}
	
	/**
	 * Creates a JTextPane that displays detail information and the information
	 * is StyledDocument.
	 * 
	 * @return a JTextPane that has different document styles.
	 */
	public static JTextPane createReadingTextPane() {
		JTextPane textPane = new JTextPane();
		textPane.setEditable(false);
		textPane.setPreferredSize(new Dimension(400, 200));
		StyledDocument document = textPane.getStyledDocument();
		addStylesToDocument(document);
		return textPane;
	}	
	
	/**
	 * Adds styles to a StyledDocument as specified doc.
	 * 
	 * @param doc	a StyledDocument to add styles to.
	 */
	private static void addStylesToDocument(StyledDocument doc) {
		//Initialize some styles.
		Style def = StyleContext.getDefaultStyleContext().getStyle(StyleContext.DEFAULT_STYLE);

		Style regular = doc.addStyle(DOCUMENT_STYLE_REGULAR, def);
		StyleConstants.setFontFamily(def, "SansSerif");
		

		Style s = doc.addStyle(DOCUMENT_STYLE_REGULAR_ITALIC, regular);
		StyleConstants.setItalic(s, true);

		s = doc.addStyle(DOCUMENT_STYLE_REGULAR_BOLD_BLUE, regular);
		StyleConstants.setBold(s, true);
		StyleConstants.setForeground(s, Color.blue);

		s = doc.addStyle(DOCUMENT_STYLE_REGULAR_SMALL, regular);
		StyleConstants.setFontSize(s, 10);

		s = doc.addStyle(DOCUMENT_STYLE_REGULAR_LARGE_BOLD_BLUE_SHADE, regular);
		StyleConstants.setFontSize(s, 14);
		StyleConstants.setBold(s, true);
		StyleConstants.setBackground(s, Color.lightGray);
		StyleConstants.setForeground(s, Color.blue);
		
		s = doc.addStyle(DOCUMENT_STYLE_REGULAR_LARGE_BOLD_BLUE, regular);
		StyleConstants.setFontSize(s, 14);
		StyleConstants.setBold(s, true);
		StyleConstants.setForeground(s, Color.blue);
		
		s = doc.addStyle(DOCUMENT_STYLE_REGULAR_BOLD_BLUE_SHADE, regular);
		StyleConstants.setBold(s, true);
		StyleConstants.setBackground(s, Color.lightGray);
		StyleConstants.setForeground(s, Color.blue);
		
	}
}
