/*************************************************************************
PURPOSE: ( Water Clock Numeric Model )
**************************************************************************/

#ifndef WATERCLOCK_NUMERIC_H
#define WATERCLOCK_NUMERIC_H

#include "waterclock.h"

#ifdef __cplusplus
extern "C" {
#endif
int waterclock_integ(WATERCLOCK* WC);
int waterclock_deriv(WATERCLOCK* WC);
double waterclock_tick_change(WATERCLOCK* WC);
double waterclock_overflow_timer(WATERCLOCK* WC);
void waterclock_update_water_level(WATERCLOCK* WC);
#ifdef __cplusplus
}
#endif
#endif
