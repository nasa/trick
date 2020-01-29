
//========================================
//  Package
//========================================
package trick.dataproducts.trickqp.utils;

//========================================
//  Imports
//========================================
import java.rmi.Remote;
import java.rmi.RemoteException;


/**
 * Interface for remote calles needed for trick_qp.
 * Basically, other application can make these calls to 
 * trick_qp applicaiton if necessary.
 * 
 * @since Trick 10
 */
public interface QPRemoteCallInterface extends Remote {
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
	public void updateRunList(String[] dirList) throws RemoteException;
}
