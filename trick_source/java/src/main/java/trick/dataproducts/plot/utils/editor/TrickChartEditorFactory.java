
package trick.dataproducts.plot.utils.editor;

import org.jfree.chart.JFreeChart;
import org.jfree.chart.editor.ChartEditor;
import org.jfree.chart.editor.ChartEditorFactory;

/**
 * A default implementation of the {@link ChartEditorFactory} interface.
 */
public class TrickChartEditorFactory implements ChartEditorFactory {

    /**
     * Creates a new instance.
     */
    public TrickChartEditorFactory() {
    }

    /**
     * Returns a new instance of a {@link ChartEditor}.
     *
     * @param chart  the chart.
     *
     * @return A chart editor for the given chart.
     */
    public TrickChartEditor createEditor(JFreeChart chart) {
        return new TrickChartEditor(chart);
    }

}
