
package trick.dataproducts.plot.utils.editor;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.GridLayout;
import java.awt.Paint;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.util.ArrayList;
import java.util.ResourceBundle;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JColorChooser;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTabbedPane;
import javax.swing.JTextField;

import org.jfree.chart.JFreeChart;
import org.jfree.chart.editor.ChartEditor;
import org.jfree.chart.plot.Plot;
import org.jfree.chart.plot.PolarPlot;
import org.jfree.chart.title.Title;
import org.jfree.chart.util.ResourceBundleWrapper;
import org.jfree.layout.LCBLayout;
import org.jfree.ui.PaintSample;
import org.jfree.ui.StrokeChooserPanel;
import org.jfree.ui.StrokeSample;

import trick.dataproducts.trickqp.utils.ProductVar;

/**
 * A panel for editing chart properties (includes subpanels for the title,
 * legend and plot).
 */
public class TrickChartEditor extends JPanel implements ActionListener, ChartEditor {

    
	private static final long serialVersionUID = -513746078897898843L;

	/** A panel for displaying/editing the properties of the title. */
    private TrickTitleEditor titleEditor;

    /** A panel for displaying/editing the properties of the plot. */
    private TrickPlotEditor plotEditor;

    /**
     * A checkbox indicating whether or not the chart is drawn with
     * anti-aliasing.
     */
    private JCheckBox antialias;

    /** The chart background color. */
    private PaintSample background;
    
    /** The chart plot series color. */
    private ArrayList<PaintSample> seriesPaintList;
    
    /** The chart plot series color. */
    private ArrayList<StrokeSample> seriesStrokeList;
    
    /** The resourceBundle for the localization. */
    protected static ResourceBundle localizationResources
            = ResourceBundleWrapper.getBundle(
                    "org.jfree.chart.editor.LocalizationBundle");
    private static StrokeSample[] AVAILABLE_STROKE_SAMPLES = new StrokeSample[ProductVar.LineStyle.values().length];
    static {
    	for (int i = 0; i < ProductVar.LineStyle.values().length; i++) {
    		AVAILABLE_STROKE_SAMPLES[i] = new StrokeSample(ProductVar.LineStyle.values()[i].getStroke());
    	}   	
    }
    
    /**
     * Standard constructor - the property panel is made up of a number of
     * sub-panels that are displayed in the tabbed pane.
     *
     * @param chart  the chart, which properties should be changed.
     */
    public TrickChartEditor(JFreeChart chart) {
        setLayout(new BorderLayout());

        JPanel other = new JPanel(new BorderLayout());
        other.setBorder(BorderFactory.createEmptyBorder(2, 2, 2, 2));

        JPanel general = new JPanel(new BorderLayout());
        general.setBorder(BorderFactory.createTitledBorder(
            BorderFactory.createEtchedBorder(),
            localizationResources.getString("General")));

        JPanel interior = new JPanel(new LCBLayout(4));
        interior.setBorder(BorderFactory.createEmptyBorder(0, 5, 0, 5));

        this.antialias = new JCheckBox(localizationResources.getString(
                "Draw_anti-aliased"));
        this.antialias.setSelected(chart.getAntiAlias());
        interior.add(this.antialias);
        interior.add(new JLabel(""));
        interior.add(new JLabel(""));
        interior.add(new JLabel(localizationResources.getString(
                "Background_paint")));
        this.background = new PaintSample(chart.getBackgroundPaint());
        interior.add(this.background);
        JButton button = new JButton(localizationResources.getString(
                "Select..."));
        button.setActionCommand("BackgroundPaint");
        button.addActionListener(this);
        interior.add(button);

        interior.add(new JLabel(localizationResources.getString(
                "Series_Paint")));
        new JTextField(localizationResources.getString(
                "No_editor_implemented"));
        
        int seriesCount = chart.getXYPlot().getSeriesCount();
        
        JPanel seriesPaintPanel = new JPanel(new GridLayout(0, 3, 1, 1));
        seriesPaintList = new ArrayList<PaintSample>(seriesCount);
        
        for (int i = 0; i < seriesCount; i++) {
        	PaintSample aPaintSample = new PaintSample(chart.getXYPlot().getRenderer().getSeriesPaint(i));
        	seriesPaintList.add(i, aPaintSample);
        	seriesPaintPanel.add(seriesPaintList.get(i));        	
        	aPaintSample.addMouseListener(new LocalMouseListener());        	
        }        
        
        interior.add(seriesPaintPanel);
        interior.add(new JLabel(""));

        interior.add(new JLabel(localizationResources.getString(
                "Series_Stroke")));
        
        JPanel seriesStrokePanel = new JPanel(new GridLayout(0, 3, 1, 1));
        seriesStrokeList = new ArrayList<StrokeSample>(seriesCount);
        
        for (int i = 0; i < seriesCount; i++) {
        	StrokeSample aStrokeSample = new StrokeSample(chart.getXYPlot().getRenderer().getSeriesStroke(i));
        	seriesStrokeList.add(i, aStrokeSample);        	
        	seriesStrokePanel.add(seriesStrokeList.get(i));        	
        	aStrokeSample.addMouseListener(new LocalMouseListener());        	
        }
        
        interior.add(seriesStrokePanel);
        interior.add(new JLabel(""));
        
        general.add(interior, BorderLayout.NORTH);
        other.add(general, BorderLayout.NORTH);

        JPanel parts = new JPanel(new BorderLayout());
        
        Title title = chart.getTitle();
        Plot plot = chart.getPlot();

        JTabbedPane tabs = new JTabbedPane();

        this.titleEditor = new TrickTitleEditor(title);
        this.titleEditor.setBorder(BorderFactory.createEmptyBorder(2, 2, 2, 2));
        tabs.addTab(localizationResources.getString("Title"), this.titleEditor);

        if (plot instanceof PolarPlot) {
            this.plotEditor = new TrickPolarPlotEditor((PolarPlot) plot);
        }
        else {
            this.plotEditor = new TrickPlotEditor(plot);
        }
        this.plotEditor.setBorder(BorderFactory.createEmptyBorder(2, 2, 2, 2));
        tabs.addTab(localizationResources.getString("Plot"), this.plotEditor);

        tabs.add(localizationResources.getString("Other"), other);
        parts.add(tabs, BorderLayout.NORTH);
        add(parts);
    }

    /**
     * Returns a reference to the title editor.
     *
     * @return A panel for editing the title.
     */
    public TrickTitleEditor getTitleEditor() {
      return this.titleEditor;
    }

    /**
     * Returns a reference to the plot property sub-panel.
     *
     * @return A panel for editing the plot properties.
     */
    public TrickPlotEditor getPlotEditor() {
        return this.plotEditor;
    }

    /**
     * Returns the current setting of the anti-alias flag.
     *
     * @return <code>true</code> if anti-aliasing is enabled.
     */
    public boolean getAntiAlias() {
        return this.antialias.isSelected();
    }

    /**
     * Returns the current background paint.
     *
     * @return The current background paint.
     */
    public Paint getBackgroundPaint() {
        return this.background.getPaint();
    }

    /**
     * Handles user interactions with the panel.
     *
     * @param event  a BackgroundPaint action.
     */
    public void actionPerformed(ActionEvent event) {
        String command = event.getActionCommand();
        if (command.equals("BackgroundPaint")) {
            attemptModifyBackgroundPaint();
        }
    }

    /**
     * Allows the user the opportunity to select a new background paint.  Uses
     * JColorChooser, so we are only allowing a subset of all Paint objects to
     * be selected (fix later).
     */
    private void attemptModifyBackgroundPaint() {
    	Color initialColor = (background.getPaint() instanceof Color ? (Color)background.getPaint() : Color.blue);
        Color c = JColorChooser.showDialog(this, localizationResources.getString("Background_Color"), initialColor);
        if (c != null) {
            this.background.setPaint(c);
        }
    }
    
    
    /**
     * Updates the properties of a chart to match the properties defined on the
     * panel.
     *
     * @param chart  the chart.
     */
    public void updateChart(JFreeChart chart) {

        this.titleEditor.setTitleProperties(chart);
        this.plotEditor.updatePlotProperties(chart.getPlot());

        chart.setAntiAlias(getAntiAlias());
        chart.setBackgroundPaint(getBackgroundPaint());
        for (int i = 0; i < chart.getXYPlot().getSeriesCount(); i++) {
        	chart.getXYPlot().getRenderer().setSeriesPaint(i, seriesPaintList.get(i).getPaint());
        	chart.getXYPlot().getRenderer().setSeriesStroke(i, seriesStrokeList.get(i).getStroke());       	
    	}
    }
    
    /**
     * Allow the user to change the outline stroke.
     */
    private void attemptStrokeSelection(StrokeSample sample) {
        StrokeChooserPanel panel = new StrokeChooserPanel(
                sample, AVAILABLE_STROKE_SAMPLES);
        int result = JOptionPane.showConfirmDialog(this, panel,
                localizationResources.getString("Stroke_Selection"),
                JOptionPane.OK_CANCEL_OPTION, JOptionPane.PLAIN_MESSAGE);

        if (result == JOptionPane.OK_OPTION) {       	
            sample.setStroke(panel.getSelectedStroke());            
        }
    }
    
    public class LocalMouseListener extends MouseAdapter {
    	@Override
		public void mouseClicked(MouseEvent e) {
    		if (!seriesPaintList.isEmpty()) {
    			for (PaintSample theSample : seriesPaintList) {
    				if (e.getSource().equals(theSample)) {
    			        Color initialColor = (theSample.getPaint() instanceof Color ? (Color)theSample.getPaint() : Color.blue);
    			        Color c = JColorChooser.showDialog(theSample, "Change Paint Color", initialColor);
    			        if (c != null) {
    			        	theSample.setPaint(c);
    			        }
    				}
    			}
    		}
    		if (!seriesStrokeList.isEmpty()) {
    			for (StrokeSample theSample : seriesStrokeList) {
    				if (e.getSource().equals(theSample)) {
    					attemptStrokeSelection(theSample);
    				}
    			}
    		}
    	}
    }
}
