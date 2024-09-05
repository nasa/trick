// Credit: https://raw.githubusercontent.com/thatchristoph/vmd-cvs-github
//               /master/plugins/signalproc/src/sgsmooth.C
/* common definitions for the sgsmooth plugin */
#ifndef _SGSMOOTH_PLUGIN_H
#define _SGSMOOTH_PLUGIN_H

#ifdef __cplusplus
extern "C" 
{
#endif

extern double *calc_sgsmooth(const int ndat, double *input,
                             const int window, const int order);

extern double *calc_sgsderiv(const int ndat, double *input,
                             const int window, const int order, 
                             const double delta);

extern void sgs_error(const char *errmsg);
    

#ifdef __cplusplus
}
#endif

#endif
