package trick.dataproducts.plot.utils;

import org.jfree.data.xy.XYSeries;

import trick.dataproducts.trickqp.utils.ProductVar;

public class TrickXYSeries extends XYSeries {


    //========================================
    //    Public data
    //========================================



    //========================================
    //    Protected data
    //========================================



    //========================================
    //    Private data
    //========================================

    private static final long serialVersionUID = -4137551900938397559L;

    private ProductVar xVar;
    private ProductVar yVar;


    //========================================
    //   Constructors
    //========================================
    /**
     * Creates a new empty series.  By default, items added to the series will
     * be sorted into ascending order by x-value, and duplicate x-values will
     * be allowed (these defaults can be modified with another constructor.
     *
     * @param key  the series key (<code>null</code> not permitted).
     */
    public TrickXYSeries(Comparable key) {
        super(key, true, true);
    }

    /**
     * Constructs a new empty series, with the auto-sort flag set as requested,
     * and duplicate values allowed.
     *
     * @param key  the series key (<code>null</code> not permitted).
     * @param autoSort  a flag that controls whether or not the items in the
     *                  series are sorted.
     */
    public TrickXYSeries(Comparable key, boolean autoSort) {
        super(key, autoSort, true);
    }

    /**
     * Constructs a new xy-series that contains no data.  You can specify
     * whether or not duplicate x-values are allowed for the series.
     *
     * @param key  the series key (<code>null</code> not permitted).
     * @param autoSort  a flag that controls whether or not the items in the
     *                  series are sorted.
     * @param allowDuplicateXValues  a flag that controls whether duplicate
     *                               x-values are allowed.
     */
    public TrickXYSeries(Comparable key, boolean autoSort, boolean allowDuplicateXValues) {
        super(key, autoSort, allowDuplicateXValues);
    }

    //========================================
    //  Set/Get methods
    //========================================
    /**
     * Sets X variable to the specified {@link ProductVar}.
     *
     * @param x    An instance of {@link ProductVar}.
     */
    public void setXVar(ProductVar x) {
        xVar = x;
    }

    /**
     * Sets Y variable to the specified {@link ProductVar}.
     *
     * @param y    An instance of {@link ProductVar}.
     */
    public void setYVar(ProductVar y) {
        yVar = y;
    }

    /**
     * Gets X variable.
     *
     * @return    An instance of {@link ProductVar}.
     */
    public ProductVar getXVar() {
        return xVar;
    }

    /**
     * Gets Y variable.
     *
     * @return    An instance of {@link ProductVar}.
     */
    public ProductVar getYVar() {
        return yVar;
    }

    //========================================
    //   Methods
    //========================================


}
