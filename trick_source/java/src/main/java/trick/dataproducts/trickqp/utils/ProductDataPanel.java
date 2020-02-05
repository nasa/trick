
//========================================
//  Package
//========================================
package trick.dataproducts.trickqp.utils;

//========================================
//  Imports
//========================================
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.File;
import java.util.ArrayList;
import java.util.List;

import javax.swing.GroupLayout;
import javax.swing.JButton;
import javax.swing.JColorChooser;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.border.SoftBevelBorder;

import trick.common.ui.UIUtils;
import trick.common.ui.components.CommonTreeNode;
import trick.common.ui.components.FontChooser;
import trick.common.ui.components.NumberTextField;
import trick.common.utils.UnitType;
import trick.dataproducts.trickqp.TrickQPApplication;
import trick.dataproducts.trickqp.TrickQPApplication.QPTabType;

/**
 * Defines a {@link JPanel} to display product data for Trick QP GUI. 
 * 
 * @author Hong Chen
 * @since Trick 10
 */
public class ProductDataPanel extends JPanel implements KeyListener {
	
     
	
	//========================================
    //  Public data
    //========================================
	public static enum DataRepresentationType {
		COLOR_LABEL,
		NUMBER_FIELD,
		WHOLE_NUMBER_FIELD,
		UNITS_FIELD,
		FROM_UNITS_FIELD,
		FONT_FIELD,
		PROGRAM_FIELD,
		COMBO_BOX,
		PLAIN_TEXT_FIELD;
		
		private Object[] comboBoxItems = null;
		public void setComboBoxItems(Object[] objs) {
			comboBoxItems = objs.clone();
		}
		
		public Object[] getComboBoxItems() {
			return comboBoxItems;
		}
	}
	
	public static enum YesNoChoice {
		YES("Yes"),
		NO("No");
		
		private final String choiceText;
		
		private YesNoChoice(String s) {
			choiceText = s;
		}
		
		@Override
		public String toString() {
			return choiceText;
		}
	}
    //========================================
    //  Protected data
    //========================================
	
	
    //========================================
    //  Private Data
    //========================================
	
	private static final long serialVersionUID = -7505042268101113022L;

	private GroupLayout groupLayout;
	private GroupLayout.SequentialGroup horizontalGroup;
	private GroupLayout.SequentialGroup verticalGroup;
	
	private GroupLayout.ParallelGroup hParallelGroup1;
	private GroupLayout.ParallelGroup hParallelGroup2;
	
	// All data fields shown as JComponent for this data panel
	private ArrayList<JComponent> dataFields;
	
	// The tree node that is clicked to cause this data panel shown
	private CommonTreeNode relatedNode;
	
	// Remember these fields if they are available, for the purpose of easy access
	private JTextField fromUnitsField; // for a variable when it's units not specified in the data recording data
	private JTextField unitsField; // for a variable
	private JTextField fontField;  // for font
	private JTextField programField;
	
	private final static String BROWSE_FILE = "Browse...";
	
    //========================================
    //  Constructors
    //========================================
	/**
	 * Constructor with specified how many rows of data that
	 * need displaying. There will be 2 columns, one column is 
	 * the data field label and the other one is for
	 * data field.
	 */
    public ProductDataPanel() {
    	super();
    	
    	groupLayout = new GroupLayout(this);
    	setLayout(groupLayout);
    	groupLayout.setAutoCreateGaps(true);
        groupLayout.setAutoCreateContainerGaps(true);
        
        horizontalGroup = groupLayout.createSequentialGroup();  
        verticalGroup = groupLayout.createSequentialGroup();
        
        hParallelGroup1 = groupLayout.createParallelGroup(GroupLayout.Alignment.LEADING);  
		hParallelGroup2 = groupLayout.createParallelGroup(GroupLayout.Alignment.LEADING);  
		
		horizontalGroup.addGroup(hParallelGroup1);
		horizontalGroup.addGroup(hParallelGroup2);
        
        groupLayout.setHorizontalGroup(horizontalGroup);
        groupLayout.setVerticalGroup(verticalGroup);
          
        dataFields = new ArrayList<JComponent>();
    }
	

    //========================================
    //  Set/Get methods
    //========================================   
    /**
     * Sets the tree node that causes the data panel shown.
     * 
     * @param node	An instance of {@link CommonTreeNode} which the data panel is for.
     */
    public void setRelatedNode(CommonTreeNode node) {
    	relatedNode = node;
    	updateDisplayData();
    }
  
    //========================================
    //  Methods
    //========================================
   /**
    * Builds the data panel GUI for the specified tab type.
    * 
    * @param tabType	The tab type which the data panel is for.
    * 
    */
    public void buildGUIRepresentation(QPTabType tabType) {
    	// this button is displayed on top of notebook area as users may not notice
    	// such button if it's displayed at the bottom
    	JButton applyChangeButton = new JButton("Apply Change");
    	applyChangeButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				applyChange();
			}
		});
    	applyChangeButton.setToolTipText("Click to apply changes made");
    	applyChangeButton.setMnemonic(KeyEvent.VK_C);
    	// Disable the border so that the onclick color for this button would work on Mac.
    	// TODO: revisit to see if it allows to use border while not affecting onclick color appearance
    	//Border compoundBorder = new CompoundBorder(BorderFactory.createMatteBorder(2, 5, 2, 5, new Color(79, 148, 205)), new EmptyBorder(5, 10, 5, 10));
    	//applyChangeButton.setBorder(compoundBorder);
    	addTwoCompsInOneRow(new JLabel(""), applyChangeButton);
    	
    	String[] labels = getAllLabels(tabType);
    	if (labels != null) {
    		for (int i = 0; i < labels.length; i++) {
    			DataRepresentationType dataFieldType = DataRepresentationType.PLAIN_TEXT_FIELD;
    			switch (tabType) {
					case QP_PAGE_TAB:
						dataFieldType = ProductPage.getDataFieldType(labels[i]);
				  	  	break;
					case QP_PLOT_TAB:
						dataFieldType = ProductPlot.getDataFieldType(labels[i]);
				  	  	break;
					case QP_TABLE_TAB:
						dataFieldType = ProductTable.getDataFieldType(labels[i]);
						break;
					case QP_COLUMN_TAB:
						dataFieldType = ProductColumn.getDataFieldType(labels[i]);
						break;
					case QP_IN_TAB:
					case QP_X_VAR_TAB:
					case QP_Y_VAR_TAB:
					case QP_TABLE_VAR_TAB:
						dataFieldType = ProductVar.getDataFieldType(labels[i]);
						break;
					case QP_PROGRAM_TAB:
						dataFieldType = ProductExternalFunction.getDataFieldType(labels[i]);
					    break;
					case QP_OUT_TAB:
						dataFieldType = ProductMeasurement.getDataFieldType(labels[i]);
						break;
    			}
    			addRow(labels[i], dataFieldType);
    		}            
    	}
    }
   
    /**
     * Helper method for getting all labels for the specified tab.
     */
    private String[] getAllLabels(QPTabType tabType) {
    	String[] labels = null;
    	switch (tabType) {
	    	case QP_PAGE_TAB:
	    		labels = ProductPage.GUI_FIELD_LABELS;
			   	break;
	    	case QP_PLOT_TAB:
	    		labels = ProductPlot.GUI_FIELD_LABELS;
			   	break;
	    	case QP_TABLE_TAB:
	    		labels = ProductTable.GUI_FIELD_LABELS;
	    		break;
	    	case QP_X_VAR_TAB:
	    	case QP_TABLE_VAR_TAB :
	    		labels = ProductVar.GUI_FIELD_LABELS_TABLE;
	    		break;
	    	case QP_Y_VAR_TAB:
	    		labels = ProductVar.GUI_FIELD_LABELS_FULL;
	    		break;
	    	case QP_COLUMN_TAB:
	    		labels = ProductColumn.GUI_FIELD_LABELS;
	    		break;
	    	case QP_PROGRAM_TAB:
	    		labels = ProductExternalFunction.GUI_FIELD_LABELS;
    			break;
	    	case QP_IN_TAB:
	    		labels = ProductVar.GUI_FIELD_LABELS_PROGRAM;
	    	    break;
	    	case QP_OUT_TAB:
	    		labels = ProductMeasurement.GUI_FIELD_LABELS;
	    	    break;
    	}
    	return labels;
    }
   
    /**
     * Helper method for applying the node data to GUI.
     */
	@SuppressWarnings("rawtypes")
	private void updateDisplayData() {
    	// go through all dataFields
    	for (int i = 0; i < dataFields.size(); i++) {
    		JComponent thisField = dataFields.get(i);
    		
    		if (thisField instanceof JTextField) {
    			((JTextField)thisField).setText(getDataText(i));
    		} else if (thisField instanceof JComboBox) {
    			((JComboBox)thisField).setSelectedItem(getDataText(i));
    		} else if (thisField instanceof JLabel) {
    			Color color = null;
    			try {
    				color = UIUtils.getColorFromHTMLHex(getDataText(i));
    			} catch (NumberFormatException nfe) {
    				// do nothing
    			}
    			if (color != null) {
    				thisField.setBackground(color);
    				((JLabel) thisField).setText(UIUtils.colorToHTMLHex(color));
    			}
    		}
    	}
    	
    	if (unitsField != null) {
    		// if unitsField has units text, don't need fromUnitsField
    		if (fromUnitsField != null) {
	    		if (unitsField.getText() != null && !unitsField.getText().trim().isEmpty()) {
	    			fromUnitsField.setEnabled(false);
	    			unitsField.setEditable(false);
	    		} else {
	    			// otherwise, enable fromUnitsField since units in not provided from recording data
	    			fromUnitsField.setEnabled(true);
	    			unitsField.setEditable(true);
	    		}
    		}
    	}
    }
    
    /**
     * Helper method for applying the index-th data to corresponding GUI data field.
     */
    private String getDataText(int index) {
    	Object data = null;
    	int nodeType = relatedNode.getNodeType();
    	switch (nodeType) {
	   		case CommonTreeNode.PAGE_NODE:
	   			data = ((ProductPage)relatedNode.getUserObject()).getData(index);
	   			break;
	   		case CommonTreeNode.PLOT_NODE:
	   			data = ((ProductPlot)relatedNode.getUserObject()).getData(index);
	   			break;
	   		case CommonTreeNode.TABLE_NODE:
	   			data = ((ProductTable)relatedNode.getUserObject()).getData(index);
	   			break;
	   		case CommonTreeNode.COLUMN_NODE:
	   			data = ((ProductColumn)relatedNode.getUserObject()).getData(index);
	   			break;
	   		case CommonTreeNode.PROGRAM_NODE:
	   			data = ((ProductExternalFunction)relatedNode.getUserObject()).getData(index);
	   			break;
	   		case CommonTreeNode.OUTPUT_VAR_NODE:
	   			data = ((ProductMeasurement)relatedNode.getUserObject()).getData(index);
	   			break;
	   		case CommonTreeNode.INPUT_VAR_NODE:
	   			data = ((ProductVar)relatedNode.getUserObject()).getData(ProductVar.UNITS_INDEX);
	   			break;
			case CommonTreeNode.VAR_NODE:
			case CommonTreeNode.X_NODE:
			case CommonTreeNode.Y_NODE:
			case CommonTreeNode.Z_NODE:
				data = ((ProductVar)relatedNode.getUserObject()).getData(index);
				break;
    	}
    	if (data == null) {
    		return null;
    	} else {
    		return data.toString();
    	}
    }
    
    /**
     * Helper method for updating data for the related tree node.
     */
    @SuppressWarnings("rawtypes")
	private void applyChange() {
    	// Changes will not be accepted if units fail
    	if (!areUnitsCompatible()) {
    		return;
    	}
    	
    	// go through all dataFields
    	for (int i = 0; i < dataFields.size(); i++) {
    		JComponent thisField = dataFields.get(i);
    		if (thisField instanceof JTextField) {
    			if (thisField instanceof NumberTextField) {
    				NumberTextField numberField = (NumberTextField)thisField;
    				transferDataToNode(i, numberField.getValue());
    				if (numberField.getValue() != null) {
    					numberField.setText(numberField.getValue().toString());
    				}
    			} else {
    				transferDataToNode(i, ((JTextField)thisField).getText());
    			}
    		} else if (thisField instanceof JComboBox) {
    			transferDataToNode(i, ((JComboBox)thisField).getSelectedItem());
    		} else if (thisField instanceof JLabel) {
    			transferDataToNode(i, ((JLabel)thisField).getText());
    		}
    	}
    }
    
    /**
     * Helper method for passing the change to the related tree node.
     */
    private void transferDataToNode(int index, Object value) {
    	int nodeType = relatedNode.getNodeType();
    	switch (nodeType) {
	   		case CommonTreeNode.PAGE_NODE:
	   			((ProductPage)relatedNode.getUserObject()).setData(index, value);
	   			break;
	   		case CommonTreeNode.PLOT_NODE:
	   			((ProductPlot)relatedNode.getUserObject()).setData(index, value);
	   			break;
	   		case CommonTreeNode.TABLE_NODE:
	   			((ProductTable)relatedNode.getUserObject()).setData(index, value);
	   			break;
	   		case CommonTreeNode.COLUMN_NODE:
	   			((ProductColumn)relatedNode.getUserObject()).setData(index, value);
	   			break;
	   		case CommonTreeNode.PROGRAM_NODE:
	   			((ProductExternalFunction)relatedNode.getUserObject()).setData(index, value);
	   			break;
	   		case CommonTreeNode.OUTPUT_VAR_NODE:
	   			((ProductMeasurement)relatedNode.getUserObject()).setData(index, value);
	   			break;
	   		case CommonTreeNode.INPUT_VAR_NODE:
	   			((ProductVar)relatedNode.getUserObject()).setData(ProductVar.UNITS_INDEX, value);
	   			break;
			case CommonTreeNode.VAR_NODE:
			case CommonTreeNode.X_NODE:
			case CommonTreeNode.Y_NODE:
			case CommonTreeNode.Z_NODE:
				((ProductVar)relatedNode.getUserObject()).setData(index, value);
	   			break;
    	}
    	(TrickQPApplication.getInstance(TrickQPApplication.class)).productTree.getDefaultModel().nodeChanged(relatedNode);
    }
    
    /**
     * Helper method for adding one row of data field to the panel.
     * The row is shown such as: "label" "JComponent".    
     * 
     * @param label	       The label of the data field.
     * @param dataFieldType  The text that determines what kind of {@link JComponent} needs using.
     */
     //for Java 7, the type of elements of JComboBox needs to be specified to avoid the warning and it's not supported in Java 6
    @SuppressWarnings({ "unchecked", "rawtypes" })
    private void addRow(String label, DataRepresentationType dataFieldType) {
    	JComponent comp = null;
 	   	JLabel jLabel = new JLabel(label);
 	   	boolean needBrowse = false;
 	   	
 	   	switch (dataFieldType) {
 	   		case COLOR_LABEL:
 	   			comp = getColorLabel();
 	   			break;
 	   		case NUMBER_FIELD:
 	   			comp = new NumberTextField();
 	   			break;
 	   		case WHOLE_NUMBER_FIELD:
 	   			comp = new NumberTextField(true);
 	   			break;
 	   		case UNITS_FIELD:	   			
 	   			setupUnitsField();
 	   			comp = unitsField;
 	   			unitsField.addKeyListener(this);
 	   			break;
 	   		case FROM_UNITS_FIELD:
 	   			comp = new JTextField();
 	   			fromUnitsField = ((JTextField)comp);
 	   			comp.addKeyListener(this);
 	   			break;
 	   		case FONT_FIELD: 	   			
 	   			setupFontField();
 	   			comp = fontField; 	   			
 	   			break;
 	   		case PROGRAM_FIELD: 	   			
 	   			needBrowse = true;
 	   			comp = new JTextField("");
 	   			comp.setPreferredSize(new Dimension(100, 30));
 	   			programField = ((JTextField)comp);			      
 	   			((JTextField)comp).setText("PROGRAM");
 	   			break;
 	   		case COMBO_BOX:
 	   			if (dataFieldType.getComboBoxItems() != null && dataFieldType.getComboBoxItems().length > 0) {
 	   				comp = new JComboBox(dataFieldType.getComboBoxItems());
 	   			} else {
 	   				comp = new JComboBox();
 	   			}
 	   			break;
 	   		case PLAIN_TEXT_FIELD:
 	   			comp = new JTextField();
 	   			break;
 	   	}
 	   
 	   	dataFields.add(comp);
        
 	   	addTwoCompsInOneRow(jLabel, comp);
 	   	if (needBrowse) {
 	   		addTwoCompsInOneRow(new JLabel(""), getBrowseButton());
 	   	}
    }
       
    /**
     * Helper method to add 2 components in one row side-by-side.
     */
    private void addTwoCompsInOneRow(JComponent comp1, JComponent comp2) {
        hParallelGroup1.addComponent(comp1);
 	    hParallelGroup2.addComponent(comp2);
 		
 	    GroupLayout.ParallelGroup vParallelGroup = groupLayout.createParallelGroup(GroupLayout.Alignment.BASELINE);  
 	    vParallelGroup.addComponent(comp1);
 	    vParallelGroup.addComponent(comp2);
 	    
 	    verticalGroup.addGroup(vParallelGroup);
    }    
    
    /**
     * Gets the first curve of the related node if applicable.
     * 
     * @return The first curve of the related node if the related node
     *         is an instance of {@link ProductPlot} that also has at 
     *         least one curve. Otherwise, <code>null</code>.
     */
    private ProductCurve getCurveOfRelatedNode() {
    	ProductCurve firstCurve = null;
    	if (relatedNode.getUserObject() instanceof ProductPlot) {
    		ProductPlot plot = (ProductPlot)(relatedNode.getUserObject());
        	if (plot.getCurveList() != null && plot.getCurveList().size() > 0) {
        		firstCurve = plot.getCurveList().get(0);
        	}
    	}
    	return firstCurve;
    }
    
    /**
     * Checks units specified by From Units and Units fields to see they are compatible.
     */
    private boolean areUnitsCompatible() {
    	// only need to check when From Units is enabled
    	if (fromUnitsField != null && fromUnitsField.isEnabled()) {
    		// if both From units and units are empty, no need to check the compatibility
    		if ( (fromUnitsField.getText() == null || fromUnitsField.getText().trim().isEmpty()) &&
    		    (unitsField.getText() == null || unitsField.getText().trim().isEmpty())	) {
    			return true;
    		} else {
    			// check to see if From units and units are exchangeable
			    if (!UnitType.isConvertible(fromUnitsField.getText(), unitsField.getText())) {
				    JOptionPane.showMessageDialog(this,
                                                  "The From Units \"" + fromUnitsField.getText() + "\" and the Units \"" + unitsField.getText() + "\" are either incompatible or invalid!",
                                                  "Improper Units",
                                                  JOptionPane.WARNING_MESSAGE);
				    fromUnitsField.selectAll();
				    fromUnitsField.requestFocus();
				    return false;
			    } else {
			    	return true;
			    }
		    }
    	}
    	return true;
    }
    
    /**
     * Verify a specified units and show a warning message if it is invalid.
     */
    private void verifyUnits(String units) {
    	if (UnitType.getAll(units).isEmpty()) {
			JOptionPane.showMessageDialog(this,
                                         "\"" + units + "\" is invalid. Please enter a valid units!",
                                         "Invaid Units",
                                         JOptionPane.WARNING_MESSAGE);
		}
    }
    
    /** 
     * Handles the key typed event from the text field. 
     * 
     * Required by {@link KeyListener}.
     */
    public void keyTyped(KeyEvent e) {   	
    	// do nothing
    }
    
    /** 
     * Handles the key pressed event from the text field. 
     * 
     * Required by {@link KeyListener}.
     */
    public void keyPressed(KeyEvent e) {
    	if (e.getKeyCode() == KeyEvent.VK_ENTER) {
    		if (e.getComponent() == fromUnitsField) {
    			if (fromUnitsField.getText() != null) {
    				verifyUnits(fromUnitsField.getText());
    			}
    		} else if (e.getComponent() == unitsField) {
    			if (unitsField.getText() != null) {
    				verifyUnits(unitsField.getText());
    			}
    		}
    	}
    }
    
    /** 
     * TODO: Handles the key released event from the text field. 
     * 
     * Required by {@link KeyListener}.
     */
    public void keyReleased(KeyEvent e) {
        // do nothing
    }
    
    /**
     * Gets a {@link JButton} for showing file chooser to select a desired file.
     * 
     * @return	An instance of {@link JButton} with showing a file chooser action injected.
     */
    private JButton getBrowseButton() {
    	final JButton comp = new JButton(BROWSE_FILE);
		comp.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				File file = UIUtils.chooseOpenFile(null, null, "so", comp);
				if (file != null && programField != null) {
					programField.setText(file.getPath());
				}
			}
		});
		return comp;
    }
    
    /**
     * Gets a {@link JLabel} for showing color chooser to select a desired color. 
     * The background of this label shows the selected color.
     * 
     * @return	An instance of {@link JLabel} with showing a color chooser action injected.
     */
    private JLabel getColorLabel() {
    	final JLabel colorLabel = new JLabel(CommonProduct.DEFAULT_COLOR_TEXT, JLabel.CENTER);
    	Dimension d = colorLabel.getPreferredSize();
    	Dimension newD = new Dimension(d.width + 20, d.height + 10);
    	colorLabel.setPreferredSize(newD);
    	colorLabel.setMaximumSize(newD);
    	colorLabel.setMinimumSize(newD);
    	
    	colorLabel.setOpaque(true);
    	colorLabel.setBorder(new SoftBevelBorder(SoftBevelBorder.RAISED));
    	
    	colorLabel.addMouseListener(new MouseAdapter() {
    		@Override
			public void mouseClicked(MouseEvent e) {
    			Color newColor = JColorChooser.showDialog(colorLabel, "Choose Color", colorLabel.getBackground());
    			if (newColor != null) {
    				colorLabel.setBackground(newColor);
    				colorLabel.repaint();
    				colorLabel.setText(UIUtils.colorToHTMLHex(newColor));
    			}
    		}
    	});
    	return colorLabel;
    }
    
    /**
     * Helper method for units data field setup.
     */
    private void setupUnitsField() {
    	unitsField = new JTextField();
    	unitsField.setEditable(false);
    	unitsField.addMouseListener(new MouseAdapter() {
    		@Override
			public void mouseClicked(MouseEvent e) {
    			ProductVar selectedVar = null;
				if (relatedNode.getUserObject() instanceof ProductVar) {
					selectedVar = (ProductVar)relatedNode.getUserObject();
				}
		    	
				if (selectedVar != null) {
					String selectedVarUnits = selectedVar.getUnits();
					if (selectedVarUnits != null && !selectedVarUnits.trim().isEmpty()) {
						List<String> unitList = UnitType.getAll(selectedVarUnits);
		    		    if (unitList != null && unitList.size() > 0) {
		    		    	Object newUnits = UIUtils.showListInputDialog(unitsField, 
		    		    			                                      "Change " + selectedVarUnits + " to:", 
		    		    			                                      "Change Units ...", JOptionPane.QUESTION_MESSAGE, 
		    		    			                                      null, 
		    		    			                                      unitList.toArray(), 
		    		    			                                      selectedVarUnits);
		    			    if (newUnits != null) {
		    				    unitsField.setText(newUnits.toString());
		    			    }
		    		    }   		
					}
				}
    		}
    	});
    }
    
    /**
     * Helper method for font data field setup.
     */
    private void setupFontField() {
    	fontField = new JTextField();
    	fontField.setPreferredSize(new Dimension(100, 30));			
    	fontField.setEditable(false);			   
    	fontField.setText("Default");
    	fontField.addMouseListener(new MouseAdapter() {
    		@Override
			public void mouseClicked(MouseEvent e) {
    			Font font = FontChooser.showDialog(fontField, "Choose Font", getFontFromText(fontField.getText()));
    			if (font != null) {
    				// The plot utility requires the font string format as 
    				// e.g. "-*-helvetica-bold-r-*--10-*-*-*-*-*-*-*"
    				StringBuffer tmpBuf = new StringBuffer();
    				if (font.isBold()) {
    					tmpBuf.append("bold");
    				} else {
    					tmpBuf.append("medium");
    				}
    				tmpBuf.append("-");
    				if (font.isItalic()) {
    					if (font.getFamily().equalsIgnoreCase("helvetica")) {
    						tmpBuf.append("o");
    					} else {
    						tmpBuf.append("i");
    					}
    				} else {
    					tmpBuf.append("r");
    				}
    				fontField.setText("-*-" + font.getFamily() + "-" + tmpBuf.toString() + "-*--" + font.getSize() + "-*-*-*-*-*-*-*");
    			}
    		}
    	});
    }
    
    /**
     * Helper method for generating an instance of {@link Font} for the specified font text.
     * 
     * The format of the font text should be "-*-font family-weight-slant-*--size-*-*-*-*-*-*-*".
     * @param fontText name of the font
     * @return font object
     */
    public static Font getFontFromText(String fontText) {
    	String[] attrs = null;
    	
    	// after the split, 
    	// attrs[2]: font family
    	// attrs[3]: weight
    	// attrs[4]: slant
    	// attrs[7]: size
    	attrs = fontText.split("-");
    	
    	if (fontText.equalsIgnoreCase("Default") || attrs == null || attrs.length < 7) {
    		return null;
    	}
    	
    	int fontStyle = Font.PLAIN;
    	if (attrs[3].equalsIgnoreCase("bold")) {
    		fontStyle += Font.BOLD;
    	}
    	if (!attrs[4].equalsIgnoreCase("r")) {
    		fontStyle += Font.ITALIC;
    	}
    	    	   	
    	return new Font(attrs[2], fontStyle, Integer.parseInt(attrs[7]));
    }
}
