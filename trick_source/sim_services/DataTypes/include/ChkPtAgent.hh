#ifndef CHKPTAGENT_HH
#define CHKPTAGENT_HH

#include <stdio.h>
#include <iostream>

class AllocInfo;
class DataType;
class MemMgr;

/**
 Base Class for dumping a checkpoint.
 */
    class ChkPtAgent {

        public:

        /**
         */
        virtual ~ChkPtAgent() {};

        /**
         Push base name onto the left-side name stack.
         */
        virtual void pushName( const std::string& name ) = 0;

        /**
         Push array element index onto the left-side name stack.
         */
        virtual void pushIndex( int index ) = 0;

        /**
         Pop element from the left-side name stack;
         */
        virtual void popNameElement() = 0;

        /**
         Write a declaration that describes the given ALLOC_INFO object.
         @return whether an error-condition has happened.
         */
        virtual bool writeDeclaration( std::ostream& chkpnt_os,
                                       std::string name,
                                       const DataType * dataType) = 0;
        /**
         @return whether an error-condition has happened.
         */
        virtual bool writeVariable( std::ostream& out_s,
                                    MemMgr* memMgr,
                                    void* address,
                                    const DataType* dataType ) = 0;
        /**
         Restore Checkpoint.
         */
        virtual int restore( std::istream* checkpoint_stream) = 0;

    };
#endif
