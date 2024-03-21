/*************************************************************************
PURPOSE: ( Water Clock Numeric Model )
**************************************************************************/

#ifndef WATRERCLOCK_NUMERIC_H
#define WATERCLOCK_NUMERIC_H

#include "waterclock.h"

#ifdef __cplusplus
extern "C" {
#endif
int waterclock_integ(WATERCLOCK*);
int waterclock_deriv(WATERCLOCK*);
double waterclock_tick_change(WATERCLOCK*);
double waterclock_overflow_timer(WATERCLOCK*);
void waterclock_update_water_level(WATERCLOCK* WC);
#ifdef __cplusplus
}
#endif
#endif
