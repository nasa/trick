#ifndef COMPOSITE_DATA_TYPE_H
#define COMPOSITE_DATA_TYPE_H

#include "DataType.hh"
#include "StructMember.hh"
#include "BitfieldStructMember.hh"
#include "Value.hh"
#include "VarAccessInfo.hh"
#include <vector>
#include <string>
#include <stdexcept>

class LexicalAnalyzer;

/**
 A CompositeDataType represents user-defined types, such as structs, unions, or classes.
 It is 'composed' of ordered lists of component types.
 */
class CompositeDataType : public DataType {

public:

    /**
     Constructor for CompositeDataType.
     @param sizeof_struct Size, in bytes, of the struct, class or union that this class represents.
     */
    CompositeDataType( TypeDictionary* typeDictionary,
                       std::string name,
                       size_t sizeof_struct,
                       void *(*allocator)(int),
                       void (*deAllocator)(void*) );

    /* ==================================================================== */
    /*                         RULE OF THREE INTERFACE                      */
    /* ==================================================================== */

    /**
     Copy Constructor.
     @param original The instance of CompositeDataType that is to be copied.
     */
    CompositeDataType ( const CompositeDataType & original );

    /**
     Destructor for CompositeDataType.
      */
    ~CompositeDataType ();

    /**
     Assignment operator for CompositeDataType.
     @param rhs right-hand-side.
    */
    CompositeDataType & operator=( const CompositeDataType & rhs );

    /* ==================================================================== */
    /*                          VIRTUAL INTERFACE                         */
    /* ==================================================================== */

    /**
     @return does the DataType or any member of the DataType represent a pointer?
     */
     bool containsPointer() const;

    /**
     */
    DataType * clone() const;

    /**
     */
    bool validate() ;

    /**
     @return The size (in bytes) of an instance of the CompositeDataType.
     */
    size_t getSize() const ;

    /**
     */
    TypeClass::e getTypeClass() const {
        return TypeClass::COMPOSITE;
    };

    /**
     */
    void* createInstance(unsigned int num) const;

    /**
     */
    void deleteInstance(void* address) const;

    /**
     */
    void clearValue(void * address) const;

    /**
     Assign a value to the variable at the given address.
     @param address Base-address of the (possibly arrayed) variable.
     @param value The Value to be assigned to the element.
     */
    void assignValue(void * address, Value * value) const;

    /**
     Print the value at the given address, to the given stream.
     @param s The stream to print to.
     @param address Address of the (entire) variable.
     */
    void printValue(std::ostream &s, void *address) const;

    /**
     */
    std::string toString() const;

    /* ==================================================================== */
    /*                       CLASS SPECIFIC INTERFACE                       */
    /* ==================================================================== */

    /**
     Add a regular, non-bitfield, non-static data member to the CompositeDataType.
     @param memberName Name of the data member.
     @param offset The offset (in bytes) of the data-member from the beginning of
     the struct, union or class.
     @param typeName
     @param n_dims
     @param dims
     */
    void addRegularMember( std::string memberName,
                           int offset,
                           std::string typeName,
                           unsigned n_dims,
                           int dims[]) ;

    /**
     Add a static data member to the CompositeDataType.
     @param memberName Name of the data member.
     @param memberAddress The offset (in bytes) of the data-member from the beginning of
     the struct, union or class.
     @param typeName
     @param n_dims
     @param dims
     */
    void addStaticMember(std::string memberName,
                         void * memberAddress,
                         std::string typeSpecName,
                         unsigned int n_dims,
                         int dims[] ) ;

    /**
     Add a bitfield data member to the CompositeDataType.
     @param member_name Name of the data member.
     @param getter Pointer to a function that returns the value of a bitfield in the addressed class/struct.
     @param getter Pointer to a function that sets the value of a bitfield in the addressed class/struct.
     */
    template <class T> void addBitFieldMember( std::string member_name,
                                               T(*getter)(void* address),
                                               void(*setter)(void* address, T value) ) {

        memberList.push_back( new BitfieldStructMember<T>(member_name, getter, setter));

    }

    /**
     */
    std::string getTypeSpecName() const;

    /**
     */
    int getMemberCount() const;

    /**
     */
    StructMember* getStructMember (const int index) const;

    /**
     */
    bool getMemberInfo( LexicalAnalyzer* lexer, void* baseAddress, VarAccessInfo& varAccessInfo );

private:
    CompositeDataType();

    bool is_valid;
    std::vector<StructMember*> memberList;
    std::vector<StructMember*>::iterator memberListIterator;
    std::string name;
    size_t structSize; /** Sizeof the struct/or class represented by the CompositeDataType. */
    void* (*allocator)(int);
    void (*deAllocator)(void*);
    TypeDictionary* typeDictionary;
};
#endif
