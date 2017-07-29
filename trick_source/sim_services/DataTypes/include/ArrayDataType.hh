#ifndef ARRAY_DATA_TYPE_HH
#define ARRAY_DATA_TYPE_HH

#include "DataType.hh"
#include <stddef.h>
#include <string>
#include <stdexcept>
#include <VarAccessInfo.hh>

#define MAX_CONSTRAINED_DIMS 8

class LexicalAnalyzer;

/**
 */
class ArrayDataType : public DataType {

    public:
    /**
     Constructor for ArrayDataType.
     @param typeSpecifierName Name of the type on with this type is based.
     @param n_ptrs Number of pointers.
     @param n_cdims Number of dimensions. Cannot be 0.
     @param dims dimension sizes.
     */
    ArrayDataType( TypeDictionary* typeDictionary,
                   std::string typeSpecifierName,
                   unsigned int n_dims,
                   int dims[] )  ;
    /**
     Constructor for ArrayDataType.
     @param typeDictionary The Type Dictionary.
     @param typeSpecifierName Name of the type on with this type is based.
     @param dims dimension sizes.
     */
    ArrayDataType( TypeDictionary* typeDictionary,
                   std::string typeSpecName,
                   unsigned int dimensionSize ) ;

    /**
     Create an ArrayDataType with a different elementCount, but otherwise
     identical to the original.
     */
    ArrayDataType ( const ArrayDataType & original, unsigned int newSize );

    /* ==================================================================== */
    /*                         RULE OF THREE INTERFACE                      */
    /* ==================================================================== */

    /**
     Copy Constructor for ArrayDataType.
     */
    ArrayDataType ( const ArrayDataType & original );

    /**
     Destructor for ArrayDataType.
     */
    ~ArrayDataType ();

    /**
     Assignment operator for ArrayDataType.
     @param rhs right-hand-side.
     */
    ArrayDataType& operator=( const ArrayDataType & rhs );

    /* ==================================================================== */
    /*                          VIRTUAL INTERFACE                         */
    /* ==================================================================== */

    /**
     */
    bool containsPointers() const;

    /**
     */
    DataType * clone () const;

    /**
     */
     bool validate();

    /**
     @return The size (in bytes) of an instance of the ArrayDataType.
     */
    size_t getSize() const ;

    TypeClass::e getTypeClass() const {
        return TypeClass::ARRAY;
    }

    /**
     @return The number of arrayed Type-specifier elements of the DataType.
    */
    unsigned int getTotalElementCount() const;

    /**
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

    /* ==================================================================== */
    /*                       CLASS SPECIFIC INTERFACE                       */
    /* ==================================================================== */

    /**
     */
    bool getElementInfo( LexicalAnalyzer* lexer, void* baseAddress, VarAccessInfo& varAccessInfo);

    /**
     @return 0 if the ArrayDataType represents a pointer,
     otherwise return the number of elements in the array.
     */
    unsigned int getElementCount() const;

    /**
     */
    const DataType * getSubType() const;


    private:
    bool initArrayDataType( TypeDictionary* typeDictionary, std::string typeSpecName, unsigned int n_dims, int dims[] );
    ArrayDataType(){}

    size_t typeSize;
    unsigned int elementCount;
    bool is_valid;
    std::string typeSpecName;
    DataType * ownDataType;
    const DataType * subType;
    TypeDictionary* typeDictionary;

};
#endif
