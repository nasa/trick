
#ifndef DPC_PLOT_HH
#define DPC_PLOT_HH


#include "DPV/DPV_view.hh"
#include "DPM/DPM_relation.hh"
#include "DPC/DPC_curve.hh"

/**
 * DPC_plot represents a 2-D data plot.
 * @author John M. Penn
 */

class DPC_plot {

public:

    /**
     * Constructor
     */
    DPC_plot( DPM_relation *Relation ) ;

    /**
     * DESTRUCTOR.
     */
    ~DPC_plot();

    /**
     * Tell the view to create an external representation of this DPC_plot object.
     * NOTE: This member function is ONLY meant to be called by the DPC_page
     * class.
     */
    DPV_pointer render( DPV_view *view,
                        DPV_pointer parent_view_data ) ;

    /**
     *
     */
    void notify( DPV_view *view, DPV_message msg );

    /**
     *
     */
    int add_curve( DPC_curve *curve) ;

    /**
     * Return the number of curves that have been generated for this plot.
     */
    int getNumCurves() {
        return( (int)curve_list.size());
    }

    /**
     * Set the title of the plot.
     */
    int setTitle(const char* in_title) {
        return( relation->setTitle(in_title));
    }

// ============================
// DPV_VIEW INTERFACE FUNCTIONS
// ============================

    /**
     * Return the value of the attribute (if any) associated with the key.
     * If the attribute wasn't specified or is invalid, return NULL.
     */
    const char *getAttribute(const char *key) {
        return( relation->AttributeValue(key) );
    }

    /**
     * Return the title of the plot, which may be NULL.
     */
    const char *getTitle() {
        return( relation->getTitle());
    }

    /**
     * Return the X-axis label of the plot, which may be NULL.
     */
    const char *getXLabel() {
        return( relation->getXAxisLabel());
    }

    /**
     * Return the Y-axis label of the plot, which may be NULL.
     */
    const char *getYLabel() {
        return( relation->getYAxisLabel());
    }

    /**
     * Return the Y-axis format of the plot.
     */
    const char * getYFormat() {
        return (relation->getYAxisFormat());
    }

    // Plot range for -all- curves
    double getXMin();
    double setXMin(double value);

    double getXMax();
    double setXMax(double value);

    double getYMin();
    double setYMin(double value);

    double getYMax();
    double setYMax(double value);

private:
    DPM_relation *relation;
    std::vector <DPC_curve *> curve_list;
    DPV_pointer view_data;

    // Plot range
    double xMin;
    double xMax;
    double yMin;
    double yMax;
};

#endif
