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
 * Defines the class UInt128.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_UINT128_HH
#define ER7_UTILS_UINT128_HH

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// System includes
#include <iosfwd>
#include <stdint.h>
#include <string>


namespace er7_utils {


/**
 * Implements a 128 bit unsigned integer.
 *
 * Opportunities for improvement abound. Three key ones are:
 * - The division algorithm is terribly slow. Improve it somehow.
 * - Evaluation is not lazy. Add lazy evaluation.
 * - There's a lot of unnecessary creation and destruction.
 */
class UInt128 {
ER7_UTILS_MAKE_SIM_INTERFACES(UInt128)

private:

   /**
    * Low order bits of the number.
    */
   uint64_t low; //!< trick_units(--)

   /**
    * High order bits of the number.
    */
   uint64_t high; //!< trick_units(--)


public:

   // Constructors and destructors.
   // Note that the the copy constructor, assignment operator, and destructor
   // are not declared. The implicitly-defined versions do the right thing.

   /**
    * Default constructor. Default value is zero.
    */
   UInt128 ()
   :
      low (0),
      high (0)
   { }

   /**
    * Non-default constructor, unsigned int to UInt128.
    * @param val0  Value to be converted to a Uint128.
    */
   UInt128 (unsigned int val0)
   :
      low (val0),
      high (0)
   { }

   /**
    * Non-default constructor, unsigned long long to UInt128
    * (plus optional high bits).
    * @param val0  Value containing the 64 low order bits.
    * @param val1  Value containing the 64 high order bits.
    */
   UInt128 (unsigned long long val0, unsigned long long val1=0ull)
   :
      low (val0),
      high (val1)
   { }

   /**
    * Non-default constructor, unsigned long to UInt128
    * (plus optional high bits).
    * @param val0  Value containing the 64 low order bits.
    * @param val1  Value containing the 64 high order bits.
    */
   UInt128 (unsigned long val0, unsigned long val1=0ull)
   :
      low (val0),
      high (val1)
   { }


   /**
    * Division function
    * @param divisor    Quantity by which this is to be divided.
    * @param quotient   Quotient of this divided by divisor (set).
    * @param remainder  Remainder after division (set).
    */
   void divide (
      UInt128 divisor,
      UInt128& quotient,
      UInt128& remainder);

   /**
    * Greatest common divisor of two UInt128 values.
    * @param a  A UInt128 value.
    * @param b  Another UInt128 value.
    * @return gcd of a and b.
    */
   friend UInt128 gcd (UInt128 a, UInt128 b);


   // Swap functions.

   /**
    * Swap contents of this with the contents of other.
    * @param other  Object whose contents are to be swapped with those of this.
    */
   void swap (UInt128& other);


   /**
    * Swap contents of two UInt128 objects.
    * @param a  UInt128 instance whose contents are to be swapped.
    * @param b  Other UInt128 instance whose contents are to be swapped.
    */
   friend void swap (UInt128& a, UInt128& b)
   {
      a.swap (b);
   }


   // Operators.

   /**
    * One's complement operator.
    * @return UInt128 object containing the one's complement of this.
    */
   UInt128 operator~ () const;

   /**
    * Unary - operator.
    * @return UInt128 object containing the two's complement of this.
    */
   UInt128 operator- () const;

   /**
    * Unary + operator.
    * @return UInt128 object containing a copy of this.
    */
   UInt128 operator+ () const
   {
      return *this;
   }


   /**
    * Left shift assignment operator.
    * @param shift  Number of bits to shift.
    * @return Reference to this, after being left-shifted by shift bits.
    */
   UInt128& operator<<= (unsigned int shift);

   /**
    * Left shift operator.
    * @param a  A UInt128 value.
    * @param shift  Number of bits to shift.
    * @return Parameter a left-shifted by shift bits.
    */
   friend UInt128 operator<< (const UInt128& a, unsigned int shift)
   {
      UInt128 result = a;
      result <<= shift;
      return result;
   }

   /**
    * Right shift assignment operator.
    * @param shift  Number of bits to shift.
    * @return Reference to this, after being right-shifted by shift bits.
    */
   UInt128& operator>>= (unsigned int shift);

   /**
    * Right shift operator.
    * @param a  A UInt128 value.
    * @param shift  Number of bits to shift.
    * @return Parameter a right-shifted by shift bits.
    */
   friend UInt128 operator>> (const UInt128& a, unsigned int shift)
   {
      UInt128 result = a;
      result >>= shift;
      return result;
   }

   /**
    * Bitwise or assignment operator.
    * @param other  Quantity to be bitwise-ored with this.
    * @return Reference to this, after being bitwise-ored with other.
    */
   UInt128& operator|= (const UInt128& other)
   {
      low |= other.low;
      high |= other.high;
      return *this;
   }

   /**
    * Bitwise or operator.
    * @param a  A UInt128 value.
    * @param b  Another UInt128 value.
    * @return Bitwise-or of a and b.
    */
   friend UInt128 operator| (const UInt128& a, const UInt128& b)
   {
      UInt128 result = a;
      result |= b;
      return result;
   }

   /**
    * Bitwise or operator.
    * @param a  A UInt128 value.
    * @param b  An unsigned 64 bit integer.
    * @return Bitwise-or of a and b.
    */
   friend UInt128 operator| (const UInt128& a, unsigned long long b)
   {
      UInt128 result = a;
      result.low |= b;
      return result;
   }

   /**
    * Bitwise or operator.
    * @param a  A UInt128 value.
    * @param b  An unsigned 64 bit integer.
    * @return Bitwise-or of a and b.
    */
   friend UInt128 operator| (const UInt128& a, unsigned long b)
   {
      UInt128 result = a;
      result.low |= b;
      return result;
   }

   /**
    * Bitwise or operator.
    * @param a  An unsigned 64 bit integer.
    * @param b  A UInt128 value.
    * @return Bitwise-or of a and b.
    */
   friend UInt128 operator| (unsigned long long a, const UInt128& b)
   {
      UInt128 result = b;
      result.low |= a;
      return result;
   }

   /**
    * Bitwise or operator.
    * @param a  An unsigned 64 bit integer.
    * @param b  A UInt128 value.
    * @return Bitwise-or of a and b.
    */
   friend UInt128 operator| (unsigned long a, const UInt128& b)
   {
      UInt128 result = b;
      result.low |= a;
      return result;
   }

   /**
    * Bitwise and assignment operator.
    * @param other  Quantity to be bitwise-anded with this.
    * @return Reference to this, after being bitwise-anded with other.
    */
   UInt128& operator&= (const UInt128& other)
   {
      low &= other.low;
      high &= other.high;
      return *this;
   }

   /**
    * Bitwise and operator.
    * @param a  A UInt128 value.
    * @param b  Another UInt128 value.
    * @return Bitwise-and of a and b.
    */
   friend UInt128 operator& (const UInt128& a, const UInt128& b)
   {
      UInt128 result = a;
      result &= b;
      return result;
   }

   /**
    * Bitwise and operator.
    * @param a  A UInt128 value.
    * @param b  An unsigned 64 bit integer.
    * @return Bitwise-and of a and b.
    */
   friend UInt128 operator& (const UInt128& a, unsigned long long b)
   {
      unsigned long long result = a.low;
      result &= b;
      return UInt128(result);
   }

   /**
    * Bitwise and operator.
    * @param a  A UInt128 value.
    * @param b  An unsigned 64 bit integer.
    * @return Bitwise-and of a and b.
    */
   friend UInt128 operator& (const UInt128& a, unsigned long b)
   {
      unsigned long result = a.low;
      result &= b;
      return UInt128(result);
   }

   /**
    * Bitwise and operator.
    * @param a  An unsigned 64 bit integer.
    * @param b  A UInt128 value.
    * @return Bitwise-and of a and b.
    */
   friend UInt128 operator& (unsigned long long a, const UInt128& b)
   {
      unsigned long long result = a;
      result &= b.low;
      return UInt128(result);
   }

   /**
    * Bitwise and operator.
    * @param a  An unsigned 64 bit integer.
    * @param b  A UInt128 value.
    * @return Bitwise-and of a and b.
    */
   friend UInt128 operator& (unsigned long a, const UInt128& b)
   {
      unsigned long result = a;
      result &= b.low;
      return UInt128(result);
   }


   /**
    * Increment (addition assignment) operator.
    * @param other  Quantity by which this is to be incremented.
    * @return Reference to this, after being incremented by other.
    */
   UInt128& operator+= (const UInt128& other);

   /**
    * Increment operator.
    * @return Reference to this, after being incremented by one.
    */
   UInt128& operator++ ()
   {
      return operator += (1ull);
   }

   /**
    * Addition operator.
    * @param a  A UInt128 value.
    * @param b  Another UInt128 value.
    * @return Sum of a and b.
    */
   friend UInt128 operator+ (const UInt128& a, const UInt128& b)
   {
      UInt128 result = a;
      result += b;
      return result;
   }

   /**
    * Decrement (subtraction assignment) operator.
    * @param other  Quantity by which this is to be decremented.
    * @return Reference to this, after being decremented by other.
    */
   UInt128& operator-= (const UInt128& other);

   /**
    * Decrement operator.
    * @return Reference to this, after being decremented by one.
    */
   UInt128& operator-- ()
   {
      return operator -= (1ull);
   }

   /**
    * Subtraction operator.
    * @param a  A UInt128 value.
    * @param b  Another UInt128 value.
    * @return Difference between a and b.
    */
   friend UInt128 operator- (const UInt128& a, const UInt128& b)
   {
      UInt128 result = a;
      result -= b;
      return result;
   }

   /**
    * Multiplication assignment operator.
    * @param other  Quantity by which this is to be multiplied.
    * @return Reference to this, after being multiplied by other.
    */
   UInt128& operator*= (const UInt128& other);

   friend UInt128 operator* (const UInt128& a, const UInt128& b)
   {
      UInt128 result = a;
      result *= b;
      return result;
   }

   /**
    * Division assignment operator.
    * @param other  Quantity by which this is to be divided.
    * @return Reference to this, after being divided by other.
    */
   UInt128& operator/= (const UInt128& other)
   {
      UInt128 quotient, remainder;
      divide (other, quotient, remainder);
      *this = quotient;
      return *this;
   }

   friend UInt128 operator/ (const UInt128& a, const UInt128& b)
   {
      UInt128 result = a;
      result /= b;
      return result;
   }

   /**
    * Modulus assignment operator.
    * @param other  Quantity by which this is to be divided.
    * @return Reference to this, after being divided by other.
    */
   UInt128& operator%= (const UInt128& other)
   {
      UInt128 quotient, remainder;
      divide (other, quotient, remainder);
      *this = remainder;
      return *this;
   }

   friend UInt128 operator% (const UInt128& a, const UInt128& b)
   {
      UInt128 result = a;
      result %= b;
      return result;
   }

   /**
    * Conversion operator, to double.
    * @return This as a double.
    */
   operator double () const;

   /**
    * Conversion operator, to string.
    * @return This as a string.
    */
   operator std::string () const;

   /**
    * Stream insertion operator.
    * @param stream  Stream into which to write string equivalent of num.
    * @param num     UInt128 to be written to the stream.
    * @return Stream.
    */
   friend std::ostream & operator<< (std::ostream& stream, const UInt128 & num);

   /**
    * Comparison operator (equality).
    * @param a  A UInt128 value.
    * @param b  Another UInt128 value.
    * @return True if a and b are equal, false otherwise.
    */
   friend bool operator== (const UInt128& a, const UInt128& b)
   {
      return (a.high == b.high) && (a.low == b.low);
   }

   /**
    * Comparison operator (equality).
    * @param a  A UInt128 value.
    * @param b  An unsigned 64 bit integer.
    * @return True if a and b are equal, false otherwise.
    */
   friend bool operator== (const UInt128& a, unsigned long long b)
   {
      return (a.high == 0ull) && (a.low == b);
   }

   /**
    * Comparison operator (equality).
    * @param a  A UInt128 value.
    * @param b  An unsigned 64 bit integer.
    * @return True if a and b are equal, false otherwise.
    */
   friend bool operator== (const UInt128& a, unsigned long b)
   {
      return (a.high == 0ull) && (a.low == b);
   }

   /**
    * Comparison operator (equality).
    * @param a  An unsigned 64 bit integer.
    * @param b  A UInt128 value.
    * @return True if a and b are equal, false otherwise.
    */
   friend bool operator== (unsigned long long a, const UInt128& b)
   {
      return (b == a);
   }

   /**
    * Comparison operator (equality).
    * @param a  An unsigned 64 bit integer.
    * @param b  A UInt128 value.
    * @return True if a and b are equal, false otherwise.
    */
   friend bool operator== (unsigned long a, const UInt128& b)
   {
      return (b == a);
   }

   /**
    * Comparison operator (inequality).
    * @param a  A UInt128 value.
    * @param b  Another UInt128 value.
    * @return False if a and b are equal, true otherwise.
    */
   friend bool operator!= (const UInt128& a, const UInt128& b)
   {
      return (a.high != b.high) || (a.low != b.low);
   }

   /**
    * Comparison operator (inequality).
    * @param a  A UInt128 value.
    * @param b  An unsigned 64 bit integer.
    * @return False if a and b are equal, true otherwise.
    */
   friend bool operator!= (const UInt128& a, unsigned long long b)
   {
      return (a.high != 0ull) || (a.low != b);
   }

   /**
    * Comparison operator (inequality).
    * @param a  A UInt128 value.
    * @param b  An unsigned 64 bit integer.
    * @return False if a and b are equal, true otherwise.
    */
   friend bool operator!= (const UInt128& a, unsigned long b)
   {
      return (a.high != 0ull) || (a.low != b);
   }

   /**
    * Comparison operator (inequality).
    * @param a  An unsigned 64 bit integer.
    * @param b  A UInt128 value.
    * @return False if a and b are equal, true otherwise.
    */
   friend bool operator!= (unsigned long long a, const UInt128& b)
   {
      return (b != a);
   }

   /**
    * Comparison operator (inequality).
    * @param a  An unsigned 64 bit integer.
    * @param b  A UInt128 value.
    * @return False if a and b are equal, true otherwise.
    */
   friend bool operator!= (unsigned long a, const UInt128& b)
   {
      return (b != a);
   }

   /**
    * Comparison operator (less than).
    * @param a  A UInt128 value.
    * @param b  Another UInt128 value.
    * @return True if a is less than b, false otherwise.
    */
   friend bool operator< (const UInt128& a, const UInt128& b)
   {
      return (a.high < b.high) ||
             ((a.high == b.high) && (a.low < b.low));
   }

   /**
    * Comparison operator (less than).
    * @param a  A UInt128 value.
    * @param b  An unsigned 64 bit integer.
    * @return True if a is less than b, false otherwise.
    */
   friend bool operator< (const UInt128& a, unsigned long long b)
   {
      return (a.high == 0ull) && (a.low < b);
   }

   /**
    * Comparison operator (less than).
    * @param a  A UInt128 value.
    * @param b  An unsigned 64 bit integer.
    * @return True if a is less than b, false otherwise.
    */
   friend bool operator< (const UInt128& a, unsigned long b)
   {
      return (a.high == 0ull) && (a.low < b);
   }

   /**
    * Comparison operator (less than).
    * @param a  An unsigned 64 bit integer.
    * @param b  A UInt128 value.
    * @return True if a is less than b, false otherwise.
    */
   friend bool operator< (unsigned long long a, const UInt128& b)
   {
      return (b > a);
   }

   /**
    * Comparison operator (less than).
    * @param a  An unsigned 64 bit integer.
    * @param b  A UInt128 value.
    * @return True if a is less than b, false otherwise.
    */
   friend bool operator< (unsigned long a, const UInt128& b)
   {
      return (b > a);
   }

   /**
    * Comparison operator (less than or equal to).
    * @param a  A UInt128 value.
    * @param b  Another UInt128 value.
    * @return True if a is less than or equal to b, false otherwise.
    */
   friend bool operator<= (const UInt128& a, const UInt128& b)
   {
      return (a.high < b.high) ||
             ((a.high == b.high) && (a.low <= b.low));
   }

   /**
    * Comparison operator (less than or equal to).
    * @param a  A UInt128 value.
    * @param b  An unsigned 64 bit integer.
    * @return True if a is less than or equal to b, false otherwise.
    */
   friend bool operator<= (const UInt128& a, unsigned long long b)
   {
      return (a.high == 0ull) && (a.low <= b);
   }

   /**
    * Comparison operator (less than or equal to).
    * @param a  A UInt128 value.
    * @param b  An unsigned 64 bit integer.
    * @return True if a is less than or equal to b, false otherwise.
    */
   friend bool operator<= (const UInt128& a, unsigned long b)
   {
      return (a.high == 0ull) && (a.low <= b);
   }

   /**
    * Comparison operator (less than or equal to).
    * @param a  An unsigned 64 bit integer.
    * @param b  A UInt128 value.
    * @return True if a is less than b, false otherwise.
    */
   friend bool operator<= (unsigned long long a, const UInt128& b)
   {
      return (b >= a);
   }

   /**
    * Comparison operator (less than or equal to).
    * @param a  An unsigned 64 bit integer.
    * @param b  A UInt128 value.
    * @return True if a is less than b, false otherwise.
    */
   friend bool operator<= (unsigned long a, const UInt128& b)
   {
      return (b >= a);
   }

   /**
    * Comparison operator (greater than).
    * @param a  A UInt128 value.
    * @param b  Another UInt128 value.
    * @return True if a is greater than b, false otherwise.
    */
   friend bool operator> (const UInt128& a, const UInt128& b)
   {
      return (a.high > b.high) ||
             ((a.high == b.high) && (a.low > b.low));
   }

   /**
    * Comparison operator (greater than).
    * @param a  A UInt128 value.
    * @param b  An unsigned 64 bit integer.
    * @return True if a is greater than b, false otherwise.
    */
   friend bool operator> (const UInt128& a, unsigned long long b)
   {
      return (a.high == 0ull) && (a.low > b);
   }

   /**
    * Comparison operator (greater than).
    * @param a  A UInt128 value.
    * @param b  An unsigned 64 bit integer.
    * @return True if a is greater than b, false otherwise.
    */
   friend bool operator> (const UInt128& a, unsigned long b)
   {
      return (a.high == 0ull) && (a.low > b);
   }

   /**
    * Comparison operator (greater than).
    * @param a  An unsigned 64 bit integer.
    * @param b  A UInt128 value.
    * @return True if a is greater than b, false otherwise.
    */
   friend bool operator> (unsigned long long a, const UInt128& b)
   {
      return (b < a);
   }

   /**
    * Comparison operator (greater than).
    * @param a  An unsigned 64 bit integer.
    * @param b  A UInt128 value.
    * @return True if a is greater than b, false otherwise.
    */
   friend bool operator> (unsigned long a, const UInt128& b)
   {
      return (b < a);
   }

   /**
    * Comparison operator (greater than or equal to).
    * @param a  A UInt128 value.
    * @param b  Another UInt128 value.
    * @return True if a is greater than or equal to b, false otherwise.
    */
   friend bool operator>= (const UInt128& a, const UInt128& b)
   {
      return (a.high > b.high) ||
             ((a.high == b.high) && (a.low >= b.low));
   }

   /**
    * Comparison operator (greater than or equal to).
    * @param a  A UInt128 value.
    * @param b  An unsigned 64 bit integer.
    * @return True if a is greater than or equal to b, false otherwise.
    */
   friend bool operator>= (const UInt128& a, unsigned long long b)
   {
      return (a.high == 0ull) && (a.low >= b);
   }

   /**
    * Comparison operator (greater than or equal to).
    * @param a  A UInt128 value.
    * @param b  An unsigned 64 bit integer.
    * @return True if a is greater than or equal to b, false otherwise.
    */
   friend bool operator>= (const UInt128& a, unsigned long b)
   {
      return (a.high == 0ull) && (a.low >= b);
   }

   /**
    * Comparison operator (greater than or equal to).
    * @param a  An unsigned 64 bit integer.
    * @param b  A UInt128 value.
    * @return True if a is greater than b, false otherwise.
    */
   friend bool operator>= (unsigned long long a, const UInt128& b)
   {
      return (b <= a);
   }

   /**
    * Comparison operator (greater than or equal to).
    * @param a  An unsigned 64 bit integer.
    * @param b  A UInt128 value.
    * @return True if a is greater than b, false otherwise.
    */
   friend bool operator>= (unsigned long a, const UInt128& b)
   {
      return (b <= a);
   }
};


}


#endif


/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
