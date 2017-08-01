
#ifndef DPC_STD_CURVE_HH
#define DPC_STD_CURVE_HH

#include <udunits2.h>
#include "DPM/DPM_run.hh"
#include "DPM/DPM_curve.hh"
#include "DPC/DPC_curve.hh"
#include "DPC/DPC_datastream_supplier.hh"
#include <stdexcept>
#include <string>

/**
 * This class provides all of the data necessary to represent a curve of a plot.
 * @author John M. Penn
 */
class DPC_std_curve: public DPC_curve {

public:

    /**
     * Constructor.
     */
    DPC_std_curve(
                  DPM_curve*  Curve_spec,
                  DPM_run*    Run,
                  DPC_datastream_supplier *ds_supplier,
                  DPM_time_constraints* Time_constraints ) ;

    /**
     * Destructor.
     */
    ~DPC_std_curve();

    /**
     * Return the Units of the X variable.
     */
    const char *getXUnits();

    /**
     * Return the Units of the Y variable.
     */
    const char *getYUnits();

    /**
     * Return the RUN directory path.
     */
    const char *getDataSrcLbl();

    /**
     * Get the next (X, Y) pair for the curve.
     * @return 1 if data was returned in x_value and y_value, 0 if there is no more data.
     */
    int getXY(double *x_value, double *y_value);

    /**
     * Reset the source DataStreams for this curve to their beginnings, so
     * getXY() will get points from the beginning.
     */
    void begin();

private:

    DataStream *ds[2];
    char * x_actual_units;
    char * y_actual_units;
    char * data_src_label;
    cv_converter * time_conversion;
};
#endif
