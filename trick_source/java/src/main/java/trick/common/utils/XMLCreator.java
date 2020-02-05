
//========================================
//	Package
//========================================
package trick.common.utils;

//========================================
//	Imports
//========================================
import java.io.FileOutputStream;
import java.io.IOException;

import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Text;



/**
 * This XML creator using Java API for XML Processing.
 * 
 *
 * @since Trick 10
 */
public abstract class XMLCreator {
	
	//========================================
	//	Public data
	//========================================
	
	
	
	//========================================
	//	Protected data
	//========================================
	protected Document dom;
	protected Object rootObj;
	
	//========================================
	//	Private Data
	//========================================
	private String xmlFile;
	private String dtdFile;
	private String publicID;
	
	private JFrame callerMainFrame;
	
	
	//========================================
	//	Constructors
	//========================================
	public XMLCreator(){
		createDocument();
	}

	//========================================
	//	Set/Get methods
	//========================================
	/**
	 * Sets the XML file for parsing.
	 * 
	 * @param xmlFile	The XML file name.
	 */
	public void setXMLFile(String xmlFile) {
		this.xmlFile = xmlFile;
	}
	
	/**
	 * Sets the root object of the XML.
	 * 
	 * @param obj	The root object.
	 */
	public void setRoot(Object obj) {
		rootObj = obj;
	}
	
	/**
	 * Sets the DTD file for the XML.
	 * 
	 * @param dtdFile	The DTD file name for the XML.
	 */
	public void setDTDFile(String dtdFile){
		this.dtdFile = dtdFile;
	}
	
	/**
	 * Sets the DTD public identifier.
	 * 
	 * @param publicID	The public identifier for the XML file.
	 */
	public void setPublicID(String publicID) {
		this.publicID = publicID;
	}
	
	/**
	 * Sets the frame that calls the XMLCreator to generate XML file.
         * @param frame the frame
	 */
	public void setCallerFrame(JFrame frame) {
	    callerMainFrame = frame;
	}
	
	//========================================
	//	Methods
	//========================================	
	/**
	 * Runs the XML creator to generate the XML file.
	 * 
	 * @return True if the file is generated successfully, false otherwise.
	 */
	public boolean runCreator() {
		if (createDOMTree()) {
		    printToFile();
		    return true;
		} else {
		    if (callerMainFrame != null) {
		        JOptionPane.showMessageDialog(callerMainFrame, 
                                              xmlFile + " could not be saved \n" + "because it doesn not have any plottable plot(s) or table(s) defined !", 
                                              "File Not Created",                         
                                              JOptionPane.WARNING_MESSAGE);
		    } else {
		        System.out.println(xmlFile + " could not be saved because it doesn not have any plottable plot(s) or table(s) defined !");
		    }
		    return false;
		}
	}
	
	
	/**
	 * Using JAXP in implementation independent manner create a document object
	 * using which we create a xml tree in memory.
	 */
	private void createDocument() {
		//get an instance of factory
		DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
		try {
			//get an instance of builder
			DocumentBuilder db = dbf.newDocumentBuilder();

			//create an instance of DOM
			dom = db.newDocument();

		} catch(ParserConfigurationException pce) {
			System.out.println("Error while trying to instantiate DocumentBuilder " + pce);
			System.exit(1);
		}
	}
	
	
	/**
	 * Creates a child {@link Element} of the specified parent {@link Element}.
	 * 
	 * @param parentElem	The parent element which needs creating a child element for.
	 * @param tagName		The tag name of the child element.
	 * @param tagText		The text for the child element.
	 * 
	 * @return A newly created element.
	 */
	protected Element createChildElement(Element parentElem, String tagName, String tagText) {
		if (tagText == null || "".equals(tagText.trim())) {
			return null;
		}
		Element childElem = dom.createElement(tagName);
		Text childText = dom.createTextNode(tagText);
		childElem.appendChild(childText);
		if (parentElem != null) {
			parentElem.appendChild(childElem);
		}
		return childElem;
	}
	
	/**
	 * Creates a child {@link Element} of the specified parent {@link Element}.
	 * 
	 * @param parentElem	The parent element which needs creating a child element for.
	 * @param tagName		The tag name of the child element.
	 * @param tagObject		The object for the child element.
	 * 
	 * @return A newly created element.
	 */
	protected Element createChildElement(Element parentElem, String tagName, Object tagObject) {
		if (tagObject == null) {
			return null;
		}
		return createChildElement(parentElem, tagName, tagObject.toString());
	}
	
	/**
	 * This shall create the desired XML structure.
         * @return true or false
	 */
	protected abstract boolean createDOMTree();
	
	
	/**
	 * Helper method which uses Java API for XML Processing 
	 * to print the XML document to file.
     */
	private void printToFile(){
		try
		{
			Transformer tr = TransformerFactory.newInstance().newTransformer();
			tr.setOutputProperty(OutputKeys.INDENT, "yes");
			tr.setOutputProperty(OutputKeys.METHOD,"xml");
			tr.setOutputProperty("{http://xml.apache.org/xslt}indent-amount", "4");
			tr.setOutputProperty(OutputKeys.ENCODING, "ISO-8859-1");
			tr.setOutputProperty(OutputKeys.DOCTYPE_PUBLIC, publicID);
			tr.setOutputProperty(OutputKeys.DOCTYPE_SYSTEM, dtdFile);

			//to send the output to a file
			tr.transform( new DOMSource(dom),new StreamResult(new FileOutputStream(xmlFile)));
			
			//to send the output to console
			//tr.transform( new DOMSource(dom),new StreamResult(System.out));

		} catch(IOException ie) {
		    ie.printStackTrace();
		} catch (TransformerException te) {
			
		}
	}
	
}
