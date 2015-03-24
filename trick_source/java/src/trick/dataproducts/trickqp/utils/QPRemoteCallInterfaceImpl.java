
//========================================
//  Package
//========================================
package trick.dataproducts.trickqp.utils;

//========================================
//  Imports
//========================================
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
import java.util.Arrays;
import java.util.List;

import trick.dataproducts.trickqp.TrickQPApplication;
import trick.dataproducts.utils.SessionRun;

/**
 * Implementation for remote calles needed for trick_qp.
 * Basically, other application can make these calls to 
 * trick_qp applicaiton if necessary.
 * 
 * @since Trick 10
 */
public class QPRemoteCallInterfaceImpl extends UnicastRemoteObject implements QPRemoteCallInterface {
    //========================================
    //  Public data
    //========================================
    
    
    //========================================
    //  Protected data
    //========================================
	
	
    //========================================
    //  Private Data
    //========================================
	private TrickQPApplication application;
	
    //========================================
    //  Constructors
    //========================================
	public QPRemoteCallInterfaceImpl() throws RemoteException {
		application = TrickQPApplication.getInstance(TrickQPApplication.class);
	}
    
    //========================================
    //  Set/Get methods
    //======================================== 

    
    //========================================
    //  Methods
    //========================================
	/**
	 * Updates the run list for trick_qp through another application.
	 */
	public void updateRunList(String[] dirList) throws RemoteException {

		if (dirList != null ) {
			boolean needUpdate = false;
			Object[] allExistingRuns = application.runList.getAllData();
			if (allExistingRuns != null) {
				int len = allExistingRuns.length;
				String[] existingDirs = new String[len];
				for (int i = 0; i < len; i++) {
					SessionRun eachRun = (SessionRun)allExistingRuns[i];
					existingDirs[i] = eachRun.getDir();
				}
				List<String> allExistingDirList = Arrays.asList(existingDirs);
				for (String eachDir : dirList) {
					if (!allExistingDirList.contains(eachDir)) {
						needUpdate = true;
						break;
					}
				}
			}
			if (needUpdate) {
				application.getActionController().handleAddRuns(dirList);
			}
		}
	}
	
}
