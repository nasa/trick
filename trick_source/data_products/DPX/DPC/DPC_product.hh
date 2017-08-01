
#ifndef DPC_PRODUCT_HH
#define DPC_PRODUCT_HH

#include "DPM/DPM_time_constraints.hh"
#include "DPC/DPC_datastream_supplier.hh"
#include "DPV/DPV_view.hh"

#include "DPM/DPM_session.hh"
#include "DPM/DPM_product.hh"
#include "DPC/DPC_page.hh"
#include "DPC/DPC_table.hh"

/**
 * DPC_product represents a collection of DPC_pages (each of which
 * may contain DPC_plots) and DPC_tables.
 * @author John M. Penn
 */
class DPC_product {

public:

    /**
     * CONSTRUCTOR.
     */
    DPC_product( DPM_session *Session, const char *ProductFileName)  ;

    /**
     * DESTRUCTOR.
     */
    ~DPC_product();

    /**
     * Tell the view to create an external representation of this DPC_product object.
     * @param view is a pointer to an DPV_view object, which provides the capability
     *        to generate an viewable representation of an DPC_product.
     */
    DPV_pointer render( DPV_view *view );

    /**
     *
     */
    void notify( DPV_view *view, DPV_message msg );

    /**
     * Returns the number of DPC_page objects contained by this DPC_product.
     */
    int NumberOfPages();

    /**
     * Return a pointer to the i'th DPC_page object contained in the DPC_product.
     * @param index Ranging from 0 to the number of pages minus one. If the index
     * is invalid, this function returns NULL;
     */
    DPC_page *getPage( unsigned int index);

    /**
     * Returns the number of DPC_table objects contained by this DPC_product.
     */
    int NumberOfTables();

    /**
     * Return a pointer to the i'th DPC_table object contained in the DPC_product.
     * @param index Ranging from 0 to the number of tables minus one. If the index
     * is invalid, this function returns NULL;
     */
    DPC_table *getTable( unsigned int index);

// ============================
// VIEW INTERFACE FUNCTIONS
// ============================

    /**
     * Get the title of the product;
     */
    const char *getTitle();
    /**
     * Return the value of the attribute (if any) associated with the key.
     * If the attribute wasn't specified or is invalid, return NULL.
     */
    const char *getAttribute(const char *key);



private:

    DPM_product *product_spec;
    DPC_datastream_supplier *datastream_supplier;
    std::vector <DPC_page *> page_list;
    std::vector <DPC_table *> table_list;
    DPV_pointer view_data;
};
#endif
