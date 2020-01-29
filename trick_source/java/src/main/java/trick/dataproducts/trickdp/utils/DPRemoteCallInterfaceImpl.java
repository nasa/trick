
//========================================
//  Package
//========================================
package trick.dataproducts.trickdp.utils;

//========================================
//  Imports
//========================================
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;

import trick.dataproducts.trickdp.TrickDPApplication;
import trick.dataproducts.utils.Session;

/**
 * Implementation for remote calles needed for trick_dp.
 * Basically, other application can make these calls to 
 * trick_dp applicaiton if necessary.
 * 
 * @since Trick 10
 */
public class DPRemoteCallInterfaceImpl extends UnicastRemoteObject implements DPRemoteCallInterface {
    //========================================
    //  Public data
    //========================================
    
    
    //========================================
    //  Protected data
    //========================================
	
	
    //========================================
    //  Private Data
    //========================================
	private TrickDPApplication application;
	
    //========================================
    //  Constructors
    //========================================
	public DPRemoteCallInterfaceImpl() throws RemoteException {
		application = TrickDPApplication.getInstance(TrickDPApplication.class);
	}
    
    //========================================
    //  Set/Get methods
    //======================================== 

    
    //========================================
    //  Methods
    //========================================
	/**
     * Sets the presentation preference for trick_dp from another application.
     * The presentation options are: "Simple", "Comparison", "Delta", "Contrast";
     * 
     * @see Session#PRESENTATION_OPTIONS
     */
    public void setPresentation(String pt) throws RemoteException {
        application.setPreferredPresentation(pt);
    }
    
    /**
     * Sets the mode preference for trick_qp from another application.
     * The mode options are: "Plot", "Table". 
     * 
     * @see Session#MODE_OPTIONS
     */
    public void setMode(String md) throws RemoteException {
        application.setDisplayMode(md);
    }
}
