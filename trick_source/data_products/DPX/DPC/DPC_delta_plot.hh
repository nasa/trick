/*
 * $Id: DPC_delta_plot.hh 1843 2011-08-01 20:19:57Z penn $
 */

#ifndef DPC_DELTA_PLOT_HH
#define DPC_DELTA_PLOT_HH

#include <stdexcept>
#include "DPC/DPC_datastream_supplier.hh"
#include "DPM/DPM_relation.hh"
#include "DPM/DPM_run.hh"
#include "DPM/DPM_time_constraints.hh"
#include "DPC/DPC_plot.hh"

/**
 * This class represents a data plot.
 * @author John M. Penn
 * @version $Id: DPC_delta_plot.hh 1843 2011-08-01 20:19:57Z penn $
 */
class DPC_delta_plot: public DPC_plot {

public:

    /**
     * Constructor for Delta session
     */
    DPC_delta_plot( DPC_datastream_supplier *DS_Supplier,
                    DPM_run *Run1,
                    DPM_run *Run2,
                    DPM_relation *Relation,
                    DPM_time_constraints *parent_time_constraints )
        throw (std::invalid_argument);

    /**
     * Constructor for Contrast session
     */
    DPC_delta_plot( DPC_datastream_supplier *DS_Supplier,
                    std::vector <DPM_run *> *RunListp,
                    DPM_relation *Relation,
                    DPM_time_constraints *parent_time_constraints )
        throw (std::invalid_argument);

    /**
     * Destructor.
     */
    ~DPC_delta_plot() {}

};

#endif
