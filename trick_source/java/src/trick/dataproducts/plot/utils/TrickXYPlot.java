
package trick.dataproducts.plot.utils;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Font;
import java.awt.Shape;
import java.math.RoundingMode;
import java.text.DecimalFormat;

import org.jfree.chart.axis.NumberAxis;
import org.jfree.chart.labels.StandardXYToolTipGenerator;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.plot.XYPlot;
import org.jfree.data.xy.XYSeriesCollection;

import trick.dataproducts.trickqp.utils.ProductDataPanel;
import trick.dataproducts.trickqp.utils.ProductPage;
import trick.dataproducts.trickqp.utils.ProductPlot;
import trick.dataproducts.trickqp.utils.ProductVar;

/**
 * Extends {@link XYPlot} so that Trick plotting specification can
 * be applied to the plot.
 *
 * @author Hong Chen
 *
 */
public class TrickXYPlot extends XYPlot {


    //========================================
    //    Public data
    //========================================



    //========================================
    //    Protected data
    //========================================



    //========================================
    //    Private Data
    //========================================

    private static final long serialVersionUID = -3995993644038371850L;


    //========================================
    //   Constructors
    //========================================
    /**
     * Creates a new plot with the specified dataset, axes and renderer
     * that is for a particular {@link ProductPage} and {@link ProductPlot}.
     *
     * @param aPage the {@link ProductPage} which the plot is with.
     * @param aPlot the {@link ProductPlot} spec plotting.
     * @param dataset  the dataset (<code>null</code> permitted).
     * @param guiXAxis  the domain axis (<code>null</code> permitted).
     * @param guiYAxis  the range axis (<code>null</code> permitted).
     * @param renderer  the renderer (<code>null</code> permitted).
     */
    public TrickXYPlot(ProductPage aPage,
                       ProductPlot aPlot,
                       XYSeriesCollection dataset,
                       NumberAxis guiXAxis,
                       NumberAxis guiYAxis,
                       TrickXYLineAndShapeRenderer renderer) {

        super(dataset, guiXAxis, guiYAxis, renderer);

        setAxisRange(dataset, guiXAxis, guiYAxis);

        // the order of the following calls matters as the lower level of
        // specs would override the higher level of specs
        applyPageSpec(aPage, guiXAxis, guiYAxis);

        applyPlotSpec(aPage, aPlot, dataset, guiXAxis, guiYAxis);

        // TODO: applyXVarSpec

        applyYVarSpec(dataset, renderer);

        setOrientation(PlotOrientation.VERTICAL);
        renderer.setBaseToolTipGenerator(new StandardXYToolTipGenerator());
    }


    //========================================
    //  Methods
    //========================================
    /**
     * Helper method for setting the range for both X and Y axis.
     * The min and max values are based on all series data.
     */
    private void setAxisRange(XYSeriesCollection dataset, NumberAxis guiXAxis,
            NumberAxis guiYAxis) {
        if (dataset == null || dataset.getSeriesCount() < 1) {
            return;
        }
        double minX = dataset.getSeries(0).getMinX();
        double maxX = dataset.getSeries(0).getMaxX();
        double minY = dataset.getSeries(0).getMinY();
        double maxY = dataset.getSeries(0).getMaxY();

        for (int i = 1; i < dataset.getSeriesCount(); i++) {
            minX = Math.min(minX, dataset.getSeries(i).getMinX());
            maxX = Math.max(maxX, dataset.getSeries(i).getMaxX());
            minY = Math.min(minY, dataset.getSeries(i).getMinY());
            maxY = Math.max(maxY, dataset.getSeries(i).getMaxY());
        }
        guiXAxis.setRange(minX, maxX);
        guiYAxis.setRange(minY, maxY);
    }

    /**
     * Helper method for applying page spec to gui.
     */
    private void applyPageSpec(ProductPage aPage, NumberAxis guiXAxis, NumberAxis guiYAxis) {

        if (aPage.getForegroundColor() != null) {
            guiXAxis.setTickLabelPaint(aPage.getForegroundColor());
            guiXAxis.setLabelPaint(aPage.getForegroundColor());
            guiYAxis.setTickLabelPaint(aPage.getForegroundColor());
            guiYAxis.setLabelPaint(aPage.getForegroundColor());
        }

        if (aPage.getBackgroundColor() != null) {
            setBackgroundPaint(aPage.getBackgroundColor());
        }
    }

    /**
     * Helper method for applying plot spec to gui.
     */
    private void applyPlotSpec(ProductPage aPage,
                               ProductPlot aPlot,
                               XYSeriesCollection dataset,
                               NumberAxis guiXAxis,
                               NumberAxis guiYAxis) {

        applyPlotXAxisSpec(aPlot, dataset, guiXAxis);

        applyPlotYAxisSpec(aPlot, dataset, guiYAxis);

        // Plot Start and Stop

        // Plot Foreground
        if (aPlot.getForegroundColor() != null) {
            guiXAxis.setTickLabelPaint(aPlot.getForegroundColor());
            guiXAxis.setLabelPaint(aPlot.getForegroundColor());
            guiYAxis.setTickLabelPaint(aPlot.getForegroundColor());
            guiYAxis.setLabelPaint(aPlot.getForegroundColor());
        }

        // Plot Background
        if (aPlot.getBackgroundColor() != null) {
            setBackgroundPaint(aPlot.getBackgroundColor());
        }

        // Plot Grid and Grid Color
        if (aPlot.getGrid().equalsIgnoreCase("Yes")) {
            setDomainGridlinesVisible(true);
            setRangeGridlinesVisible(true);
            if (aPlot.getGridColor() != null) {
                setDomainGridlinePaint(aPlot.getGridColor());
                setRangeGridlinePaint(aPlot.getGridColor());
            }

            // use this stroke for both domain and range grid to make it more clear
            BasicStroke stroke = new BasicStroke(1.0f, BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND);
            setDomainGridlineStroke(stroke);
            setRangeGridlineStroke(stroke);
        } else {
            setDomainGridlinesVisible(true);
            setRangeGridlinesVisible(true);
        }

        // Plot Font
        if (aPlot.getFont() != null && !(aPlot.getFont().isEmpty())) {
            Font plotFont = ProductDataPanel.getFontFromText(aPlot.getFont());
            if (plotFont != null) {
                guiXAxis.setTickLabelFont(plotFont);
                guiXAxis.setLabelFont(plotFont);
                guiYAxis.setTickLabelFont(plotFont);
                guiYAxis.setLabelFont(plotFont);
            }
        }
    }

    /**
     * Helper method for applying Y axis spec to gui.
     */
    private void applyPlotYAxisSpec(ProductPlot aPlot,
                                XYSeriesCollection dataset,
                                NumberAxis guiYAxis) {
        // apply Plot Y Axis Label
        // if no label provided, the Y variable name is used if they are all the same within the collection
        if (aPlot.getYAxis() != null) {
            guiYAxis.setLabel(aPlot.getYAxis().getLabel());
        } else {
            ProductVar theYVar = getYVarFromSeriesCollection(dataset);
            if (theYVar != null) {
                guiYAxis.setLabel(theYVar.getName());
                guiYAxis.setFixedAutoRange(dataset.getSeries(0).getMaxY() - dataset.getSeries(0).getMinY());
            }
        }

        // Y Axis Format
        if (aPlot.getYAxis() != null && aPlot.getYAxis().getFormat() != null && !aPlot.getYAxis().getFormat().isEmpty()) {
            try {
            DecimalFormat newFormat = new DecimalFormat(aPlot.getYAxis().getFormat());
            newFormat.setRoundingMode(RoundingMode.HALF_UP);
            guiYAxis.setNumberFormatOverride(newFormat);
            } catch (IllegalArgumentException iae) {
                // do nothing if the provided format is invalid
            }
        }

        if (aPlot.getYMin() != null && aPlot.getYMax() != null) {
            guiYAxis.setRange(aPlot.getYMin().doubleValue(), aPlot.getYMax().doubleValue());
        } else if (aPlot.getYMin() != null) {
            guiYAxis.setRange(aPlot.getYMin().doubleValue(), guiYAxis.getUpperBound());
        } else if (aPlot.getYMax() != null) {
            guiYAxis.setRange(guiYAxis.getLowerBound(), aPlot.getYMax().doubleValue());
        }
    }

    /**
     * Helper method for applying X axis spec to gui.
     */
    private void applyPlotXAxisSpec(ProductPlot aPlot,
                                XYSeriesCollection dataset,
                                NumberAxis guiXAxis) {
        // apply Plot X Axis Label
        // if no label provided, the X variable name is used if they are all the same within the collection
        if (aPlot.getXAxis() != null) {
            guiXAxis.setLabel(aPlot.getXAxis().getLabel());
        } else {
            ProductVar theXVar = getXVarFromSeriesCollection(dataset);
            if (theXVar != null) {
                guiXAxis.setLabel(theXVar.getName());
                guiXAxis.setFixedAutoRange(dataset.getSeries(0).getMaxX() - dataset.getSeries(0).getMinX());
            }
        }

        if (aPlot.getXMin() != null && aPlot.getXMax() != null) {
            guiXAxis.setRange(aPlot.getXMin().doubleValue(), aPlot.getXMax().doubleValue());
        } else if (aPlot.getXMin() != null) {
            guiXAxis.setRange(aPlot.getXMin().doubleValue(), guiXAxis.getUpperBound());
        } else if (aPlot.getXMax() != null) {
            guiXAxis.setRange(guiXAxis.getLowerBound(), aPlot.getXMax().doubleValue());
        }
    }

    /**
     * Helper method for applying Y variable spec to gui.
     */
    private void applyYVarSpec(XYSeriesCollection dataset,
            TrickXYLineAndShapeRenderer renderer) {
        // Y Var Line Color, Line Style, Symbol Style, Symbol Size
        for (int i = 0; i < dataset.getSeriesCount(); i++) {
            TrickXYSeries xySeries = null;
            if (dataset.getSeries(i) instanceof TrickXYSeries) {
                xySeries = (TrickXYSeries)dataset.getSeries(i);
                // color
                Color lineColor = xySeries.getYVar().getLineColor();
                if (lineColor != null) {
                    renderer.setSeriesPaint(i, lineColor);
                }
                // style
                ProductVar.LineStyle lineStyle = xySeries.getYVar().getLineStyle();
                if (lineStyle != null) {
                    renderer.setSeriesStroke(i, lineStyle.getStroke());
                }
                // symbol
                ProductVar.SymbolStyle symbolStyle = xySeries.getYVar().getSymbolStyle();
                Shape symbolShape = xySeries.getYVar().getSymbolShape(symbolStyle);
                if (symbolShape != null) {
                    renderer.setSeriesShapesVisible(i, true);
                       renderer.setSeriesShape(i, symbolShape);
                       renderer.setBaseShapesFilled(symbolStyle.isFilled());
                } else {
                    renderer.setSeriesShapesVisible(i, false);
                    renderer.setBaseShapesFilled(false);
                }
            }
        }
    }

    /**
     * Helper method for getting the same X variable name from the specified {@link XYSeriesCollection}.
     */
    private ProductVar getXVarFromSeriesCollection(XYSeriesCollection dataset) {
        ProductVar theXVar = null;

        if (dataset.getSeriesCount() > 0) {
            if (dataset.getSeries(0) instanceof TrickXYSeries) {
                theXVar = ((TrickXYSeries)dataset.getSeries(0)).getXVar();
                for (int i = 1; i < dataset.getSeriesCount(); i++) {
                    TrickXYSeries theXYSeries = null;
                    if (dataset.getSeries(i) instanceof TrickXYSeries) {
                        theXYSeries = (TrickXYSeries)dataset.getSeries(i);
                        if (!(theXVar.getName().equals(theXYSeries.getXVar().getName()))) {
                            theXVar = null;
                            break;
                        }
                    }
                }
            }
        }
        return theXVar;
    }

    /**
     * Helper method for getting the same Y variable name from the specified {@link XYSeriesCollection}.
     */
    private ProductVar getYVarFromSeriesCollection(XYSeriesCollection dataset) {
        ProductVar theYVar = null;
        if (dataset.getSeriesCount() > 0) {
            if (dataset.getSeries(0) instanceof TrickXYSeries) {
                theYVar = ((TrickXYSeries)dataset.getSeries(0)).getYVar();
                for (int i = 1; i < dataset.getSeriesCount(); i++) {
                    TrickXYSeries theXYSeries = null;
                    if (dataset.getSeries(i) instanceof TrickXYSeries) {
                        theXYSeries = (TrickXYSeries)dataset.getSeries(i);
                        if (!(theYVar.getName().equals(theXYSeries.getYVar().getName()))) {
                            theYVar = null;
                            break;
                        }
                    }
                }
            }
        }
        return theYVar;
    }

}
