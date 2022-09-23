
#ifndef DPC_PAGE_HH
#define DPC_PAGE_HH

#include "DPM/DPM_time_constraints.hh"
#include "DPC/DPC_datastream_supplier.hh"
#include "DPV/DPV_view.hh"

#include "DPM/DPM_page.hh"
#include "DPM/DPM_run.hh"
#include "DPC/DPC_standard_plot.hh"
#include "DPC/DPC_delta_plot.hh"

/**
 * DPC_page is a container for DPC_plot objects.
 * @author John M. Penn
 */
class DPC_page {

public:

    /**
     * CONSTRUCTOR.
     */
    DPC_page( DPC_datastream_supplier *DS_Supplier,
              std::vector <DPM_run *> *RunListp,
              DPM_page *PageSpec,
              DPM_time_constraints *ParentTimeConstraints )  ;

    /**
     * DESTRUCTOR.
     */
    ~DPC_page();

    /**
     * Tell the view to create an external representation of this DPC_page object.
     * NOTE: This member function is ONLY meant to be called by the DPC_product
     * class.
     */
    DPV_pointer render( DPV_view *view,
                        DPV_pointer parent_view_data );


    /**
     *
     */
    void notify( DPV_view *view, DPV_message msg);

// ============================
// DPV_VIEW INTERFACE FUNCTIONS
// ============================

    /**
     * Return the value of the attribute (if any) associated with the key.
     * If the attribute wasn't specified or is invalid, return NULL.
     */
    const char *getAttribute(const char *key);

    /**
     * Get the title of the age;
     */
    const char *getTitle();

private:

    DPM_page *page_spec;
    std::vector <DPC_plot *> plot_list;
    DPV_pointer view_data;
};
#endif
