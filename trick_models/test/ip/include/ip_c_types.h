/* TRICK HEADER
PURPOSE:
    (Test all possible C data types recognized by the Interface Code
     Generator and the executive input processor.)

REFERENCE:
    (((Bailey, Robert W.)
      (User's Guide and Operational Procedures Volume ...
       of the Trick Simulation Environment) (MDSS-HD TM-6.24.26-04)
      (McDonnell Douglas Space Systems - Houston Division) (March 1993) (--)))

ASSUMPTIONS AND LIMITATIONS:
    ((Unlimited software array dimensions and sizing)
     (Unlimited software dynamically allocated array dimensions and sizing)
     (Excessively large structures may overrun hardware memory)
     (ICG ignores all compilier directives except '#define')
     (ICG ignores all typedef declarations other than 'typedef struct'
      and 'typedef enum'))

PROGRAMMERS:
    (((Robert W. Bailey) (LinCom) (10/1/90) (Trick-CR-00000) (Initial Release)))
*/

/*
 * $Log: ip_c_types.h,v $
 * Revision 5.1  2004-08-05 13:06:55-05  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:49:19  lin
 * Bump version number for 04
 *
 * Revision 1.3  2003/05/29 18:57:16  vetter
 * Add Bitfield Test To IP Test
 *
 * Revision 1.2  2002/10/07 15:16:26  lin
 * Add rcs version info to all trick_models files
 *
 */
 
#ifndef _IP_C_TYPES
#define _IP_C_TYPES

#define TWO 2         /* THE Value 2 */

typedef struct {
   unsigned int bits_a : 5  ;   /* -- Test bits  5  */
   unsigned int        : 2  ;   /* -- Test bits  7  */
   unsigned int bits_b : 9  ;   /* -- Test bits  16 */
   unsigned int bits_c : 16 ;   /* -- Test bits  32 */
   double       doub_x      ;   /* -- Miscellaneous double */
   unsigned int bits_d : 7  ;   /* -- Test bits */ 
} BITS ;
                                                                                
typedef struct {    /* FUNDAMENTAL 'C' DATA TYPES ----------------------------*/

char            c ; /* -- Single character */
unsigned char  uc ; /* -- Single unsigned character */
char *         cp ; /* -- Single string */
short           s ; /* -- Single short integer */
unsigned short us ; /* -- Single unsigned short integer */
int             i ; /* -- Integer */
unsigned int   ui ; /* -- Single unsigned integer */
long            l ; /* -- Single long integer */
unsigned long  ul ; /* -- Single unsigned long integer */
float           f ; /* -- Single precision floating point value*/
double          d ; /* -- Double precision floating point value */

} C_TYPES ; /*----------------------------------------------------------------*/

typedef struct {    /* ARRAYS ------------------------------------------------*/

char            ca[2][2][2] ; /* -- characters (strings) */
unsigned char  uca[2][2][2] ; /* -- unsigned character s*/
short           sa[2][2][2] ; /* -- short integers */
unsigned short usa[2][2][2] ; /* -- unsigned short integers */
int             ia[2][2][2] ; /* -- integers */
unsigned int   uia[2][2][2] ; /* -- unsigned integers */
long            la[2][2][2] ; /* -- long integers */
unsigned long  ula[2][2][2] ; /* -- unsigned long integers */
float           fa[2][2][2] ; /* -- single precision floating point */
double          da[2][2][2] ; /* -- double precision floating point */

} C_ARR_TYPES ; /*------------------------------------------------------------*/

typedef struct {    /* POINTERS -- UNCONSTRAINED ARRAYS ----------------------*/

  char           **** cpp ; /* -- strings */
  unsigned char  *** ucpp ; /* -- unsigned characters */
  short          ***  spp ; /* -- short integers */
  unsigned short *** uspp ; /* -- unsigned short integers */
  int            ***  ipp ; /* -- integers */
  unsigned int   *** uipp ; /* -- unsigned integers */
  long           ***  lpp ; /* -- long integers */
  unsigned long  *** ulpp ; /* -- unsigned long integers */
  float          ***  fpp ; /* -- single precision floating point */
  double         ***  dpp ; /* -- double precision floating point */

} C_PTR_TYPES ; /*------------------------------------------------------------*/

typedef struct {    /* MIXED POINTERS AND ARRAYS -----------------------------*/

    char           *** cpa[TWO] ; /* -- characters (strings) */
    unsigned char  ** ucpa[TWO] ; /* -- unsigned characters */
    short          **  spa[TWO] ; /* -- short integers */
    unsigned short ** uspa[TWO] ; /* -- unsigned short integers */
    int            **  ipa[TWO] ; /* -- integers */
    unsigned int   ** uipa[TWO] ; /* -- unsigned integers */
    long           **  lpa[TWO] ; /* -- long integers */
    unsigned long  ** ulpa[TWO] ; /* -- unsigned long integers */
    float          **  fpa[TWO] ; /* -- single precision floating points */
    double         **  dpa[TWO] ; /* -- double precision floating points */

} C_MIX_TYPES ; /*------------------------------------------------------------*/

#endif
