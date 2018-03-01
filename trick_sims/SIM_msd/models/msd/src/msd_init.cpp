/************************TRICK HEADER*************************
PURPOSE: Set the initial data values of the MSD
PROGRAMMERS:
    (((Scott P. Fennell) (CACI International Inc.) (January 2018) (Trick Learning Project)))
*************************************************************/

/* Model Include files */
#include "../include/msd.hh"

/* default data job */
int msd_default_data(MSD &M) {
    M = MSD(
            1.0, /* m */
            2.0, /* k */
            0.5, /* b */
            5.0, /* F */
            0.0, /* v_0 */
            5.0  /* x_0 */

    );

    return 0;
}

/* initialization job */
int msd_init(MSD &M) {
    return 0;
}
