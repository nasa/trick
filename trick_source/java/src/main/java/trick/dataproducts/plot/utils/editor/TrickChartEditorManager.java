
package trick.dataproducts.plot.utils.editor;

import org.jfree.chart.JFreeChart;
import org.jfree.chart.editor.ChartEditor;
import org.jfree.chart.util.ParamChecks;

/**
 * The central point for obtaining {@link ChartEditor} instances for editing
 * charts.  Right now, the API is minimal - the plan is to extend this class
 * to provide customization options for chart editors (for example, make some
 * editor items read-only).
 */
public class TrickChartEditorManager {

    /** This factory creates new {@link ChartEditor} instances as required. */
    static TrickChartEditorFactory factory = new TrickChartEditorFactory();

    /**
     * Private constructor prevents instantiation.
     */
    private TrickChartEditorManager() {
        // nothing to do
    }

    /**
     * Returns the current factory.
     *
     * @return The current factory (never <code>null</code>).
     */
    public static TrickChartEditorFactory getChartEditorFactory() {
        return factory;
    }

    /**
     * Sets the chart editor factory.
     *
     * @param f  the new factory (<code>null</code> not permitted).
     */
    public static void setChartEditorFactory(TrickChartEditorFactory f) {
        ParamChecks.nullNotPermitted(f, "f");
        factory = f;
    }

    /**
     * Returns a component that can be used to edit the given chart.
     *
     * @param chart  the chart.
     *
     * @return The chart editor.
     */
    public static TrickChartEditor getChartEditor(JFreeChart chart) {
        return factory.createEditor(chart);
    }
}
