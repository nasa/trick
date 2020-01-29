
package trick.dataproducts.plot.utils;

import java.awt.Font;

import org.jfree.chart.ChartFactory;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.block.BlockBorder;
import org.jfree.chart.plot.Plot;

public class TrickChart extends JFreeChart {

    //========================================
    //    Public data
    //========================================
    /** The default font for the primary title. */
    public static final Font TRICK_DEFAULT_TITLE_FONT = new Font("SansSerif", Font.BOLD, 12);

    /** The default font for sub titles. */
    public static final Font TRICK_DEFAULT_SUBTITLE_FONT = new Font("SansSerif", Font.PLAIN, 12);

    /** The default font for legend. */
    public static final Font TRICK_DEFAULT_LEGEND_FONT = new Font("SansSerif", Font.BOLD, 10);


    //========================================
    //    Protected data
    //========================================



    //========================================
    //    Private Data
    //========================================
    private static final long serialVersionUID = -7190474578193337645L;

    //========================================
    //   Constructors
    //========================================
    /**
     * Creates a new chart based on the supplied plot.  The chart will have
     * a legend added automatically, but no title (although you can easily add
     * one later).
     * <br><br>
     * Note that the  {@link ChartFactory} class contains a range
     * of static methods that will return ready-made charts, and often this
     * is a more convenient way to create charts than using this constructor.
     *
     * @param plot  the plot (<code>null</code> not permitted).
     */
    public TrickChart(Plot plot) {
        super(plot);
        changeLegend();
        setAntiAlias(false);
    }

    /**
     * Creates a new chart with the given title and plot.  A default font
     * ({@link #DEFAULT_TITLE_FONT}) is used for the title, and the chart will
     * have a legend added automatically.
     * <br><br>
     * Note that the {@link ChartFactory} class contains a range
     * of static methods that will return ready-made charts, and often this
     * is a more convenient way to create charts than using this constructor.
     *
     * @param title  the chart title (<code>null</code> permitted).
     * @param plot  the plot (<code>null</code> not permitted).
     */
    public TrickChart(String title, Plot plot) {
        super(title, plot);
        changeLegend();
        setAntiAlias(false);
    }

    /**
     * Creates a new chart with the given title and plot.  The
     * <code>createLegend</code> argument specifies whether or not a legend
     * should be added to the chart.
     * <br><br>
     * Note that the  {@link ChartFactory} class contains a range
     * of static methods that will return ready-made charts, and often this
     * is a more convenient way to create charts than using this constructor.
     *
     * @param title  the chart title (<code>null</code> permitted).
     * @param titleFont  the font for displaying the chart title
     *                   (<code>null</code> permitted).
     * @param plot  controller of the visual representation of the data
     *              (<code>null</code> not permitted).
     * @param createLegend  a flag indicating whether or not a legend should
     *                      be created for the chart.
     */
    public TrickChart(String title, Font titleFont, Plot plot, boolean createLegend) {
        super(title, titleFont, plot, createLegend);
        if (createLegend) {
            changeLegend();
        }
        setAntiAlias(false);
    }

    //========================================
    //   Methods
    //========================================
    private void changeLegend() {
        getLegend().setItemFont(TRICK_DEFAULT_LEGEND_FONT);
        getLegend().setFrame(BlockBorder.NONE);
        getLegend().setBackgroundPaint(null);
    }

    /**
     * Uses the title of this chart to represent the chart.
     */
    @Override
    public String toString() {
        return getTitle().getText();
    }
}
