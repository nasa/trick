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
 * Defines non-inlined methods of class UInt128.
 */

/*
Purpose: ()
*/


// Local includes
#include "../include/uint128.hh"

// System includes
#include <algorithm>
#include <iostream>
#include <cmath>
#include <stdexcept>
#include <stdint.h>
#include <string>


namespace er7_utils {


void
UInt128::swap (
   UInt128& other)
{
   std::swap (low, other.low);
   std::swap (high, other.high);
}

UInt128
UInt128::operator~ ()
const
{
   UInt128 result (*this);
   result.low = ~result.low;
   result.high = ~result.high;
   return result;
}

UInt128
UInt128::operator- ()
const
{
   UInt128 result (*this);
   if (result.low == 0) {
      result.high = ~result.high + 1;
   }
   else {
      result.low = ~result.low + 1;
      result.high = ~result.high;
   }
   return result;
}


UInt128&
UInt128::operator<<= (
   unsigned int shift)
{
   if (shift >= 128) {
      low = high = 0;
   }
   else if (shift >= 64) {
      high = low << (shift-64);
      low = 0;
   }
   else if (shift > 0) {
      uint64_t carry = low & (0xffffffffffffffffULL << (64-shift));
      low <<= shift;
      high <<= shift;
      high |= carry >> (64-shift);
   }
   return *this;
}

UInt128&
UInt128::operator>>= (
   unsigned int shift)
{
   if (shift >= 128) {
      low = high = 0;
   }
   else if (shift >= 64) {
      low = high >> (shift-64);
      high = 0;
   }
   else if (shift > 0) {
      uint64_t carry = high & (0xffffffffffffffffULL >> (64-shift));
      high >>= shift;
      low >>= shift;
      low |= carry << (64-shift);
   }
   return *this;
}

UInt128&
UInt128::operator+= (
   const UInt128& other)
{
   uint64_t old_low = low;
   uint64_t old_high = high;

   low += other.low;
   high += other.high;
   if (low < old_low) {
      ++high;
   }
   if (high < old_high) {
      throw std::domain_error ("Overflow");
   }
   return *this;
}

UInt128&
UInt128::operator-= (
   const UInt128& other)
{
   if (*this < other) {
      throw std::domain_error ("Overflow");
   }

   uint64_t old_low = low;
   uint64_t old_high = high;
   const UInt128& neg_other = other.operator-();

   low += neg_other.low;
   high += neg_other.high;
   if (low < old_low) {
      ++high;
   }
   if (high > old_high) {
      throw std::domain_error ("Overflow");
   }
   return *this;
}

UInt128&
UInt128::operator*= (const UInt128& other)
{
   uint64_t prod[8] = {0, 0, 0, 0, 0, 0, 0, 0};
   uint64_t this_arr[4];
   uint64_t other_arr[4];

   this_arr[0] = low & 0x00000000ffffffffULL;
   this_arr[1] = low >> 32;
   this_arr[2] = high & 0x00000000ffffffffULL;
   this_arr[3] = high >> 32;
   other_arr[0] = other.low & 0x00000000ffffffffULL;
   other_arr[1] = other.low >> 32;
   other_arr[2] = other.high & 0x00000000ffffffffULL;
   other_arr[3] = other.high >> 32;

   for (int ii = 0; ii < 4; ++ii) {
      uint64_t fact = this_arr[ii];
      uint64_t carry = 0;
      for (int jj = 0; jj < 4; ++jj) {
         uint64_t prod_ii_jj =
            fact * other_arr[jj] + prod[ii+jj] + carry;
         prod[ii+jj] = prod_ii_jj & 0x00000000ffffffffULL;
         carry = prod_ii_jj >> 32;
      }
      prod[ii+4] += carry;
   }
   if ((prod[7] > 0) || (prod[6] > 0) || (prod[5] > 0) || (prod[4] > 0)) {
      throw std::domain_error ("Overflow");
   }
   low = prod[0] | (prod[1] << 32);
   high = prod[2] | (prod[3] << 32);
   return *this;
}

void
UInt128::divide (
   UInt128 divisor,
   UInt128& quotient,
   UInt128& remainder)
{
   if ((divisor.high == 0ull) && (divisor.low == 0ull)) {
      throw std::domain_error ("Divide by zero");
   }

   if ((divisor.high == 0ull) && (divisor.low == 1ull)) {
      quotient = *this;
      remainder = 0ull;
   }

   else if ((high == 0ull) && (divisor.high == 0ull)) {
      uint64_t qlow = low / divisor.low;
      uint64_t rlow = low % divisor.low;
      quotient = qlow;
      remainder = rlow;
   }

   else {
      quotient = 0u;
      remainder = 0u;
      UInt128 mask;
      mask.high = 0x8000000000000000ULL;
      for (int ii = 0; ii < 128; ++ii) {
         quotient <<= 1;
         remainder <<= 1;
         if ((*this & mask) != 0ull) {
            remainder |= 1u;
         }
         if (remainder >= divisor) {
            remainder -= divisor;
            quotient |= 1u;
         }
         mask >>= 1;
      }
   }
}

UInt128::operator double ()
const
{
   return double(high)*0x1p64 + double(low);
}


UInt128::operator std::string ()
const
{
   std::string result;
   if (*this == 0ull) {
      result = "0";
   }
   else {
      UInt128 copy = *this;
      UInt128 ten = 10ull;
      UInt128 quotient;
      UInt128 remainder;
      while (copy != 0ull) {
         copy.divide (ten, quotient, remainder);
         result += '0' + remainder.low;
         copy = quotient;
      }
      std::reverse (result.begin(), result.end());
   }
   return result;
}

std::ostream & operator<< (
   std::ostream& stream,
   const UInt128 & num)
{
   stream << std::string(num);
   return stream;
}


UInt128
gcd (
   UInt128 a,
   UInt128 b)
{
   for (UInt128 rem = a % b; rem != 0ull; rem = a % b) {
      a = b;
      b = rem;
   }
   return b;
}


}


/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
