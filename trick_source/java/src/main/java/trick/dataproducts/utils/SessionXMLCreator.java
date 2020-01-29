
//========================================
//	Package
//========================================
package trick.dataproducts.utils;

//========================================
//	Imports
//========================================
import java.util.Iterator;
import java.util.List;

import org.w3c.dom.Element;

import trick.common.utils.XMLCreator;

/**
 * This XML creator for creating {@link Session} XML file 
 * using Java API for XML Processing.
 * 
 * @author Hong Chen
 * @since Trick 10
 */
public class SessionXMLCreator extends XMLCreator {
	
	//========================================
	//	Public data
	//========================================
	
	
	
	//========================================
	//	Protected data
	//========================================
	
	
	
	//========================================
	//	Private Data
	//========================================
	
	
	
	
	//========================================
	//	Constructors
	//========================================
	public SessionXMLCreator(){
		super();
	}

	//========================================
	//	Set/Get methods
	//========================================
	
	
	
	//========================================
	//	Methods
	//========================================	
	/**
	 * This shall create the desired XML structure.
	 * Required by XMLCreator.
	 * 
	 * @return true if root object is created successfully, false otherwise. 
	 * 
	 * @see trick.common.utils.XMLCreator
	 */
	@Override
	protected boolean createDOMTree() {
		//create the root element and session XML file only has one <session> element.
		Element rootElem = dom.createElement("session");
		dom.appendChild(rootElem);

		if (rootObj instanceof Session) {
			Session sessionObj = (Session)rootObj;
			
			// session attributes
			rootElem.setAttribute("version", sessionObj.getVersion());
			rootElem.setAttribute("presentation", sessionObj.getPresentation());
			
			if (sessionObj.getDevice() != null && !sessionObj.getDevice().isEmpty()) {
			    if (sessionObj.getDevice().equals(Session.DEVICE_OPTIONS[Session.FILE_DEVICE])) {
			        rootElem.setAttribute("device", sessionObj.getDevice()+":"+sessionObj.getDeviceFileName());
			    } else {
			        rootElem.setAttribute("device", sessionObj.getDevice());
			    }
			}
			
			if (sessionObj.getGnuplotTerminal() != null && !sessionObj.getGnuplotTerminal().isEmpty()) {
				rootElem.setAttribute("gnuplot_terminal", sessionObj.getGnuplotTerminal());
			}
			
			if (sessionObj.getMode() != null && !sessionObj.getMode().isEmpty()) {
				rootElem.setAttribute("mode", sessionObj.getMode());
			}
			
			// for <tstart> ... </tsart>
			createChildElement(rootElem, "tstart", Double.toString(sessionObj.getTStart()));
			// for <tstop> ... </tstop>
			createChildElement(rootElem, "tstop", Double.toString(sessionObj.getTStop()));
			// for <frequency> ... </frequency>
			createChildElement(rootElem, "frequency", Double.toString(sessionObj.getFrequency()));
						
			// for <run> ...</run>
			List<SessionRun> runs = sessionObj.getRuns();
			if (runs != null) {
				Iterator<SessionRun> it = runs.iterator();
				while (it.hasNext()) {
					SessionRun sessionRun = it.next();					
					createSessionRunElement(rootElem, sessionRun);
				}
			}
			// for <product_files> ... </product_files>
			createProductFilesElement(rootElem, sessionObj.getProductFiles());
		}
		
		return true;
	}
	
	/**
	 * Creates a run {@link Element} and appends to <session> element.
	 * 
	 * @param run	The {@link SessionRun} needs creating the XML representation.
	 * 
	 */
	private void createSessionRunElement(Element rootElem, SessionRun run) {
		Element sessionRunElem = dom.createElement("run");
		if (run.getTimename() != null && !("".equals(run.getTimename()))) {
			sessionRunElem.setAttribute("timename", run.getTimename());
		}
		
		// create dir element and dir text node and attach it to seesionRunElem
		// <dir> ... </dir>
		if (run.getDir() != null) {
			createChildElement(sessionRunElem, "dir", run.getDir());
		}
		rootElem.appendChild(sessionRunElem);
	}
	
	
	/**
	 * Creates a product_files {@link Element} and appends to <session> element.
	 * 
	 * @param files	A {@link List} of files for <product_files> element.
	 */
	private void createProductFilesElement(Element rootElem, List files) {
		if (files != null) {
			Iterator it = files.iterator();
			Element productFilesElem = dom.createElement("product_files");
			while (it.hasNext()) {
				String filePath = (String)it.next();
				// <file> ... </file>
				createChildElement(productFilesElem, "file", filePath);
			}
			rootElem.appendChild(productFilesElem);
		}
	}
}
