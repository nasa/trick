
#ifndef GP_VIEW_CURVE_NODE_HH
#define GP_VIEW_CURVE_NODE_HH

#include "DPC/DPC_curve.hh"
#include "DPV/UTILS/DPV_textbuffer.hh"

extern "C"{
#include <X11/Intrinsic.h>
}

/**
 * This class represents a viewable curve.
 */
class GPViewCurveNode {
    public:
    /**
     * GPViewCurveNode constructor.
     * @param Curve reference to the abstract curve, from which the
     *        viewable curve is to be generated.
     */
    GPViewCurveNode( Widget          Toplevel,
                     DPV_TextBuffer *Textbuf,
                     DPC_curve      *Curve,
                     int             Curve_number
                   );

    /**
     * Destructor.
     */
    ~GPViewCurveNode();

    Widget         toplevel;       /**< Top level widget. */
    DPC_curve      *dpc_curve;     /**< The abstract curve. */
    DPV_TextBuffer *textbuf;
    std::string    data_src_label; /**< Where we got our data from. */

private:
    static int instance_count;
};
#endif
