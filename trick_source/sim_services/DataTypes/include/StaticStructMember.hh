#ifndef STATIC_STRUCT_MEMBER_H
#define STATIC_STRUCT_MEMBER_H

#include "StructMember.hh"
#include "DataType.hh"
#include <string>

/**
 */
class StaticStructMember : public StructMember {

public:

    /**
     Constructor.
     */
    StaticStructMember( std::string memberName,
                        void * memberAddress,
                        TypeDictionary* typeDictionary,
                        std::string typeSpecName,
                        unsigned int n_dims,
                        int dims[] );

    /**
     Copy Constructor.
    */
    StaticStructMember ( const StaticStructMember & original );

    /**
     Clone.
     */
    StructMember * clone () const;

    /**
     Destructor.
     */
    ~StaticStructMember();

    /**
     */
    bool validate();

    /**
    */
    MemberClass::e getMemberClass() const {
        return MemberClass::STATIC;
    }

    /**
     */
    bool containsPointers() const ;

    /**
      To check for circular reference only.
     */
    const DataType*  getDataType();

    /**
     */
    void* getAddress();

    /**
     */
    void clearValue(void *struct_address) const;

    /**
     */
    void assignValue(void *struct_address, Value *v) const;

    /**
     */
    void printValue(std::ostream &s, void *struct_address) const;

    /**
     Product a string representation of this StaticStructMember.
     */
    std::string toString() const;

private:
    //int byte_offset;
    void * memberAddress;
    bool is_valid;
    std::string typeSpecName;
    DataType * declDataType;
    const DataType * subType;
    TypeDictionary* typeDictionary;

};
#endif
