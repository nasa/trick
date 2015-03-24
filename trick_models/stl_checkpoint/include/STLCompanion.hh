/*
    PURPOSE: (Illustrate how to checkpoint STLs) 
    LIBRARY_DEPENDENCIES: (
      (STLCompanion_checkpoint.o)
      (STLCompanion_post_checkpoint.o)
      (STLCompanion_restart.o)
    )
*/

#ifndef STLCOMPANION_HH
#define STLCOMPANION_HH

#include <string>

#include "STLCheckpoint.hh"

/* STLCheckpoint's friend

This class's sole purpose in life is to keep up the STLs in the STLCheckpoint class.
Since this is a friend to the STLCheckpoint class, the methods of this class work
with the STLs in STLCheckpoint directly.
*/

class STLCompanion {

    public:

        STLCompanion() {} ;

        /* writes out the STL data in arrays in the memory manager */
        int checkpoint(STLCheckpoint * in_stlc, std::string var_name) ;

        /* deletes the STL data in arrays from the memory manager */
        int post_checkpoint(STLCheckpoint * in_stlc, std::string var_name) ;

        /* restores memory from a checkpoint.  the data resides in memory
           manager.  deletes the STL data in arrays after restoration */
        int restart(STLCheckpoint * in_stlc, std::string var_name) ;
} ;

#endif

