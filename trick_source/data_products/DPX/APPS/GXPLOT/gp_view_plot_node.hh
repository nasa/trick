
#ifndef GP_VIEW_PLOT_NODE_HH
#define GP_VIEW_PLOT_NODE_HH

#include <vector>
#include "gp_view_curve_node.hh"

#include "DPC/DPC_plot.hh"
#include "DPV/UTILS/DPV_textbuffer.hh"

extern "C"{
#include <X11/Intrinsic.h>
}

/**
 * This class represents an X-Windows based Plot for Trick Data Products.
 * @author John M. Penn
 */
class GPViewPlotNode {
public:
    DPC_plot   *plot;              /**< The source of this plot's data. */
    DPV_TextBuffer *textbuf;
    Widget      toplevel;          /**< This is the toplevel X-application
                                        Widget. */
    int         selected;          /**< description. */
    int         current;           /**< FIXME This is a hack. */
    std::vector <GPViewCurveNode *> curve_node_list; /**< This is the list
                                                        of curve nodes for
                                                        for this plot. */
    /**
     * GPViewPlotNode constructor.
     * @param - Toplevel - the toplevel X-application Widget.
     * @param - textbuf - gnuplot command text.
     * @param - Plot - the DPC_plot object from which this viewable plot
     *          gets its data.
     */
    GPViewPlotNode( Widget         Toplevel,
                    DPV_TextBuffer *Textbuf ,
                    DPC_plot       *Plot,
                    int            Plot_number);

    /**
     * GPViewPlotNode destructor.
     */
    ~GPViewPlotNode();

    /**
     *  This function tidies up some things in this GPViewPlotNode AFTER the child
     *  CurveViewNodes have been created.
     */
    void finalize();

private:
    static int instance_count;
};
#endif
