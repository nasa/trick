package trick.dataproducts.plot.utils;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.event.ActionEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.File;

import javax.swing.AbstractAction;
import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JCheckBoxMenuItem;
import javax.swing.JPanel;
import javax.swing.border.Border;
import javax.swing.border.EtchedBorder;

import org.jfree.chart.ChartMouseEvent;
import org.jfree.chart.ChartMouseListener;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.annotations.XYPointerAnnotation;
import org.jfree.chart.entity.XYItemEntity;
import org.jfree.chart.event.ChartChangeEvent;
import org.jfree.chart.event.ChartChangeListener;
import org.jfree.chart.event.ChartProgressEvent;
import org.jfree.chart.event.ChartProgressListener;
import org.jfree.chart.panel.CrosshairOverlay;
import org.jfree.chart.plot.Crosshair;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.renderer.xy.XYItemRenderer;
import org.jfree.data.xy.XYDataset;
import org.jfree.ui.Layer;
import org.jfree.ui.RectangleAnchor;
import org.jfree.ui.TextAnchor;


public class TrickChartControlPanel extends JPanel implements ChartChangeListener, ChartMouseListener, ChartProgressListener {


    //========================================
    //    Public data
    //========================================



    //========================================
    //    Protected data
    //========================================



    //========================================
    //    Private Data
    //========================================
    private JFreeChart trickChart;
    private TrickChartPanel chartPanel;
    private JPanel controlPanel;

    private static final long serialVersionUID = -6328244469933876791L;

    private JCheckBox showLegendCheck;
    private JCheckBox showGridCheck;
    private JCheckBox showCrosshairCheck;
    private JButton printButton;
    private JButton moreButton;
    private JCheckBoxMenuItem showControlCheck;

    private int lastSelectedSeries = -1;

    private Crosshair crosshairX;
    private Crosshair crosshairY;

    //========================================
    //   Constructors
    //========================================
    /**
     * Constructs a panel containing a chart and its control.
     * The <code>useBuffer</code> flag controls whether or not an
     * offscreen <code>BufferedImage</code> is maintained for the chart.
     * If the buffer is used, more memory is consumed, but panel repaints
     * will be a lot quicker in cases where the chart itself hasn't
     * changed (for example, when another frame is moved to reveal the panel).
     *
     * WARNING: If you set the <code>useBuffer</code>
     * flag to false, note that the mouse zooming rectangle will (in that case)
     * be drawn using XOR, and there is a SEVERE performance problem with that
     * on JRE6 on Windows.
     *
     * @param chart  the chart.
     * @param useBuffer  a flag controlling whether or not an off-screen buffer
     *                   is used (read the warning above before setting this
     *                   to <code>false</code>).
     */
    public TrickChartControlPanel(JFreeChart chart, boolean useBuffer) {
        setLayout(new BorderLayout());
        setDoubleBuffered(true);

        updateUI();
        trickChart = chart;
        trickChart.addChangeListener(this);
        trickChart.addProgressListener(this);

        trickChart.getXYPlot().setRangeCrosshairLockedOnData(false);
        trickChart.getXYPlot().setDomainCrosshairLockedOnData(false);

        chartPanel = new TrickChartPanel(chart, useBuffer);
        chartPanel.setDefaultDirectoryForSaveAs(new File("."));
        chartPanel.setMouseWheelEnabled(true);
        chartPanel.setAutoscrolls(true);
        chartPanel.addChartMouseListener(this);
        Border border = BorderFactory.createEtchedBorder(EtchedBorder.RAISED);
        chartPanel.setBorder(border);

        CrosshairOverlay overlay = new CrosshairOverlay();
        crosshairX = new Crosshair(0.0);
        crosshairX.setVisible(false);
        crosshairX.setPaint(Color.red);
        crosshairY = new Crosshair(0.0);
        crosshairY.setVisible(false);
        crosshairY.setPaint(Color.blue);
        overlay.addDomainCrosshair(crosshairX);
        overlay.addRangeCrosshair(crosshairY);
        chartPanel.addOverlay(overlay);
        crosshairX.setLabelVisible(true);
        crosshairX.setLabelAnchor(RectangleAnchor.BOTTOM_RIGHT);
        crosshairX.setLabelBackgroundPaint(new Color(255, 255, 0, 100));
        crosshairY.setLabelVisible(true);
        crosshairY.setLabelBackgroundPaint(new Color(255, 255, 0, 100));

        controlPanel = new JPanel();

        setupControls();

        controlPanel.add(showLegendCheck);
        controlPanel.add(showGridCheck);
        controlPanel.add(showCrosshairCheck);
        controlPanel.add(printButton);
        controlPanel.add(moreButton);

        this.add(chartPanel, BorderLayout.CENTER);
        this.add(controlPanel, BorderLayout.SOUTH);

        showControlCheck = new JCheckBoxMenuItem("Show Control", true);
        showControlCheck.addActionListener(new AbstractAction("Show Control") {

            private static final long serialVersionUID = -2185831057523724894L;

            @Override
            public void actionPerformed(ActionEvent e) {
                if (showControlCheck.isSelected()) {
                    controlPanel.setVisible(true);
                } else {
                    controlPanel.setVisible(false);
                }
            }
        });

        chartPanel.getPopupMenu().add(showControlCheck);
    }

    /**
     * Sets up all control gui components.
     */
    private void setupControls() {
        // by default, legend is shown
        showLegendCheck = new JCheckBox("Show Legend", true);
        showLegendCheck.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                if (!showLegendCheck.isSelected()) {
                    trickChart.getLegend().setVisible(false);
                } else {
                    trickChart.getLegend().setVisible(true);
                }
            }
        });

        // depending on the DP_ file setting
        showGridCheck = new JCheckBox("Show Grid", false);
        if (trickChart.getXYPlot().isDomainGridlinesVisible()) {
            showGridCheck.setSelected(true);
        }
        showGridCheck.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                if (!showGridCheck.isSelected()) {
                    trickChart.getXYPlot().setRangeGridlinesVisible(false);
                    trickChart.getXYPlot().setDomainGridlinesVisible(false);
                } else {
                    trickChart.getXYPlot().setRangeGridlinesVisible(true);
                    trickChart.getXYPlot().setDomainGridlinesVisible(true);
                }
            }
        });

        // by default, crosshair is not shown
        showCrosshairCheck = new JCheckBox("Show Crosshair", false);
        showCrosshairCheck.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                if (!showCrosshairCheck.isSelected()) {
                    crosshairX.setVisible(false);
                    crosshairY.setVisible(false);
                } else {
                    crosshairX.setVisible(true);
                    crosshairY.setVisible(true);
                }
            }
        });

        printButton = new JButton(new AbstractAction("Print...") {
            private static final long serialVersionUID = 4694651015847994699L;

            @Override
            public void actionPerformed(ActionEvent e) {
                chartPanel.createChartPrintJob();
            }
        });

        moreButton = new JButton(new AbstractAction("More...") {
            private static final long serialVersionUID = 4106804422858015720L;

            @Override
            public void actionPerformed(ActionEvent e) {
                chartPanel.getPopupMenu().show((JButton)e.getSource(), 0, 0);
            }
        });
    }

    /**
     * Gets the instance of {@link JFreeChart} of this panel.
     *
     * @return the instance of {@link JFreeChart} which this panel holds.
     */
    public JFreeChart getTheChart() {
        return trickChart;
    }

    @Override
    public void chartChanged(ChartChangeEvent event) {

    }

    @Override
    public void chartMouseClicked(ChartMouseEvent event) {
        /*
         * TODO: complete it!
        MouseEvent me = event.getTrigger();
        if ((me.getModifiers() & MouseEvent.CTRL_MASK) > 0) {
            String response = JOptionPane.showInputDialog(this,
                                                          "Formula:",
                                                          "Curve Formula",
                                                          JOptionPane.QUESTION_MESSAGE);
            if (response != null && !response.trim().isEmpty()) {
                    // applyFormulaToPlot();
            }
        } else if ((me.getModifiers() & MouseEvent.SHIFT_MASK) > 0) {
            // manipulateCurve();
        }
        */
    }

    @Override
    public void chartProgress(ChartProgressEvent event) {
        crosshairX.setValue(trickChart.getXYPlot().getDomainCrosshairValue());
        crosshairY.setValue(trickChart.getXYPlot().getRangeCrosshairValue());
    }

    @Override
    public void chartMouseMoved(ChartMouseEvent event) {
        XYPlot plot = event.getChart().getXYPlot();
        if (event.getEntity() instanceof XYItemEntity) {
            XYItemEntity e = (XYItemEntity) (event.getEntity());
            XYDataset dataset = e.getDataset();

            for (int i = 0; i < plot.getDatasetCount(); i++) {
                XYDataset test = plot.getDataset(i);
                if (test == dataset) {
                    XYItemRenderer r = plot.getRenderer(i);

                    if (r instanceof TrickXYLineAndShapeRenderer) {
                        TrickXYLineAndShapeRenderer sel = (TrickXYLineAndShapeRenderer) r;
                        if (lastSelectedSeries != -1) {
                            r.removeAnnotations();
                        }
                        if (sel.isSelectionActive()) {
                            sel.setSelectedSeries(-1);
                            sel.setSelectedItem(-1);
                        } else {
                            sel.setSelectedSeries(e.getSeriesIndex());
                            lastSelectedSeries = e.getSeriesIndex();
                            XYPointerAnnotation annotation = new XYPointerAnnotation(
                                    "series " + e.getSeriesIndex(), test.getXValue(e.getSeriesIndex(), e.getItem()), test.getYValue(e.getSeriesIndex(), e.getItem()), -Math.PI / 4.0);
                            annotation.setTextAnchor(TextAnchor.BOTTOM_LEFT);
                            annotation.setPaint(sel.getSeriesPaint(e.getSeriesIndex()));
                            annotation.setArrowPaint(sel.getSeriesPaint(e.getSeriesIndex()));
                            plot.getRenderer().addAnnotation(annotation, Layer.BACKGROUND);
                        }
                    }
                }
            }
        } else {
            if (lastSelectedSeries != -1) {
                plot.getRenderer().removeAnnotations();
            }
        }
    }

    //========================================
    //   Inner classes
    //========================================
}
