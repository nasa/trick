#ifndef WCS_EXT_H
#define WCS_EXT_H
/*
PURPOSE:
( Functions to convert narrow (char) strings to wide (wchar_t) strings and
  vice-versa, as well as functions to determine the number of characters
  that would result, prior to actually performing a conversion.
)

PROGRAMMERS:
    (((John M. Penn) (L-3 Communications) (08/2006)))
*/

#ifdef __cplusplus
extern "C" {
#endif

#if __Lynx__
int wcs_to_ncs_len (void);
int ncs_to_wcs_len (void);
int wcs_to_ncs (void);
int ncs_to_wcs(void);
#else
#include <stddef.h>
#include <wchar.h>
size_t wcs_to_ncs_len (const wchar_t *wcs);
size_t ncs_to_wcs_len (const char *ncs);
size_t wcs_to_ncs (const wchar_t *wcs, char *ncs, size_t ncs_max_len );
size_t ncs_to_wcs(const char *ncs, wchar_t *wcs, size_t w_max_size );
#endif

#ifdef __cplusplus
}
#endif

#endif

