
package trick.dataproducts.plot.utils;

import java.awt.Font;

import org.jfree.chart.StandardChartTheme;

public class TrickChartTheme extends StandardChartTheme {

	private static final long serialVersionUID = 4199990670106681300L;

	/**
     * Creates a new default instance.
     *
     * @param name  the name of the theme (<code>null</code> not permitted).
     */
    public TrickChartTheme(String name) {
        super(name);
        this.setExtraLargeFont(new Font("Tahoma", Font.BOLD, 14));
        this.setLargeFont(new Font("Tahoma", Font.BOLD, 12));
    }
}
