
//========================================
//	Package
//========================================
package trick.dataproducts.trickdp.utils;

//========================================
//	Imports
//========================================
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;
import java.util.Vector;

import javax.swing.JOptionPane;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.TreePath;
import javax.xml.parsers.ParserConfigurationException;

import trick.common.TrickApplication;
import trick.common.ui.UIUtils;
import trick.common.ui.panels.DataPanel;
import trick.dataproducts.trickdp.TrickDPApplication;
import trick.dataproducts.utils.FileTreePanel;
import trick.dataproducts.utils.Session;
import trick.dataproducts.utils.SessionRun;
import trick.dataproducts.utils.SessionDomParser;

import org.xml.sax.SAXException;

/**
 * Trick DP Application action controller that controls actions for {@link TrickDPApplication}.
 *
 * @author Hong Chen
 * @since Trick 10
 */
public class TrickDPActionController {
	//========================================
	//	Public data
	//========================================
	
	
	//========================================
	//	Protected data
	//========================================
	
	
	//========================================
	//	Private Data
	//========================================
	// The application which this action controller is for
    private TrickDPApplication application;
	
    // The process for running QP
    private Process qpProcess;
    
    private String dpFilter;
    
	//========================================
	//	Constructors
	//========================================
	/**
     * Default constructor.
     */
    public TrickDPActionController() {
        application = TrickDPApplication.getInstance(TrickDPApplication.class);
    }

	
	//========================================
	//	Set/Get methods
	//========================================
	
	
	//========================================
	//	Methods
	//========================================
    /**
     * Invoked when New Session is selected.
     */
    public void handleNewSession() {
        int choice = JOptionPane.showConfirmDialog(application.getMainFrame(),
                                                   "Abandon current Session and start over?",
                                                   "Abandon Session",
                                                   JOptionPane.YES_NO_OPTION);
        // If "Yes" is selected, reload trees and remove all data from the lists.
        if (choice == JOptionPane.YES_OPTION) {
            application.simRunTree.cleanUp();
            application.simRunTree.rebuild();
            application.simDPTree.cleanUp();
            application.simDPTree.rebuild();
            application.runList.removeAllData();
            application.dpList.removeAllData();
        }
    }

    /**
     * Invoked when Open Session is selected.
     */
    @SuppressWarnings("rawtypes")
    public void handleOpenSession() {
        File file = UIUtils.chooseOpenFile(null, "Session_", "xml", application.getMainFrame());
        // if no file is selected, do nothing
        if (file == null) {
            return;
        }

        try {
            Session sessionObject = SessionDomParser.parse(file);
            List listData;
            if (sessionObject.getRuns() != null) {
                listData = sessionObject.getRuns();
                for (int i = 0; i < listData.size(); i ++) {
                    application.runList.addData(listData.get(i));
                }
            }

            if (sessionObject.getProductFiles() != null) {
                listData = sessionObject.getProductFiles();
                for (int i = 0; i < listData.size(); i ++) {
                    application.dpList.addData(listData.get(i).toString());
                }
            }

            if (sessionObject.getGnuplotTerminal() != null) {
                application.selectGnuplot();
                application.setGnuplotTerminal(sessionObject.getGnuplotTerminal());
            }

            if (sessionObject.getDevice() != null) {
                application.setDevice(sessionObject.getDevice());
            }

        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (ParserConfigurationException e) {
            e.printStackTrace();
        } catch (SAXException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * Invoked when Save Session is selected.
     */
    public void handleSaveSession() {
        if (application.runList.getAllData() == null || application.runList.getAllData().length < 1) {
            JOptionPane.showMessageDialog(application.getMainFrame(),
                                          "No run directory chosen. At lease one RUN_ dir needs to be selected!",
                                          "No RUN_ Directory",
                                          JOptionPane.ERROR_MESSAGE);
    		return;
    	}
    	
    	if (application.dpList.getAllData() == null || application.dpList.getAllData().length < 1) {
    		JOptionPane.showMessageDialog(application.getMainFrame(),
                                          "No DP_ spec file chosen. At lease one DP_ file needs to be selected!",
                                          "No DP_ File",
                                          JOptionPane.ERROR_MESSAGE);
    		return;
    	}
    	
    	File file = UIUtils.chooseSaveFile(null, "Session_", "xml", application.getMainFrame());
        if (file == null) {
            return;
        }
        Session session = generateSessionData();
        saveSessionData(session, file.getPath());
    }
    
    /**
     * Invoked when Refresh (from main menu) is selected.
     */
    public void handleRefreshSession() {
    	refreshSimRunTree(false);
        refreshSimDPTree(false);
    }
    
    /**
     * Invoked when Single Plot is selected.
     */
    public void handleSinglePlot() {
    	launchPlot();
    }
    
    /**
     * Invoked when Comparison Plot is selected.
     */
    public void handleComparisonPlot() {
        launchPlot();
    }
    
    /**
     * Invoked when Error Plot is selected.
     */
    public void handleErrorPlot() {    	
    	launchPlot();
    }
    
    /**
     * Invoked when Contrast Plot is selected.
     */
    public void handleContrastPlot() {
    	launchPlot();
    }
    
    /**
     * Invoked when Tabular Data is selected.
     */
    public void handleTabularData() {
    	launchPlot();
    }
    
    /**
     * Invoked when Tabular Error Data is selected.
     */
    public void handleTabularErrorData() {
    	launchPlot();
    }

    /**
     * Helper method for setting up the session data and calling the plot program launching.
     */
    private void launchPlot() {
        Session session = generateSessionData();
        saveSessionData(session, null);
        application.launchPlotProgram(application.sessionFile);
    }

    /**
     * Helper method for generating a {@Link Session} object for current session.
     */
    private Session generateSessionData() {
        Session session = new Session(application.versionField.getText(), application.getPreferredPresentation());
    	session.setMode(application.getDisplayMode());
    	if (application.fileDevice != null) {
    	    session.setDeviceFileName(application.fileDevice.getPath());
    	}
        return session;
    }
    
    /**
     * Helper method for saving current session data to the specified file.
     */
    private void saveSessionData(Session session, String file) {
        application.saveSessionToFile(session, file, application.runList.getAllData(), application.dpList.getAllData());
    }
    
    
    /**
     * Invoked when Import Sim is selected.
     */
    public void handleImportSimDir() {

    	File dir = UIUtils.chooseDir("Import SIM Directory", application.trickProperties.getProperty("TRICK_DP_IMPORT_DIR"), application.getMainFrame());
        if (dir != null) {
            if (!dir.exists()) {
                // in the file chooser, user may double click SIM dir so he is inside SIM dir when he clicks ok
                // when this happens, the final dir name is appended twice (it will be /blah/blah/blah/SIM_whatever/SIM_whatever)
                // so take that last name off of the dir name
                dir = dir.getParentFile();
            }
            String msg = application.simRunTree.importDir(dir);
            if (msg != null) {
                // import failed for some reason
                JOptionPane.showMessageDialog(application.getMainFrame(), msg, "Error", JOptionPane.WARNING_MESSAGE);
                return;
            }
            
            application.trickProperties.setProperty("TRICK_DP_IMPORT_DIR",dir.getParent());
        }
    }
    
    /**
     * Invoked when Add Run Dir is selected.
     */
    public void handleAddRunDir() {
    	File dir = UIUtils.chooseDir("Add RUN Directory", application.trickProperties.getProperty("TRICK_DP_IMPORT_DIR"), application.getMainFrame());
        if (dir != null) {
            if (!dir.exists()) {
                // in the file chooser, user may double click RUN dir so he is inside RUN dir when he clicks ok
                // when this happens, the final dir name is appended twice (it will be /blah/blah/blah/RUN_whatever/RUN_whatever)
                // so take that last name off of the dir name
                dir = dir.getParentFile();
            }
            if (!dir.getName().startsWith("RUN")) {
                JOptionPane.showMessageDialog(application.getMainFrame(), 
                		                      dir.getPath() +"\n is not a RUN directory.",
                                              "Error", 
                                              JOptionPane.WARNING_MESSAGE);
                return;
            }
            application.runList.addData(new SessionRun(dir.getPath()));
        }
    }
    
    /**
     * Invoked when Add Runs (pop-up menu) is selected.
     */
    public void handleAddRuns() {
    	// expanding the tree first
    	application.simRunTree.expandSelectedNodes();
    	for (String eachItem : application.simRunTree.getSelectedItems()) {   		
    		application.runList.addData(new SessionRun(eachItem));
    	}
    	for (DefaultMutableTreeNode node : application.simRunTree.getSelectedNodes()) {
    		application.simRunTree.addNodeToObserver(node, application.simDPTree);
    	}
    }
    
    /**
     * Invoked when Read DP List (pop-up menu) is selected.
     */
    public void handleReadDPList() {
    	// expanding the tree first
    	application.simRunTree.expandSelectedNodes();
    	for (DefaultMutableTreeNode node : application.simRunTree.getSelectedNodes()) {
    		application.simRunTree.addNodeToObserver(node, application.simDPTree);
    	}
    }
    
    /**
     * Invoked when Add DP is selected.
     */
    public void handleAddDP() {
    	File file = UIUtils.chooseOpenFile(application.trickProperties.getProperty("TRICK_DP_IMPORT_DIR"), null, null, application.getMainFrame());
        if (file != null) {            
            if (!(file.getName().startsWith("DP"))) {
                JOptionPane.showMessageDialog(application.getMainFrame(), 
                		                      file.getName() +"\n is not a DP file.",
                                              "Error", 
                                              JOptionPane.WARNING_MESSAGE);
                return;
            }
            application.dpList.addData(file.getAbsolutePath());
        }
    }
    
    /**
     * Invoked when Edit DP is selected.
     * @param dpFile name of file to edit
     */
    public void handleEditDP(String dpFile) {
        if (dpFile == null) {
            JOptionPane.showMessageDialog(application.getMainFrame(),
                                          "No DP_ file was selected!",
                                          "Error", 
                                          JOptionPane.WARNING_MESSAGE);
        } else {
            String[] dpAndRun = null;
            String[] runPaths = application.getSelectedRunPaths();
            int combinedLen = 1;
            if (runPaths != null && runPaths.length > 0) {
                combinedLen = combinedLen + runPaths.length;
            }
            if (combinedLen == 0) {
                return;
            } else {
                dpAndRun = new String[combinedLen];
                if (runPaths != null && runPaths.length > 0) {
                    for (int i = 0; i < runPaths.length; i++) {
                        dpAndRun[i] = runPaths[i];
                    }
                }
                dpAndRun[combinedLen-1] = dpFile;
            }
            launchQP(dpAndRun);
        }
    }
    
    /**
     * Invoked when Filter DP is selected.
     */
    public void handleFilterDP() {
    	Object inputValue = JOptionPane.showInputDialog(application.getMainFrame(), "Please enter DP file filter:", dpFilter);
    	if (inputValue != null) {
    		dpFilter = inputValue.toString();
    	}
    	// sets the filename match criteria for the tree file fitler
        application.simDPTree.getFilter().setFilenameMatch(dpFilter);
        refreshSimDPTree(false);
    }
        
    /**
     * Invoked when Refresh (pop-up menu) is selected for selected tree nodes.
     */
    public void handleRefreshSelected() {   	 
    	 // refresh sim run tree based on the file system
    	 // refresh dp tree conditionally, only refresh those displayed ones, not based on the file system
    	 if (TrickApplication.getPopupInvoker() == DataPanel.SIM_RUN_TREE) {
    		 refreshSimRunTree(true);
         } else if (TrickApplication.getPopupInvoker() == DataPanel.SIM_DP_TREE) {
        	 refreshSimDPTree(true);
         }
     }
     
    /**
     * Helper method for launching quick plot. The previous subprocess of trick quickplot 
     * application will be destroyed before another one is created.
     * @param initialArgs argument list to send to QP
     * 
     */
     public void launchQP(String[] initialArgs) {
    	 // the command variable program command name and arguments
    	 List<String> command = new ArrayList<String>();
         String fileSeparator = java.io.File.separator;
         String pathSeparator = System.getProperty("path.separator");
         
         String javaPath = UIUtils.getTrickHome() + fileSeparator + "libexec/trick" + fileSeparator + "java";
         String distPath = javaPath + fileSeparator + "dist";
         String libPath = javaPath + fileSeparator + "lib";
         String classPath = distPath + fileSeparator + "*" + pathSeparator +
        		            libPath + fileSeparator + "*" + pathSeparator +
        		            libPath + fileSeparator;
         
         command.add("java");
         command.add("-cp");
         command.add(classPath);
         command.add("trick.dataproducts.trickqp.TrickQPApplication");    	 
         
         if (initialArgs != null) {
        	 for (String arg : initialArgs) {
        		 command.add(arg);
        	 }
         }        
         try {            
             // The subprocess of a process is not killed through destroy() method.
        	 // Therefore, launching trick quickplot through calling the the
        	 // application itself directly instead of using the trick_qp script. 
        	 // Only one trick quickplot application is up if it is launched through
        	 // the same trick_dp application.
             if (qpProcess != null) {           	 
                 qpProcess.destroy();
             } 
             
             qpProcess = (new ProcessBuilder(command)).start();
         } catch (IOException ioe) {
             System.out.println("IOException for launching quick plot!");
         }
     }
     
    /**
     * Refreshes the sim dp tree.
     * 
     * @param onlySelected	<code>true</code> if only refreshing the selected nodes,
     *                      <code>false</code> if refreshing the entire tree.
     */
    private void refreshSimDPTree(boolean onlySelected) {
    	Vector<DefaultMutableTreeNode> nodesToRefresh = null;
    	
    	// remember all expanded paths
		Enumeration<TreePath> enumPaths = application.simDPTree.getTree().getExpandedDescendants(new TreePath(application.simDPTree.getRoot().getPath()));
    	
		if (onlySelected) {
    		// only refreshing selected nodes
    		nodesToRefresh = application.simDPTree.getSelectedFirstGenerationNodes();
    	} else {
	    	// For DP tree, refreshing is based on what are chosen (or already displayed) not what are in file system.   		
	        nodesToRefresh = application.simDPTree.getAllChildren(application.simDPTree.getRoot());
    	}
    	if (nodesToRefresh != null) {
            for (DefaultMutableTreeNode eachNode : nodesToRefresh) {           	
          	  	// don't refresh dir that contains sim dirs since only those dirs under shown sim dir need to be refreshed
                if ( !(application.simDPTree.getRoot()).isNodeChild(eachNode) ) {                   	
               	    application.simDPTree.refreshNode(eachNode);
                } else {               	
              	  	Vector<DefaultMutableTreeNode> simDirs = application.simDPTree.getAllChildren(eachNode);
              	  	for (DefaultMutableTreeNode eachSimDir : simDirs) {             	  		
              		    application.simDPTree.refreshNode(eachSimDir);
              	  	}
                }
            }
        }
    	
    	// put expanded paths back
		restoreTreeExpansion(application.simDPTree, enumPaths);
    }
    
    /**
     * Refreshes sim run tree.
     * 
     * @param onlySelected	<code>true</code> if only refreshing the selected nodes,
     *                      <code>false</code> if refreshing the entire tree.
     */
    private void refreshSimRunTree(boolean onlySelected) {
    	// remember all expanded paths
		Enumeration<TreePath> enumPaths = application.simRunTree.getTree().getExpandedDescendants(new TreePath(application.simRunTree.getRoot().getPath()));
    	
		if (onlySelected) {
    		// only refreshing selected nodes
    		application.simRunTree.refreshSelectedNodes();
    	} else {    		
    		// refreshing the whole tree starting from root
    		application.simRunTree.refreshNode(application.simRunTree.getRoot());    	    		
    	}
		
		// put expanded paths back
		restoreTreeExpansion(application.simRunTree, enumPaths);
    }
    
    /**
     * Helper method for restoring the tree expansion after it is updated.
     */
    private void restoreTreeExpansion(FileTreePanel treePanel, Enumeration expandedPaths) {    	
    	if (expandedPaths == null) {
    		return;
    	}
    	
    	// put expanded paths back
		while (expandedPaths.hasMoreElements()) {
			TreePath oldTreePath = (TreePath)expandedPaths.nextElement();
			if (oldTreePath.getPath() != null) {
				// the tree is different now after refreshing, find the path in the new tree based on the node names
				TreePath newTreePath = treePanel.getPathByName(oldTreePath.getPath());
				if (newTreePath != null) {
					treePanel.getTree().expandPath(newTreePath);
				}
			}
		}
    	
    }
}
