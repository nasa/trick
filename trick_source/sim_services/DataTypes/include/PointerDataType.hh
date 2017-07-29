#ifndef POINTER_DATA_TYPE_HH
#define POINTER_DATA_TYPE_HH

#include "DataType.hh"
#include <stddef.h>
#include <string>
#include <stdexcept>

/**
 */
class PointerDataType : public DataType {

    public:

    /**
     Constructor for PointerDataType.
     @param typeDictionary The Type Dictionary.
     @param typeSpecifierName Name of the type on with this type is based.
     @param n_cdims Number of dimensions. Cannot be 0.
     @param dims dimension sizes.
     */
    PointerDataType( TypeDictionary * typeDictionary,
                     std::string typeSpecifierName,
                     unsigned int n_dims,
                     int dims[] ) ;
    /**
     Constructor for ArrayDataType.
     @param typeDictionary The Type Dictionary.
     @param typeSpecifierName Name of the type on with this type is based.
     @param dims dimension sizes.
     */
    PointerDataType( TypeDictionary * typeDictionary,
                     std::string typeSpecifierName ) ;

    /* ==================================================================== */
    /*                         RULE OF THREE INTERFACE                      */
    /* ==================================================================== */

    /**
     Copy Constructor for PointerDataType.
     */
    PointerDataType ( const PointerDataType & original );

    /**
     Destructor for PointerDataType.
     */
    ~PointerDataType ();

    /**
     Assignment operator for PointerDataType.
     @param rhs right-hand-side.
     */
    PointerDataType& operator=( const PointerDataType & rhs );

    /* ==================================================================== */
    /*                          VIRTUAL INTERFACE                         */
    /* ==================================================================== */

    /**
     */
     bool validate();

    /**
     @return The size (in bytes) of an instance of the PointerDataType.
     */
    size_t getSize() const ;

    /**
     */
    TypeClass::e getTypeClass() const {
        return TypeClass::POINTER;
    }

    /**
     */
    bool containsPointers() const;

    /**
     */
    DataType * clone () const;

    /**
     Create zero or more instances of this DataType.
     */
    void* createInstance(unsigned int num) const ;

    /**
     */
    void deleteInstance(void* address) const;

    /**
     */
    void clearValue(void * address) const;

    /**
     */
    void assignValue(void * address, Value * value) const;

    /**
     @param s The stream to print to.
     @param base_addr Address of the (entire) variable.
     */
    void printValue(std::ostream &s, void * address ) const;

    /**
     */
    std::string getTypeSpecName() const;

    /**
     */
    std::string makeDeclaration(std::string declarator, bool isDirectDeclarator) const;

    /**
     PointerDataType class specific method that returns the DataType to which this pointer is pointing.
     */
    const DataType * getSubType() const;


    private:
    bool initPointerDataType( TypeDictionary * typeDictionary, std::string typeSpecName, unsigned int n_dims, int dims[]);
    PointerDataType(){}

    bool             is_valid;
    std::string      typeSpecName;
    unsigned int     elementCount;
    DataType *       ownDataType;
    const DataType * subType;
    TypeDictionary * typeDictionary;

};
#endif
