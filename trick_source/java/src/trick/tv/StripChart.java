package trick.tv;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.FocusAdapter;
import java.awt.event.FocusEvent;
import java.awt.event.KeyEvent;
import java.awt.geom.Rectangle2D;
import java.util.ArrayList;
import java.util.Collection;

import javax.swing.AbstractAction;
import javax.swing.ImageIcon;
import javax.swing.JCheckBox;
import javax.swing.JCheckBoxMenuItem;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JRadioButton;
import javax.swing.border.TitledBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import org.jdesktop.swingx.JXButton;
import org.jdesktop.swingx.JXFrame;
import org.jdesktop.swingx.JXPanel;
import org.jdesktop.swingx.JXRadioGroup;
import org.jdesktop.swingx.JXTextField;
import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.axis.ValueAxis;
import org.jfree.chart.event.AxisChangeEvent;
import org.jfree.chart.event.AxisChangeListener;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.renderer.xy.XYLineAndShapeRenderer;
import org.jfree.chart.title.LegendTitle;
import org.jfree.chart.title.TextTitle;
import org.jfree.data.Range;
import org.jfree.data.xy.XYSeriesCollection;

import trick.common.utils.vs.Variable;

/**
 * displays a single strip chart
 *
 * @author Derek Bankieris
 */
public class StripChart extends JXPanel {

    /** available display modes */
    public enum Mode {All, Strip, Fixed};

    /** the manager managing this strip chart */
    StripChartManager stripChartManager;

    /** the collection of YXSeries that this strip chart is plotting */
    XYSeriesCollection dataSet = new XYSeriesCollection();

    /** the main chart */
    JFreeChart chart;

    /** the settings panel */
    JXPanel settingsPanel;

    /** the display mode */
    Mode mode;

    /** when in auto-range mode, the width of the range */
    double fixedAutoRange;

    /** a combo box to hold variables available for adding to the strip chart */
    ComboBox<Variable<?>> addComboBox = new ComboBox<>();

    /** a combo box to hold variables available for removing from the strip chart */
    ComboBox<Variable<?>> removeComboBox = new ComboBox<>();

    /** a combo box to hold variables available for specifying as the independent variable */
    ComboBox<Variable<?>> domainComboBox = new ComboBox<>();

    /**
     * constructor
     *
     * @param stripChartManager the manager for this strip chart
     * @param domainVariable the independent variable
     * @param rangeVariables the dependent variables
     * @param allVariables all variables available for plotting
     * @param mode the initial mode
     * @param autoRange the width of the range when in auto-range mode
     * @param linesVisible the visibility of the lines
     * @param pointsVisible the visibility of the points
     * @param legendVisible the visibility of the legend
     */
    // Until JFreeChart fixes setBaseShape, we must use the deprecated setShape.
    @SuppressWarnings("deprecation")
    StripChart(StripChartManager stripChartManager, final Variable<?> domainVariable,
      Collection<? extends Variable<?>> rangeVariables, Collection<Variable<?>> allVariables,
      final Mode mode, double autoRange, final boolean linesVisible,
      final boolean pointsVisible, final boolean legendVisible) {

        this.stripChartManager = stripChartManager;
        fixedAutoRange = autoRange;

        setOpaque(false);
        setLayout(new BorderLayout());

        // Configure the chart.
        chart = ChartFactory.createScatterPlot("Strip Chart", "", "Value", dataSet,
          PlotOrientation.VERTICAL, true, true, false);
        chart.setBackgroundPaint(null);
        chart.setAntiAlias(false);

        // Configure the plot.
        final XYPlot plot = chart.getXYPlot();
        plot.setBackgroundPaint(Color.white);
        plot.setDomainGridlinePaint(Color.gray);
        plot.setRangeGridlinePaint(Color.gray);

        final XYLineAndShapeRenderer renderer = (XYLineAndShapeRenderer)plot.getRenderer();
        // TODO: Change to setBaseShape once JFreeChart fixes it.
        renderer.setShape(new Rectangle2D.Double(-2, -2, 1, 1));

        add(new ChartPanel(chart, true), BorderLayout.CENTER);

        settingsPanel = new JXPanel(new GridBagLayout()) {{

            final ValueAxis domainAxis = plot.getDomainAxis();
            domainAxis.setLabel(domainVariable.name);

            final JRadioButton allRadioButton = new JRadioButton(new AbstractAction() {
                {
                putValue(NAME, "All");
                putValue(SHORT_DESCRIPTION, "View all data.");
                }
                public void actionPerformed(ActionEvent event) {
                    setMode(Mode.All);
                }
            }) {{
                setSelected(mode == Mode.All);
            }};

            final JXTextField rangeTextField = new JXTextField() {{
                setEnabled(mode == Mode.Strip);
                setHorizontalAlignment(TRAILING);
                setColumns(4);
                setMinimumSize(getPreferredSize());
                setText(Double.toString(fixedAutoRange));
                setToolTipText("Size of the strip.");
                addActionListener(new ActionListener() {
                    public void actionPerformed(ActionEvent event) {
                        try {
                            fixedAutoRange = Double.parseDouble(getText().trim());
                            domainAxis.setFixedAutoRange(fixedAutoRange);
                        }
                        catch (NumberFormatException nfe) {}
                        setText(Double.toString(fixedAutoRange));
                    }
                });
                addFocusListener(new FocusAdapter() {
                    @Override
                    public void focusLost(FocusEvent event) {
                        setText(Double.toString(fixedAutoRange));
                    }
                });
            }};

            final JRadioButton stripRadioButton = new JRadioButton(new AbstractAction() {
                {
                putValue(NAME, "Strip");
                putValue(SHORT_DESCRIPTION, "View only the latest strip of data.");
                }
                public void actionPerformed(ActionEvent event) {
                   setMode(Mode.Strip);
                }
            }) {{
                setSelected(mode == Mode.Strip);
                addChangeListener(new ChangeListener() {
                    public void stateChanged(ChangeEvent event) {
                        rangeTextField.setEnabled(isSelected());
                    }
                });
            }};

            final JRadioButton fixedRadioButton = new JRadioButton(new AbstractAction() {
                {
                putValue(NAME, "Fixed");
                putValue(SHORT_DESCRIPTION, "View only a fixed strip of data.");
                }
                public void actionPerformed(ActionEvent event) {
                    setMode(Mode.Fixed);
                }
            }) {{
                setSelected(mode == Mode.Fixed);
            }};

            domainAxis.addChangeListener(new AxisChangeListener() {
                public void axisChanged(AxisChangeEvent event) {
                    if (domainAxis.isAutoRange()) {
                        if (domainAxis.getFixedAutoRange() == 0) {
                            StripChart.this.mode = Mode.All;
                            allRadioButton.setSelected(true);
                        }
                        else {
                            StripChart.this.mode = Mode.Strip;
                            stripRadioButton.setSelected(true);
                        }
                    }
                    else {
                        StripChart.this.mode = Mode.Fixed;
                        fixedRadioButton.setSelected(true);
                    }
                }
            });

            new JXRadioGroup<JRadioButton>() {{
                add(allRadioButton);
                add(stripRadioButton);
                add(fixedRadioButton);
            }};

            GridBagConstraints constraints = new GridBagConstraints() {{
                gridy = 0;
                fill = BOTH;
            }};

            add(new JXPanel(new GridBagLayout()) {{
                setBorder(new TitledBorder("Domain Axis"));

                GridBagConstraints constraints = new GridBagConstraints() {{
                    gridx = 0;
                    fill = BOTH;
                    weightx = weighty = 1;
                }};

                add(allRadioButton, constraints);
                add(stripRadioButton, constraints);
                add(fixedRadioButton, constraints);

                constraints.gridx = 1;
                constraints.gridy = 1;

                add(rangeTextField, constraints);
            }}, constraints);

            add(new JXPanel(new GridBagLayout()) {{
                setBorder(new TitledBorder("Display"));

                GridBagConstraints constraints = new GridBagConstraints() {{
                    gridx = 0;
                    fill = BOTH;
                    weightx = weighty = 1;
                }};

                add(new JCheckBox(new AbstractAction() {
                    {
                    putValue(NAME, "Lines");
                    putValue(SHORT_DESCRIPTION, "Draw lines between the data points.");
                    }
                    public void actionPerformed(ActionEvent event) {
                        renderer.setBaseLinesVisible(((JCheckBox)event.getSource()).isSelected());
                    }
                }) {{
                    setSelected(linesVisible);
                    fireActionPerformed(new ActionEvent(this, 0, ""));
                }}, constraints);

                add(new JCheckBox(new AbstractAction() {
                    {
                    putValue(NAME, "Points");
                    putValue(SHORT_DESCRIPTION, "Draw the data points.");
                    }
                    public void actionPerformed(ActionEvent event) {
                        renderer.setBaseShapesVisible(((JCheckBox)event.getSource()).isSelected());
                    }
                }) {{
                    setSelected(pointsVisible);
                    fireActionPerformed(new ActionEvent(this, 0, ""));
                }}, constraints);

                add(new JCheckBox(new AbstractAction() {
                    LegendTitle legend = chart.getLegend();
                    {
                    putValue(NAME, "Show Legend");
                    putValue(SHORT_DESCRIPTION, "Toggle display of the legend.");
                    }
                    public void actionPerformed(ActionEvent event) {
                        if (((JCheckBox)event.getSource()).isSelected()) {
                            if(chart.getLegend() == null) {
                                chart.addLegend(legend);
                            }
                        }
                        else {
                            chart.removeLegend();
                        }
                    }
                }) {{
                    setSelected(legendVisible);
                    fireActionPerformed(new ActionEvent(this, 0, ""));
                }}, constraints);
            }}, constraints);

            constraints.weightx = 1;

            add(new JXPanel(new GridBagLayout()) {{
                setBorder(new TitledBorder("Variables"));

                GridBagConstraints constraints = new GridBagConstraints() {{
                    gridx = 0;
                    fill = BOTH;
                }};

                add(new JXButton(new AbstractAction() {
                    {
                    putValue(NAME, "Add");
                    putValue(SHORT_DESCRIPTION, "Add the selected variable.");
                    }
                    public void actionPerformed(ActionEvent event) {
                        chartVariable(addComboBox.getSelectedItem());
                    }
                }), constraints);

                add(new JXButton(new AbstractAction() {
                    {
                    putValue(NAME, "Remove");
                    putValue(SHORT_DESCRIPTION, "Remove the selected variable.");
                    }
                    public void actionPerformed(ActionEvent event) {
                        removeVariable(removeComboBox.getSelectedItem());
                    }
                }), constraints);

                add(new JLabel("Domain Axis: "), constraints);

                constraints.gridx = 1;
                constraints.gridy = 0;
                constraints.weightx = 1;

                add(addComboBox, constraints);

                constraints.gridy = GridBagConstraints.RELATIVE;

                add(removeComboBox, constraints);
                add(domainComboBox, constraints);
            }}, constraints);

            domainComboBox.addActionListener(new ActionListener() {

                Variable<?> oldSelection;

                public void actionPerformed(ActionEvent event) {
                    Variable<?> newSelection = domainComboBox.getSelectedItem();
                    if (newSelection != oldSelection) {
                        oldSelection = newSelection;
                        dataSet.removeAllSeries();
                        for (int i = 0; i < removeComboBox.getItemCount(); ++i) {
                            chartVariable(removeComboBox.getItemAt(i));
                        }
                        domainAxis.setLabel(newSelection != null ? newSelection.toString() : "");
                    }
                }
            });
        }};
        add(settingsPanel, BorderLayout.SOUTH);

        setMode(mode);

        for (Variable<?> variable : allVariables) {
            addVariable(variable);
        }

        domainComboBox.setSelectedItem(domainVariable);

        for (Variable<?> variable : rangeVariables) {
            chartVariable(variable);
        }
    }

    /**
     * sets the visibility of the settings panel
     *
     * @param visible the visibility of the settings panel
     */
    public void setSettingsVisible(boolean visible) {
        settingsPanel.setVisible(visible);
    }

    /**
     * gets the visibility of the settings panel
     *
     * @return the visibility of the settings panel
     */
    public boolean areSettingsVisible() {
        return settingsPanel.isVisible();
    }

    /**
     * sets the current plotting mode
     *
     * @param mode the mode to be set
     */
    void setMode(Mode mode) {
        ValueAxis domainAxis = chart.getXYPlot().getDomainAxis();
        switch (mode) {
            case All:
                domainAxis.setFixedAutoRange(0);
                domainAxis.setAutoRange(true);
                break;
            case Strip:
                domainAxis.setFixedAutoRange(fixedAutoRange);
                domainAxis.setAutoRange(true);
                break;
            case Fixed:
                domainAxis.setAutoRange(false);
                break;
        }
    }

    /**
     * returns the current plotting mode
     *
     * @return the current plotting mode
     */
    public Mode getMode() {
        ValueAxis domainAxis = chart.getXYPlot().getDomainAxis();
        if (!domainAxis.isAutoRange()) {
            return Mode.Fixed;
        }
        if (domainAxis.getFixedAutoRange() == 0) {
            return Mode.All;
        }
        return Mode.Strip;
    }

    /**
     * gets the chart's title
     *
     * @return the chart's title
     */
    public TextTitle getTitle() {
        return chart.getTitle();
    }

    /**
     * adds a variable to the strip chart, making it available for plotting
     *
     * @param variable the varible to be added
     */
    void addVariable(Variable<?> variable) {
        addToComboBox(addComboBox, variable);
        addToComboBox(domainComboBox, variable);
    }

    /**
     * adds the variable to the chart, actually plotting it
     *
     * @param variable the varible to be charted
     */
    void chartVariable(Variable<?> variable) {
        if (variable != null) {
            dataSet.addSeries(stripChartManager.getPair(domainComboBox.getSelectedItem(), variable).getSeries());
            addComboBox.removeItem(variable);
            addToComboBox(removeComboBox, variable);
        }
    }

    /**
     * removes the variable from the chart and updates the combo boxes
     *
     * @param variable the varible to be removed
     */
    void removeVariable(Variable<?> variable) {
        if (variable != null) {
            removeVariableFromDataSet(variable);
            addToComboBox(addComboBox, variable);
            removeComboBox.removeItem(variable);
        }
    }

    /**
     * removes the variable from the chart without updating the combo boxes
     *
     * @param variable the varible to be removed
     */
    void removeVariableFromDataSet(Variable<?> variable) {
        for (int i = 0; i < dataSet.getSeriesCount(); ++i) {
            if (dataSet.getSeriesKey(i).equals(variable.toString())) {
                dataSet.removeSeries(i);
                break;
            }
        }
    }

    public void deleteAllVariables() {
        for (int i = domainComboBox.getItemCount(); i >= 0; --i) {
            deleteVariable(domainComboBox.getItemAt(i));
        }
    }

    /**
     * deletes the variable from the strip chart, making it no longer available for plotting
     *
     * @param variable the varible to be deleted
     */
    public void deleteVariable(Variable<?> variable) {
        removeVariableFromDataSet(variable);
        addComboBox.removeItem(variable);
        removeComboBox.removeItem(variable);
        domainComboBox.removeItem(variable);
    }

    /**
     * adds the given object to the given combo box if it is not already contained, maintaining alphabetic ordering
     *
     * @param comboBox the combo box to be added to
     * @param variable the varible to be added
     */
    @SuppressWarnings("unchecked")
    void addToComboBox(JComboBox comboBox, Object object) {
        for (int i =  0; i < comboBox.getItemCount(); ++i) {
            int compare = comboBox.getItemAt(i).toString().compareTo(object.toString());
            if (compare == 0) {
                return;
            }
            else if (compare > 0) {
                comboBox.insertItemAt(object, i);
                return;
            }
        }
        comboBox.addItem(object);
    }

    /**
     * returns the variables currently being plotted
     *
     * @return the variables currently being plotted
     */
    public ArrayList<Variable<?>> getPlottedVariables() {
        ArrayList<Variable<?>> variables = new ArrayList<>();
        for (int i = 0; i < removeComboBox.getItemCount(); ++i) {
            variables.add(removeComboBox.getItemAt(i));
        }
        return variables;
    }

    /**
     * returns the domain axis bounds
     *
     * @return the domain axis bounds
     */
    public Range getDomainBounds() {
        return chart.getXYPlot().getDomainAxis().getRange();
    }

    /**
     * sets the domain axis bounds
     *
     * @param range the domain axis bounds
     */
    public void setDomainBounds(Range range) {
        chart.getXYPlot().getDomainAxis().setRange(range);
    }

    /**
     * returns the range axis bounds
     *
     * @return the range axis bounds
     */
    public Range getRangeBounds() {
        return chart.getXYPlot().getRangeAxis().getRange();
    }

    /**
     * returns the range axis bounds
     *
     * @param range the range axis bounds
     */
    public void setRangeBounds(Range range) {
        chart.getXYPlot().getRangeAxis().setRange(range);
    }

    /**
     * returns the current domain variable
     *
     * @return the current domain variable
     */
    public Variable<?> getDomainVariable() {
        return domainComboBox.getSelectedItem();
    }

    /**
     * returns the range of the domain axis to track when auto-scrolling
     *
     * @return the range of the domain axis to track when auto-scrolling
     */
    public double getFixedAutoRange() {
        return fixedAutoRange;
    }

    /**
     * returns whether or not lines are being drawn
     *
     * @return whether or not lines are being drawn
     */
    public boolean areLinesVisible() {
        return ((XYLineAndShapeRenderer)chart.getXYPlot().getRenderer()).getBaseLinesVisible();
    }

    /**
     * returns whether or not shapes are being drawn
     *
     * @return whether or not shapes are being drawn
     */
    public boolean arePointsVisible() {
        return ((XYLineAndShapeRenderer)chart.getXYPlot().getRenderer()).getBaseShapesVisible();
    }

    /**
     * returns whether or not the legend is being drawn
     *
     * @return whether or not the legend is being drawn
     */
    public boolean isLegendVisible() {
        return chart.getLegend() != null;
    }

    /**
     * gets the underlying chart
     *
     * @return the underlying chart object
     */
    public JFreeChart getChart() {
        return chart;
    }

    /**
     * a frame for displaying a strip chart
     */
    public static class Frame extends JXFrame {

        /** the strip chart icon */
        public static final ImageIcon imageIcon = new ImageIcon(
          StripChart.class.getResource("resources/kmplot.gif"));

        /**
         * constructor
         *
         * @param stripChart the strip chart to be framed
         * @param bounds the bounds of the frame. <code>null</code> allows
         * the look and feel to determine the bounds.
         */
        public Frame(final StripChart stripChart, Rectangle bounds) {
            setDefaultCloseOperation(DISPOSE_ON_CLOSE);
            setIconImage(imageIcon.getImage());
            setTitle("Strip Chart");
            setContentPane(stripChart);

            setJMenuBar(new JMenuBar() {{
                add(new JMenu("View") {{
                    setMnemonic(KeyEvent.VK_V);
                    add(new JCheckBoxMenuItem() {{
                        setSelected(stripChart.areSettingsVisible());
                        setAction(new AbstractAction("Settings") {
                            {
                            putValue(SHORT_DESCRIPTION, "Toggle the visibility of the settings panel.");
                            putValue(MNEMONIC_KEY, KeyEvent.VK_S);
                            }
                            public void actionPerformed(ActionEvent actionEvent) {
                                stripChart.setSettingsVisible(isSelected());
                            }
                        });
                    }});
                }});
            }});

            if (bounds != null) {
                setBounds(bounds);
            }
            else {
                pack();
            }
        }

    }

    /**
     * JComboBox.getSelectedItem() is not generic, returning an Object instead of E.
     * The reason for this is that an editable combo box allows you to type a string, which cannot
     * be constrained to the generic type and may be returned by getSelectedItem(), directly into
     * the component. However, getItemAt(getSelectedIndex()) will always return an E. If the
     * selected item is a custom string in an editable combo box, the return value is null. We're
     * not using editable combo boxes in this file, so this class relieves us of having to type
     * comboBox.getItemAt(comboBox.getSelectedIndex()) in place of comboBox.getSelectedItem().
     */
    private class ComboBox<E> extends JComboBox<E> {

        @Override
        public E getSelectedItem() {
            return getItemAt(getSelectedIndex());
        }

    }

}
