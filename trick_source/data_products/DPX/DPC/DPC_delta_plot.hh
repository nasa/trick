
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
        ;

    /**
     * Constructor for Contrast session
     */
    DPC_delta_plot( DPC_datastream_supplier *DS_Supplier,
                    std::vector <DPM_run *> *RunListp,
                    DPM_relation *Relation,
                    DPM_time_constraints *parent_time_constraints )
        ;

    /**
     * Destructor.
     */
    ~DPC_delta_plot() {}

};

#endif
