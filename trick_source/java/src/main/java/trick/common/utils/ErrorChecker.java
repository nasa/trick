
//========================================
//	Package
//========================================
package trick.common.utils;

//========================================
//	Imports
//========================================
import org.xml.sax.SAXParseException;
import org.xml.sax.helpers.DefaultHandler;


/**
 * Defines erro handler to deal with irregularities that come up
 * during the parsing of a XML document.
 * 
 *
 * @since Trick 10
 */
public class ErrorChecker extends DefaultHandler{
	
	//========================================
	//	Public data
	//========================================
	
	
	//========================================
	//	Protected data
	//========================================
	
	
	//========================================
	//	Private Data
	//========================================
	// Exception message
	private String exceptionMessage;
	
	
	//========================================
	//	Constructors
	//========================================
	public ErrorChecker(){
		
	}

	//========================================
	//	Set/Get methods
	//========================================
	
	
	//========================================
	//	Methods
	//========================================	
	public void error(SAXParseException e) {
		exceptionMessage = "Parsing error: " + e.getMessage();
		//System.out.println(exceptionMessage);		
	}
	
	public void warning(SAXParseException e) {
		exceptionMessage = "Parsing error: " + e.getMessage();
		//System.out.println(exceptionMessage);
	}
	
	public void fatalError(SAXParseException e) {
		exceptionMessage = "Parsing error: " + e.getMessage();
		//System.out.println(exceptionMessage);
		//System.out.println("Cannot continue.");
		//System.exit(1);
	}
	
	/**
	 * Gets the exception message.
	 * 
	 * @return a text message describing the exception.
	 */
	public String getExceptionMessage() {
		return exceptionMessage;
	}
	
}
