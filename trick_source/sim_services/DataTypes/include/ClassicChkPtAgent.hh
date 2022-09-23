#ifndef CLASSIC_CHKPTAGENT_HH
#define CLASSIC_CHKPTAGENT_HH

#include "ChkPtAgent.hh"
#include <stdio.h>
#include <iostream>
#include <vector>

class ArrayDataType;
class PointerDataType;
class CompositeDataType;

/**
 Base Class for dumping a checkpoint.
 */
    class ClassicChkPtAgent : public ChkPtAgent {

        public:

        /**
         */
         ClassicChkPtAgent( std::ostream& errs ) : errorStream(errs) {
            reducedCheckpoint = true;
            hexfloatCheckpoint = false;
            makeCompactArrays = true;
            debugLevel = 0;
         };

        /**
         */
         ~ClassicChkPtAgent() {};

        /**
         Push base name onto the left-side name stack.
         Tested in: ClassicChkPtAgentTest::nameStackFunctions.
         */
        void pushName( const std::string& name );

        /**
         Push array element index onto the left-side name stack.
         Tested in: ClassicChkPtAgentTest::nameStackFunctions.
         */
        void pushIndex( int index );

        /**
         Pop element from the left-side name stack;
         Tested in: ClassicChkPtAgentTest::nameStackFunctions.
         */
        void popNameElement();

        /**
         @return the current name in the left-side-name stack.
         Tested in: ClassicChkPtAgentTest::nameStackFunctions.
         */
        std::string getLeftSideName();

        /**
         Write a declaration that describes the data-type of the given AllocInfo object.
         */
        bool writeDeclaration( std::ostream& out_s,
                               std::string name,
                               const DataType * dataType );

        //bool writeDeclaration( std::ostream& out_s, const std::string& name, const DataType* dataType );

        /**
         Write a list of assignment statements that describe the contents of the
         object, of the given data-type, at the given address.
         */
        bool writeVariable( std::ostream& out_s,
                            MemMgr * memMgr,
                            void * address,
                            const DataType * dataType );
        /**
         Restore Checkpoint.
         */
        int restore( std::istream* checkpoint_stream);

        /**
         Checkpoint writing utilities.
         */
        bool writeArrayAssignments( std::ostream&         out_s,
                                    MemMgr *              memMgr,
                                    void *                address,
                                    const ArrayDataType * arrayDataType);

        /**
         */
        bool writePointerValue( std::ostream&         out_s,
                                MemMgr *              memMgr,
                                void *                address,
                                const PointerDataType * pointerDataType);

        /**
         */
        bool writeCompositeAssignments( std::ostream&             out_s,
                                        MemMgr *                  memMgr,
                                        void *                    address,
                                        const CompositeDataType * compDataType);

        /**
         Tested in: ClassicChkPtAgentTest::writeAssignment.
         */
        void writeAssignment( std::ostream& out_s,
                              std::string   rhs);

        /**
         Tested in: ClassicChkPtAgentTest::getSubName1,
                    ClassicChkPtAgentTest::getSubName2,
                    ClassicChkPtAgentTest::getSubName3.
         */
        bool getSubName( std::ostream& nameStream,
                         void * searchAddress,
                         const DataType * searchType,
                         void * baseAddress,
                         const DataType * baseType );

        /**
         Tested in: ClassicChkPtAgentTest::getSubName1,
                    ClassicChkPtAgentTest::getSubName2,
                    ClassicChkPtAgentTest::getSubName3.
         */
        bool getCompositeSubName( std::ostream& nameStream,
                                  void * searchAddress,
                                  const DataType * searchType,
                                  void * baseAddress,
                                  const CompositeDataType * baseType );
        /**
         Tested in: ClassicChkPtAgentTest::getSubName2,
                    ClassicChkPtAgentTest::getSubName3.
         */
         bool getArraySubName( std::ostream& nameStream,
                               void * searchAddress,
                               const DataType * searchType,
                               void * baseAddress,
                               const ArrayDataType * baseType );

        void setReducedCheckpoint(bool flag);
        void setHexfloatCheckpoint(bool flag);
        void setMakeCompactArrays(bool flag);
        void setDebugLevel(int level);

        private:

        std::ostream& errorStream;
        bool reducedCheckpoint;  /**< ** Reduced Checkpoint flag. */
        bool hexfloatCheckpoint; /**< ** HexFloat Checkpoint flag. */
        bool makeCompactArrays;  /**< ** . */
        int  debugLevel;         /**< ** Debug Level. */

        std::vector <std::string> leftSideNameStack;
    };
#endif
