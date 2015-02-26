package trick.dataproducts.plot.utils;

import javax.swing.JOptionPane;

import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;

import trick.dataproducts.plot.utils.editor.TrickChartEditor;
import trick.dataproducts.plot.utils.editor.TrickChartEditorManager;

public class TrickChartPanel extends ChartPanel {


    private static final long serialVersionUID = -507551123952070511L;

    /**
     * Constructs a panel containing a chart.  The <code>useBuffer</code> flag
     * controls whether or not an offscreen <code>BufferedImage</code> is
     * maintained for the chart.  If the buffer is used, more memory is
     * consumed, but panel repaints will be a lot quicker in cases where the
     * chart itself hasn't changed (for example, when another frame is moved
     * to reveal the panel).  WARNING: If you set the <code>useBuffer</code>
     * flag to false, note that the mouse zooming rectangle will (in that case)
     * be drawn using XOR, and there is a SEVERE performance problem with that
     * on JRE6 on Windows.
     *
     * @param chart  the chart.
     * @param useBuffer  a flag controlling whether or not an off-screen buffer
     *                   is used (read the warning above before setting this
     *                   to <code>false</code>).
     */
    public TrickChartPanel(JFreeChart chart, boolean useBuffer) {
        super(chart, useBuffer);
    }

    /**
     * Displays a dialog that allows the user to edit the properties for the
     * current chart.
     */
    @Override
    public void doEditChartProperties() {
        TrickChartEditor editor = TrickChartEditorManager.getChartEditor(getChart());
        int result = JOptionPane.showConfirmDialog(this, editor,
                localizationResources.getString("Chart_Properties"),
                JOptionPane.OK_CANCEL_OPTION, JOptionPane.PLAIN_MESSAGE);
        if (result == JOptionPane.OK_OPTION) {
            editor.updateChart(getChart());
        }
    }
}
