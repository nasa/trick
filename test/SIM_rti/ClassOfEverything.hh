/**
@file

@verbatim
PURPOSE:
    (Test input processor)
LIBRARY DEPENDENCY:
    ()
@endverbatim
*******************************************************************************/

#ifndef BALL_TEST_HH
#define BALL_TEST_HH

// Model include files.
#include "trick/mm_macros.hh"
#define NUM 8

typedef enum {
    FIRST,
    SECOND,
    THIRD,
    _FORTH,
    FIFTH,
    SIXTH,
    SEVENTH
} MY_ENUM ;

class ClassOfEverything {

  friend class InputProcessor ;
  friend void init_attrClassOfEverything() ;

  public:
   // Default constructor and destructor.
   ClassOfEverything() {};
   ~ClassOfEverything() {};

   /* maybe someday we'll be able to do something like this. */
   double d_test ;          /* -- blah */

   double d ;          /* kg blah */
   double da[3] ;      /* kg blah */
   double * dp ;       /* kg blah */
   double daa[2][3] ;  /* kg blah */
   double * dap[4] ;   /* kg blah */
   double ** dpp ;     /* kg blah */
   double daaa[2][3][4] ; /* kg blah */
   double daaaa[2][3][4][5] ; /* kg blah */

   float f ;          /* kg blah */
   float fa[3] ;      /* kg blah */
   float * fp ;       /* kg blah */
   float faa[2][3] ;  /* kg blah */
   float * fap[4] ;   /* kg blah */
   float ** fpp ;     /* kg blah */

   float  f_rad ; /* rad float test value */
   double d_deg ; /* degree blah */

   char c ;            /* -- blah */
   char ca[20] ;       /* -- blah */
   char * cp ;         /* -- blah */
   char caa[3][16] ;   /* -- blah */
   char * cap[4] ;     /* -- blah */
   char **cpp;

   unsigned char uc ;            /* -- blah */
   unsigned char uca[20] ;       /* -- blah */
   unsigned char * ucp ;         /* -- blah */
   unsigned char ucaa[2][3] ;    /* -- blah */
   unsigned char * ucap[4] ;     /* -- blah */
   unsigned char **ucpp;

   short s ;            /* -- blah */
   short sa[20] ;       /* -- blah */
   short * sp ;         /* -- blah */
   short saa[2][3] ;    /* -- blah */
   short * sap[4] ;     /* -- blah */
   short **spp;

   unsigned short us ;            /* -- blah */
   unsigned short usa[20] ;       /* -- blah */
   unsigned short * usp ;         /* -- blah */
   unsigned short usaa[2][3] ;    /* -- blah */
   unsigned short * usap[4] ;     /* -- blah */
   unsigned short **uspp;

   MY_ENUM e ;
   MY_ENUM ea[4] ;
   MY_ENUM * ep ;
   MY_ENUM eaa[2][3] ;
   MY_ENUM * eap[4] ;
   MY_ENUM ** epp ;

   int i ;          /* -- blah */
   int ia[3] ;      /* -- blah */
   int * ip ;       /* -- blah */
   int iaa[2][3] ;  /* -- blah */
   int * iap[NUM] ; /* -- blah */
   int ** ipp ;     /* -- blah */

   unsigned int ui ;            /* -- blah */
   unsigned int uia[20] ;       /* -- blah */
   unsigned int * uip ;         /* -- blah */
   unsigned int uiaa[2][3] ;    /* -- blah */
   unsigned int * uiap[4] ;     /* -- blah */
   unsigned int **uipp;

   long l ;            /* -- blah */
   long la[20] ;       /* -- blah */
   long * lp ;         /* -- blah */
   long laa[2][3] ;    /* -- blah */
   long * lap[4] ;     /* -- blah */
   long **lpp;

   unsigned long ul ;            /* -- blah */
   unsigned long ula[20] ;       /* -- blah */
   unsigned long * ulp ;         /* -- blah */
   unsigned long ulaa[2][3] ;    /* -- blah */
   unsigned long * ulap[4] ;     /* -- blah */
   unsigned long **ulpp;

   long long ll ;            /* -- blah */
   long long lla[20] ;       /* -- blah */
   long long * llp ;         /* -- blah */
   long long llaa[2][3] ;    /* -- blah */
   long long * llap[4] ;     /* -- blah */
   long long **llpp;

   unsigned long long ull ;            /* -- blah */
   unsigned long long ulla[20] ;       /* -- blah */
   unsigned long long * ullp ;         /* -- blah */
   unsigned long long ullaa[2][3] ;    /* -- blah */
   unsigned long long * ullap[4] ;     /* -- blah */
   unsigned long long **ullpp;

   bool b ;            /* -- blah */
   bool ba[20] ;       /* -- blah */
   bool * bp ;         /* -- blah */
   bool baa[2][3] ;    /* -- blah */
   bool * bap[4] ;     /* -- blah */
   bool **bpp;

   char cbit_0 : 2 ; /**< -- blah */
   char cbit_1 : 3 ; /**< -- blah */
   char cbit_2 : 3 ; /**< -- blah */

   unsigned char ucbit_0 : 2 ; /**< -- blah */
   unsigned char ucbit_1 : 3 ; /**< -- blah */
   unsigned char ucbit_2 : 3 ; /**< -- blah */

   char cpad[2] ;

   short sbit_0 : 4 ; /**< -- blah */
   short sbit_1 : 5 ; /**< -- blah */
   short sbit_2 : 7 ; /**< -- blah */

   unsigned short usbit_0 : 4 ; /**< -- blah */
   unsigned short usbit_1 : 5 ; /**< -- blah */
   unsigned short usbit_2 : 7 ; /**< -- blah */

   int bit_0 : 4 ; /**< -- blah */
   int bit_1 : 5 ; /**< -- blah */
   int bit_2 : 6 ; /**< -- blah */
   int pad : 17 ; /**< -- blah */

   unsigned int ubit_0 : 4 ; /**< -- blah */
   unsigned int ubit_1 : 5 ; /**< -- blah */
   unsigned int ubit_2 : 6 ; /**< -- blah */
   unsigned int upad : 17 ; /**< -- blah */

   long lbit_0 : 4 ; /**< -- blah */
   long lbit_1 : 5 ; /**< -- blah */
   long lbit_2 : 6 ; /**< -- blah */
   long lpad : 17 ; /**< -- blah */

   unsigned long ulbit_0 : 4 ; /**< -- blah */
   unsigned long ulbit_1 : 5 ; /**< -- blah */
   unsigned long ulbit_2 : 6 ; /**< -- blah */
   unsigned long ulpad : 17 ; /**< -- blah */

   bool boolbit_0 : 1 ; /**< -- blah */
   bool boolbit_1 : 1 ; /**< -- blah */
   bool boolbit_2 : 1 ; /**< -- blah */

   private:
       ClassOfEverything (const ClassOfEverything &);
       ClassOfEverything & operator= (const ClassOfEverything &);

};

#endif /* _BALL_HH_ */

