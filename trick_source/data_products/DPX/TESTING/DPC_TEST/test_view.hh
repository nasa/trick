#include "DPV/DPV_view.hh"
#include "DPC/DPC_product.hh"

class test_view: public DPV_view {

public:

  /**
   * Constructor.
   */

  test_view() { cout << "TEST_PEER CONSTRUCTOR" << endl; };

  DPV_pointer create_product_view( DPC_product* product);

  DPV_pointer create_page_view( DPV_pointer parent_data,
    				 DPC_page* page);

  DPV_pointer create_plot_view( DPV_pointer parent_data,
				 DPC_plot* plot);

  DPV_pointer create_table_view( DPV_pointer parent_data,
				 DPC_table* table);

  DPV_pointer create_curve_view( DPV_pointer parent_data,
				  DPC_curve* curve);

  void finalize_product_view( DPV_pointer product_view );

  void finalize_page_view( DPV_pointer page_view );

  void finalize_plot_view( DPV_pointer plot_view );

};
