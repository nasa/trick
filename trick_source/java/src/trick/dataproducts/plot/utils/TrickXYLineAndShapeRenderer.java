package trick.dataproducts.plot.utils;

import org.jfree.chart.renderer.xy.XYLineAndShapeRenderer;

public class TrickXYLineAndShapeRenderer extends XYLineAndShapeRenderer {

    //========================================
    //    Public data
    //========================================



    //========================================
    //    Protected data
    //========================================


    //========================================
    //    Private data
    //========================================

    private static final long serialVersionUID = -7308381022424004094L;

    private int selectedSeries = -1;
    private int selectedItem = -1;

    //========================================
    //   Constructors
    //========================================
    public TrickXYLineAndShapeRenderer() {
        super(true, false);
        setBaseShapesFilled(false);
    }



    //========================================
    //  Set/Get methods
    //========================================
    public void setSelectedSeries(int series) {
        this.selectedSeries = series;
    }

    public void setSelectedItem(int item) {
        this.selectedItem = item;
    }

    //========================================
    //   Methods
    //========================================
    public boolean isSelectionActive() {
        return (selectedSeries > -1 && selectedItem > -1);
    }
}
