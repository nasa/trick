
//========================================
//	Package
//========================================
package trick.common.ui.components;

import javax.swing.JTextField;
import javax.swing.text.AttributeSet;
import javax.swing.text.BadLocationException;
import javax.swing.text.Document;
import javax.swing.text.PlainDocument;


/**
 * An extension of {@link JTextField} that only accepts numbers.
 * 
 * 
 * @since Trick 10
 */
public class NumberTextField extends JTextField {

	
	//========================================
	//	Public data
	//========================================
	
	
	//========================================
	//	Protected data
	//========================================
	
	
	//========================================
	//	Private Data
	//========================================
	private static final long serialVersionUID = -9181145899991928113L;

	// By default, this text field is good for double unless intOnly is set to be true.
	// TODO: should support all Number types?
	private boolean intOnly = false;
	
	//========================================
	//	Constructors
	//========================================
	/**
	 * Default constructor.
	 */
	public NumberTextField() {
		super();
	}
	
	/**
	 * Constructs a new empty text field only for integers.
	 * 
	 * @param isIntOnly		Whether this text field is only for integers.
	 */
	public NumberTextField(boolean isIntOnly) {
		this();
		intOnly = isIntOnly;		
	}
	
	/**
	 * Constructs a new empty text field with the specified number of columns.
	 * 
	 * @param columns	The number of columns to use to calculate the preferred width.
	 */
	public NumberTextField(int columns) {
		super(columns);
	}

	/**
	 * Constructs a new text field with the specified text. If the text represents 
	 * an invalid double, only set text columns to a default number 5.
	 * 
	 * @param text		The text that represents a valid number to be displayed.
	 */
	public NumberTextField(String text) {
		super(text);
		if (getValue() == null) {
			setColumns(5);
		}
	}
	
	/**
	 * Constructs a new text field with the specified text. If the text represents 
	 * an invalid double, only set text clolumns to the specified number of columns.
	 * 
	 * @param text		The text that represents a valid number to be displayed.
	 * @param columns	The number of columns to use to calculate the preferred width.
	 */
	public NumberTextField(String text, int columns) {
		super(text, columns);
	}
	
	
	//========================================
	//	Set/Get methods
	//========================================	
	/**
	 * Limits the text field only for integers.
         * @param intOnly true or false
	 */
	public void setIntOnly(boolean intOnly) {
		this.intOnly = intOnly;
	}
	
	/**
	 * Gets value of the text field.
	 * 
	 * @return the {@link Number} of this text field if it's valid.
	 */
	public Number getValue() {
        try {
        	if (intOnly) {
        		return Integer.valueOf(getText());
        	}
        	return Double.valueOf(getText());
    	} catch (NumberFormatException e) {
            return null;
        } catch (NullPointerException e) {
        	return null;
        }
    }
    
 
	//========================================
	//	Methods
	//========================================
    /**
     * Uses the number document for this number text field.
     */
    @Override
	protected Document createDefaultModel() {
    	return new NumberTextDocument();
    }
    
    /**
     * Checks to see if any input is valid based on double data type.
     * 
     * @return <code>true</code> if any input can be part of a number, 
     *         <code>false</code> otherwise.
     */
    @Override
	public boolean isValid() {
    	try {   	 
    		if (intOnly) {
    			Integer.parseInt(getText());
    		} else {
    			Double.parseDouble(getText());
    		}
    		return true;
    	} catch (NumberFormatException e) {
    	    return false;
    	} catch (NullPointerException e) {
    		return false;
    	}
    }
    
    
    //========================================
	//	Inner classes
	//========================================
    /**
     * Class for valid number document.
     */
    class NumberTextDocument extends PlainDocument {
    	
    	
		private static final long serialVersionUID = -6195020045069585237L;

		@Override
		public void insertString(int offs, String str, AttributeSet a) throws BadLocationException {
        	if (str == null) {
        		return;
        	}
        	String oldString = getText(0, getLength());
        	String newString = oldString.substring(0, offs) + str + oldString.substring(offs);
        	try {
        		if (intOnly) {
        			Integer.parseInt(newString + "0");
        		} else {
        			Double.parseDouble(newString + "0");
        		}
        		super.insertString(offs, str, a);
        	} catch (NumberFormatException e) {
        	}
        }
    }
    
}
