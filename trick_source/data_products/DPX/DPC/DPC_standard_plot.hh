
#ifndef DPC_STANDARD_PLOT_HH
#define DPC_STANDARD_PLOT_HH

#include "DPC/DPC_datastream_supplier.hh"
#include "DPC/DPC_plot.hh"
#include "DPM/DPM_relation.hh"
#include "DPM/DPM_run.hh"
#include "DPM/DPM_time_constraints.hh"
#include <stdexcept>

/**
 * This class represents a data plot.
 * @author John M. Penn
 */
class DPC_standard_plot : public DPC_plot
{
public:
    /**
     * Constructor
     */
    DPC_standard_plot(DPC_datastream_supplier * DS_Supplier,
                      std::vector<DPM_run *> * RunListp,
                      DPM_relation * Relation,
                      DPM_time_constraints * parent_time_constraints);

    /**
     * Destructor.
     */
    ~DPC_standard_plot() {}
};

#endif
