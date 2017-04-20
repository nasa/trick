#ifndef CHECKPOINTAGENT_HH
#define CHECKPOINTAGENT_HH
/*
    PURPOSE: ( CheckPointAgent - virtual base class for a component that
               writes an external representation of the simulation state.)
*/
#include "trick/attributes.h"
#include "trick/io_alloc.h"
#include <stdio.h>
#include <iostream>
#include <vector>

namespace Trick {

    class MemoryManager;

    typedef enum {
        BASE_NAME = 0,
        ELEM_NAME = 1,
        ARRAY_INDEX = 2
    } VarNameElementType;


    typedef struct {
        VarNameElementType type;
        int index;
        std::string name;
    } VarNameElement;

/**
 Base Class for dumping a checkpoint.
 */
    class CheckPointAgent {

        public:


        /**
         */
        virtual ~CheckPointAgent() {};

        /**
         Test incoming attributes permission check.
         */
        virtual bool input_perm_check(ATTRIBUTES * attr) = 0 ;

        /**
         Test incoming attributes permission check.
         */
        virtual bool output_perm_check(ATTRIBUTES * attr) = 0 ;

        /**
         Write a declaration that describes the given ALLOC_INFO object.
         */
        virtual void write_decl( std::ostream& chkpnt_os, ALLOC_INFO *alloc_info) =0;

        /**
         Push base name onto the left-side name stack.
         */
        virtual void push_basename( const char* name) ;

        /**
         Push struct element name onto the left-side name stack.
         */
        virtual void push_struct_elem( const char* name) ;

        /**
         Push array element index onto the left-side name stack.
         */
        virtual void push_array_elem( int index) ;

        /**
         Pop element from the left-side name stack;
         */
        virtual void pop_elem() ;

        /**
         Write an assignment statement that assigns the specified
         object to the name on the left-side stack.
         */
        virtual void assign_rvalue( std::ostream& chkpnt_os,
                                    void*       address,
                                    ATTRIBUTES* attr,
                                    int         curr_dim,
                                    int         offset
                                    )=0;
        /**
         Restore Checkpoint.
         */
        virtual int restore( std::istream* checkpoint_stream)=0;

        /**
         */
        void set_reduced_checkpoint(bool flag);
        void set_hexfloat_checkpoint(bool flag);
        void set_debug_level(int level);

        bool reduced_checkpoint;  /**< ** Reduced Checkpoint flag. */
        bool hexfloat_checkpoint; /**< ** HexFloat Checkpoint flag. */
        int  debug_level;         /**< ** Debug Level. */
        std::vector < VarNameElement > leftside_stack; /**< ** Left-side name stack. */


    };
}
#endif
