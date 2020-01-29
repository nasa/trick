package trick.tv;

import java.awt.Color;
import java.awt.Font;
import java.awt.Paint;
import java.awt.Rectangle;
import java.util.ArrayList;
import java.util.HashSet;

import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlType;

import org.jdesktop.swingx.JXFrame;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.axis.ValueAxis;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.title.TextTitle;
import org.jfree.data.Range;

import trick.common.utils.vs.Variable;
import trick.sie.utils.SieEnumeration;
import trick.tv.StripChart;

public class TVBean {

    public double cyclePeriod;

    @XmlElement(name = "variable")
    public ArrayList<Variable<? extends TrickViewFluent>> variables =
      new ArrayList<Variable<? extends TrickViewFluent>>();

    @XmlElement(name = "stripChart")
    public ArrayList<StripChartBean> stripCharts = new ArrayList<StripChartBean>();

    @XmlElement(name = "enumeration")
    public HashSet<SieEnumeration> enumerations = new HashSet<SieEnumeration>();

    public TVBean() {}

    public TVBean(TVApplication tv) {
        cyclePeriod = tv.getCyclePeriod();
        variables = tv.variableTable.getVariables();

        for (Variable<? extends TrickViewFluent> variable : variables) {
            TrickViewFluent value = variable.getValue();
            if (value instanceof TVEnumeration) {
                enumerations.add(((TVEnumeration)value).enumeration);
            }
        }

        for (StripChart stripChart : tv.stripChartManager.getStripCharts()) {
            stripCharts.add(new StripChartBean(stripChart));
        }
    }

    @XmlType(name = "stripChart")
    public static class StripChartBean {

        @XmlElement(required = true)
        public String domainVariable;
        @XmlElement(name = "plottedVariable")
        public ArrayList<String> plottedVariables = new ArrayList<String>();
        @XmlElement(required = true)
        public StripChart.Mode mode;
        public double fixedAutoRange;
        public boolean areLinesVisible;
        public boolean arePointsVisible;
        public boolean isLegendVisible;
        public boolean areSettingsVisible;
        public boolean isAntiAliased;
        public boolean isBackgroundSet;
        public int backgroundRgb;
        public TitleBean title;
        public boolean isHorizontal;
        public int outlineRgb;
        public int plotBackgroundRgb;
        @XmlElement(required = true)
        public AxisBean domainAxis;
        @XmlElement(required = true)
        public AxisBean rangeAxis;
        @XmlElement(required = true)
        public BoundsBean bounds;

        public StripChartBean() {}

        public StripChartBean(StripChart stripChart) {
            domainVariable = stripChart.getDomainVariable().name;
            for (Variable variable : stripChart.getPlottedVariables()) {
                plottedVariables.add(variable.name);
            }
            mode = stripChart.getMode();
            fixedAutoRange = stripChart.getFixedAutoRange();
            areLinesVisible = stripChart.areLinesVisible();
            arePointsVisible = stripChart.arePointsVisible();
            isLegendVisible = stripChart.isLegendVisible();
            areSettingsVisible = stripChart.areSettingsVisible();

            JFreeChart chart = stripChart.getChart();
            isAntiAliased = chart.getAntiAlias();

            Paint paint = chart.getBackgroundPaint();
            isBackgroundSet = paint != null;
            if (isBackgroundSet) {
                backgroundRgb = ((Color)paint).getRGB();
            }

            TextTitle textTitle = stripChart.getTitle();
            if (textTitle != null) {
                title = new TitleBean(textTitle);
            }

            XYPlot plot = chart.getXYPlot();
            isHorizontal = plot.getOrientation() == PlotOrientation.HORIZONTAL;
            outlineRgb = ((Color)plot.getOutlinePaint()).getRGB();
            plotBackgroundRgb = ((Color)plot.getBackgroundPaint()).getRGB();
            domainAxis = new AxisBean(plot.getDomainAxis());
            rangeAxis = new AxisBean(plot.getRangeAxis());
            Rectangle bounds = ((JXFrame)stripChart.getTopLevelAncestor()).getBounds();
            this.bounds = new BoundsBean(bounds.x, bounds.y, bounds.width, bounds.height);
        }

        public void restore(StripChart stripChart) {
            stripChart.setSettingsVisible(areSettingsVisible);

            JFreeChart chart = stripChart.getChart();
            chart.setAntiAlias(isAntiAliased);
            if (isBackgroundSet) {
                chart.setBackgroundPaint(new Color(backgroundRgb));
            }

            if (title != null) {
                title.restore(stripChart.getTitle());
            }
            else {
                chart.setTitle((TextTitle)null);
            }

            XYPlot plot = chart.getXYPlot();
            plot.setOrientation(isHorizontal ? PlotOrientation.HORIZONTAL : PlotOrientation.VERTICAL);
            plot.setOutlinePaint(new Color(outlineRgb));
            plot.setBackgroundPaint(new Color(plotBackgroundRgb));

            boolean restoreRange = mode == StripChart.Mode.Fixed;
            domainAxis.restore(plot.getDomainAxis(), restoreRange);
            rangeAxis.restore(plot.getRangeAxis(), restoreRange);
        }

        @XmlType(name = "title")
        public static class TitleBean {

            @XmlElement(required = true)
            public String text;
            @XmlElement(required = true)
            public FontBean font;
            public int rgb;

            public TitleBean() {}

            public TitleBean(TextTitle textTitle) {
                text = textTitle.getText();
                font = new FontBean(textTitle.getFont());
                rgb = ((Color)textTitle.getPaint()).getRGB();
            }

            public void restore(TextTitle title) {
                title.setText(text);
                title.setFont(font.restore());
                title.setPaint(new Color(rgb));
            }

        }

        @XmlType(name = "font")
        public static class FontBean {

            @XmlElement(required = true)
            public String name;
            public int style;
            public int size;

            public FontBean() {}

            public FontBean(Font font) {
                name = font.getName();
                style = font.getStyle();
                size = font.getSize();
            }

            public Font restore() {
                return new Font(name, style, size);
            }

        }

        @XmlType(name = "axis")
        public static class AxisBean {

            @XmlElement(required = true)
            public String label;
            @XmlElement(required = true)
            public FontBean font;
            public int rgb;
            public boolean areTickMarksVisible;
            public boolean areTickLabelsVisible;
            @XmlElement(required = true)
            public FontBean tickLabelFont;
            public int tickLabelRgb;
            @XmlElement(required = true)
            public RangeBean range;

            public AxisBean() {}

            public AxisBean(ValueAxis axis) {
                label = axis.getLabel();
                font = new FontBean(axis.getLabelFont());
                rgb = ((Color)axis.getLabelPaint()).getRGB();
                areTickMarksVisible = axis.isTickMarksVisible();
                areTickLabelsVisible = axis.isTickLabelsVisible();
                tickLabelFont = new FontBean(axis.getTickLabelFont());
                tickLabelRgb = ((Color)axis.getTickLabelPaint()).getRGB();
                range = new RangeBean(axis.getRange());
            }

            public void restore(ValueAxis axis, boolean restoreRange) {
                axis.setLabel(label);
                axis.setLabelFont(font.restore());
                axis.setLabelPaint(new Color(rgb));
                axis.setTickMarksVisible(areTickMarksVisible);
                axis.setTickLabelsVisible(areTickLabelsVisible);
                axis.setTickLabelFont(tickLabelFont.restore());
                axis.setTickLabelPaint(new Color(tickLabelRgb));
                if (restoreRange) {
                    axis.setRange(range.restore());
                }
            }

            @XmlType(name = "range")
            public static class RangeBean {

                public double lower;
                public double upper;

                public RangeBean() {}

                public RangeBean(Range range) {
                    lower = range.getLowerBound();
                    upper = range.getUpperBound();
                }

                public Range restore() {
                    return new Range(lower, upper);
                }

            }

        }

        @XmlType(name = "bounds")
        public static class BoundsBean {

            public int x;
            public int y;
            public int width;
            public int height;

            public BoundsBean() {};

            public BoundsBean(int x, int y, int width, int height) {
                this.x = x;
                this.y = y;
                this.width = width;
                this.height = height;
            }

        }

    }

}
