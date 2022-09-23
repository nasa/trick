/**
@file

@verbatim
PURPOSE:
    (Test input processor)
LIBRARY DEPENDENCY:
    ((ClassOfEverything.o))
@endverbatim
*******************************************************************************/

#ifndef BALL_TEST_HH
#define BALL_TEST_HH

// System include files.
#include <string>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <map>
#include <list>
#include <vector>

#include <sys/types.h>

// Model include files.
#include "trick/mm_macros.hh"
#include "test_ip/include/NoICG.hh"
#include "exclude_me/include/exclude_me.hh"
#include "test_ip/include/Namespace_tests.hh"

/** @class Ball
    @brief ball in C++
 */
#define TEST 80

namespace enums_in_namespace_test {
typedef enum {
CITY , STATE
} Location ;

enum Occupations {
 Engineer , Doctor
} ;

}

typedef enum {
    FIRST,
    SECOND,
    THIRD,
    _FORTH,
    FIFTH,
    SIXTH,
    SEVENTH
} MY_ENUM ;

enum Fruit {
 Banana , Cherry
} ;

#ifndef SWIG
typedef const struct ConstStruct_ {
    double d[4] ;
} ConstStruct ;
#endif

class AlsoCannotCopy {
   friend class InputProcessor ;
   friend void init_attrClassOfEverything() ;

   public:
       int i ;
       AlsoCannotCopy () {} ;

   private:
       AlsoCannotCopy (const AlsoCannotCopy &);
       AlsoCannotCopy & operator= (const AlsoCannotCopy &);

};

class CannotCopy {

   friend class InputProcessor ;
   friend void init_attrClassOfEverything() ;

   public:
       int i ;
       CannotCopy () {} ;
       AlsoCannotCopy alsocannot[4] ;
       AlsoCannotCopy * alsocannot_p ;

   private:
       CannotCopy (const CannotCopy &);
       CannotCopy & operator= (const CannotCopy &);

};


class CanCopy {

   public:
       int  ii ;
       int  jj ;
       CanCopy () {} ;

       //CanCopy (const CanCopy &) ;
       //CanCopy & operator= (const CanCopy &) ;

};

template <class T, class U, class V>
class MyTemplate {
    public:
    T var1 ;
    U var2 ;
    V var3 ;
    typedef int template_int ;
} ;

template <class T>
class MyPrivTemplate {
    T var1 ;
} ;

template <class A, class B>
class TTT {
    public:

        TTT() {
            aa = 0 ;
            bb = 0 ;
            cc = NULL ;
        } ;
        A aa ;
        B bb ;
        TTT<A,B> * ttt ;
        typedef TTT<A,B> C ;
        C * cc ;
} ;

//typedef TTT< int , double > my_TTT_int_double ;

#define NUM 8

typedef union {
   int i ;         /* -- blah */
   double d ;      /* ** blah */
   long long l ;
} UnionTest ;

union UnionTest2 {
   int i ;         /* -- blah */
   double d ;      /* ** blah */
   long long l ;
} ;

typedef struct Foo {
   double meow ;
   double bark ;

} Bar , *Bar2 ;

typedef union DupName {
   double d ;
   int i ;

} DupName ;

typedef int Integer ;
typedef int AnotherInteger ;
typedef long long Myint64 ;

class ClassOfEverything {

  friend class InputProcessor ;
  friend void init_attrClassOfEverything() ;

  public:
   // Default constructor and destructor.
   ClassOfEverything() ;
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

   CanCopy can ;
   CanCopy can2 ;
   CanCopy cana[20] ;
   CanCopy * canp ;
   CanCopy canaa[2][3] ;
   CanCopy * canap[4] ;
   CanCopy ** canpp ;

   CannotCopy cannot ;
   CannotCopy cannot2 ;

   UnionTest ut ;
   union UnionTest2 ut2 ;

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

   std::string str ;

   // For now we have SWIG ignore templates with wstring
   std::vector< std::wstring > vwstr ;

   MyTemplate< double , int , short > my_template_var ;
   MyTemplate< bool , bool , bool > my_template_var_bool ;
   MyPrivTemplate< double > my_priv_template_var ;
   MyPrivTemplate< double * > my_priv_template_var2 ;
   TTT< int , double > TTT_var ;


   int ikg ;          /* kg unit-ed integer value testing */
   int ilbm ;         /* lb unit-ed integer value testing */

   double dkg ;       /* kg unit-ed double value testing */
   double dlbm ;      /* lb unit-ed double value testing */
   double dno_units ; /* -- unitless double value testing */
   double ds ;        /* s unit-ed double value testing */

   Integer iii ;
   AnotherInteger aiii ;
   Myint64 i64 ;

   Integer iiia[6] ;

   const int & cir ;
   int const & icr ;

   /* test comments */
   int i2 ; // (m)
   int i3 ; // (m) description
   int i4 ; /* (m)*/

   typedef int sizeType ;
   typedef double mydouble ;

   ClassOfEverything::sizeType st ;
   ClassOfEverything::mydouble md ;

   MyTemplate< double , int , short >::template_int my_template_var_int ;
   int invisible_int ;

   static double sdaa[3][3] ;

   static const double scd ;
   const static double csd ;
   static double const sdc ;

   // These need to be ignored by ICG
   // ICG is now compiler based.  Thse are ignored properly.
/*
   static const double scdi = 42.0 ;
   const static double csdi = 42.0 ;
   static double const sdci = 42.0 ;
*/

   typedef enum {
    ONE , TWO
   } Numbers ;

   enum Animals {
       Duck,
       Cow
   } ;

   Animals ani_1 ;
   Animals animal_function() const { return Duck ; } ;

   // other enum types
   enums_in_namespace_test::Location e_loc ;
   enums_in_namespace_test::Occupations e_occ ;
   Fruit e_fruit ;
   Numbers e_numbers ;

   double das[sizeof(double)] ;

   int8_t  i8t ;
   uint8_t  ui8t ;
   int16_t  i16t ;
   uint16_t  ui16t ;
   int32_t  i32t ;
   uint32_t  ui32t ;
   int64_t  i64t ;
   uint64_t  ui64t ;

   size_t sizet ;

   u_char u_c ;
   u_short u_s ;
   u_int u_i ;
   u_long u_l ;
   quad_t q ;
   u_quad_t uq ;

   std::map < std::string , int > msi ;
   std::list < std::string  > ls ;

   private:
       ClassOfEverything (const ClassOfEverything &);
       ClassOfEverything & operator= (const ClassOfEverything &);

};

#ifdef SWIG
%template(MapStringInt) std::map< std::string , int > ;
%template(ListString) std::list< std::string  >;
#endif

typedef struct test_struct {
    int i ; /* (m) I am in meters! */
    double d ; /* (m) I am in meters! */
} TEST_STRUCT ;

#ifdef SWIG
%struct_str(ClassOfEverything)
#endif

namespace my_ns {

class AA {
    public:
        std::string str ;
        BB * bbp ;
        BB ** bbpp ;
        double mass ; /* kg mass */
} ;

} ;

// Test.hh
class Test {
    public:
    Test *t;
    const char * foo(){ return "called foo" ; } ;
};

// test polymorphism in the input file.
class Abstract {
   public:
      Abstract( int in_id ) : id(in_id) {} ;
      virtual ~Abstract() {} ;
      int id ;
      virtual void speak() = 0 ;
} ;

class Cat : public Abstract {
   TRICK_MM_INTERFACE(Cat, Cat)
   public:
      Cat() : Abstract(1) {} ;
      virtual void speak() { std::cout << "meow" << std::endl ; }
} ;

class Dog : public Abstract {
   TRICK_MM_INTERFACE(Dog, Dog)
   public:
      Dog() : Abstract(2) {} ;
      virtual void speak() { std::cout << "bark" << std::endl ; }
} ;

#if 0
// This #if 0 block is in here to ensure that convert_swig does not generate code for excluded code
// Do not delete it!
class doNotIncludeMe {
    public:
        int ii ;
} ;
#endif

#endif /* _BALL_HH_ */

