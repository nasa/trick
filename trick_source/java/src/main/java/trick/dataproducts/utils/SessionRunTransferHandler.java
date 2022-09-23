
package trick.dataproducts.utils;

//========================================
//    Imports
//========================================
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.io.IOException;

import javax.swing.DefaultListModel;
import javax.swing.JComponent;
import javax.swing.JList;
import javax.swing.TransferHandler;


/**
 * Defines a {@link TransferHandler} for reordering a list of {@link SessionRun}.
 *
 * @since Trick 10
 *
 */
public class SessionRunTransferHandler extends TransferHandler {

    //========================================
    //    Public data
    //========================================



    //========================================
    //    Protected data
    //========================================



    //========================================
    //    Private data
    //========================================
    
    private static final long serialVersionUID = 2828887120847674520L;

    // For SessionRun transferring
    private DataFlavor sessionRunFlavor;
    
    // The SessionRun list that needs reording
    private JList list;
    
    // The object that is being transfered
    private Object sourceObject;
    
    //========================================
    //    Constructors
    //========================================
    /**
     * Default constructor.
     * @param list not sure
     */
    public SessionRunTransferHandler(JList list) {
    	this.list = list;
        try {
            String mimeType = DataFlavor.javaJVMLocalObjectMimeType +
                              ";class=\"" +
                              trick.dataproducts.utils.SessionRun[].class.getName() +
                              "\"";
            sessionRunFlavor = new DataFlavor(mimeType);
            MouseListener mouseListener = new MouseAdapter() {
                public void mousePressed(MouseEvent mouseEvent) {
                    JList theList = (JList) mouseEvent.getSource();
                    int sourceIndex = theList.locationToIndex(mouseEvent.getPoint());
                    if (sourceIndex >= 0) {
                        sourceObject = theList.getModel().getElementAt(sourceIndex);
                    }
                }
            };
            list.addMouseListener(mouseListener);
        } catch (ClassNotFoundException e) {
            System.out.println("SessionRunTransferHandler: unable to create data flavor");
        }        
    }

    //========================================
    //    Set/Get methods
    //========================================



    //========================================
    //    Methods
    //========================================
    @Override
    @SuppressWarnings("unchecked")
    public boolean importData(TransferHandler.TransferSupport support) {
        if (!canImport(support)) {
          return false;
        }
        
        JList.DropLocation dl = (JList.DropLocation) support.getDropLocation();
        int dropTargetIndex = dl.getIndex();
        
        DefaultListModel model = (DefaultListModel)list.getModel();
        
        Object transferredData = null;
        
        try {
        	// retrieving the transfer data instead of the sourceObject due to 
        	// this drag&drop could happen between applications even though
        	// it is not the intension
        	transferredData = support.getTransferable().getTransferData(sessionRunFlavor);
        } catch (IOException ioe) {
        	return false;
        } catch (UnsupportedFlavorException ufe) {
        	return false;
        }
        
        if (!model.contains(transferredData)) {
        	return false;
        }
        
        int oldLoc = model.indexOf(transferredData);
        model.removeElement(transferredData);
        
        if (oldLoc > dropTargetIndex) {
        	model.add(dropTargetIndex, transferredData);
        } else {       	
            model.add(dropTargetIndex-1, transferredData);           
        }
        
        list.setSelectedValue(transferredData, false);       
        return true;
    }       
    
    @Override
    public boolean canImport(TransferHandler.TransferSupport support) {  	
        if (!support.isDataFlavorSupported(sessionRunFlavor)) {
            return false;
        }
        JList.DropLocation dl = (JList.DropLocation) support.getDropLocation();
        
        if (dl.getIndex() == -1 || dl.getIndex() >= list.getModel().getSize()) {
          return false;
        } else {
            return true;
        }
    }
    
    @Override
    protected Transferable createTransferable(JComponent c) {
        if (list != null) {           
            if (sourceObject == null) {
            	return null;
            }
            return new SessionRunTransferable((SessionRun)sourceObject);
        }
        return null;
    }

    @Override
    public int getSourceActions(JComponent c) {
        return MOVE;
    }
    
    
    //========================================
    //    Inner classes
    //========================================
    /**
     * Defines a {@link Transferable} for transferring a {@link SessionRun}
     * from a {@link JList}.
     *
     */
    public class SessionRunTransferable implements Transferable {
    	SessionRun run;

        public SessionRunTransferable(SessionRun run) {
            this.run = run;
        }

        public Object getTransferData(DataFlavor flavor) throws UnsupportedFlavorException {
            if(!isDataFlavorSupported(flavor)) {
                throw new UnsupportedFlavorException(flavor);
            }
            return run;
        }

        public DataFlavor[] getTransferDataFlavors() {
            return new DataFlavor[] {sessionRunFlavor};
        }

        public boolean isDataFlavorSupported(DataFlavor flavor) {
            return sessionRunFlavor.equals(flavor);
        }
    }
}
