
package trick.dataproducts.trickqp.utils;

//========================================
//    Imports
//========================================
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.io.IOException;
import java.util.ArrayList;

import javax.swing.JComponent;
import javax.swing.JList;
import javax.swing.JOptionPane;
import javax.swing.JTree;
import javax.swing.TransferHandler;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreePath;

import trick.common.ui.UIUtils;
import trick.common.ui.components.CommonTreeNode;
import trick.common.utils.LogVar;
import trick.dataproducts.trickqp.TrickQPApplication;


/**
 * Defines a {@link TransferHandler} for handling dragging data either from
 * a {@link JList} or a {@link JTree} to a {@link JTree}.
 *
 * @since Trick 10
 *
 */
public class DataTransferHandler extends TransferHandler {

    //========================================
    //    Public data
    //========================================



    //========================================
    //    Protected data
    //========================================



    //========================================
    //    Private data
    //========================================

    private static final long serialVersionUID = 2731172208451888941L;

    // For data transferring within the same frame
    private DataFlavor localArrayListFlavor;

    // For data transferring between different frames.
    private DataFlavor serialArrayListFlavor;

    // For tree node transferring
    private DataFlavor commonTreeNodeFlavor;

    private String localArrayListType = DataFlavor.javaJVMLocalObjectMimeType + ";class=java.util.ArrayList";


    //========================================
    //    Constructors
    //========================================
    /**
     * Default constructor.
     */
    public DataTransferHandler() {
        try {
            localArrayListFlavor = new DataFlavor(localArrayListType);

            String mimeType = DataFlavor.javaJVMLocalObjectMimeType +
                              ";class=\"" +
                              trick.common.ui.components.CommonTreeNode[].class.getName() +
                              "\"";
            commonTreeNodeFlavor = new DataFlavor(mimeType);
        } catch (ClassNotFoundException e) {
            System.out.println("ArrayListTransferHandler: unable to create data flavor");
        }
        serialArrayListFlavor = new DataFlavor(ArrayList.class, "ArrayList");
    }

    //========================================
    //    Set/Get methods
    //========================================



    //========================================
    //    Methods
    //========================================
    @Override
    public boolean importData(JComponent c, Transferable t) {

        ArrayList alist = null;
        if (!canImport(c, t.getTransferDataFlavors())) {
            return false;
        }
        try {
            if (hasLocalArrayListFlavor(t.getTransferDataFlavors())) {
                alist = (ArrayList) t.getTransferData(localArrayListFlavor);
            } else if (hasSerialArrayListFlavor(t.getTransferDataFlavors())) {
                alist = (ArrayList) t.getTransferData(serialArrayListFlavor);
            } else if (hasCommonTreeNodeFlavor(t.getTransferDataFlavors())) {
                alist = (ArrayList) t.getTransferData(commonTreeNodeFlavor);
            } else {
                return false;
            }
        } catch (UnsupportedFlavorException ufe) {
            System.out.println("importData: unsupported data flavor");
            return false;
        } catch (IOException ioe) {
            System.out.println("importData: I/O exception");
            return false;
        }

        JTree target = (JTree)c;
        // Get drop location info.
        JTree.DropLocation dl = target.getDropLocation();
        TreePath dest = dl.getPath();

        // make sure the data is moving to a tree
        if (dest.getLastPathComponent() instanceof CommonTreeNode) {
            if (alist != null && alist.size() > 0) {
                if (alist.get(0) instanceof LogVar) {
                    importDataFromList(target, dest, alist);
                } else  if (alist.get(0) instanceof CommonTreeNode) {
                    importDataFromTree(target, dest, alist);
                } else {
                    return false;
                }
            } else {
                return false;
            }
        } else {
            return false;
        }
        return true;
    }

    /**
     * Helper method for replacing a X node.
     */
    private int dropToX(JTree target, TreePath dest, ProductVar xVar) {
        CommonTreeNode targetNode = (CommonTreeNode)dest.getLastPathComponent();

        int choice = UIUtils.showOkCancelDialog("Replace X?", "Replace X variable?", target);
        if (choice == UIUtils.OK_OPTION) {
            // getting the parent node of the target
            CommonTreeNode targetParentNode = (CommonTreeNode)targetNode.getParent();
            // removing the old X
            ((DefaultTreeModel)target.getModel()).removeNodeFromParent(targetNode);
            // creating a new X
            CommonTreeNode newX = new CommonTreeNode(xVar, CommonTreeNode.X_NODE);
            // inserting the new X
            ((DefaultTreeModel)target.getModel()).insertNodeInto(newX, targetParentNode, 0);
            target.scrollPathToVisible(new TreePath(newX.getPath()));

            // sets the user object
            if (targetParentNode.getNodeType() == CommonTreeNode.CURVE_NODE) {
                ((ProductCurve)targetParentNode.getUserObject()).setX(xVar);
            } else if (targetParentNode.getNodeType() == CommonTreeNode.VARCASE_NODE) {
                ((ProductVarcase)targetParentNode.getUserObject()).setX(xVar);
            }
        }
        return choice;
    }

    /**
     * Helper method for dragging data from a {@link JList}.
     */
    private void importDataFromList(JTree target, TreePath dest, ArrayList alist) {
        TrickQPApplication application = TrickQPApplication.getInstance(TrickQPApplication.class);

        CommonTreeNode targetNode = (CommonTreeNode)dest.getLastPathComponent();
        // if dragging over a X node, replace it with the 1st var in the list
        if (targetNode.getNodeType() == CommonTreeNode.X_NODE) {       	
        	LogVar var = (LogVar)alist.get(0);
        	// can't drop an arrayed-variable to an X-node
        	if (var.getName().indexOf('-') != -1) {
        		return;
        	}
            ProductVar xVar = new ProductVar(var.getName(), var.getUnits());
            dropToX(target, dest, xVar);
        } else {
            application.addVar();
        }
    }

    /**
     * Helper method for dragging data from a {@link JTree}.
     */
    private void importDataFromTree(JTree target, TreePath dest, ArrayList alist) {
        TrickQPApplication application = TrickQPApplication.getInstance(TrickQPApplication.class);

        CommonTreeNode targetNode = (CommonTreeNode)dest.getLastPathComponent();
        int targetNodeType = targetNode.getNodeType();
        targetNode.getParent();

        CommonTreeNode sourceNode = (CommonTreeNode)alist.get(0);
        int sourceNodeType = sourceNode.getNodeType();
        CommonTreeNode sourceParentNode = (CommonTreeNode)sourceNode.getParent();
        CommonProduct sourceParentObj = null;        
        boolean success = false;
        int objLoc = -1; // this will be set if the object needs removing based on the index instead of object itself
        if (sourceNode != targetNode) {
        	switch (sourceNodeType) {
            case CommonTreeNode.Y_NODE:
            case CommonTreeNode.Z_NODE:
                if (targetNodeType == CommonTreeNode.X_NODE) {
                    ProductVar xVar = (ProductVar)sourceNode.getUserObject();
                    if (dropToX(target, dest, xVar) == UIUtils.OK_OPTION) {
                        // sets the either X or Y properly as the source node would be removed
                        if (sourceNodeType == CommonTreeNode.Y_NODE) {
                            ((ProductCurve)sourceParentNode.getUserObject()).setY(null);
                        } else {
                            ((ProductCurve)sourceParentNode.getUserObject()).setZ(null);
                        }
                        ((DefaultTreeModel)target.getModel()).removeNodeFromParent(sourceNode);
                        success = true;
                    }
                }
                break;           
            case CommonTreeNode.CURVE_NODE:           	
                if (targetNodeType == CommonTreeNode.PLOT_NODE) { // drag&drop the source curve to the target plot which can be its own parent plot as well
                    ProductCurve curve = (ProductCurve)sourceNode.getUserObject();
                    // insert this curve as the last child of the target plot node
                    application.getActionController().addCurveToPlot(targetNode, curve, true);
                    success = true;
                    sourceParentObj = (CommonProduct)sourceParentNode.getUserObject();
                } else if (targetNodeType == CommonTreeNode.CURVE_NODE) { // drag&drop the source curve to the target curve
                	 ProductCurve curve = (ProductCurve)sourceNode.getUserObject();
                	 // gets the target parent which should be a plot node
                	 CommonTreeNode targetParentNode = (CommonTreeNode)targetNode.getParent();
                     // insert this curve before this target curve
                     application.getActionController().addCurveToPlot(targetParentNode, curve, targetParentNode.getIndex(targetNode), true);
                     success = true;
                     sourceParentObj = (CommonProduct)sourceParentNode.getUserObject();
                     // If a curve is moved around within the same plot, there are 2 same curve objects (not the tree node) existed before one is removed.
                     // So need to make sure the correct one gets removed, which is the curve at source place.
                     if (sourceParentNode == targetParentNode) {
                    	 objLoc = sourceParentNode.getIndex(sourceNode);
                     }
                }
                if (success) {
                	((DefaultTreeModel)target.getModel()).removeNodeFromParent(sourceNode);
                }
                break;
            case CommonTreeNode.PLOT_NODE:
                if (targetNodeType == CommonTreeNode.PAGE_NODE) {
                    ProductPlot plot = (ProductPlot)sourceNode.getUserObject();
                    application.getActionController().addPlotToPage(targetNode, plot, true);
                    success = true;
                    sourceParentObj = (CommonProduct)sourceParentNode.getUserObject();
                    ((DefaultTreeModel)target.getModel()).removeNodeFromParent(sourceNode);
                } 
                break;
            case CommonTreeNode.PAGE_NODE:
            	// remember the source node expansion status
            	boolean sourceExpanded = application.productTree.getTree().isExpanded(new TreePath(sourceNode.getPath()));
            	// since any page node's parent is the generic Plots node not an instance of CommonProduct
            	sourceParentObj = null;
            	if (targetNodeType == CommonTreeNode.PLOTS_NODE) {
            		ProductPage page = (ProductPage)sourceNode.getUserObject();
            		((DefaultTreeModel)target.getModel()).removeNodeFromParent(sourceNode);
            		CommonTreeNode newPageNode = application.getActionController().addPageToPlots(page);
            		if (sourceExpanded) {
            			application.productTree.expandNode(newPageNode);
            		}
                    success = true;
            	} else if (targetNodeType == CommonTreeNode.PAGE_NODE) {          		
            		((DefaultTreeModel)target.getModel()).removeNodeFromParent(sourceNode);
            		((DefaultTreeModel)target.getModel()).insertNodeInto(sourceNode, sourceParentNode, sourceParentNode.getIndex(targetNode));
            		if (sourceExpanded) {
            			application.productTree.expandNode(sourceNode);
            		}
            		success = true;
            	}           	
            	break;
            case CommonTreeNode.COLUMN_NODE:
                if (targetNodeType == CommonTreeNode.TABLE_NODE) { // drag&drop the source column to the target table which can be its own parent table as well
                    ProductColumn column = (ProductColumn)sourceNode.getUserObject();
                    // insert this column as the last child of the target table node
                    application.getActionController().addColumnToTable(targetNode, column, true);
                    success = true;
                    sourceParentObj = (CommonProduct)sourceParentNode.getUserObject();
                } else if (targetNodeType == CommonTreeNode.COLUMN_NODE) { // drag&drop the source column to the target column
                	ProductColumn column = (ProductColumn)sourceNode.getUserObject();
                	// gets the target parent which should be a table node
                	CommonTreeNode targetParentNode = (CommonTreeNode)targetNode.getParent();
                	// insert this column before this target column
                	application.getActionController().addColumnToTable(targetParentNode, column, targetParentNode.getIndex(targetNode), true);
                    success = true;
                    sourceParentObj = (CommonProduct)sourceParentNode.getUserObject();
                    // If a column is moved around within the same table, there are 2 same column objects (not the tree node) existed before one is removed.
                    // So need to make sure the correct one gets removed, which is the column at source place.
                    if (sourceParentNode == targetParentNode) {
                   	    objLoc = sourceParentNode.getIndex(sourceNode);
                    }
                }
                if (success) {
                	((DefaultTreeModel)target.getModel()).removeNodeFromParent(sourceNode);
                }
                break;
        	}
        }

        if (success) {
            if (sourceParentObj != null && sourceParentObj instanceof CommonProduct) {
            	if (objLoc == -1) {
                    sourceParentObj.removeChild(sourceNode.getUserObject());
            	} else {
            		sourceParentObj.removeChild(objLoc);
            	}
            }
        } else {
            JOptionPane.showMessageDialog(target,
                                              "A proper drop node must be selected to insert this new node!",
                                              "Incorrect Drop Location",
                                              JOptionPane.WARNING_MESSAGE);

        }
    }

    /**
     * Helper method for checking if it has localArrayListFlavor.
     */
    private boolean hasLocalArrayListFlavor(DataFlavor[] flavors) {
        if (localArrayListFlavor == null) {
            return false;
        }

        for (int i = 0; i < flavors.length; i++) {
            if (flavors[i].equals(localArrayListFlavor)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Helper method for checking if it has serialArrayListFlavor.
     */
    private boolean hasSerialArrayListFlavor(DataFlavor[] flavors) {
        if (serialArrayListFlavor == null) {
            return false;
        }

        for (int i = 0; i < flavors.length; i++) {
            if (flavors[i].equals(serialArrayListFlavor)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Helper method for checking if it has commonTreeNodeFlavor
     */
    private boolean hasCommonTreeNodeFlavor(DataFlavor[] flavors) {
        if (commonTreeNodeFlavor == null) {
            return false;
        }

        for (int i = 0; i < flavors.length; i++) {
            if (flavors[i].equals(commonTreeNodeFlavor)) {
                return true;
            }
        }
        return false;
    }

    @Override
    public boolean canImport(JComponent c, DataFlavor[] flavors) {
        if (hasLocalArrayListFlavor(flavors)) {
            return true;
        }
        if (hasSerialArrayListFlavor(flavors)) {
            return true;
        }
        if (hasCommonTreeNodeFlavor(flavors)) {
            return true;
        }
        return false;
    }

    @Override
    @SuppressWarnings("deprecation") // In Java 7, JList method getSelectedValues() is deprecated and replaced by getSelectedValuesList()
    protected Transferable createTransferable(JComponent c) {
        if (c instanceof JList) {
            JList source = (JList) c;
            //indices = source.getSelectedIndices();
            Object[] values = source.getSelectedValues();
            if (values == null || values.length == 0) {
                return null;
            }
            ArrayList<Object> alist = new ArrayList<Object>(values.length);
            for (int i = 0; i < values.length; i++) {
                alist.add(values[i]);
            }
            return new ArrayListTransferable(alist);
        } else if (c instanceof JTree) {

            JTree source = (JTree)c;
            TreePath[] paths = source.getSelectionPaths();
            if(paths != null) {
                // for tree, the list only has one node
                ArrayList<Object> selectedNodes = new ArrayList<Object>();
                CommonTreeNode node = (CommonTreeNode)paths[0].getLastPathComponent();
                selectedNodes.add(node);
                return new NodesTransferable(selectedNodes);
            }
        }
        return null;
    }

    @Override
    public int getSourceActions(JComponent c) {
        return COPY_OR_MOVE;
    }

    /**
     * Helper method for copying a {@link CommonTreeNode}.
     */
    private CommonTreeNode copy(Object nodeObj) {
        return new CommonTreeNode(nodeObj);
    }


    //========================================
    //    Inner classes
    //========================================
    /**
     * Defines a {@link Transferable} for transferring {@link ArrayList}
     * from a {@link JList}.
     */
    public class ArrayListTransferable implements Transferable {
        ArrayList<Object> data;

        public ArrayListTransferable(ArrayList<Object> alist) {
            data = alist;
        }

        public Object getTransferData(DataFlavor flavor) throws UnsupportedFlavorException {
            if (!isDataFlavorSupported(flavor)) {
                throw new UnsupportedFlavorException(flavor);
            }
            return data;
        }

        public DataFlavor[] getTransferDataFlavors() {
            return new DataFlavor[] { localArrayListFlavor, serialArrayListFlavor };
        }

        public boolean isDataFlavorSupported(DataFlavor flavor) {
            if (localArrayListFlavor.equals(flavor)) {
                return true;
            }
            if (serialArrayListFlavor.equals(flavor)) {
                return true;
            }
            return false;
        }
    }

    /**
     * Defines a {@link Transferable} for transferring a {@link CommonTreeNode}
     * from a {@link JTree}.
     *
     */
    public class NodesTransferable implements Transferable {
        ArrayList<Object> nodes;

        public NodesTransferable(ArrayList<Object> nodes) {
            this.nodes = nodes;
        }

        public Object getTransferData(DataFlavor flavor) throws UnsupportedFlavorException {
            if(!isDataFlavorSupported(flavor)) {
                throw new UnsupportedFlavorException(flavor);
            }
            return nodes;
        }

        public DataFlavor[] getTransferDataFlavors() {
            return new DataFlavor[] {commonTreeNodeFlavor};
        }

        public boolean isDataFlavorSupported(DataFlavor flavor) {
            return commonTreeNodeFlavor.equals(flavor);
        }
    }

}
