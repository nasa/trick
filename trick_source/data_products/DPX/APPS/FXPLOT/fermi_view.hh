
#ifndef FERMI_VIEW_HH
#define FERMI_VIEW_HH

#include "DPV/DPV_view.hh"
#include <Xm/Xm.h>
#include <X11/Xlib.h>

#include "curve_view_node.hh"
#include "plot_view_node.hh"
#include "table_view_node.hh"
#include "page_view_node.hh"
#include "product_view_node.hh"

/**
 * @brief An X-Windows based view.
 * @author John M. Penn
 */
class fermi_view: public DPV_view {

public:

  static const DPV_message  SHOW_WINDOW           = 0;
  static const DPV_message  RAISE_WINDOW          = 1;
  static const DPV_message  DATA_UPDATE           = 2;

  /**
   * Constructor.
   */
  fermi_view() {};

  /**
   * fermi_view destructor.
   * This is responsible for deleting the product nodes which
   * in turn deletes subordinate nodes.
   */
  ~fermi_view();

  fermi_view(Widget toplevel);

  DPV_pointer render_product( DPC_product* product);
  DPV_pointer render_page( DPV_pointer parent_data, DPC_page* page);
  DPV_pointer render_table( DPV_pointer parent_data, DPC_table* table);
  DPV_pointer render_plot( DPV_pointer parent_data, DPC_plot* plot);
  DPV_pointer render_curve( DPV_pointer parent_data, DPC_curve* curve);

  // FIXME: This is a hack
  DPV_pointer update_plot( DPV_pointer parent_data, DPC_plot* plot);

//  void finalize_product_view( DPV_pointer product_view );

  void finalize_page_view( DPV_pointer page_view );
  void finalize_table_view( DPV_pointer table_view );
  void finalize_plot_view( DPV_pointer plot_view );

  void notify_product( DPV_pointer product_view, DPV_message msg );
  void notify_page( DPV_pointer page_view, DPV_message msg );
  void notify_table( DPV_pointer table_view, DPV_message msg );
  void notify_plot( DPV_pointer plot_view, DPV_message msg );
  void notify_curve( DPV_pointer curve_view, DPV_message msg );

private:
  // FIXME: This is a hack
  void update_finalize_plot_view( DPV_pointer plot_view );
  // FIXME: This is a hack
  int update_curve( DPV_pointer parent_data, DPC_curve* curve);

  Widget           toplevel;
  Display         *display;
  Screen          *screen;
  Colormap         colormap;
  vector <ProductViewNode *> product_node_list;
  int xpos;
  int ypos;
  int width;
  int height;

};

#endif
