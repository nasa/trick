
package trick.dataproducts.plot.utils;

import java.awt.Color;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.ArrayList;

import javax.swing.text.BadLocationException;
import javax.swing.text.Style;
import javax.swing.text.StyleConstants;
import javax.swing.text.StyleContext;
import javax.swing.text.StyledDocument;

import org.jfree.data.xy.XYSeries;

import trick.common.ui.TrickFileFilter;
import trick.common.ui.UIUtils;
import trick.common.utils.BinaryDataReader;
import trick.common.utils.CSVDataReader;
import trick.common.utils.DataReader;
import trick.dataproducts.trickqp.utils.ProductColumn;
import trick.dataproducts.trickqp.utils.ProductPage;
import trick.dataproducts.trickqp.utils.ProductPlot;
import trick.dataproducts.trickqp.utils.ProductTable;
import trick.dataproducts.trickqp.utils.ProductVar;
import trick.dataproducts.utils.SessionRun;

/**
 * This data reader is for reading recorded data from different type of files.
 *
 * @author Hong Chen
 * @since Trick 13
 */
public class PlotUtils {

    //========================================
    //    Public data
    //========================================



    //========================================
    //    Protected data
    //========================================



    //========================================
    //    Private Data
    //========================================



    //========================================
    //   Methods
    //========================================
    /**
     * Writes the logged data based on the {@link ProductTable} spec to the {@link StyledDocument}
     * for a specified {@link SessionRun}. The data is then displayed in the form of a table.
     * This is much faster than using Java table component for a large set of data.
     *
     * @param run        The RUN_ directory that contains the logged data.
     * @param theTable    The table spec.
     * @param doc        The document that holds the logged data in order
     *                  for the data to be shown in the form of a table.
     */
    public static void insertTableData(SessionRun run, ProductTable theTable, StyledDocument doc) {
        ArrayList<DataReader> varDataReaderList = null;
        File runDir = new File(run.getDir());
        if (runDir != null && runDir.exists()) {
            File[] files = UIUtils.getListFiles(runDir, TrickFileFilter.TRICK_DATA_RECORD);
            StringBuffer eachRow = new StringBuffer();

            // do nothing if no data recording files found
            if (files == null) {
                return;
            }

            // style for header
            StyleContext context = new StyleContext();
            Style headingStyle = context.getStyle(StyleContext.DEFAULT_STYLE);
            StyleConstants.setAlignment(headingStyle, StyleConstants.ALIGN_RIGHT);
            StyleConstants.setBold(headingStyle, true);
            StyleConstants.setFontSize(headingStyle, 14);
            StyleConstants.setSpaceAbove(headingStyle, 4);
            StyleConstants.setSpaceBelow(headingStyle, 4);

            try {
                doc.insertString(doc.getLength(), "Title: " + theTable.getTitle() + "\n", headingStyle);
                doc.insertString(doc.getLength(), "Run: " + run.getDir() + "\n\n", headingStyle);
            } catch (BadLocationException ble) {
                ble.printStackTrace();
            }
            eachRow.setLength(0);

            varDataReaderList = new ArrayList<DataReader>();

            // build up the data reader list for reading the data and write the variable names on table
            // variable names row
            for (ProductColumn theColumn : theTable.getColumnList()) {
                DataReader eachReader = getVarDataReader(runDir, theColumn.getVar());
                eachRow.append(theColumn.getVar().getShortName() + " {" + theColumn.getVar().getUnits() + "}" + "\t");

                if (eachReader == null || eachReader.locateVarIndex(theColumn.getVar().getName()) == -1) {
                    continue;
                }
                try {
                    eachReader.beginRead(theColumn.getVar());
                } catch (IOException e) {
                    e.printStackTrace();
                }
                theColumn.dataReader = eachReader;
                varDataReaderList.add(eachReader);
            }
            eachRow.append("\n");

            if (varDataReaderList.size() < 1) {
                return;
            }

            try {
                StyleConstants.setBackground(headingStyle, Color.lightGray);
                doc.insertString(doc.getLength(), eachRow.toString(), headingStyle);
                eachRow.setLength(0);
            } catch (BadLocationException ble) {
                ble.printStackTrace();
            }

            double eachVarValue = Double.NaN;

            // style for data
            context = new StyleContext();
            Style dataStyle = context.getStyle(StyleContext.DEFAULT_STYLE);
            StyleConstants.setAlignment(dataStyle, StyleConstants.ALIGN_RIGHT);
            StyleConstants.setBold(dataStyle, false);
            StyleConstants.setFontSize(dataStyle, 12);
            StyleConstants.setForeground(dataStyle, Color.BLUE);

            int lineIndex = 0;

            while (true) {
                // get one value for each variable at a time
                for (ProductColumn theColumn : theTable.getColumnList()) {
                    try {
                        eachVarValue = theColumn.dataReader.getVarValue();

                        if (Double.isNaN(eachVarValue)) {
                            break;
                        }

                        String fmt = theColumn.getFormat();
                        if (fmt != null && fmt != "") {
                            eachRow.append( String.format(fmt, eachVarValue) + "\t");
                        } else {
                            eachRow.append(eachVarValue + "\t");
                        }

                    } catch (FileNotFoundException e) {
                        System.err.println(e);
                    } catch (IOException e) {
                        System.err.println(e);
                    }
                }

                if (Double.isNaN(eachVarValue)) {
                    break;
                }
                eachRow.append("\n");
                try {
                    if ((lineIndex & 1) == 0) {
                        StyleConstants.setBackground(dataStyle, Color.white);
                    } else {
                        StyleConstants.setBackground(dataStyle, Color.lightGray);
                    }
                    doc.insertString(doc.getLength(), eachRow.toString(), dataStyle);
                    eachRow.setLength(0);
                    lineIndex++;
                } catch (BadLocationException ble) {
                    ble.printStackTrace();
                }
            }
            // end data reader
            for (DataReader eachReader : varDataReaderList) {
                eachReader.endRead();
            }
            for (ProductColumn theColumn : theTable.getColumnList()) {
                theColumn.dataReader = null;
            }
        }
    }

    /**
     * Builds a {@link XYSeries} for x and y of the specified plot spec from the recorded data that is in a particular run directory.
     *
     * @param run   A {@link SessionRun} instance.
     * @param page  A {@link ProductPage} instance.
     * @param plot  A {@link ProductPlot} instance.
     * @param xVar  The X variable.
     * @param yVar  The Y variable.
     * @return      An instance of {@link XYSeries}.
     */
    public static TrickXYSeries getXYVarSeries(
                                               SessionRun run,
                                               ProductPage page,
                                               ProductPlot plot,
                                               ProductVar xVar,
                                               ProductVar yVar
                                               ) {
        TrickXYSeries series = null;
        DataReader dataReaderX = null;
        DataReader dataReaderY = null;

        File runDir = new File(run.getDir());
        if (runDir != null && runDir.exists()) {
            File[] files = UIUtils.getListFiles(runDir, TrickFileFilter.TRICK_DATA_RECORD);

            // do nothing if no data recording files found
            if (files == null) {
                return series;
            }

            dataReaderX = getVarDataReader(runDir, xVar);
            if (dataReaderX == null || dataReaderX.locateVarIndex(xVar.getName()) == -1) {
                return series;
            }
            dataReaderX.setTimename(run.getTimename());
            configureDataReader(page, plot, dataReaderX);

            dataReaderY = getVarDataReader(runDir, yVar);
            if (dataReaderY == null || dataReaderY.locateVarIndex(yVar.getName()) == -1) {
                return series;
            }
            dataReaderY.setTimename(run.getTimename());
            configureDataReader(page, plot, dataReaderY);

            try {
                String description = getSeriesDescription(runDir.getName(), xVar, yVar);
                series = new TrickXYSeries(description, false, true);
                series.setDescription(description);

                dataReaderX.beginRead(xVar);
                dataReaderY.beginRead(yVar);

                double xValue = dataReaderX.getVarValue();
                double yValue = dataReaderY.getVarValue();

                while (!dataReaderX.isEnd() && Double.isNaN(xValue)) {
                    xValue = dataReaderX.getVarValue();
                }

                while (!dataReaderY.isEnd() && Double.isNaN(yValue)) {
                    yValue = dataReaderY.getVarValue();
                }

                // If either data reader doesn't have time data recorded, then time stamp is
                // not a concern. Simply just both x & y as they are.
                // If xVar reader's time stamp is bigger, reads more yVar till yVar reader's
                // time stamp almost equals to the xVar reader's time stamp and vice versa.
                // The xVar and yVar values are added to the series only if their time stamp
                // matches within 1e-9. TODO: Is 1e-9 good enough?
                while (!Double.isNaN(xValue) && !Double.isNaN(yValue)) {
                    if (DataReader.nearlyEqual(dataReaderX.getTimeValue(), dataReaderY.getTimeValue()) ||
                                                              Double.isNaN(dataReaderX.getTimeValue()) ||
                                                              Double.isNaN(dataReaderY.getTimeValue())) {
                        series.add(xValue, yValue);
                        xValue = dataReaderX.getVarValue();
                        yValue = dataReaderY.getVarValue();
                    } else if (dataReaderX.getTimeValue() > dataReaderY.getTimeValue()) {
                        yValue = dataReaderY.getVarValue();
                    } else {
                        xValue = dataReaderX.getVarValue();
                    }
                }
            } catch (FileNotFoundException e) {
                System.err.println(e);
            } catch (IOException e) {
                System.err.println(e);
            }

            dataReaderX.endRead();
            dataReaderY.endRead();

            if (series != null) {
                series.setXVar(xVar);
                series.setYVar(yVar);
            }
        }

        return series;
    }

    /**
     * Configures the reader by applying {@link ProductPage} and {@link ProductPlot} specs.
     */
    private static void configureDataReader(ProductPage page, ProductPlot plot, DataReader theReader) {
        if (page.getTStart() != null && plot.getTStart() != null) {
            if (page.getTStart().doubleValue() > plot.getTStart().doubleValue()) {
                theReader.setTStart(page.getTStart());
            } else {
                theReader.setTStart(plot.getTStart());
            }
        } else {
            if (page.getTStart() != null && plot.getTStart() == null) {
                theReader.setTStart(page.getTStart());
            } else if (page.getTStart() == null && plot.getTStart() != null) {
                theReader.setTStart(plot.getTStart());
            } else {
                theReader.setTStart(page.getTStart());
            }
        }

        if (page.getTStop() != null && plot.getTStop() != null) {
            if (page.getTStop().doubleValue() < plot.getTStop().doubleValue()) {
                theReader.setTStop(page.getTStop());
            } else {
                theReader.setTStop(plot.getTStop());
            }
        } else {
            if (page.getTStop() != null && plot.getTStop() == null) {
                theReader.setTStop(page.getTStop());
            } else if (page.getTStop() == null && plot.getTStop() != null) {
                theReader.setTStop(plot.getTStop());
            } else {
                theReader.setTStop(page.getTStop());
            }
        }
    }

    /**
     * Helper method for getting the corresponding reader for the specified variable.
     *
     * @param runDir The RUN_ dir that contains the recorded data.
     * @param var The {@link ProductVar} object which needs getting data for.
     *
     * @return    An instance of {@link DataReader}.
     */
    private static DataReader getVarDataReader(File runDir, ProductVar var) {
        DataReader theReader = null;
        File[] files = UIUtils.getListFiles(runDir, TrickFileFilter.TRICK_DATA_RECORD);
        for (int i = 0; i < files.length; i++) {
            if (files[i].getPath().endsWith(".csv")) {
                theReader = new CSVDataReader(files[i].getPath());
            } else if (files[i].getPath().endsWith(".trk")) {
                theReader = new BinaryDataReader(files[i].getPath());
            } else if (files[i].getPath().endsWith(".h5")) {
                // TODO: set up data reader
            }
            if (theReader == null) {
                continue;
            }

            if (theReader.locateVarIndex(var.getName()) != -1) {
                break;
            }
        }
        return theReader;
    }

    /**
     * Makes up the description for a series for the specified X & Y variables.
     */
    private static String getSeriesDescription(String runDir, ProductVar xVar, ProductVar yVar) {
        // use variable name & units for label for plotting by default
        String xVarLabel = xVar.getName() + " (" + xVar.getUnits() + ")";
        String yVarLabel = yVar.getName() + " (" + yVar.getUnits() + ")";

        // if label for the variable is specified, the specified label is used for plotting instead of its name&units
        if (xVar.getLabel() != null && !(xVar.getLabel().isEmpty())) {
            xVarLabel = xVar.getLabel();
        }

        if (yVar.getLabel() != null && !(yVar.getLabel().isEmpty())) {
            yVarLabel = yVar.getLabel();
        }
        return yVarLabel + " VS. " + xVarLabel + " [" + runDir + "]";
    }

}
