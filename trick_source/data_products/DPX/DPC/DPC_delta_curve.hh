
#ifndef DPC_DELTA_CURVE_HH
#define DPC_DELTA_CURVE_HH

#include "DPC/DPC_curve.hh"
#include "DPC/DPC_datastream_supplier.hh"
#include "DPM/DPM_run.hh"
#include <stdexcept>
#include <string>
#include <udunits2.h>

/**
 * This class provides all of the data necessary to represent a curve of a plot.
 * @author John M. Penn
 */
class DPC_delta_curve: public DPC_curve {

public:

    /**
     * Constructor.
     */
    DPC_delta_curve(
                    DPM_curve* Curve_spec,
                    DPM_run* RUN1,
                    DPM_run* RUN2,
                    DPC_datastream_supplier *ds_supplier,
                    DPM_time_constraints* Time_constraints ) ;

    /**
     * Destructor.
     */
    ~DPC_delta_curve();

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
    DPM_var * delta_x_var;
    DPM_var * delta_y_var;
    char * x_actual_units;
    char * y_actual_units;
    char * run_dir1;
    char * run_dir2;
    char * data_src_label;
    cv_converter* time_conversion;
};
#endif
