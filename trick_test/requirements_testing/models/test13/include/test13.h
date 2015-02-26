#ifndef _test13_h_
#define _test13_h_

#include <wchar.h>

typedef struct { /*---- FUNDAMENTAL 'C' DATA TYPES ----------------------------*/
  char          c ; /* -- character */
  unsigned char       uc ; /* -- unsigned character */
  char *              cp ; /* -- string */
  short                s ; /* -- short integer */
  unsigned short      us ; /* -- unsigned short integer */
  int                  i ; /* -- integer */
  unsigned int        ui ; /* -- unsigned integer */
  long                 l ; /* -- long integer */
  unsigned long       ul ; /* -- unsigned long integer */
  float                f ; /* -- single precision floating point value */
  double               d ; /* -- double precision floating point value */
  long long           ll ; /* -- long long */
  unsigned long long ull ; /* -- unsigned long long */
  wchar_t             wc ; /* -- wide character */
  wchar_t *           ws ; /* -- wide string */
} C_TYPES ; /*----------------------------------------------------------------*/

typedef struct { /*---- ARRAYS ------------------------------------------------*/
  char                 ca[2][2][2] ; /* -- characters */
  unsigned char       uca[2][2][2] ; /* -- unsigned characters */
  short                sa[2][2][2] ; /* -- short integers */
  unsigned short      usa[2][2][2] ; /* -- unsigned short integers */
  int                  ia[2][2][2] ; /* -- integers */
  unsigned int        uia[2][2][2] ; /* -- unsigned integers */
  long                 la[2][2][2] ; /* -- long integers */
  unsigned long       ula[2][2][2] ; /* -- unsigned long integers */
  float                fa[2][2][2] ; /* -- single precision floating point */
  double               da[2][2][2] ; /* -- double precision floating point */
  long long           lla[2][2][2] ; /* -- long longs */
  unsigned long long ulla[2][2][2] ; /* -- unsigned long longs */
  wchar_t             wca[2][2][2] ; /* -- wide characters */
} C_ARR_TYPES ; /*------------------------------------------------------------*/

typedef struct { /*---- POINTERS -- UNCONSTRAINED ARRAYS ----------------------*/
  char               ****  cpp ; /* -- strings */
  unsigned char      ***  ucpp ; /* -- unsigned characters */
  short              ***   spp ; /* -- short integers */
  unsigned short     ***  uspp ; /* -- unsigned short integers */
  int                ***   ipp ; /* -- integers */
  unsigned int       ***  uipp ; /* -- unsigned integers */
  long               ***   lpp ; /* -- long integers */
  unsigned long      ***  ulpp ; /* -- unsigned long integers */
  float              ***   fpp ; /* -- single precision floating point */
  double             ***   dpp ; /* -- double precision floating point */
  long long          ***  llpp ; /* -- long longs */
  unsigned long long *** ullpp ; /* -- unsigned long longs */
  wchar_t            **** wcpp ; /* -- wide strings */
} C_PTR_TYPES ; /*------------------------------------------------------------*/

typedef struct { /*---- MIXED POINTERS AND ARRAYS -----------------------------*/
  char               ***  cpa[2] ; /* -- strings */
  unsigned char      **  ucpa[2] ; /* -- unsigned characters */
  short              **   spa[2] ; /* -- short integers */
  unsigned short     **  uspa[2] ; /* -- unsigned short integers */
  int                **   ipa[2] ; /* -- integers */
  unsigned int       **  uipa[2] ; /* -- unsigned integers */
  long               **   lpa[2] ; /* -- long integers */
  unsigned long      **  ulpa[2] ; /* -- unsigned long integers */
  float              **   fpa[2] ; /* -- single precision floating points */
  double             **   dpa[2] ; /* -- double precision floating points */
  long long          **  llpa[2] ; /* -- long longs */
  unsigned long long ** ullpa[2] ; /* -- unsigned long longs */
  wchar_t            *** wcpa[2] ; /* -- wide strings */
} C_MIX_TYPES ; /*------------------------------------------------------------*/

typedef struct { /*---- COMPOSITE DATA STRUCTURES ----------------------------*/
C_TYPES      c_types ;             /* -- Standard C types */
C_ARR_TYPES  c_array_types ;       /* -- Arrayed C types */
C_PTR_TYPES  c_pointer_types ;     /* -- Pointered C types */
C_MIX_TYPES  c_mixed_types ;       /* -- Mixed arrays and pointer types */
} TEST13 ; /*-----------------------------------------------------------------*/

#endif
