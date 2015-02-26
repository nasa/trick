#ifndef STRUCT_MEMBER_H
#define STRUCT_MEMBER_H

#include <string>
#include <vector>

class Value;
class AllocInfo;
class TypeDictionary;
class MemMgr;

namespace MemberClass {
    enum e {
        NORMAL   = 0,
        STATIC   = 1,
        BITFIELD = 2
    };
};

/**
 StructMember represents a data-member of a CompositeDeclaration.
 */
class StructMember {

public:

    /**
     Constructor for StructMember.
     @param name The name of the member.
     */
    StructMember(std::string name);

    /**
    Constructor for StructMember.
    @warning This should ONLY be called by a constructor of a derived class.
    */
    StructMember (const StructMember & original);

    /**
    Clone.
    */
    virtual StructMember * clone () const = 0;

    /**
    Destructor.
    */
    virtual ~StructMember () { /* Nothing to do */ }

    /**
     Get the name of the member.
     @return std::string containing the name of the member.
     */
    std::string getName() const;

    /**
    */
    virtual MemberClass::e getMemberClass() const = 0;

    /**
     */
    virtual bool validate() = 0;

    /**
     @return does the DataType or any member of the DataType represent a pointer?
     */
    virtual bool containsPointer() const { return false; }

    /**
     */
    virtual void clearValue(void *struct_address) const = 0;

    /**
     Assign a value to the data-member, described by this StructMember,
     that is within in an instance of the composite-type described by
     the parent CompositeDeclaration.

     @param struct_address Address of an instance of the composite type.
     */
    virtual void assignValue(void *struct_address,  Value *v) const = 0;

    /**
     @param s
     @param strcut_address
     */
    virtual void printValue(std::ostream &s, void *struct_address) const = 0;

    /**
    Get a string representation of this StructMember.
     */
    virtual std::string toString() const = 0;

private:
    std::string name;

};
#endif
