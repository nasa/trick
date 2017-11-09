
#ifndef DPC_CURVE_HH
#define DPC_CURVE_HH

#include <iostream>
#include <stdexcept>
#include "DPM/DPM_var.hh"
#include "DPV/DPV_view.hh"

/**
 * This class provides all of the data necessary to represent a curve of a plot.
 * @author John M. Penn
 */
class DPC_curve {

public:

    /**
     * CONSTRUCTOR.
     */
    DPC_curve() {
        x_var = NULL;
        y_var = NULL;
        view_data = NULL;
    }

    /**
     * DESTRUCTOR.
     */
    virtual ~DPC_curve(){}

    /**
     * Return the name of the X variable.
     */
    const char *getXVarName() {
        return( x_var->getName() );
    };

    /**
     * Return the name of the X variable.
     */
    const char *getXVarShortName() {
        return( x_var->getShortName() );
    };

    /**
     * Return the name of the Y variable.
     */
    const char *getYVarName() {
        return( y_var->getName() );
    }

    /**
     * Return the name of the Y variable.
     */
    const char *getYVarShortName() {
        return( y_var->getShortName() );
    };

    /**
     * Return the Units of the X variable.
     */
    virtual const char *getXUnits() =0;

    /**
     * Return the Units of the Y variable.
     */
    virtual const char *getYUnits() =0;

    /**
     * Return the RUN directory path.
     */
    virtual const char *getDataSrcLbl() {
        return( "None");
    }

    /**
     * Return the value of the attribute associated with the
     * given key for the X variable.
     */
    const char *getXAttribute(const char *key) {
        return( x_var->AttributeValue(key) );
    }

    /**
     * Return the value of the attribute associated with the
     * given key for the Y variable.
     */
    const char *getYAttribute(const char *key) {
        return( y_var->AttributeValue(key) );
    }

    /**
     * Get the next (X, Y) pair for the curve.
     * @return 1 if data was returned in x_value and y_value, 0 if there is no more data.
     */
    virtual int getXY(double *, double *) {
        return 0;
    }

    /**
     * Reset the source DataStreams for this curve to their beginnings, so
     * getXY() will get points from the beginning.
     */
    void begin() {}

    /**
     * Tell the view to create an external representation of this DPC_curve object.
     * NOTE: This member function is ONLY meant to be called by the DPC_plot
     * class, or subclasses thereof.
     */
    DPV_pointer render( DPV_view *view,
                        DPV_pointer parent_view_data ) {
        begin();
        if ( view != NULL ) {
            view_data = view->render_curve( parent_view_data, this);
        } else {
            view_data = NULL;
        }
        return (view_data);
    }

    /**
     * Tell the view to make this DPC_curve object readily apparent.
     */
    void emphasize( DPV_view *view, DPV_message msg );

protected:
    DPM_var* x_var;
    DPM_var* y_var;

private:
    DPV_pointer view_data;

};
#endif
