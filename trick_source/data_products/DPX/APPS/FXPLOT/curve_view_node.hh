
#ifndef CURVE_VIEW_NODE_HH
#define CURVE_VIEW_NODE_HH

#include "DPC/DPC_curve.hh"

extern "C"{
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include "XY.h"
}

/**
 * This class represents a viewable curve.
 */
class CurveViewNode {
    public:
    /**
     * CurveViewNode constructor.
     * @param Curve reference to the abstract curve, from which the
     *        viewable curve is to be generated.
     */
    CurveViewNode( Widget        Toplevel,
                   XYCurve      *fermi_curve,
                   DPC_curve    *Curve);

    /**
     * Destructor.
     */
    ~CurveViewNode();

    Widget      toplevel;       /**< Top level widget. */
    DPC_curve  *dpc_curve;      /**< The abstract curve. */
    XYCurve    *fermi_curve;    /**< The fermi curve. */
    const char *data_src_label; /**< Where we got our data from. */

private:
    static int instance_count;
};
#endif
