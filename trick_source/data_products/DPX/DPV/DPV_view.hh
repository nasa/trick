
#ifndef DPC_VIEW_HH
#define DPC_VIEW_HH
#include <stddef.h> // for NULL

typedef void *DPV_pointer;
typedef int DPV_message;

class DPC_product;
class DPC_page;
class DPC_plot;
class DPC_table;
class DPC_curve;

/**
 * This is the base-class for classes which provide a viewable representation
 * of a Trick data product. This class is meant to be subclassed and it's
 * member functions are meant to be overridden.
 * @author John M. Penn
 */
class DPV_view {

public:

  virtual ~DPV_view(){}

  /**
   * Override this member function to render a representation
   * of an DPC_product object.
   * @param product is a pointer to the instance of DPC_product, for which a
   *        representation is to be rendered.
   */
  virtual  DPV_pointer render_product( DPC_product* ) {
    return ( (DPV_pointer)NULL);
  };

  /**
   * Override this member function to render a representation
   * of an DPC_page object.
   * @param parent_data is an opaque pointer reference to the view of the
   *        product, which is the parent of the page.
   * @param page is a pointer to the instance of DPC_page, for which a
   *        viewable representation is to be rendered.
   */
  virtual  DPV_pointer render_page( DPV_pointer, DPC_page* ) {
    return ( (DPV_pointer)NULL);
  };

  /**
   * Override this member function to render a representation
   * of an DPC_plot object.
   * @param parent_data is an opaque pointer reference to the view of the
   *        page, which is the parent of the plot.
   * @param plot is a pointer to the instance of DPC_plot, for which a
   *        viewable representation is to be rendered.
   */
  virtual DPV_pointer render_plot( DPV_pointer, DPC_plot* ) {
    return ( (DPV_pointer)NULL);
  };

  /**
   * Override this member function to render a representation
   * of an DPC_table object.
   * @param parent_data is an opaque pointer reference to the view of the
   *        page, which is the parent of the table
   * @param plot is a pointer to the instance of DPC_table, for which a
   *        viewable representation is to be rendered.
   */
  virtual DPV_pointer render_table( DPV_pointer, DPC_table* ) {
    return ( (DPV_pointer)NULL);
  };

  /**
   * Override this member function to render a representation
   * of an DPC_curve object.
   * @param parent_data is an opaque pointer reference to the view of the
   *        plot, which is the parent of the curve.
   * @param curve is a pointer to the instance of DPC_curve, for which
   *        a viewable representation is to be rendered.
   */
  virtual DPV_pointer render_curve( DPV_pointer, DPC_curve* ) {
    return ( (DPV_pointer)NULL);
  };

  /**
   * Override this member function to perform product view processing after all of
   * the subordinate page views have been rendered (by render_page).
   */
  virtual void finalize_product_view( DPV_pointer ) {};

  /**
   * Override this member function to perform page view processing after all of
   * the subordinate plot views have been rendered (by render_plot_view).
   */
  virtual void finalize_page_view( DPV_pointer ) {};

  /**
   */
  virtual void finalize_table_view( DPV_pointer ) {};

  /**
   * Override this member function to perform plot view processing after all of
   * the subordinate curve views have been rendered (by render_curve_view).
   */
  virtual void finalize_plot_view( DPV_pointer ) {};

  virtual void notify_product( DPV_pointer, DPV_message ) {};
  virtual void notify_page( DPV_pointer, DPV_message ) {};
  virtual void notify_table( DPV_pointer, DPV_message ) {};
  virtual void notify_plot( DPV_pointer, DPV_message ) {};
  virtual void notify_curve( DPV_pointer, DPV_message ) {};

};
#endif
