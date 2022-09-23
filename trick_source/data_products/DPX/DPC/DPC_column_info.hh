
#ifndef DPC_COLUMN_INFO_HH
#define DPC_COLUMN_INFO_HH
#include <string.h>
#include <iostream>
#include <udunits2.h>

class DPC_column_info {
public:
    DPC_column_info( DPM_var    * Var,
                     const char * Units_s,
                     int          DS_ix,
                     cv_converter * Conversion_fn) {
        var = Var;
        units = strdup(Units_s);
        ds_ix = DS_ix;
		time_conversion = Conversion_fn;
    }

    DPM_var    *var;
    const char *units;
    int        ds_ix ;
    cv_converter  *time_conversion;
};

#endif
