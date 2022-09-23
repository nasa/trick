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
 * Defines the class NChooseM.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_N_CHOOSE_M_HH
#define ER7_UTILS_N_CHOOSE_M_HH

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// System includes
#include <stdint.h>


namespace er7_utils {


/**
 * Computes binomial coefficients @f$ \binom n m @f$, aka n choose m.
 *
 * ### Usage
 * The class provides two mechanisms for computing binomial coefficients:
 *   - Table lookup via for small n, 67 or less, via operator ().
 *   - Computed with iterated multiplication, via compute().
 */
class NChooseM {
ER7_UTILS_MAKE_SIM_INTERFACES(NChooseM)

public:

   /**
    * Default constructor.
    * @param init_triangle_nrows  Initial number of rows in the triangle;
    *                             defaults to zero.
    * @throw std::overflow_error if init_triangle_nrows > 68.
    */
   NChooseM (unsigned int init_triangle_nrows=0);

   /**
    * Copy constructor.
    * @param src Object to be copied.
    */
   NChooseM (const NChooseM & src);

   /**
    * Copy and swap assignment operator.
    * @param src  Object to be assigned to this.
    */
   NChooseM& operator= (NChooseM src)
   {
      swap (src);
      return *this;
   }

   /**
    * Destructor.
    */
   ~NChooseM ();


   /**
    * Resize the triangle if the current number of rows is less than the
    * specified size.
    * @param new_size Requested number of rows in the triangle.
    * @throw std::overflow_error if new_triangle_nrows > 68.
    */
   void resize (unsigned int new_size);

   /**
    * Compute n choose m via table lookup.
    * @note The triangle is automatically resized if needed.
    * @param n  'n' in n choose m.
    * @param m  'm' in n choose m.
    * @throw std::overflow_error if n > 67.
    * @return n choose m.
    */
   uint64_t operator() (unsigned int n, unsigned int m);

   /**
    * Compute n choose m via iterated multiplication.
    * @param n  'n' in n choose m.
    * @param m  'm' in n choose m.
    * @return n choose m.
    */
   double compute (unsigned int n, unsigned int m);

   /**
    * Swap contents of this with the contents of other.
    * @param other  NChooseM instance whose contents are to be
    *               swapped with those of this.
    */
   void swap (NChooseM & other);

   /**
    * Swap contents of a pair of NChooseM objects.
    * @param a  NChooseM instance whose contents are to be swapped.
    * @param b  Other NChooseM instance whose contents are to be swapped.
    */
   friend void swap (NChooseM & a, NChooseM & b)
   {
      a.swap (b);
   }

private:


   // Member data

   /**
    * Pascal's triangle.
    *
    * While this is conceptually a two dimensional array, it is implemented as a
    * flat array. The nth row conceptually contains 1+n/2 elements (truncated),
    * the values of n choose m where m ranges from 0 to n/2. Note that a full
    * Pascal's triangle would have m range from 0 to n. The latter half of each
    * row doesn't need to be stored because n choose m == n choose (n-m).
    */
   uint64_t* triangle; //!< trick_units(--)

   /**
    * Number of rows in the triangle.
    */
   unsigned int triangle_nrows; //!< trick_units(--)


   // Member functions

   /**
    * Compute the index of the zeroth element of the nth row in the triangle.
    * @param n  Row number
    * @return Computed index.
    */
   static unsigned int row_index (unsigned int n)
   {
      // Less hackish version:
      // The nth row starts at element number (n+1)*(n+1)/4 if n is odd,
      // element number n*(n+2)/4 if n is even.
      return ((n & 1u) ? (n+1u)*(n+1u) : n*(n+2u)) >> 2u;
   }

   /**
    * Construct the triangle row that represents the 'n' in n choose m.
    * @note This recursively constructs rows of lesser 'n' values.
    * @param n              Row number to be constructed.
    * @param last_known_row Last row that is already populated.
    */
   void construct_row (unsigned int n, unsigned int last_known_row);
};


}

#endif


/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
