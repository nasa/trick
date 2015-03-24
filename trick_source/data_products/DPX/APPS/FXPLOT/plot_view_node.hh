
#ifndef PLOT_VIEW_NODE_HH
#define PLOT_VIEW_NODE_HH

#include <vector>
#include "curve_view_node.hh"

#include "DPC/DPC_plot.hh"

extern "C"{
#include <X11/Intrinsic.h>
}

/**
 * This class represents an X-Windows based Plot for Trick Data Products.
 * @author John M. Penn
 */
class PlotViewNode {
public:
    DPC_plot   *plot;              /**< The source of this plot's data. */
    Widget      toplevel;          /**< This is the toplevel X-application
                                        Widget. */
    Widget      main_form_widget;  /**< This is the main form for the plot.
                                        It is the parent of the ctrls form
                                        widget and the xy_widget.*/
    Widget      ctrls_form_widget; /**< This is the form on which the
                                        toggle buttons and print button
                                        are placed. */
    Widget      toggle1;           /**< Toggle button Widget for toggling
                                        the plot's cross-hairs. */
    Widget      toggle2;           /**< Toggle button Widget for toggling
                                        the plot's values display. */
    Widget      toggle3;           /**< Toggle button Widget for toggling
                                        the plot's grid. */
    Widget      toggle4;           /**< description. */
    Widget      print_button;      /**< Print button Widget. */
    Widget      xy_widget;         /**< The XYWidget ("Fermi") plot
                                        Widget.*/
    XYCurve    *curves;            /**< Pointer to an array of XYCurve
                                        structures. Each XYCurve structure
                                        contains the data for one curve.
                                        This class populates these
                                        structures and then passes them
                                        to the XYWidget to generate the
                                        curves.*/
    int         n_curves;          /**< This is the number of XYCurve
                                        elements in the
                                        PlotViewNode::curves array. */
    int         selected;          /**< description. */
    int         current;           /**< FIXME This is a hack. */
    int         needs_refresh ;    /**< FIXME This is a hack. */
    std::vector <CurveViewNode *> curve_node_list; /**< This is the list
                                                        of curve nodes for
                                                        for this plot. */
    /**
     * PlotViewNode constructor.
     * @param - Toplevel - the toplevel X-application Widget.
     * @param - Parent_form - the form Widget on which this plot is placed.
     * @param - Plot - the DPC_plot object from which this viewable plot
     *          gets its data.
     * @param - Plot_number - the plot number assigned to this plot.
     */
    PlotViewNode( Widget Toplevel, Widget Parent_form, DPC_plot* Plot);

    /**
     * PlotViewNode destructor.
     */
    ~PlotViewNode();

    /**
     *  This function tidies up some things in this PlotViewNode AFTER the child
     *  CurveViewNodes have been created.
     */
    void finalize();

    /**
     * Return a pointer to the array of curves contained within the plot.
     */
    XYCurve *getXYCurveArray() {
        return( curves);
    }

private:
    static int instance_count;
};
#endif
