
//========================================
//    Package
//========================================
package trick.dataproducts.utils;

//========================================
//    Imports
//========================================
import java.util.ArrayList;
import java.util.List;
import java.io.IOException;
import java.io.File;

import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.w3c.dom.Element;
import org.w3c.dom.NodeList;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;

/**
 * This XML parser uses Java XML DOM parser to parse a XML file.
 *
 *
 * @since Trick 10
 */
public class SessionDomParser {

    //========================================
    //    Constructors
    //========================================
    public SessionDomParser(){
    }

    //========================================
    //    Methods
    //========================================
    public static Session parse(File file)
     throws ParserConfigurationException, SAXException, IOException {
        // parse the source
        DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance() ;

        // Ignore the Session.dtd reference in the session xml file.
        dbf.setFeature("http://apache.org/xml/features/nonvalidating/load-dtd-grammar", false);
        dbf.setFeature("http://apache.org/xml/features/nonvalidating/load-external-dtd", false);

        final Element elem = dbf.newDocumentBuilder().parse(file).getDocumentElement();

        Session sessionObj = new Session(elem.getAttribute("version"), elem.getAttribute("presentation"));

        if (elem.hasAttribute("device")) {
            sessionObj.setDevice(elem.getAttribute("device"));
        }

        if (elem.hasAttribute("mode")) {
            sessionObj.setMode(elem.getAttribute("mode"));
        }

        if (elem.hasAttribute("gnuplot_terminal")) {
            sessionObj.setGnuplotTerminal(elem.getAttribute("gnuplot_terminal"));
        }

        if (elem.hasAttribute("machine")) {
            sessionObj.setMachine(elem.getAttribute("machine"));
        }
        if (elem.hasAttribute("port")) {
            sessionObj.setPort(Integer.parseInt(elem.getAttribute("port")));
        }

        sessionObj.setTStart(Double.parseDouble(getTextValue(elem, "tstart")));
        sessionObj.setTStop(Double.parseDouble(getTextValue(elem, "tstop")));
        sessionObj.setFrequency(Double.parseDouble(getTextValue(elem, "frequency")));

        // Retrieve all run elements
        NodeList childElements = elem.getElementsByTagName("run");
        if (childElements != null && childElements.getLength() > 0) {
            for (int i = 0 ; i < childElements.getLength(); i++) {
                //get the element
                Element runElem = (Element)childElements.item(i);
                sessionObj.addRun(getRun(runElem));
            }
        }

        childElements = elem.getElementsByTagName("product_files");
        if (childElements != null && childElements.getLength() > 0) {
            // only one product_files element
            Element productFilesElem = (Element)childElements.item(0);
            sessionObj.setProductFiles(getTextListValue(productFilesElem, "file"));
        }

        return sessionObj;
    }

    protected static String getTextValue(Element elem, String tagName) {
        String textVal = null;
        NodeList nl = elem.getElementsByTagName(tagName);
        if(nl != null && nl.getLength() > 0) {
            Element el = (Element)nl.item(0);
            // making sure that the el is elem's direct child
            if (((Element)el.getParentNode()).equals(elem)) {
                textVal = el.getFirstChild().getNodeValue();
            }
        }

        return textVal;
    }

    protected static List<String> getTextListValue(Element elem, String tagName) {
        List<String> textValList = null;
        // the occurrence of tagName is one or more
        NodeList nl = elem.getElementsByTagName(tagName);
        if (nl != null && nl.getLength() > 0) {
            textValList = new ArrayList<String>();
            for (int i = 0; i < nl.getLength(); i++) {
                Element theNode = (Element)nl.item(i);
                textValList.add(theNode.getFirstChild().getNodeValue());
            }
        }

        return textValList;
    }

    /**
     * Gets a {@link SessionRun} instance based on the run element.
     *
     * @param runElem    The run element.
     *
     * @return An instance of {@link SessionRun}
     */

    private static SessionRun getRun(Element runElem) {
        // only one dir under each run
        String runDir = getTextValue(runElem, "dir");
        SessionRun theRun = new SessionRun(runDir);
        if (runElem.hasAttribute("timename")) {
            theRun.setTimename(runElem.getAttribute("timename"));
        }
        return theRun;
    }


}
