/**
 * @if Er7UtilsUseGroups
 * @addtogroup Er7Utils
 * @{
 * @addtogroup Utils
 * @{
 * @endif
 */

/**
 * @file
 * Defines the class Ratio128.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_RATIO128_HH
#define ER7_UTILS_RATIO128_HH

// Local includes
#include "uint128.hh"

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// System includes
#include <iosfwd>
#include <stdint.h>
#include <string>


namespace er7_utils {


/**
 * Represents a rational number as a sign times the ratio of two unsigned
 * 128 bit integers.
 */
class Ratio128 {
ER7_UTILS_MAKE_SIM_INTERFACES(Ratio128)

private:
   // Data members

   /**
    * The sign of the rational.
    */
   int sign; //!< trick_units(--)

   /**
    * The numerator.
    */
   UInt128 num; //!< trick_units(--)

   /**
    * The denominator.
    */
   UInt128 den; //!< trick_units(--)

public:

   /**
    * Default constructor; default value is zero.
    */
   Ratio128 ()
   :
      sign (0),
      num (0ull),
      den (1ull)
   { }

   /**
    * Conversion constructor, unsigned long long to rational,
    * plus optional denominator and sign.
    */
   Ratio128 (
      unsigned long long num_in,
      unsigned long long den_in=1ull,
      int sign_in=1)
   :
      sign ((num_in == 0ull) ? 0 : ((sign_in < 0) ? -1 : 1)),
      num (num_in),
      den (den_in)
   {
      reduce ();
   }

   /**
    * Conversion constructor, unsigned long to rational,
    * plus optional denominator and sign.
    */
   Ratio128 (
      unsigned long num_in,
      unsigned long den_in=1ull,
      int sign_in=1)
   :
      sign ((num_in == 0ul) ? 0 : ((sign_in < 0) ? -1 : 1)),
      num (num_in),
      den (den_in)
   {
      reduce ();
   }

   /**
    * Conversion constructor, unsigned to rational, plus optional denominator
    * and sign.
    */
   Ratio128 (
      unsigned int num_in,
      unsigned int den_in=1u,
      int sign_in=1)
   :
      sign ((num_in == 0ull) ? 0 : ((sign_in < 0) ? -1 : 1)),
      num (num_in),
      den (den_in)
   {
      reduce ();
   }

   /**
    * Conversion constructor, long long to rational, plus optional denominator.
    */
   Ratio128 (
      long long num_in,
      long long den_in=1LL)
   :
      sign (0),
      num (0ull),
      den (1ull)
   {
      make_ratio (num_in, den_in);
   }

   /**
    * Conversion constructor, long to rational, plus optional denominator.
    */
   Ratio128 (
      long num_in,
      long den_in=1L)
   :
      sign (0),
      num (0ull),
      den (1ull)
   {
      make_ratio (num_in, den_in);
   }

   /**
    * Conversion constructor, int to rational, plus optional denominator.
    */
   Ratio128 (
      int num_in,
      int den_in=1LL)
   :
      sign (0),
      num (0ull),
      den (1ull)
   {
      make_ratio (num_in, den_in);
   }

   /**
    * Conversion constructor, double to rational.
    * @throw std::domain_error Input value cannot be represented exactly.
    */
   explicit Ratio128 (double dval);


   // Swap, as a member and as a function.
   void swap (Ratio128& other);

   friend void swap (Ratio128& a, Ratio128& b)
   {
      a.swap(b);
   }


   // Unary operators. Only + and - are defined.

   /**
    * Unary plus operator; returns copy.
    */
   friend Ratio128 operator+ (const Ratio128& a)
   {
      return a;
   }

   /**
    * Unary minus (negation) operator.
    */
   friend Ratio128 operator- (const Ratio128& a)
   {
      Ratio128 negated (a);
      negated.sign = -a.sign;
      return negated;
   }


   // Conversion operators.
   // FIXME (Future) Make these explicit upon transition to C++11.

   /**
    * Convert to double.
    */
   operator double () const;

   /**
    * Convert to std::string, output is "[-]<numerator>/<denominator>".
    */
   operator std::string () const;


   // Math functions.

   /**
    * Absolute value.
    */
   Ratio128 abs () const {
      Ratio128 result = *this;
      result.sign = (sign == 0) ? 0 : 1;
      return result;
   }

   /**
    * Multiplicative inverse (additive inverse is operator-).
    */
   Ratio128 inverse () const;

   /**
    * Round to half precision by shifting both the numerator and denominator
    * one bit to the right, adjusting the numerator to best maintain accuracy.
    */
   void round ();


   // Math operators.
   // These are implemented in two parts, the arithmetic assignment operators
   // and the binary arithmetic operators. The former are member functions;
   // the latter are friend functions implemented in terms of the
   // arithmetic assignment operators.
   // Note: There are no increment operators. This is intentional.

   /**
    * Addition.
    */
   Ratio128 & operator+= (const Ratio128 & other);

   /**
    * Subtraction.
    */
   Ratio128 & operator-= (const Ratio128 & other);

   /**
    * Multiplication.
    */
   Ratio128 & operator*= (const Ratio128 & other);

   /**
    * Division.
    */
   Ratio128 & operator/= (const Ratio128 & other)
   {
      *this *= other.inverse();
      return *this;
   }


   // Arithmetic operators.

   /**
    * Returns a+b.
    */
   friend Ratio128 operator+ (const Ratio128 & a, const Ratio128 & b)
   {
      Ratio128 result = a;
      result += b;
      return result;
   }

   /**
    * Returns a+b.
    */
   template<typename T>
   friend Ratio128 operator+ (T a, const Ratio128 & b)
   {
      Ratio128 result = Ratio128 (a);
      result += b;
      return result;
   }

   /**
    * Returns a+b.
    */
   template<typename T>
   friend Ratio128 operator+ (const Ratio128 & a, T b)
   {
      Ratio128 result = a;
      result += Ratio128(b);
      return result;
   }

   /**
    * Returns a-b.
    */
   friend Ratio128 operator- (const Ratio128 & a, const Ratio128 & b)
   {
      Ratio128 result = a;
      result -= b;
      return result;
   }

   /**
    * Returns a-b.
    */
   template<typename T>
   friend Ratio128 operator- (T a, const Ratio128 & b)
   {
      Ratio128 result = Ratio128 (a);
      result -= b;
      return result;
   }

   /**
    * Returns a-b.
    */
   template<typename T>
   friend Ratio128 operator- (const Ratio128 & a, T b)
   {
      Ratio128 result = a;
      result -= Ratio128(b);
      return result;
   }

   /**
    * Returns a*b.
    */
   friend Ratio128 operator* (const Ratio128 & a, const Ratio128 & b)
   {
      Ratio128 result = a;
      result *= b;
      return result;
   }

   /**
    * Returns a*b.
    */
   template<typename T>
   friend Ratio128 operator* (T a, const Ratio128 & b)
   {
      Ratio128 result = Ratio128 (a);
      result *= b;
      return result;
   }

   /**
    * Returns a*b.
    */
   template<typename T>
   friend Ratio128 operator* (const Ratio128 & a, T b)
   {
      Ratio128 result = a;
      result *= Ratio128(b);
      return result;
   }

   /**
    * Returns a/b.
    */
   friend Ratio128 operator/ (const Ratio128 & a, const Ratio128 & b)
   {
      Ratio128 result = a;
      result /= b;
      return result;
   }

   /**
    * Returns a/b.
    */
   template<typename T>
   friend Ratio128 operator/ (T a, const Ratio128 & b)
   {
      Ratio128 result = Ratio128 (a);
      result /= b;
      return result;
   }

   /**
    * Returns a/b.
    */
   template<typename T>
   friend Ratio128 operator/ (const Ratio128 & a, T b)
   {
      Ratio128 result = a;
      result /= Ratio128(b);
      return result;
   }


   // I/O stream operators.

   /**
    * Stream insertion.
    */
   friend std::ostream & operator<< (std::ostream& stream, const Ratio128& rat);


   // Comparison operators.

   /**
    * Returns a&lt;b.
    */
   friend bool operator< (const Ratio128 & a, const Ratio128 & b);

   /**
    * Returns a&lt;b.
    */
   template<typename T>
   friend bool operator< (T a, const Ratio128 & b)
   {
      return Ratio128(a) < b;
   }

   /**
    * Returns a&lt;b.
    */
   template<typename T>
   friend bool operator< (const Ratio128 & a, T b)
   {
      return a < Ratio128(b);
   }


   /**
    * Returns a>=b.
    */
   friend bool operator>= (const Ratio128 & a, const Ratio128 & b)
   {
      return !(a < b);
   }

   /**
    * Returns a>=b.
    */
   template<typename T>
   friend bool operator>= (T a, const Ratio128 & b)
   {
      return Ratio128(a) >= b;
   }

   /**
    * Returns a>=b.
    */
   template<typename T>
   friend bool operator>= (const Ratio128 & a, T b)
   {
      return a >= Ratio128(b);
   }


   /**
    * Returns a<=b.
    */
   friend bool operator<= (const Ratio128 & a, const Ratio128 & b);

   /**
    * Returns a<=b.
    */
   template<typename T>
   friend bool operator<= (T a, const Ratio128 & b)
   {
      return Ratio128(a) <= b;
   }

   /**
    * Returns a<=b.
    */
   template<typename T>
   friend bool operator<= (const Ratio128 & a, T b)
   {
      return a <= Ratio128(b);
   }


   /**
    * Returns a>b.
    */
   friend bool operator> (const Ratio128 & a, const Ratio128 & b)
   {
      return !(a <= b);
   }

   /**
    * Returns a>b.
    */
   template<typename T>
   friend bool operator> (T a, const Ratio128 & b)
   {
      return Ratio128(a) > b;
   }

   /**
    * Returns a>b.
    */
   template<typename T>
   friend bool operator> (const Ratio128 & a, T b)
   {
      return a > Ratio128(b);
   }


   /**
    * Returns a==b.
    * Note: Implementation assumes a and b are normalized (reduced).
    */
   friend bool operator== (const Ratio128 & a, const Ratio128 & b)
   {
      return (a.sign == b.sign) && (a.num == b.num) && (a.den == b.den);
   }

   /**
    * Returns a==b.
    */
   template<typename T>
   friend bool operator== (T a, const Ratio128 & b)
   {
      return Ratio128(a) == b;
   }

   /**
    * Returns a==b.
    */
   template<typename T>
   friend bool operator== (const Ratio128 & a, T b)
   {
      return a == Ratio128(b);
   }


   /**
    * Returns a!=b.
    */
   friend bool operator!= (const Ratio128 & a, const Ratio128 & b)
   {
      return !(a == b);
   }

   /**
    * Returns a!=b.
    */
   template<typename T>
   friend bool operator!= (T a, const Ratio128 & b)
   {
      return Ratio128(a) != b;
   }

   /**
    * Returns a!=b.
    */
   template<typename T>
   friend bool operator!= (const Ratio128 & a, T b)
   {
      return a != Ratio128(b);
   }


private:

   /**
    * Constructs a rational from two signed integers.
    */
   void make_ratio (int64_t num_in, int64_t den_in);

   /**
    * Reduces a rational to canonical form.
    */
   void reduce ();

   /**
    * Returns a rough (1.5 ULP) estimate of the double precision equivalent.
    */
   double to_double () const;
};


}


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
