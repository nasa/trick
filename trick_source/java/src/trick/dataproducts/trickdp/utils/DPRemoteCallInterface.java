
//========================================
//  Package
//========================================
package trick.dataproducts.trickdp.utils;

//========================================
//  Imports
//========================================
import java.rmi.Remote;
import java.rmi.RemoteException;


/**
 * Interface for remote calles needed for trick_dp.
 * Basically, other application can make these calls to 
 * trick_dp applicaiton if necessary.
 * 
 * @since Trick 10
 */
public interface DPRemoteCallInterface extends Remote {
    //========================================
    //  Public data
    //========================================
    

    //========================================
    //  Protected data
    //========================================
	
	
    //========================================
    //  Private Data
    //========================================
	

	
    //========================================
    //  Constructors
    //========================================
	
    
    //========================================
    //  Set/Get methods
    //======================================== 

    
    //========================================
    //  Methods
    //========================================
	public void setPresentation(String pt) throws RemoteException;
    public void setMode(String md) throws RemoteException;
}
