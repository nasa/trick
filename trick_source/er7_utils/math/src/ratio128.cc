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
 * Defines non-inlined methods of class Ratio128.
 */

/*
Purpose: ()
*/


// Local includes
#include "../include/ratio128.hh"

// System includes
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <stdint.h>
#include <string>


namespace er7_utils {


Ratio128::Ratio128 (
   double dval)
:
   sign (0),
   num (0u),
   den (1u)
{
   if (dval != 0.0) {
      if (dval < 0.0) {
         dval = -dval;
         sign = -1;
      }
      else {
         sign = 1;
      }
      int expo;
      double fnorm = std::frexp (dval, &expo);
      Ratio128 rval =
         Ratio128((unsigned long long)(fnorm*0x1p53), 1ull<<53, sign);
      if (expo > 0) {
         if (expo >= (128-53)) {
            throw std::domain_error ("Overflow");
         }
         rval.num <<= expo;
      }
      else if (expo < 0) {
         if (expo <= -128) {
            throw std::domain_error ("Underflow");
         }
         rval.den <<= -expo;
      }
      *this = rval;
   }
}


void
Ratio128::swap (Ratio128& other)
{
   std::swap (sign, other.sign);
   num.swap (other.num);
   den.swap (other.den);
}


Ratio128
Ratio128::inverse ()
const
{
   Ratio128 inverted (*this);
   if (sign == 0) {
      throw std::domain_error ("Divide by zero");
   }
   inverted.num.swap (inverted.den);
   return inverted;
}

void
Ratio128::round ()
{
   if ((num > 1ull) && (den > 1ull)) {
      if ((den & 1ull) == 0ull) {
         den >>= 1;
         num >>= 1;
         num += 1ull;
      }
      else {
         uint64_t eps = uint64_t(to_double()*0.5 + 0.5);
         den >>= 1;
         num >>= 1;
         num += eps;
      }
      reduce ();
   }
}

Ratio128&
Ratio128::operator+= (
   const Ratio128 & other)
{
   if (num == 0ull) {
      *this = other;
   }
   else if (other.num != 0ull) {
      UInt128 fact = gcd (den, other.den);
      UInt128 b = den / fact;
      UInt128 d = other.den / fact;
      UInt128 numa = num*d;
      UInt128 numb = other.num*b;
      if (sign * other.sign > 0) {
         num = numa + numb;
      }
      else if (numa > numb) {
         num = numa - numb;
      }
      else {
         num = numb - numa;
         sign = -sign;
      }
      den *= d;
      reduce ();
   }
   return *this;
}

Ratio128&
Ratio128::operator-= (
   const Ratio128 & other)
{
   if (num == 0ull) {
      *this = -other;
   }
   else if (other.num != 0ull) {
      *this += -other;
   }
   return *this;
}

Ratio128&
Ratio128::operator*= (
   const Ratio128 & other)
{
   if (other.num == 0ull) {
      sign = 0;
      num = 0ull;
      den = 1ull;
   }
   else if (num != 0ull) {
      UInt128 fact1 = gcd (num, other.den);
      UInt128 fact2 = gcd (den, other.num);
      num = (num/fact1)*(other.num/fact2);
      den = (den/fact2)*(other.den/fact1);
      sign *= other.sign;
   }
   return *this;
}


bool
operator< (
   const Ratio128 & a,
   const Ratio128 & b)
{
   if (a.sign < b.sign) {
      return true;
   }
   else if (a.sign > b.sign) {
      return false;
   }
   else if (a.sign == 0) {
      return false;
   }
   else {
      return ((a-b).sign < 0);
   }
}


bool
operator<= (
   const Ratio128 & a,
   const Ratio128 & b)
{
   if (a.sign < b.sign) {
      return true;
   }
   else if (a.sign > b.sign) {
      return false;
   }
   else if (a.sign == 0) {
      return true;
   }
   else {
      return ((a-b).sign <= 0);
   }
}


double
Ratio128::to_double ()
const
{
   double ratio = double(num) / double(den);
   if (sign < 0) {
      ratio = -ratio;
   }
   return ratio;
}


Ratio128::operator double ()
const
{
   // Get a first cut at the double equivalent of the rational.
   // This will be within 1.5 ULP, worst case. The error typically is
   // half an ULP or less.
   double dval = to_double();

   // Try to refine this to within half an ULP.
   // Failures can happen when
   // - The subtractions result in overly large denominators, or
   // - One ULP is not representable as a Ratio128 (dval < 5.29e-23).
   try {
      Ratio128 rval = Ratio128(dval);
      Ratio128 err = *this - rval;
      if (err.num != 0ull) {

         // Create a rational that represents one ULP.
         int expo;
         std::frexp (dval, &expo);
         Ratio128 ulp = Ratio128(1ull, 1ull<<53);
         if (expo > 0) {
            ulp.num <<= expo;
         }
         else if (expo < 0) {
            ulp.den <<= -expo;
         }
         ulp.reduce();

         // Replace the double value with the value offset by one ULP
         // if this new value represents a better approximation.
         ulp.sign = err.sign;
         double nval = dval + ulp.to_double();
         if ((*this-nval).abs() < err.abs()) {
            dval = nval;
         }
      }
   }
   // Something wicked happened. C'est la vie. We're already close.
   catch (std::domain_error) {
      std::cout << "\n\n\n\nOoops\n\n\n\n";
   }

   return dval;
}


Ratio128::operator std::string ()
const
{
   std::string result;
   if (sign < 0) {
      result = "-";
   }
   result += std::string(num);
   if (den != 1ull) {
      result += '/';
      result += std::string(den);
   }
   return result;
}


std::ostream & operator<< (
   std::ostream& stream,
   const Ratio128& rat)
{
   stream << std::string(rat);
   return stream;
}


void
Ratio128::make_ratio (
   int64_t num_in,
   int64_t den_in)
{
   if (den_in == 0) {
      throw std::domain_error ("Zero denominator");
   }

   if (num_in != 0) {
      if (den_in < 0) {
         den = uint64_t (-den_in);
         num_in = -num_in;
      }
      else {
         den = uint64_t (den_in);
      }
      if (num_in < 0) {
         sign = -1;
         num = uint64_t (-num_in);
      }
      else {
         sign = 1;
         num = uint64_t (num_in);
      }
      reduce ();
   }
}

void
Ratio128::reduce ()
{
   if (den == 0ull) {
      throw std::domain_error ("Zero denominator");
   }

   if (num == 0ull) {
      sign = 0ull;
      den = 1ull;
   }

   else {
      UInt128 fact = gcd (num, den);
      num /= fact;
      den /= fact;
   }
}


}


/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
