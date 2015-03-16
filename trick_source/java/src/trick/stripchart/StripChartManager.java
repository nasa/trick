/*
 * $Id: StripChartManager.java 3489 2014-04-14 19:42:09Z hchen3 $
 */

package trick.stripchart;

/** Deprecated imports to be removed in the next release. ********************/
import java.awt.Font;
import java.awt.Paint;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;

import org.jfree.chart.JFreeChart;
import org.jfree.chart.axis.ValueAxis;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.title.TextTitle;
import org.jfree.data.Range;
import org.jfree.data.xy.XYSeries;

import trick.common.utils.vs.Variable;
/*****************************************************************************/

/**
 * manages any number of strip charts and their accompanying data
 *
 * @author Derek Bankieris
 */
public class StripChartManager {

    /** variables whose historical values are being logged */
    HashMap<Variable, ValueLog> valueLogs = new HashMap<Variable, ValueLog>();

    /** variables being logged with respect to another variable */
    HashMap<PairKey, VariablePair> variablePairs = new HashMap<PairKey, VariablePair>();

    /** the strip charts being managed */
    ArrayList<StripChart> stripCharts = new ArrayList<StripChart>();

    /** the number of times data has been logged */
    int count = 0;

    /**
     * begins logging of historical values of the <code>variables</code> and makes them
     * available for use in the strip charts. Only variables that can be parsed as doubles
     * can be added.
     *
     * @param variables the variables to be recorded
     */
    public void addVariables(Collection<Variable> variables) {
        for (Variable variable : variables) {
            addVariable(variable);
        }
    }

    /**
     * begins logging of historical values of the <code>variable</code> and makes it
     * available for use in the strip charts. Only variables that can be parsed as doubles
     * can be added.
     *
     * @param variable the variable to be recorded
     */
    public void addVariable(Variable variable) {
        if (!valueLogs.containsKey(variable)) {
            try {
                Double.parseDouble(variable.getValue().toVariableServer());
            }
            catch (NumberFormatException numberFormatException) {
                throw new IllegalArgumentException(variable.name + " cannot be parsed as a double.\n" +
                  numberFormatException, numberFormatException);
            }

            valueLogs.put(variable, new ValueLog(variable, count));
            for (StripChart stripChart : stripCharts) {
                stripChart.addVariable(variable);
            }
        }
    }

    /**
     * clears the record of all variables being logged with respect to another variable
     */
    public void clearData() {
        for (ValueLog valueLog : valueLogs.values()) {
            valueLog.clear(count);
        }
        for (VariablePair variablePair : variablePairs.values()) {
            variablePair.clear();
        }
    }

    /**
     * clears all value logs and variable pairs and removes all varibles from the strip charts
     */
    public void removeAllVariables() {
        valueLogs.clear();
        variablePairs.clear();
        for (StripChart stripChart : stripCharts) {
            stripChart.deleteAllVariables();
        }
    }

    /**
     * clears the <code>variable</code>'s value log and any pairs it is
     * participating in and removes it from the strip charts
     */
    public void removeVariable(Variable variable) {
        valueLogs.remove(variable);

        // Copy the key set so we don't modify the iterator, producing a ConcurrentModificationException.
        for (PairKey pairKey : new ArrayList<PairKey>(variablePairs.keySet())) {
            if (pairKey.domainVariable == variable || pairKey.rangeVariable == variable) {
                variablePairs.remove(pairKey);
            }
        }
        for (StripChart stripChart : stripCharts) {
            stripChart.deleteVariable(variable);
        }
    }

    /**
     * returns an existing variable pair or a new one if none exists
     *
     * @return a variable pair for the given arguments
     */
    public VariablePair getPair(Variable domainVariable, Variable rangeVariable) {
        PairKey pairKey = new PairKey(domainVariable, rangeVariable);
        if (variablePairs.containsKey(pairKey)) {
            return variablePairs.get(pairKey);
        }

        VariablePair variablePair = new VariablePair(valueLogs.get(domainVariable), valueLogs.get(rangeVariable));
        variablePairs.put(pairKey, variablePair);

        return variablePair;
    }

    /**
     * updates all value logs and variable pairs with the latest variable values
     */
    public void update() {
        for (ValueLog valueLog : valueLogs.values()) {
            valueLog.update();
        }

        for (VariablePair variablePair : variablePairs.values()) {
            variablePair.update();
        }

        ++count;
    }

    /**
     * creates a new strip chart
     *
     * @param domainVariable the independent variable
     * @param rangeVariables the dependent variables
     *
     * @return a new strip chart
     */
    public StripChart createStripChart(Variable domainVariable, Collection<Variable> rangeVariables) {
        return createStripChart(domainVariable, rangeVariables, StripChart.Mode.Strip, 30, true, false, true);
    }

    /**
     * creates a new strip chart
     *
     * @param domainVariable the independent variable
     * @param rangeVariables the dependent variables
     * @param mode the initial mode
     * @param fixedAutoRange the width when in fixed-range mode
     * @param linesVisible the visibility of the lines
     * @param pointsVisible the visibility of the points
     * @param legendVisible the visibility of the legend
     *
     * @return a new strip chart
     */
    public StripChart createStripChart(Variable domainVariable, Collection<Variable> rangeVariables,
      StripChart.Mode mode, double fixedAutoRange, boolean linesVisible, boolean pointsVisible,
      boolean legendVisible) {

        // Ensure all specified variables have already been added.
        addVariable(domainVariable);
        for (Variable variable : rangeVariables) {
            addVariable(variable);
        }

        StripChart stripChart = new StripChart(this, domainVariable, rangeVariables, valueLogs.keySet(),
          mode, fixedAutoRange, linesVisible, pointsVisible, legendVisible);
        stripCharts.add(stripChart);
        return stripChart;
    }

    /**
     * returns the strip charts being mainaged
     *
     * @return the strip charts being managed. The array list is
     * cloned, and thus safe to modify
     */
    @SuppressWarnings("unchecked")
    public ArrayList<StripChart> getStripCharts() {
        return (ArrayList<StripChart>)stripCharts.clone();
    }

    /**
     * return the number of strip charts being managed
     *
     * @return the strip chart count
     */
    public int getStripChartCount() {
        return stripCharts.size();
    }

    /**
     * removes the strip chart from management
     *
     * @param stripChart the strip chart to release
     */
    public void releaseStripChart(StripChart stripChart) {
        stripCharts.remove(stripChart);
    }

    /**
     * saves the state of the strip charts
     *
     * @param objectOutputStream the save destination
     */
    @Deprecated
    public void saveStripCharts(ObjectOutputStream objectOutputStream) throws IOException {
        objectOutputStream.writeObject(new ArrayList<Variable>(valueLogs.keySet()));
        objectOutputStream.writeObject(new Integer(stripCharts.size()));
        for (StripChart stripChart : stripCharts) {
            objectOutputStream.writeObject(stripChart.getDomainVariable());
            objectOutputStream.writeObject(stripChart.getPlottedVariables());

            objectOutputStream.writeObject(stripChart.getMode());
            objectOutputStream.writeObject(new Double(stripChart.getFixedAutoRange()));
            objectOutputStream.writeObject(new Boolean(stripChart.areLinesVisible()));
            objectOutputStream.writeObject(new Boolean(stripChart.arePointsVisible()));
            objectOutputStream.writeObject(new Boolean(stripChart.isLegendVisible()));

            objectOutputStream.writeObject(stripChart.areSettingsVisible());

            JFreeChart chart = stripChart.getChart();
            objectOutputStream.writeObject(new Boolean(chart.getAntiAlias()));
            objectOutputStream.writeObject(chart.getBackgroundPaint());

            TextTitle textTitle = stripChart.getTitle();
            if (textTitle != null) {
                objectOutputStream.writeObject(Boolean.TRUE);
                objectOutputStream.writeObject(textTitle.getText());
                objectOutputStream.writeObject(textTitle.getFont());
                objectOutputStream.writeObject(textTitle.getPaint());
            }
            else {
                objectOutputStream.writeObject(Boolean.FALSE);
            }

            XYPlot plot = chart.getXYPlot();
            objectOutputStream.writeObject(plot.getOrientation());
            objectOutputStream.writeObject(plot.getOutlinePaint());
            objectOutputStream.writeObject(plot.getBackgroundPaint());

            for (ValueAxis axis : new ValueAxis[] {plot.getDomainAxis(), plot.getRangeAxis()}) {
                objectOutputStream.writeObject(axis.getLabel());
                objectOutputStream.writeObject(axis.getLabelFont());
                objectOutputStream.writeObject(axis.getLabelPaint());
                objectOutputStream.writeObject(axis.isTickMarksVisible());
                objectOutputStream.writeObject(axis.isTickLabelsVisible());
                objectOutputStream.writeObject(axis.getTickLabelFont());
                objectOutputStream.writeObject(axis.getTickLabelPaint());
            }

            if (stripChart.getMode() == StripChart.Mode.Fixed) {
                objectOutputStream.writeObject(stripChart.getDomainBounds());
                objectOutputStream.writeObject(stripChart.getRangeBounds());
            }
        }
    }

    /**
     * restores a set of strip charts
     *
     * @param objectInputStream the source of the strip charts
     *
     * @return the restored strip charts
     */
    @SuppressWarnings("unchecked")
    @Deprecated
    public ArrayList<StripChart> loadStripCharts(ObjectInputStream objectInputStream)
      throws IOException, ClassNotFoundException {
        for (Variable variable : (ArrayList<Variable>)objectInputStream.readObject()) {
            addVariable(variable);
        }

        ArrayList<StripChart> loadedStripCharts = new ArrayList<StripChart>();
        int count = ((Integer)objectInputStream.readObject()).intValue();
        while (count-- > 0) {
            StripChart stripChart = createStripChart(
              (Variable)objectInputStream.readObject(),
              (ArrayList<Variable>)objectInputStream.readObject(),
              (StripChart.Mode)objectInputStream.readObject(),
              ((Double)objectInputStream.readObject()).doubleValue(),
              ((Boolean)objectInputStream.readObject()).booleanValue(),
              ((Boolean)objectInputStream.readObject()).booleanValue(),
              ((Boolean)objectInputStream.readObject()).booleanValue());

            stripChart.setSettingsVisible(((Boolean)(objectInputStream.readObject())).booleanValue());

            JFreeChart chart = stripChart.getChart();
            chart.setAntiAlias(((Boolean)objectInputStream.readObject()).booleanValue());
            chart.setBackgroundPaint((Paint)objectInputStream.readObject());

            if (((Boolean)objectInputStream.readObject()).booleanValue()) {
                TextTitle textTitle = stripChart.getTitle();
                textTitle.setText((String)objectInputStream.readObject());
                textTitle.setFont((Font)objectInputStream.readObject());
                textTitle.setPaint((Paint)objectInputStream.readObject());
            }
            else {
                stripChart.getChart().setTitle((TextTitle)null);
            }

            XYPlot plot = stripChart.getChart().getXYPlot();
            plot.setOrientation((PlotOrientation)objectInputStream.readObject());
            plot.setOutlinePaint((Paint)objectInputStream.readObject());
            plot.setBackgroundPaint((Paint)objectInputStream.readObject());

            for (ValueAxis axis : new ValueAxis[] {plot.getDomainAxis(), plot.getRangeAxis()}) {
                axis.setLabel((String)objectInputStream.readObject());
                axis.setLabelFont((Font)objectInputStream.readObject());
                axis.setLabelPaint((Paint)objectInputStream.readObject());
                axis.setTickMarksVisible(((Boolean)objectInputStream.readObject()).booleanValue());
                axis.setTickLabelsVisible(((Boolean)objectInputStream.readObject()).booleanValue());
                axis.setTickLabelFont((Font)objectInputStream.readObject());
                axis.setTickLabelPaint((Paint)objectInputStream.readObject());
            }

            if (stripChart.getMode() == StripChart.Mode.Fixed) {
                stripChart.setDomainBounds((Range)objectInputStream.readObject());
                stripChart.setRangeBounds((Range)objectInputStream.readObject());
            }

            loadedStripCharts.add(stripChart);
        }

        return loadedStripCharts;
    }

    /**
     * a log of a variable's values
     */
    static class ValueLog {

        /** the variable being logged */
        Variable variable;

        /** the log of values */
        ArrayList<Double> values = new ArrayList<Double>();

        /** the manager count at which this log started */
        int logStart;

        /**
         * constructor
         *
         * @param variable the variable to be logged
         * @param logStart the manager count at which this log started
         */
        public ValueLog(Variable variable, int logStart) {
            this.variable = variable;
            this.logStart = logStart;
        }

        /**
         * updates the log by adding the most recent value
         */
        public void update() {
            values.add(new Double(variable.getValue().toVariableServer()));
        }

        /**
         * gets the manager count at which this log started
         *
         * @return the manager count at which this log started
         */
        public int getLogStart() {
            return logStart;
        }

        /**
         * returns the variable being logged
         *
         * @return the variable being logged
         */
        public Variable getVariable() {
            return variable;
        }

        /**
         * returns the <code>i</code>th value in the log
         *
         * @param i the index
         *
         * @return the <code>i</code>th value in the log
         */
        public Double get(int i) {
            return values.get(i);
        }

        /**
         * returns the last value in the log
         *
         * @return the latest value in the log
         */
        public Double getLast() {
            return values.get(values.size() - 1);
        }

        /**
         * returns the size the log
         *
         * @return the length of the log
         */
        public int getSize() {
            return values.size();
        }

        /**
         * clears all data
         */
        public void clear(int logStart) {
            values.clear();
            this.logStart = logStart;
        }

    }

    /**
     * represents a pair of variables to be used as a hashing key
     */
    static class PairKey {

        /** the independent variable */
        Variable domainVariable;

        /** the dependent variable */
        Variable rangeVariable;

        /**
         * constructor
         *
         * @param domainVariable the independent variable
         * @param rangeVariable the depdendent variable
         */
        public PairKey(Variable domainVariable, Variable rangeVariable) {
            this.domainVariable = domainVariable;
            this.rangeVariable = rangeVariable;
        }

        @Override
        public boolean equals(Object object) {
            return object != null && object instanceof PairKey &&
              rangeVariable.equals(((PairKey)object).rangeVariable) &&
              domainVariable.equals(((PairKey)object).domainVariable);
        }

        @Override
        public int hashCode() {
            return domainVariable.hashCode() + rangeVariable.hashCode();
        }

    }

    /**
     *  represents a domain-range pair of <code>ValueLog</code>s
     *  and a corresponding <code>XYSeries</code>
     */
    static class VariablePair {

        /** the independent variable's value log */
        ValueLog domainValueLog;

        /** the dependent variable's value log */
        ValueLog rangeValueLog;

        /** the corresponding series */
        XYSeries xySeries;

        /**
         * constructor
         *
         * @param domainValueLog the independent variable's value log
         * @param rangeValueLog the dependent variable's value log
         */
        public VariablePair(ValueLog domainValueLog, ValueLog rangeValueLog) {
            this.domainValueLog = domainValueLog;
            this.rangeValueLog = rangeValueLog;
            xySeries = new XYSeries(rangeValueLog.getVariable().toString(), false, true);

            if (domainValueLog.getLogStart() < rangeValueLog.getLogStart()) {
                for (int i = 0; i < rangeValueLog.getSize(); ++i) {
                    xySeries.add(
                      domainValueLog.get(i + rangeValueLog.getLogStart() - domainValueLog.getLogStart()),
                      rangeValueLog.get(i));
                }
            }
            else {
                for (int i = 0; i < domainValueLog.getSize(); ++i) {
                    xySeries.add(
                      domainValueLog.get(i),
                      rangeValueLog.get(i + domainValueLog.getLogStart() - rangeValueLog.getLogStart()));
                }
            }
        }

        /**
         * updates this variable pair by adding the latest values
         */
        public void update() {
            xySeries.add(domainValueLog.getLast(), rangeValueLog.getLast());
        }

        /**
         * clears the <code>XYSeries</code>
         */
        public void clear() {
            xySeries.clear();
        }

        /**
         * returns the xy series
         *
         * @return the xy series
         */
        public XYSeries getSeries() {
            return xySeries;
        }

    }

}
