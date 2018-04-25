
//========================================
//    Package
//========================================
package trick.dataproducts.trickqp.utils;

//========================================
//    Imports
//========================================
import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.swing.JOptionPane;
import javax.swing.SwingUtilities;
import javax.swing.tree.DefaultMutableTreeNode;

import trick.common.TrickApplication;
import trick.common.ui.TrickFileFilter;
import trick.common.ui.UIUtils;
import trick.common.ui.components.CommonTreeNode;
import trick.common.utils.BinaryDataReader;
import trick.common.utils.CSVDataReader;
import trick.common.utils.DataReader;
import trick.common.utils.LogHeaderReader;
import trick.common.utils.LogVar;
import trick.common.utils.LogVar.DisplayType;
import trick.common.utils.UnitType;
import trick.dataproducts.trickqp.TrickQPApplication;
import trick.dataproducts.utils.SessionRun;


/**
 * Trick QP Application action controller that controls actions that need
 * handling for {@link TrickQPApplication}.
 *
 * @author Hong Chen
 * @since Trick 10
 */
public class TrickQPActionController {
    //========================================
    //    Public data
    //========================================


    //========================================
    //    Protected data
    //========================================


    //========================================
    //    Private Data
    //========================================
    // The application which this action controller is for
    private TrickQPApplication application;

    private HashMap<String, List<Object>> runVarMap;

    private String dpFileName = null;

    private List<Object> hiddenVarListForSearch;

    //========================================
    //    Constructors
    //========================================
    /**
     * Default constructor.
     */
    public TrickQPActionController() {
        application = TrickQPApplication.getInstance(TrickQPApplication.class);
        runVarMap = new HashMap<String, List<Object>>();
    }


    //========================================
    //    Set/Get methods
    //========================================


    //========================================
    //    Methods
    //========================================

    /**
     * Invoked when New DP is selected.
     */
    public void handleNewDP() {
        if (application.plotsNode.getChildCount() > 0      ||
                application.tablesNode.getChildCount() > 0 ||
                application.programsNode.getChildCount() > 0) {
            if (UIUtils.showOkCancelDialog("Warning", "Abandon current DP and start over?", application.getMainFrame()) != UIUtils.OK_OPTION) {
                return;
            }
        }
        resetGUI();
        dpFileName = null;
    }

    /**
     * Invoked when Open DP is selected.
     */
    public void handleOpenDP() {
        if (application.plotsNode.getChildCount() > 0  ||
            application.tablesNode.getChildCount() > 0 ||
            application.programsNode.getChildCount() > 0) {
            if (UIUtils.showOkCancelDialog("Warning", "Abandon current DP and start over?", application.getMainFrame()) != UIUtils.OK_OPTION) {
                return;
            }
        }
        resetGUI();

        // TODO: clean up each ProductDataPanel

        File file = UIUtils.chooseOpenFile("DP_Product", "DP_", "xml", application.getMainFrame());
        // if no file is selected, do nothing
        if (file == null) {
            // so dpFileName doesn't remember the last value
            dpFileName = null;
            return;
        }
        dpFileName = file.getAbsolutePath();
        openDP(dpFileName);
    }

    /**
     * Invoked when Refresh DP is selected.
     */
    public void handleRefreshDP() {

        // remove all variables from displayed varlist and remove all RUNdir mappings
        application.varList.removeAllData();
        runVarMap.clear();
        // now add all variables back into displayed varlist for existing RUN dirs
        Object[] allRuns = application.runList.getAllData();
        if (allRuns != null && allRuns.length > 0) {
            SessionRun thisRun = null;
            for (int i = 0; i < allRuns.length; i++) {
                if (allRuns[i] instanceof SessionRun) {
                    thisRun = (SessionRun)allRuns[i];
                }
                //File runDir = (File)allRuns[i];
                File runDir = new File(thisRun.getDir());
                if (runDir.exists()) {
                    // update the special entry for RUNdir count in the variable list
                    application.varList.addData(application.runList.getAllData().length);
                    addVarsForRun(runDir);
                } else {
                    application.runList.removeData(runDir);
                    // update the special entry for RUNdir count in the variable list
                    application.varList.addData(application.runList.getAllData().length);
                }
            }

            // finally create an array variable for each group of sequential components
            create_arrays();

            // always contract vars initially
            contractVar();
        }
    }

    /**
     * Invoked when Save is selected.
     *
     * @return True if a DP_ file is actually saved, false otherwise.
     */
    public boolean handleSaveDP() {
        if (dpFileName != null) {
            application.saveDPToFile(dpFileName, true);

            // reset the productTree change status once the DP_ file is saved
            application.productTree.setChanged(false);

            return true;
        } else {
            return saveAs();
        }
    }

    /**
     * Invoked when Save As is selected.
     *
     * @return True if a DP_ file is actually saved, false otherwise.
     */
    public boolean handleSaveAsDP() {
        return saveAs();
    }


    /**
     * Helper method for adding specified new {@link ProductVar} variables to selected node(s) in the tree.
     * If newVars has only one element, it's for single variable. If newVars has more than one variable,
     * it's for array variable.
     * @param  newVars new variables
     * @param  selectedTreeNodes tree to add new variables
     */
    public void handleAddVarToSelected(ProductVar[] newVars, ArrayList<DefaultMutableTreeNode> selectedTreeNodes) {
        CommonTreeNode plotNode;
        CommonTreeNode pageNode;
        CommonTreeNode tableNode;
        for (int j = 0; j < selectedTreeNodes.size(); j++) {
            if (selectedTreeNodes.get(j) instanceof CommonTreeNode) {
                CommonTreeNode treeNode = (CommonTreeNode)selectedTreeNodes.get(j);

                switch (treeNode.getNodeType()) {
                    case CommonTreeNode.CURVE_NODE:
                        // If a curve already had varcase node, no var can be added.
                        // var needs to be added to one of curve's varcase node.
                        if (((ProductCurve)treeNode.getUserObject()).hasVarcase()) {
                            JOptionPane.showMessageDialog(application.getMainFrame(),
                                                          "Since the Curve already has Varcase node(s), you must\n" +
                                                          "select one Varcase node in order to add a variable!",
                                                          "Varcase Not Selected",
                                                          JOptionPane.WARNING_MESSAGE);
                            return;
                        }
                        if (newVars.length > 1) {
                            JOptionPane.showMessageDialog(application.getMainFrame(),
                                    "You can not add more than one variable to a curve!",
                                    "Ineligible Curve",
                                    JOptionPane.WARNING_MESSAGE);
                        } else if (newVars.length == 1) {
                            addVarToCurve(treeNode, newVars[0]);
                        }
                        break;
                    case CommonTreeNode.VARCASE_NODE:
                        if (newVars.length > 1) {
                            JOptionPane.showMessageDialog(application.getMainFrame(),
                                    "You can not add more than one variable to a varcase!",
                                    "Ineligible Varcase",
                                    JOptionPane.WARNING_MESSAGE);
                        } else if (newVars.length == 1) {
                            addVarToVarcase(treeNode, newVars[0]);
                        }
                        break;
                    case CommonTreeNode.PLOT_NODE:
                        for (ProductVar eachVar : newVars) {
                            addVarToPlot(treeNode, eachVar, true);
                        }
                         break;
                    case CommonTreeNode.PAGE_NODE:
                        // create a plot for each var
                        for (ProductVar eachVar : newVars) {
                            plotNode = addPlotToPage(treeNode, null, true);
                            addVarToPlot(plotNode, eachVar, true);
                        }
                        break;
                    case CommonTreeNode.X_NODE:
                    case CommonTreeNode.Y_NODE:
                        //plotNode = (CommonTreeNode)treeNode.getParent();
                        //addVarToPlot(plotNode, newVar);
                        // TODO: show warning!
                        break;
                    case CommonTreeNode.TABLE_NODE:
                        // create a new column for each var in the table
                        // TODO: 1st column time
                        for (ProductVar eachVar : newVars) {
                            addColumnToTable(treeNode, new ProductColumn(eachVar), true);
                        }
                        break;
                    case CommonTreeNode.COLUMN_NODE:
                        if (newVars.length > 1) {
                            JOptionPane.showMessageDialog(application.getMainFrame(),
                                    "You can not add more than one variable to a column!",
                                    "Ineligible Column",
                                    JOptionPane.WARNING_MESSAGE);
                        } else if (newVars.length == 1) {
                            // One column has one variable, can't add var if there is already variable defined.
                            if (treeNode.getChildCount() < 1) {
                                addVarToColumn(treeNode, newVars[0], true);
                            } else {
                                JOptionPane.showMessageDialog(application.getMainFrame(),
                                        "The column already has a variable!",
                                        "Ineligible Column",
                                        JOptionPane.WARNING_MESSAGE);
                            }
                        }
                        break;
                    //case CommonTreeNode.PROGRAMS_NODE:
                    //case CommonTreeNode.PROGRAM_NODE:
                    case CommonTreeNode.INPUT_NODE:
                        if (application.programsNode.getChildCount() > 0) {
                            //CommonTreeNode inputNode = (CommonTreeNode)(((CommonTreeNode)application.programsNode.getChildAt(0)).getChildAt(0));
                            for (ProductVar eachVar : newVars) {
                                addVarToInput(eachVar, true);
                            }
                        }
                        break;
                    case CommonTreeNode.PLOTS_NODE:
                        pageNode = addPageToPlots(new ProductPage());
                        for (ProductVar eachVar : newVars) {
                            plotNode = addPlotToPage(pageNode, new ProductPlot(), true);
                            addVarToPlot(plotNode, eachVar, true);
                        }
                        break;
                    case CommonTreeNode.TABLES_NODE:
                        tableNode = addNewTable(null);
                        // TODO: time should be from varlist instead of hardcode
                        ProductVar timeVar = new ProductVar(TrickApplication.DEFAULT_TIME_NAME, "s");
                        addColumnToTable(tableNode, new ProductColumn(timeVar), true);
                        for (ProductVar eachVar : newVars) {
                            addColumnToTable(tableNode, new ProductColumn(eachVar), true);
                        }
                        break;
                } // end switch
            } // end if selected node is instanceof CommonTreeNode
        } // end for each selected node
    }

    /**
     * Invoked when Search is performed.
     */
    public void handleSearchVar() {
        if (hiddenVarListForSearch == null) {
            hiddenVarListForSearch = new ArrayList<Object>();
        } else {
            for (Object hiddenObj : hiddenVarListForSearch) {
                LogVar hiddenVar = (LogVar)hiddenObj;
                if (hiddenVar.getPrevDisplay() != null) {
                    hiddenVar.setDisplay(hiddenVar.getPrevDisplay());
                }
            }
            hiddenVarListForSearch.clear();
            application.varList.refreshData();
        }
        if (!application.searchField.getText().isEmpty()) {
            for (Object eachObj : application.varList.getAllData()) {
                LogVar eachVar = (LogVar)eachObj;
                if (!UIUtils.searchWithWildcard(eachVar.getName(), application.searchField.getText())) {
                    hiddenVarListForSearch.add(eachVar);
                    eachVar.setPrevDisplay(eachVar.getDisplay());
                    eachVar.setDisplay(DisplayType.HIDDEN);
                }
            }
            application.varList.refreshData();
        }
    }

    /**
     * Invoked when Add Var is selected from Vars menu or user double clicks a variable.
     */
    public void handleAddVar() {
        Object[] selectedVars = application.varList.getSelectedData();
        if (selectedVars != null) {
            CommonTreeNode pageNode = null;
            CommonTreeNode plotNode = null;
            ArrayList<DefaultMutableTreeNode> selectedTreeNodes = application.productTree.getSelectedNodes();

            for (int i = 0; i < selectedVars.length; i++) {
                LogVar varFrom = (LogVar)selectedVars[i];
                if ((varFrom.getName().indexOf('-') != -1) || (varFrom.getName().matches("-(?!>)"))) {
                    List<String> vars = get_components_from_array(varFrom.getName()); // expand variable
                    if ( (selectedTreeNodes!=null) && (selectedTreeNodes.size()>0)
                            && (((CommonTreeNode)selectedTreeNodes.get(0)).getNodeType()!=CommonTreeNode.PLOTS_NODE) ) {
                        ProductVar[] newVars = new ProductVar[vars.size()];
                        // stick all arrayed components under the selected node
                        for (int ii=0; ii<vars.size(); ii++) {
                            newVars[ii] = new ProductVar(vars.get(ii), varFrom.getUnits());
                        }
                        handleAddVarToSelected(newVars, selectedTreeNodes);

                    } else {
                        // put components on new page, 1 curve per plot, 9 plots per page
                        final int CURVES_PER_PLOT = 1;
                        final int PLOTS_PER_PAGE = 9;
                        int plots=0;
                        int curves=0;
                        for (int ii=0; ii<vars.size(); ii++) {
                            ProductVar newVar = new ProductVar(vars.get(ii), varFrom.getUnits());
                            if ((plots%PLOTS_PER_PAGE==0) && (curves%CURVES_PER_PLOT==0)) {
                                ProductPage newPage = new ProductPage();
                                pageNode = new CommonTreeNode(newPage, CommonTreeNode.PAGE_NODE);
                                application.productTree.addNode(application.plotsNode, pageNode, true);
                            }
                            if (curves%CURVES_PER_PLOT==0) {
                                ProductPlot newPlot = new ProductPlot();
                                plotNode = addPlotToPage(pageNode, newPlot, true);
                                plots++;
                            }
                            addVarToPlot(plotNode, newVar, true);
                            curves++;
                        }
                    }
                } else {
                    // non-arrayed variable
                    ProductVar newVar = new ProductVar(varFrom.getName(), varFrom.getUnits());
                    ProductVar[] newVars = new ProductVar[1];
                    newVars[0] = newVar;
                    if (selectedTreeNodes != null && selectedTreeNodes.size()>0 ) {
                        handleAddVarToSelected(newVars, selectedTreeNodes);
                    } else {
                        ProductPage newPage = new ProductPage();
                        pageNode = new CommonTreeNode(newPage, CommonTreeNode.PAGE_NODE);
                        application.productTree.addNode(application.plotsNode, pageNode, true);
                        ProductPlot newPlot = new ProductPlot();
                        plotNode = addPlotToPage(pageNode, newPlot, true);
                        addVarToPlot(plotNode, newVar, true);
                    }
                }
            }
        }
    }

    // traverse varlist and add an array variable for all sequential components
    public void create_arrays() {

        String current_nonarrayed_name = "";
        int num_components = 0;
        Object[] all_vars = application.varList.getAllData();
        boolean done = false;
        List<Object> components = new ArrayList<Object>();
        for (int row=0; row<all_vars.length; row++) {
            LogVar var = (LogVar) all_vars[row];
            String compname = var.getName();
            String nonarrayed_name = compname.replaceAll("\\[(\\d+)\\]", "X");
            //System.out.println(compname +" -> " +nonarrayed_name);
            if (compname.equals(nonarrayed_name)) {
                // non arrayed variable, component group done
                done = true;
            } else if (num_components==0) {
                // 1st arrayed component of group
                current_nonarrayed_name = nonarrayed_name;
                done = false;
            } else if (!nonarrayed_name.equals(current_nonarrayed_name)) {
                // this component is from different array, component group done
                row--; // start from this component next loop
                done = true;
            }

            if (!done) {
                components.add(var);
                num_components++;
                // done if it's the last row
                if (row == all_vars.length - 1) {
                    done = true;
                }
            }

            if (done) {
                if (num_components>0) {
                    //System.out.println("\ncomponents= " +components + ", num_components=" + num_components);
                    if (num_components>1) {
                        create_array_from_components(components);
                        //application.varList.addData(get_array_from_components(components));
                    }
                    current_nonarrayed_name = "";
                    components.clear();
                    num_components = 0;
                }
            }

        } // end for row
    }

    // helper function for create_arrays
    // create array variable(s) for the given list of components and put in varlist
    // (e.g. if list contains x[1], x[2], x[3], then return x[1-3]
    public void create_array_from_components(List<Object> components) {

        List<String> arrays = new ArrayList<String>();
        List<Integer> first_components = new ArrayList<Integer>();
        List<Integer> last_components = new ArrayList<Integer>();
        int components_in_range = 0;
        int start = -1;
        int first_component_index = -1;
        String compname = "";
        String front = "";
        String back = "";
        int open = -1;
        int close = -1;
        for (int ii=0; ii<components.size(); ii++) {
            LogVar var = (LogVar) components.get(ii);
            compname = var.getName();
            open = compname.lastIndexOf("[");
            if (open==-1) {
                // no more dimensions
                break;
            }
            close = compname.lastIndexOf("]");
            int index = Integer.parseInt(compname.substring(open+1,close));
            if (components_in_range==0) {
                // 1st component in range
                start = index;
                components_in_range++;
                front = compname.substring(0,open);
                back = compname.substring(close+1);
                first_component_index = ii;
            } else if ((index==start+components_in_range) &&
                       front.equals(compname.substring(0,open)) &&
                       back.equals(compname.substring(close+1))) {
                // next component in range
                components_in_range++;
            } else {
                // end of range, create new array variable
                if (components_in_range>1) {
                    String range = start + "-" + (start+components_in_range-1);
                    arrays.add(front +"(" +range +")" +back); // use parens so we can find next bracket
                    first_components.add(first_component_index);
                    last_components.add(first_component_index + components_in_range-1);
                }
                // start next range
                start = index;
                components_in_range=1;
                front = compname.substring(0,open);
                back = compname.substring(close+1);
                first_component_index = ii;
            }
        } // end for each component obj
        if (components_in_range>1) {
            // end of range is last component
            String range = start + "-" + (start+components_in_range-1);
            arrays.add(front +"(" +range +")" +back); // use parens so we can find next bracket
            first_components.add(first_component_index);
            last_components.add(first_component_index + components_in_range-1);
        }

        // create the new array object(s) and add to varlist (as expanded)
        if (!arrays.isEmpty()) {
            //System.out.println("\narrays= " +arrays);
            List<Object> arrayObjs = new ArrayList<Object>();
            for (int ii=0; ii<arrays.size(); ii++) {
                first_component_index = first_components.get(ii);
                int last_component_index = last_components.get(ii);
                LogVar arrayvar = new LogVar(components.get(first_component_index));
                arrayvar.setName(arrays.get(ii));
                arrayvar.setDisplay(DisplayType.EXPANDED);
                // change all of array's components sort name so they are displayed under array
                for (int jj=first_component_index; jj<=last_component_index; jj++) {
                    LogVar compvar = (LogVar)application.varList.getElement(components.get(jj));
                    compvar.setSortName(arrayvar.getSortName() + compvar.getSortName());
                    compvar.setLevel(arrayvar.getLevel()+1);
                    // set the array var's RUNdir count to the least component RUNdir count
                    if (compvar.getCount() < arrayvar.getCount()) {
                        arrayvar.setCount(compvar.getCount());
                    }
                }

                arrayObjs.add(arrayvar);
                application.varList.addData(arrayvar);
            }
            // see if there are more array levels to do
            create_array_from_components(arrayObjs);

            // post processing for the new arrays in varlist:
            for (int ii=0; ii<arrays.size(); ii++) {
                LogVar arrayvar = (LogVar) application.varList.getElement(arrayObjs.get(ii));
                if (arrayvar==null) {
                    System.out.println("ERROR null for" +arrayObjs.get(ii));
                    return;
                }
                // put brackets back into array name in place of parens that we substituted
                // (must keep the sort name created previously otherwise it screws up)
                String save_sort_name = arrayvar.getSortName();
                arrayvar.setName(arrays.get(ii).replaceAll("\\(", "\\[").replaceAll("\\)", "\\]"));
                arrayvar.setSortName(save_sort_name);
                // if another array level was added above this array, update this array's component levels for proper display
                if (arrayvar.getLevel()==0) {
                    continue;
                }
                first_component_index = first_components.get(ii);
                int last_component_index = last_components.get(ii);
                for (int jj=first_component_index; jj<=last_component_index; jj++) {
                    LogVar compvar = (LogVar) application.varList.getElement(components.get(jj));
                    if (compvar==null) {
                        // this should not happen!
                        System.out.println("ERROR null for" +components.get(jj));
                        continue;
                    }
                    compvar.setSortName(arrayvar.getSortName() + compvar.getSortName());
                    compvar.setLevel(arrayvar.getLevel()+1);
                }
            }

        }
    }

    // helper function used by handleAddVar and addVarsFromRun
    // expand each dimension in arrayed variable, from last to first, and return all components as a list
    public List<String> get_components_from_array(String arrayvar) {
        // MAX_DIMENSIONS is how many ranges we can handle splitting in a variable
        final int MAX_DIMENSIONS = 3;
        String[] parts = arrayvar.split("-(?!>)");
        int numparts = parts.length;
        if (numparts==1) {
            // no range to split
            return null;
        } else if (numparts > MAX_DIMENSIONS+1) {
            // variable with more than MAX_DIMENSIONS
            return null;
        }

        int dim = numparts-2; // dimensions can go from 0 to MAX_DIMENSIONS-1
        String[] frontpart = new String[MAX_DIMENSIONS];
        String endpart = "";
        int[] start = new int[MAX_DIMENSIONS];
        int[] end = new int[MAX_DIMENSIONS];
        int[] count = new int[MAX_DIMENSIONS];
        int count_total = 0;

        // pick out parts of variable name we need to create components
        while (dim>=0) {
            // start from end of variable and work backwards
            int bracket1 = parts[dim].lastIndexOf('[')+1;
            int bracket2 = parts[dim+1].indexOf(']');
            // get starting and ending index for the range
            start[dim] = Integer.parseInt(parts[dim].substring(bracket1));
            end[dim] = Integer.parseInt(parts[dim+1].substring(0,bracket2));
            // get the parts of string to go in front of and in back of the index
            int open = parts[dim].lastIndexOf("[");
            if (dim>0) {
                // not last dimension, so start at close bracket of previous dim
                int close = parts[dim].indexOf("]") + 1;
                frontpart[dim] = parts[dim].substring(close, (open+1));
            } else {
                frontpart[dim] = parts[dim].substring(0, (open+1));
            }
            // count total components
            count[dim] += (end[dim] - start[dim] + 1);
            dim--;
        }

        // if "]" is not the end of this variable path, need to keep all the remaining part
        // and append to each of generated component name as below.
        // otherwise, entpart is an empty string.
        int lastEndBracket = arrayvar.lastIndexOf(']');
        if (lastEndBracket < (arrayvar.length()-1)) {
            endpart = arrayvar.substring(lastEndBracket+1);
        }

        // build a string for each component and return as a list
        String results[] = null;
        List<String> total = new ArrayList<String>();
        // count the total components
        count_total = count[0];
        for (int ii=1; ii<MAX_DIMENSIONS; ii++) {
            if (count[ii]>0) {
                count_total *= count[ii];
            }
        }
        results = new String[count_total];
        //System.out.println("__total = " +count_total);
        count_total = 0;
        for (int ii=0; ii<count[0]; ii++) { // dimension 0
            String dim0 = frontpart[0] +(start[0]+ii) +"]";
            if (count[1]>0) {
                for (int jj=0; jj<count[1]; jj++) { // dimension 1
                    String dim1 = frontpart[1] +(start[1]+jj) +"]";
                    if (count[2]>0) {
                        for (int kk=0; kk<count[2]; kk++) { // dimension 2
                            String dim2 = frontpart[2] +(start[2]+kk) +"]";
                            results[count_total] = dim0 + dim1 + dim2 + endpart;
                            //System.out.println(results[count_total]);
                            total.add(results[count_total++]);
                        }
                    } else {
                        results[count_total] = dim0 + dim1 + endpart;
                        //System.out.println(results[count_total]);
                        total.add(results[count_total++]);
                    }
                }
            } else {
                results[count_total] = dim0 + endpart;
                //System.out.println(results[count_total]);
                total.add(results[count_total++]);
            }
        }
        return total;
    }


    // expand an arrayed variable (e.g. x[1-3]) to its individual components (e.g. x[1] x[2] x[3]) in displayed varlist
    public void expandVar() {
        Object[] selectedVars = application.varList.getSelectedData();

        // if nothing is selected, expand all expandable vars
        if (selectedVars == null || selectedVars.length == 0) {
            for (Object eachObj : application.varList.getAllData()) {
                LogVar eachVar = (LogVar)eachObj;
                // expand all array-ed vars
                if (eachVar.getName().indexOf("-") != -1) {
                    eachVar.setDisplay(DisplayType.EXPANDED);
                } else if (eachVar.getName().indexOf("[") != -1) { // if the var is one of array-ed var element, hide it
                    eachVar.setDisplay(DisplayType.NORMAL);
                }
            }
        } else { // otherwise only expand selected vars
            int varlist_indices[] = application.varList.getJList().getSelectedIndices();
            for (int ii=0; ii < selectedVars.length; ii++) {
                LogVar mainvar = (LogVar)selectedVars[ii];
                // if variable is not arrayed, do nothing
                // (even if it is expanded, it may have sublevel arrays that have been contracted)
                if (mainvar.getDisplay()==DisplayType.NORMAL) {
                    continue;
                }

                Object[] all_vars = application.varList.getAllData();
                for (int row=varlist_indices[ii]+1; row<all_vars.length; row++) {
                    LogVar var = (LogVar) all_vars[row];
                    if (var.getLevel() > mainvar.getLevel()) {
                        if (var.getName().indexOf("-")!=-1) {
                            // array
                            var.setDisplay(DisplayType.EXPANDED);
                        } else {
                            // non-array
                            var.setDisplay(DisplayType.NORMAL);
                        }
                    } else {
                        break;
                    }
                }
                mainvar.setDisplay(DisplayType.EXPANDED);
            }
        }
        application.varList.refreshData();
    }

    // contract a previously expanded variable
    public void contractVar() {
        Object[] selectedVars = application.varList.getSelectedData();

        // only want something like [0-2][0-2] and don't want things like [0][0-2]
        String arrayEx = "\\w+(\\[[0-9]+\\-[0-9]+\\])+\\w*";
        Pattern arrayPattern = Pattern.compile(arrayEx);

        // if nothing is selected, contract all contractable vars
        if (selectedVars == null || selectedVars.length == 0) {
            for (Object eachObj : application.varList.getAllData()) {
                LogVar eachVar = (LogVar)eachObj;
                // contract all array-ed vars
                Matcher matcher = arrayPattern.matcher(eachVar.getName());
                // if only based on dash (-), [0][0-2] would be displayed.
                // I am thinking at this point, only [0-2][0-2] should be displayed,
                // all its elements such as [0][0-2], [1][0-2], or [2][0-2] should be hidden.
                // so changed to use Java Pattern & Matcher
                //if (eachVar.getName().indexOf("-") != -1) {
                if (matcher.find()) {
                    eachVar.setDisplay(DisplayType.CONTRACTED);
                } else if (eachVar.getLevel() > 0 && eachVar.getName().indexOf("[") != -1 && eachVar.getName().endsWith("]")) { 
                    // if the var is one of array-ed var element & "[]" can't be in the middle (could be a pointer), also if this varisn't at level 0, hide it
                    //System.out.println("HIDDEN var..." + eachVar.getName() + " and its level ..." + eachVar.getLevel());
                    eachVar.setDisplay(DisplayType.HIDDEN);
                }
            }
        } else { // otherwise only contract selected variables
            int varlist_indices[] = application.varList.getJList().getSelectedIndices();
            for (int ii=0; ii < selectedVars.length; ii++) {
                LogVar mainvar = (LogVar)selectedVars[ii];
                // if variable is not arrayed or has already been expanded, do nothing
                if (mainvar.getDisplay()!=DisplayType.EXPANDED) {
                    continue;
                }

                Object[] all_vars = application.varList.getAllData();
                for (int row=varlist_indices[ii]+1; row<all_vars.length; row++) {
                    LogVar var = (LogVar) all_vars[row];
                    if (var.getLevel() > mainvar.getLevel()) {
                        var.setDisplay(DisplayType.HIDDEN);
                    } else {
                        break;
                    }
                }
                mainvar.setDisplay(DisplayType.CONTRACTED);
            }
        }
        application.varList.refreshData();
    }

    /**
     * Invoked when Change Unit is selected.
     */
    public void changeUnits() {
        Object selectedFirstData = application.varList.getSelectedFirstData();
        if (selectedFirstData != null) {
            LogVar firstSelectedVar = (LogVar)selectedFirstData;
            String selectedVarUnits = firstSelectedVar.getUnits();
            if (selectedVarUnits != null && !selectedVarUnits.isEmpty()) {
                List<String> unitList = UnitType.getAll(selectedVarUnits);
                if (unitList != null && unitList.size() > 0) {
                    Object newUnits = UIUtils.showListInputDialog(application.getMainFrame(),
                                                                  "Change " + selectedVarUnits + " to:",
                                                                  "Change Units ...",
                                                                  JOptionPane.QUESTION_MESSAGE,
                                                                  null,
                                                                  unitList.toArray(),
                                                                  selectedVarUnits);

                    if (newUnits != null) {
                        changeVarUnits(firstSelectedVar, newUnits.toString());
                        application.varList.refreshData();
                    }
                }
            } else {
                JOptionPane.showMessageDialog(application.getMainFrame(),
                                              "Units for " + firstSelectedVar.getName() + " is not available. Can't change units!",
                                              "No Units Sepcification",
                                              JOptionPane.WARNING_MESSAGE);
            }
         }
    }

    /**
     * Helper method for changing the units for a {@link LogVar}.
     */
    private void changeVarUnits(LogVar fromVar, String newUnits) {
        // if arrayed var, need to change the units for all elements
        if (fromVar.getName().indexOf('-') != -1) {
            Object[] allVars = application.varList.getAllData();
            for (int row = application.varList.getJList().getSelectedIndex() + 1; row < allVars.length; row++) {
                LogVar var = (LogVar) allVars[row];
                if (var.getLevel() > fromVar.getLevel()) {
                    changeVarUnits(var, newUnits);
                } else {
                    break;
                }
            }
        }

        // change the units for the var including both array and non-array vars
        fromVar.setUnits(newUnits);
    }

    /**
     * Invoked when Add Run ... is selected from Runs menu.
     */
    public void handleAddRun() {
        File dir = UIUtils.chooseDir("Add Run", ".", application.getMainFrame());

        if (dir != null && (dir.getName().startsWith("RUN") || dir.getName().startsWith("MONTE_RUN"))) {
            if (!dir.exists()) {
                // in the file chooser, user may double click RUN dir so he is inside RUN dir when he clicks ok
                // when this happens, the final dir name is appended twice (it will be /blah/blah/blah/RUN_whatever/RUN_whatever)
                // so take that last name off of the dir name
                dir = dir.getParentFile();
            }
            application.runList.addData(new SessionRun(dir.getAbsolutePath()));
            handleRefreshDP();
        }
    }

    /**
     * Invoked when one or more runs added to trick_dp. The is a remote call.
     * So add invokeLater for the GUI update as Java Swing is not thread safe.
     * TODO: Haven't added this mechanism to any other places as this is the
     *       only one is called by other application for now. Add other
     *       places if necessary.
     * @param dirList list of directories
     */
    public void handleAddRuns(final String[] dirList) {

        if (dirList != null && dirList.length > 0) {
            // start another thread for updating the list gui
            (new Thread() {
                @Override
                public void run() {
                    for (String dir : dirList) {
                        File runDir = new File(dir);
                        if (runDir.exists()) {
                            application.runList.addData(new SessionRun(runDir.getAbsolutePath()));
                            try {
                                // TODO: find out why has to be refresh for each run added?
                                SwingUtilities.invokeAndWait(new Runnable() {
                                    public void run() {
                                        handleRefreshDP();
                                    }
                                });
                            } catch (Exception ie) {

                            }
                        }
                    }
                }
            }).start();
        }
    }

    /**
     * Invoked when Remove Run is selected from Runs menu.
     */
    public void handleRemoveRun() {
        Object[] selectedData = application.runList.getSelectedData();
        if (selectedData != null && selectedData.length>0) {
            // for each selected RUN dir, remove its mapping
            for (int ii=0; ii<selectedData.length; ii++) {
                String key = selectedData[ii].toString();
                //Object[] vars = runVarMap.get(key).toArray();
                runVarMap.remove(key);
                //for (int jj=0; jj<vars.length; jj++) {
                    //application.varList.removeData(vars[jj]);
                //}
            }
            // remove the selected runs from displayed runlist
            application.runList.removeSelectedData();
            handleRefreshDP();
        }

    }

    /**
     * Invoked when New Page is selected from Plots menu.
     */
    public void handleNewPage() {
        addPageToPlots(new ProductPage());
    }

    /**
     * Invoked when New Plot is selected from Plots menu.
     */
    public void handleNewPlot() {
        addNewNode(CommonTreeNode.PAGE_NODE, CommonTreeNode.PLOT_NODE, new ProductPlot(), "Page Not Selected");
    }

    /**
     * Invoked when New Curve is selected from Plots menu.
     */
    public void handleNewCurve() {
       addNewNode(CommonTreeNode.PLOT_NODE, CommonTreeNode.CURVE_NODE, new ProductCurve(), "Plot Not Selected");
    }

    /**
     * Invoked when New Varcase is selected from Plots menu.
     */
    public void handleNewVarcase() {
        CommonTreeNode newNode = addNewNode(CommonTreeNode.CURVE_NODE, CommonTreeNode.VARCASE_NODE, new ProductVarcase(), "Curve Not Selected");
        // if it's at right place for this new varcase node, add default timename as its X
        if (newNode != null) {
            addVarToVarcase(newNode, new ProductVar(TrickApplication.DEFAULT_TIME_NAME, "s"));
        }
    }

    /**
     * Invoked when New Table is selected from Tables menu.
     */
    public void handleNewTable() {
        addNewTable(new ProductTable());
    }

    /**
     * Invoked when New Column is selected from Tables menu.
     */
    public void handleNewColumn() {
        addNewNode(CommonTreeNode.TABLE_NODE, CommonTreeNode.COLUMN_NODE, new ProductColumn(), "Table Not Selected");
    }

    /**
     * Invoked when New Program is selected from Programs menu.
     */
    public void handleNewProgram() {
        addNewProgram(new ProductExternalFunction());
    }

    /**
     * Invoked when new output is entered.
     * @param newOutput not sure
     */
    public void handleNewProgramOutput(String newOutput) {
        addVarToOutput(new ProductMeasurement(newOutput), true);
    }

    /**
     * Invoked when Subtract is selected.
     *
     * This function generates nodes for (var1 - var2)
     * @param var1 var1
     * @param var2 var2
     */
    public void handleSubtractFunction(LogVar var1, LogVar var2) {
        String units = null;
        StringBuffer varBuffer = new StringBuffer();
        varBuffer.append("delta(");

        // if this var is not a true log var, quit
        if (!var1.getIsFromLog()) {
            JOptionPane.showMessageDialog(application.getMainFrame(),
                                          "Variable " + var1.getName() + " can't not be used for functions !\n",
                                          "Error",
                                          JOptionPane.ERROR_MESSAGE);
            return;
        }
        if (!var2.getIsFromLog()) {
            JOptionPane.showMessageDialog(application.getMainFrame(),
                                          "Variable " + var2.getName() + " can't not be used for functions !\n",
                                          "Error",
                                          JOptionPane.ERROR_MESSAGE);
            return;
        }
        // var1
        varBuffer.append(var1.getName());
        varBuffer.append(":");
        if (var1.getRunDir() == null) {
            if (application.runList.getSelectedFirstData() == null) {
                varBuffer.append(((SessionRun)application.runList.getAllData()[0]).getDir());
            } else {
                varBuffer.append(((SessionRun)application.runList.getSelectedFirstData()).getDir());
            }
        } else {
            varBuffer.append(var1.getRunDir());
        }
        // TODO: do we set units for delta?
        units = var1.getUnits();
        varBuffer.append(",");

        // var2
        varBuffer.append(var2.getName());
        varBuffer.append(":");
        if (var2.getRunDir() == null) {
            if (application.runList.getSelectedFirstData() == null) {
                varBuffer.append(((SessionRun)application.runList.getAllData()[0]).getDir());
            } else {
                varBuffer.append(((SessionRun)application.runList.getSelectedFirstData()).getDir());
            }
        } else {
            varBuffer.append(var2.getRunDir());
        }
        varBuffer.append(")");

        ProductVar newVar = new ProductVar(varBuffer.toString(), units);
        ProductPage newPage = new ProductPage();
        CommonTreeNode pageNode = new CommonTreeNode(newPage, CommonTreeNode.PAGE_NODE);
        application.productTree.addNode(application.plotsNode, pageNode, true);
        ProductPlot newPlot = new ProductPlot();
        CommonTreeNode plotNode = addPlotToPage(pageNode, newPlot, true);
        addVarToPlot(plotNode, newVar, true);
    }

    /**
     * Opens a DP XML file.
     *
     * @param fileName    The name of DP XML file.
     */
    @SuppressWarnings("rawtypes")
    public void openDP(String fileName) {
        if (fileName == null) {
            return;
        } else {
            File f = new File(fileName);
            if (!f.exists()) {
                System.out.println("Warning: " + fileName + " does not exist!");
                return;
            }
        }

        // if parsing fails, stop!
        try {
            Product productObject = ProductDomParser.parse(new File(fileName));

            application.versionField.setText(productObject.getVersion());

            application.titleField.setText(productObject.getTitle());
            if (productObject.getTStart() != null) {
                application.startField.setText(productObject.getTStart().toString());
            }

            if (productObject.getTStop() != null) {
                application.stopField.setText(productObject.getTStop().toString());
            }

            if (productObject.getFrequency() != null) {
                application.freqField.setText(productObject.getFrequency().toString());
            }

            // go through all pages
            List pageList = productObject.getPageList();
            if (pageList != null) {
                for (int i = 0; i < pageList.size(); i ++){
                    ProductPage page = (ProductPage)pageList.get(i);
                    addPageToPlots(page);
                }
            } // end of (pageList != null)

            // go through all tables
            List tableList = productObject.getTableList();
            if (tableList != null) {
                    for (int i = 0; i < tableList.size(); i ++){
                    ProductTable table = (ProductTable)tableList.get(i);
                    CommonTreeNode tableNode = addNewTable(table);

                    // go through all columns
                    List columnList = table.getColumnList();
                    if (columnList != null) {
                        for (int j = 0; j < columnList.size(); j++) {
                            ProductColumn column = (ProductColumn)columnList.get(j);
                            addColumnToTable(tableNode, column, false);
                        }
                    }
                }
            }

            // go through all extfn
            List functionList = productObject.getFunctionList();
            if (functionList != null) {
                for (int i = 0; i < functionList.size(); i++) {
                    ProductExternalFunction function = (ProductExternalFunction)functionList.get(i);
                    addNewProgram(function);

                    // go through all inputs
                    List inputList = function.getInputs();
                    if (inputList != null) {
                        for (int j = 0; j < inputList.size(); j++) {
                            ProductVar var = (ProductVar)inputList.get(j);
                            addVarToInput(var, false);
                        }
                    }

                    // go through all outputs
                    List outputList = function.getOutputs();
                    if (outputList != null) {
                        for (int k = 0; k < outputList.size(); k++) {
                            ProductMeasurement measurement = (ProductMeasurement)outputList.get(k);
                            addVarToOutput(measurement, false);
                        }
                    }
                }
            }

        } catch (Exception e) {
            application.printStatusMessage(e.getMessage());
            application.printStatusMessage("\nERROR parsing " + fileName + " !!!\n");
            return;
        }

        // reset the productTree change status once a new DP_ file is opened
        // the reason for resetting the change status here as this is the last point of
        // opening a DP_ file
        application.productTree.setChanged(false);

        application.getMainFrame().setTitle(fileName + " - " + application.resourceMap.getString("mainFrame.title"));
    }

    /**
     * Adds variables to the list for the specified the run directory.
     *
     * @param runDir    The run directory.
     */
    public void addVarsForRun(File runDir) {
        File[] files = UIUtils.getListFiles(runDir, TrickFileFilter.TRICK_DATA_RECORD);

        // do nothing if no data recording files found
        if (files == null ) {
            return;
        }

        String runDirPath = runDir.toString();
        // Do nothing if this RUN dir has already been added
        if (runVarMap.containsKey(runDirPath)) {
            return;
        }

        // go through each data recording file (.header, .trk, .h5?, and .csv)
        for (int i = 0; i < files.length; i++) {

            // ignore those frame log related files.
            // TODO: better way!
            if (files[i].getPath().endsWith("log_frame.header")     ||
                files[i].getPath().endsWith("log_trickjobs.header") ||
                files[i].getPath().endsWith("log_userjobs.header")  ||
                files[i].getPath().endsWith("log_frame.trk")        ||
                files[i].getPath().endsWith("log_trickjobs.trk")    ||
                files[i].getPath().endsWith("log_userjobs.trk")) {
                continue;
            }
            // if the file is not the .header file, check to see
            // if the corresponding .header file exists. If yes, skip
            // this data file as processing a header file which is much
            // smaller should be faster
            if (!files[i].getPath().endsWith(".header")) {
                String onlyName = UIUtils.getFileNameWithoutExtension(files[i]);
                if (new File(runDir, onlyName + ".header").exists()) {
                    continue;
                }
            }
            DataReader reader = null;

            if (files[i].getName().toLowerCase().endsWith(".header")) {
                reader = new LogHeaderReader(files[i]);
            } else if (files[i].getName().toLowerCase().endsWith(".trk")) {
                reader = new BinaryDataReader(files[i]);
            } else if (files[i].getName().toLowerCase().endsWith(".h5")) {
                // TODO: add reading .h5
                continue;
            } else if (files[i].getName().toLowerCase().endsWith(".csv")) {
                reader = new CSVDataReader(files[i]);
            }
            if (reader == null) {
                continue;
            }

            List<Object> new_vars = reader.getRecordedVarList();
            List<Object> run_vars = new ArrayList<Object>();
            if (runVarMap.containsKey(runDirPath)) {
                // at least one file already read in for this RUN dir
                run_vars = runVarMap.get(runDirPath);
            }
            for (Object obj : new_vars) {
                if (!run_vars.contains(obj)) {
                    // add all arrayed variable's components to varlist
                    LogVar mainvar = (LogVar) obj;
                    List<String> components = get_components_from_array(mainvar
                            .getName());
                    // List<Object> new_vars = new ArrayList<Object>();
                    if (components != null) {
                        // arrayed
                        for (String name : components) {
                            LogVar compvar = new LogVar(obj);
                            compvar.setName(name);
                            run_vars.add(compvar);
                            // add new component variable to displayed varlist
                            application.varList.addData(compvar);
                        }
                    } else {
                        // non-arrayed
                        run_vars.add(obj);
                        // add new variable to displayed varlist
                        application.varList.addData(obj);
                    }

                    application.varList.getJList().clearSelection();
                }
            }

            // add this <key: RUN dir> <values: all variables> to the HashMap
            runVarMap.put(runDirPath, run_vars);
        }
        //System.out.println("\nmap (" +runVarMap.get(runDirPath).size() +") :" +runVarMap.get(runDirPath) +"\n");

    }

    /**
     * Helper method for cleaning up the {@link ProductTree}.
     */
    private void resetGUI() {
        // Change the title back to original
        application.getMainFrame().setTitle(application.resourceMap.getString("mainFrame.title"));

        // Remove all previous shown data
        application.plotsNode.removeAllChildren();
        application.tablesNode.removeAllChildren();
        application.programsNode.removeAllChildren();
        application.productTree.getDefaultModel().reload();

        application.resetCommonFields();
    }

    /**
     * Helper method for saving as a dp file.
     *
     * @return True if a DP_ file is actually saved, false otherwise such as due to cancelation.
     */
    private boolean saveAs() {
        File file = UIUtils.chooseSaveFile("DP_Product", "DP_", "xml", application.getMainFrame());
        if (file == null) {
            return false;
        }
        dpFileName = file.getPath();
        application.saveDPToFile(dpFileName, true);

        // reset the productTree change status once the DP_ file is saved
        application.productTree.setChanged(false);

        return true;
    }


    /**
     * Helper method for adding a var to a Plot node.
     */
    private void addVarToPlot(CommonTreeNode plotNode, Object var, boolean isNew) {
        ProductCurve curve = new ProductCurve();
        CommonTreeNode curveNode = new CommonTreeNode(curve, CommonTreeNode.CURVE_NODE);
        application.productTree.addNode(plotNode, curveNode, true);
        if (isNew) {
            ((ProductPlot)plotNode.getUserObject()).addCurve(curve);
        }
        addVarToCurve(curveNode, var);
    }

    /**
     * Helper method for adding a var to a Curve node.
     */
    private void addVarToCurve(CommonTreeNode curveNode, Object var) {
        ProductCurve curve = (ProductCurve)curveNode.getUserObject();
        int childCount = curveNode.getChildCount();
        // TODO: increase to 2 if need to support X, Y, & Z.
        if (childCount > 1) {
            JOptionPane.showMessageDialog(application.getMainFrame(),
                        "You can not add a variable to a curver that has 2 variables already!",
                        "Ineligible Curve",
                        JOptionPane.WARNING_MESSAGE);
            return;
        }

        ProductVar theVar = null;
        if (var instanceof LogVar) {
            LogVar logVar = (LogVar)var;
            theVar = new ProductVar(logVar.getName(), logVar.getUnits());
        } else if (var instanceof ProductVar) {
            theVar = (ProductVar)var;
        }
        CommonTreeNode newVarNode = null;
        switch (childCount) {
            case 0:
                ProductVar xVar = new ProductVar(TrickApplication.DEFAULT_TIME_NAME, "s");
                curve.setX(xVar);
                newVarNode = new CommonTreeNode(xVar, CommonTreeNode.X_NODE);
                application.productTree.addNode(curveNode, newVarNode, true);
                // if the new var happens to be the default time var, no need to
                // go to case 1, break here.
                if (theVar.getName().equals(xVar.getName())) {
                    break;
                }
            case 1:
                CommonTreeNode child = (CommonTreeNode)curveNode.getChildAt(0);
                int index = -1;
                if (child.getNodeType() == CommonTreeNode.X_NODE) {
                    curve.setY(theVar);
                    newVarNode = new CommonTreeNode(theVar, CommonTreeNode.Y_NODE);
                    index = curveNode.getChildCount();

                } else if (child.getNodeType() == CommonTreeNode.Y_NODE) {
                    curve.setX(theVar);
                    newVarNode = new CommonTreeNode(theVar, CommonTreeNode.X_NODE);
                    index = 0;
                }
                application.productTree.addNode(curveNode, newVarNode, index, true);
                break;
            case 2:
                // TODO: for Z?
                //curve.setZ(theVar);
                //CommonTreeNode zNode = new CommonTreeNode(theVar, CommonTreeNode.Z_NODE);
                //application.productTree.addNode(curveNode, zNode, true);
                break;
        }
    }

    /**
     * Helper method for adding a var to a Curve node.
     */
    private void addVarToVarcase(CommonTreeNode varcaseNode, Object var) {
        ProductVarcase varcase = (ProductVarcase)varcaseNode.getUserObject();
        int childCount = varcaseNode.getChildCount();
        // TODO: increase to 2 if need to support X, Y, & Z.
        if (childCount > 1) {
            JOptionPane.showMessageDialog(application.getMainFrame(),
                        "You can not add a variable to a varcase that has 2 variables already!",
                        "Ineligible Varcase",
                        JOptionPane.WARNING_MESSAGE);
            return;
        }

        ProductVar theVar = null;
        if (var instanceof LogVar) {
            LogVar logVar = (LogVar)var;
            theVar = new ProductVar(logVar.getName(), logVar.getUnits());
        } else if (var instanceof ProductVar) {
            theVar = (ProductVar)var;
        }
        CommonTreeNode newVarNode = null;
        switch (childCount) {
            case 0:
                ProductVar xVar = new ProductVar(TrickApplication.DEFAULT_TIME_NAME, "s");
                varcase.setX(xVar);
                newVarNode = new CommonTreeNode(xVar, CommonTreeNode.X_NODE);
                application.productTree.addNode(varcaseNode, newVarNode, true);
                // if the new var happens to be the default time var, no need to
                // go to case 1, break here.
                if (theVar.getName().equals(xVar.getName())) {
                    break;
                }
            case 1:
                CommonTreeNode child = (CommonTreeNode)varcaseNode.getChildAt(0);
                int index = -1;
                if (child.getNodeType() == CommonTreeNode.X_NODE) {
                    varcase.setY(theVar);
                    newVarNode = new CommonTreeNode(theVar, CommonTreeNode.Y_NODE);
                    index = varcaseNode.getChildCount();
                } else if (child.getNodeType() == CommonTreeNode.Y_NODE) {
                    varcase.setX(theVar);
                    newVarNode = new CommonTreeNode(theVar, CommonTreeNode.X_NODE);
                    index = 0;
                }
                application.productTree.addNode(varcaseNode, newVarNode, index, true);
                break;
            case 2:
                // TODO: for Z?
                //curve.setZ(theVar);
                //CommonTreeNode zNode = new CommonTreeNode(theVar, CommonTreeNode.Z_NODE);
                //application.productTree.addNode(curveNode, zNode, true);
                break;
        }
    }

    public CommonTreeNode addVarcaseToCurve(CommonTreeNode curveNode, Object varcase, boolean isNew) {
        CommonTreeNode varcaseNode = null;
        if (varcase == null) {
            varcaseNode = new CommonTreeNode(new ProductVarcase(), CommonTreeNode.VARCASE_NODE);
        } else {
            varcaseNode = new CommonTreeNode(varcase, CommonTreeNode.VARCASE_NODE);
        }
        application.productTree.addNode(curveNode, varcaseNode, true);
        ProductVarcase productVarcase = (ProductVarcase)varcaseNode.getUserObject();
        if (isNew) {
            ((ProductCurve)curveNode.getUserObject()).addVarcase(productVarcase);
        }

        if (productVarcase.getX() != null) {
            CommonTreeNode xNode = new CommonTreeNode(productVarcase.getX(), CommonTreeNode.X_NODE);
            application.productTree.addNode(varcaseNode, xNode, true);
        }
        if (productVarcase.getY() != null) {
            CommonTreeNode yNode = new CommonTreeNode(productVarcase.getY(), CommonTreeNode.Y_NODE);
            application.productTree.addNode(varcaseNode, yNode, true);
        }
        if (productVarcase.getZ() != null) {
            CommonTreeNode zNode = new CommonTreeNode(productVarcase.getZ(), CommonTreeNode.Z_NODE);
            application.productTree.addNode(varcaseNode, zNode, true);
        }

        return varcaseNode;
    }

    /**
     * Adds a new Curve to the specified Plot.
     *
     * @param plotNode    The plot node.
     * @param curve       The curve object. If <code>null</code>, a new {@link ProductCurve}
     *                    is created for the curve node, otherwise, the curve node
     *                    is created based on the specified curve object.
     * @param idx         The index location for this new object. If out of range, adding as the last one.
     *                    The valid range would be [0, total child count of this plot].
     * @param isNew       <code>true</code>, if the user object of the plot node does not
     *                    have the curve user object in its curve list, <code>false</code> otherwise.
     * @return curve node.
     */
    public CommonTreeNode addCurveToPlot(CommonTreeNode plotNode, Object curve, int idx, boolean isNew) {
        CommonTreeNode curveNode = null;
        if (curve == null) {
            curveNode = new CommonTreeNode(new ProductCurve(), CommonTreeNode.CURVE_NODE);
        } else {
            curveNode = new CommonTreeNode(curve, CommonTreeNode.CURVE_NODE);
        }
        application.productTree.addNode(plotNode, curveNode, idx, true);
        ProductCurve productCurve = (ProductCurve)curveNode.getUserObject();
        if (isNew) {
            if (idx == -1) {
                ((ProductPlot)plotNode.getUserObject()).addCurve(productCurve);
            } else {
                ((ProductPlot)plotNode.getUserObject()).addCurve(productCurve, idx);
            }
        }

        // if the curve has <varcase>...</varcase>
        if (productCurve.hasVarcase()) {
            for (ProductVarcase varcase : productCurve.getVarcaseList()) {
                addVarcaseToCurve(curveNode, varcase, false);
            }
        } else {
            if (productCurve.getX() != null) {
                CommonTreeNode xNode = new CommonTreeNode(productCurve.getX(), CommonTreeNode.X_NODE);
                application.productTree.addNode(curveNode, xNode, true);
            }
            if (productCurve.getY() != null) {
                CommonTreeNode yNode = new CommonTreeNode(productCurve.getY(), CommonTreeNode.Y_NODE);
                application.productTree.addNode(curveNode, yNode, true);
            }
            if (productCurve.getZ() != null) {
                CommonTreeNode zNode = new CommonTreeNode(productCurve.getZ(), CommonTreeNode.Z_NODE);
                application.productTree.addNode(curveNode, zNode, true);
            }
        }

        return curveNode;
    }

    /**
     * Adds a new Curve to the specified Plot.
     *
     * @param plotNode    The plot node.
     * @param curve       The curve object. If <code>null</code>, a new {@link ProductCurve}
     *                    is created for the curve node, otherwise, the curve node
     *                    is created based on the specified curve object.
     * @param isNew       <code>true</code>, if the user object of the plot node does not
     *                    have the curve user object in its curve list, <code>false</code> otherwise.
     * @return curve node.
     */
    public CommonTreeNode addCurveToPlot(CommonTreeNode plotNode, Object curve, boolean isNew) {
        return addCurveToPlot(plotNode, curve, -1, isNew);
    }

    /**
     * Adds a new Plot to the specified Page.
     *
     * @param pageNode    The page node.
     * @param plot        The plot object which can be either <code>null</code> or
     *                     an instance of {@link ProductPlot}.
     * @param isNew        <code>true</code> if the plot user object needs adding to the
     *                  plot list of the page user object, <code>false</code> otherwise.
     * @return top plot node
     *
     */
    public CommonTreeNode addPlotToPage(CommonTreeNode pageNode, Object plot, boolean isNew) {
        CommonTreeNode plotNode = null;
        if (plot == null) {
            plotNode = new CommonTreeNode(new ProductPlot(), CommonTreeNode.PLOT_NODE);
        } else {
            plotNode = new CommonTreeNode(plot, CommonTreeNode.PLOT_NODE);
        }
        application.productTree.addNode(pageNode, plotNode, true);
        if (isNew) {
            ((ProductPage)pageNode.getUserObject()).addPlot((ProductPlot)plotNode.getUserObject());
        }
        ProductPlot productPlot = (ProductPlot)plotNode.getUserObject();
        List<ProductCurve> curves = productPlot.getCurveList();
        if (curves != null && curves.size() > 0) {
            for (ProductCurve eachCurve : curves) {
                addCurveToPlot(plotNode, eachCurve, false);
            }
        }
        return plotNode;
    }

    /**
     * Helper method for adding a new Page.
     * @param page the page node
     * @return top page node
     */
    @SuppressWarnings("rawtypes")
    public CommonTreeNode addPageToPlots(ProductPage page) {
        CommonTreeNode pageNode = null;
        if (page == null) {
            pageNode = new CommonTreeNode(new ProductPage(), CommonTreeNode.PAGE_NODE);
        } else {
            pageNode = new CommonTreeNode(page, CommonTreeNode.PAGE_NODE);

            // plot list
            List plotList = page.getPlotList();
            if (plotList != null) {
                for (int j = 0; j < plotList.size(); j++) {
                    ProductPlot plot = (ProductPlot) plotList.get(j);
                    addPlotToPage(pageNode, plot, false);

                    // TODO: axis, yaxis, zaxis
                    /***
                     * DANNY addPlotToPage will add the curves List curveList =
                     * plot.getCurveList(); if (curveList != null) { for (int k
                     * = 0; k < curveList.size(); k++) { ProductCurve curve =
                     * (ProductCurve)curveList.get(k); CommonTreeNode curveNode
                     * = addCurveToPlot (plotNode, curve, false); } }
                     ***/
                }
            }
        }

        application.productTree.addNode(application.plotsNode, pageNode, true);
        return pageNode;
    }
    /**
     * Helper method for adding a new var to a table column.
     */
    private void addVarToColumn(CommonTreeNode columnNode, Object var, boolean isNew) {
        CommonTreeNode varNode = new CommonTreeNode(var, CommonTreeNode.VAR_NODE);
        application.productTree.addNode(columnNode, varNode, true);
        if (isNew) {
            ((ProductColumn)columnNode.getUserObject()).setVar((ProductVar)var);
        }
    }

    /**
     * Adds a new column to the specified table.
     *
     * @param tableNode The table node.
     * @param column    The column object which can be either <code>null</code> or
     *                  an instance of {@link ProductColumn}.
     * @param idx       The index location for this new object. If out of range, adding as the last one.
     *                  The valid range would be [0, total child count of this table].
     * @param isNew     <code>true</code> if the column user object needs adding to the
     *                  column list of the table user object, <code>false</code> otherwise.
     * @return top column node
     */
    public CommonTreeNode addColumnToTable(CommonTreeNode tableNode, Object column, int idx, boolean isNew) {
        CommonTreeNode columnNode = null;
        if (column == null) {
            columnNode = new CommonTreeNode(new ProductColumn(), CommonTreeNode.COLUMN_NODE);
        } else {
            columnNode = new CommonTreeNode(column, CommonTreeNode.COLUMN_NODE);
        }
        application.productTree.addNode(tableNode, columnNode, idx, true);
        if (isNew) {
            if (idx == -1) {
                ((ProductTable)tableNode.getUserObject()).addColumn((ProductColumn)columnNode.getUserObject());
            } else {
                ((ProductTable)tableNode.getUserObject()).addColumn((ProductColumn)columnNode.getUserObject(), idx);
            }
        }

        ProductColumn productColumn = (ProductColumn)columnNode.getUserObject();
        ProductVar var = productColumn.getVar();
        if (var != null) {
            addVarToColumn(columnNode, var, false);
        }
        return columnNode;
    }

    /**
     * Adds a new column to the specified table.
     *
     * @param tableNode The table node.
     * @param column    The column object which can be either <code>null</code> or
     *                  an instance of {@link ProductColumn}.
     * @param isNew     <code>true</code> if the column user object needs adding to the
     *                  column list of the table user object, <code>false</code> otherwise.
     * @return top column node
     */
    public CommonTreeNode addColumnToTable(CommonTreeNode tableNode, Object column, boolean isNew) {
        return addColumnToTable(tableNode, column, -1, isNew);
    }

    /**
     * Helper method for adding a new table.
     */
    private CommonTreeNode addNewTable(Object table) {
        CommonTreeNode tableNode = null;
        if (table == null) {
            tableNode = new CommonTreeNode(new ProductTable(), CommonTreeNode.TABLE_NODE);
        } else {
            tableNode = new CommonTreeNode(table, CommonTreeNode.TABLE_NODE);
        }
        application.productTree.addNode(application.tablesNode, tableNode, true);
        return tableNode;
    }


    /**
     * Helper method for adding a new program.
     */
    private CommonTreeNode addNewProgram(Object program) {
        CommonTreeNode programNode = null;
        if (program == null) {
            programNode = new CommonTreeNode(new ProductExternalFunction(), CommonTreeNode.PROGRAM_NODE);
        } else {
            programNode = new CommonTreeNode(program, CommonTreeNode.PROGRAM_NODE);
        }
        application.productTree.addNode(application.programsNode, programNode, true);
        application.productTree.addNode(programNode, new CommonTreeNode("Input", CommonTreeNode.INPUT_NODE), true);
        application.productTree.addNode(programNode, new CommonTreeNode("Output", CommonTreeNode.OUTPUT_NODE), true);
        return programNode;
    }

    /**
     * Helper method for adding a var for program input
     */
    private void addVarToInput(ProductVar var, boolean isNew) {
        // As only one program is supported, input node is the 1st child of 1st Programs child
        CommonTreeNode inputNode = (CommonTreeNode)(((CommonTreeNode)application.programsNode.getChildAt(0)).getChildAt(0));
        CommonTreeNode varNode = new CommonTreeNode(var, CommonTreeNode.INPUT_VAR_NODE);
        application.productTree.addNode(inputNode, varNode, true);
        if (isNew) {
            ProductExternalFunction program = (ProductExternalFunction)((CommonTreeNode)application.programsNode.getChildAt(0)).getUserObject();
            program.addInput(var);
        }
    }

    /**
     * Helper method for adding a var for program output
     */
    private void addVarToOutput(ProductMeasurement var, boolean isNew) {
        // As only one program is supported, output node is the 2st child of 1st Programs child
        CommonTreeNode outputNode = (CommonTreeNode)(((CommonTreeNode)application.programsNode.getChildAt(0)).getChildAt(1));
        CommonTreeNode varNode = new CommonTreeNode(var, CommonTreeNode.OUTPUT_VAR_NODE);
        application.productTree.addNode(outputNode, varNode, true);
        if (isNew) {
            ProductExternalFunction program = (ProductExternalFunction)((CommonTreeNode)application.programsNode.getChildAt(0)).getUserObject();
            program.addOutput(var);
        }

        // also need to add this newly generated output var to var list
        LogVar newOutVar = new LogVar(var.getName());
        newOutVar.setUnits("--");
        application.varList.addData(newOutVar);
    }

    /**
     * Helper method for adding a new node.
     */
    private CommonTreeNode addNewNode(int parentType, int newNodeType, Object newObj, String warningTitle) {
        ArrayList<DefaultMutableTreeNode> selectedTreeNodes = application.productTree.getSelectedNodes();
        boolean properParentSelected = false;
        // the new node
        CommonTreeNode newNode = null;

        if (selectedTreeNodes != null && selectedTreeNodes.size() > 0) {
            for (int i = 0; i < selectedTreeNodes.size(); i++) {
                if (selectedTreeNodes.get(i) instanceof CommonTreeNode) {
                    CommonTreeNode parentNode = (CommonTreeNode)selectedTreeNodes.get(i);
                    // configure the user object of the parent node if applicable
                    if (parentNode.getNodeType() == parentType) {
                        switch (parentType) {
                            case CommonTreeNode.PAGE_NODE:
                                ((ProductPage)parentNode.getUserObject()).addPlot((ProductPlot)newObj);
                                break;
                            case CommonTreeNode.PLOT_NODE:
                                ((ProductPlot)parentNode.getUserObject()).addCurve((ProductCurve)newObj);
                                break;
                            case CommonTreeNode.CURVE_NODE:
                                ProductCurve thisCurve = (ProductCurve)parentNode.getUserObject();
                                // If a curve already had variable node, no Varcase can be added.
                                if (thisCurve.getX() != null) {
                                    JOptionPane.showMessageDialog(application.getMainFrame(),
                                                                  "You can only add Varcase to a Curve that is either empty or only has Varcase node(s)!",
                                                                  "Ineligible Curve",
                                                                  JOptionPane.WARNING_MESSAGE);
                                    return null;

                                } else {
                                    ((ProductCurve)parentNode.getUserObject()).addVarcase((ProductVarcase)newObj);
                                }
                                 break;
                            case CommonTreeNode.TABLE_NODE:
                                ((ProductTable)parentNode.getUserObject()).addColumn((ProductColumn)newObj);
                                break;
                        }
                        // create the new node
                        newNode = new CommonTreeNode(newObj, newNodeType);
                        // add the new node to its parent
                        application.productTree.addNode(parentNode, newNode, true);
                        properParentSelected = true;
                    }
                }
            }
        }
        if (!properParentSelected) {
            JOptionPane.showMessageDialog(application.getMainFrame(),
                                          "A proper parent node must be selected to insert this new node!",
                                          warningTitle,
                                          JOptionPane.WARNING_MESSAGE);
        }
        return newNode;
    }


    //========================================
    //    Inner classes
    //========================================

}
