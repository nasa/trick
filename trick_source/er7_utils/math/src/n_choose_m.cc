/**
 * @if Er7UtilsUseGroups
 * @addtogroup Er7Utils
 * @{
 * @addtogroup Utils
 * @{
 * @endif
 */


// Local includes
#include "../include/n_choose_m.hh"

// Interface includes
#include "er7_utils/interface/include/alloc.hh"

// System includes
#include <algorithm>
#include <stdexcept>
#include <cstddef>
#include <cstring>


namespace er7_utils {


NChooseM::NChooseM (unsigned int init_triangle_nrows)
:
   triangle (NULL),
   triangle_nrows (0u)
{
   if (init_triangle_nrows > 0u) {
      resize (init_triangle_nrows);
   }
}


NChooseM::NChooseM (const NChooseM & src)
:
   triangle (NULL),
   triangle_nrows (src.triangle_nrows)
{
   if (triangle_nrows > 0u) {
      triangle = alloc::replicate_array<uint64_t> (
                    row_index(triangle_nrows), src.triangle);
   }
}


NChooseM::~NChooseM ()
{
   alloc::deallocate_array<uint64_t> (triangle);
}


void
NChooseM::resize (unsigned int new_size)
{
   // Ignore resize requests to shrink the triangle.
   if (new_size > triangle_nrows) {

      // 68 choose 31 is larger than INT64_MAX and thus can't be
      // represented using uint64_t.
      if (new_size > 68u) {
         throw std::overflow_error (
                  "new_size exceeds 68 in NChooseM::resize");
      }

      uint64_t* new_triangle =
         alloc::allocate_array<uint64_t> (row_index (new_size));
      unsigned int last_known_row;

      // Transfer data from the existing triangle and then delete it.
      if (triangle_nrows > 0u) {
         last_known_row = triangle_nrows - 1u;
         std::memcpy (new_triangle,
                      triangle, row_index(triangle_nrows)*sizeof(uint64_t));
         alloc::deallocate_array<uint64_t> (triangle);
      }

      // No existing table: Bootstrap by constructing row 0 (0 choose 0 is 1).
      else {
         last_known_row = 0u;
         new_triangle[0] = 1u;
      }

      // Reset data members.
      triangle = new_triangle;
      triangle_nrows = new_size;

      // Construct the triangle rows that haven't yet been constructed.
      // This call constructs the last row. Internally, all rows of lesser
      // size are constructed prior to constructing the last row.
      construct_row (new_size-1u, last_known_row);
   }
}


uint64_t
NChooseM::operator() (
   unsigned int n,
   unsigned int m)
{
   // Special case: n choose m is zero for all m>n.
   if (m > n) {
      return 0;
   }

   // Resize the triangle if needed so n choose m is pre-computed.
   if (n >= triangle_nrows) {
      resize (n+1);
   }

   // Row n of data member triangle contains values of n choose m for values of
   // m between 0 and floor(n/2), inclusive. Values for m between n/2 and m
   // are easily obtained using the fact that n choose m == n choose (n-m).
   // Return the value that corresponds to n choose m.
   return triangle[row_index(n) + std::min(m, n-m)];
}


double
NChooseM::compute (
   unsigned int n,
   unsigned int m)
{
   // Special case: n choose m is zero for all m>n.
   if (m > n) {
      return 0;
   }

   // Compute n choose m.
   double result = 1.0;
   for (double num = double(n), den = double(std::min(m, n-m));
        den > 0.0;
        --num, --den) {
      result *= num/den;
   }

   return result;
}


void
NChooseM::swap (NChooseM & other)
{
   std::swap (triangle, other.triangle);
   std::swap (triangle_nrows, other.triangle_nrows);
}


void
NChooseM::construct_row (unsigned int n, unsigned int known_n)
{
   // Construct the preceeding row if needed.
   if (n > known_n+1u) {
      construct_row (n-1u, known_n);
   }

   uint64_t* row_n = &triangle[row_index(n)];

   // n choose 0 is 1 for all n.
   row_n[0] = 1u;

   // Populate remaining elements of this row via the recursion
   // n choose m = (n-1) choose (m-1) + (n-1) choose m.
   // Note that only elements 1 to n/2 are populated.
   if (n > 1) {
      unsigned int last_m = n/2u;
      uint64_t* row_nm1 = &triangle[row_index(n-1u)];

      // Populate elements 1 .. last_m-1 using the above recursion.
      for (unsigned int m = 1; m < last_m; ++m) {
         row_n[m] = row_nm1[m-1] + row_nm1[m];
      }

      // The last element needs special handling because only half of the
      // triangle is constructed.
      row_n[last_m] = row_nm1[last_m-1] + row_nm1[n-1u-last_m];
   }
}

}


/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
