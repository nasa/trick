#ifndef ALLOCINFO_HH
#define ALLOCINFO_HH

#include <stdexcept>
#include <vector>
#include <string>

class DataType;
class CompositeDataType;
class ArrayDataType;
class TypeDictionary;
class ChkPtAgent;
class MemMgr;
class LexicalAnalyzer;
struct VarAccessInfo;


// FIXME: the follow define should be in a more appropriate place.
#define TRICK_MAX_INDEX 8

namespace StorageClass {
    enum e {
        LOCAL  = 0,
        EXTERN = 1
    };
};

/**
 * The AllocInfo class describes a chunk of memory that the MemoryManager knows about.
 */

class AllocInfo {

    public:
    /**
      AllocInfo constructor.
      @param varName The given name of the allocation  .
      @param typeSpecName
      @param dimensionsCount
      @param dimensions
      @param typeDictionary
      @param suppliedAllocation
     */
    AllocInfo( const std::string& varName,
               const std::string& typeSpecName,
               unsigned int       dimensionsCount,
               int*               dimensions,
               TypeDictionary*    typeDictionary,
               void*              suppliedAllocation = 0)  ;
    /**
      AllocInfo destructor.
     */
    AllocInfo( const std::string& declaration,
               TypeDictionary*    typeDictionary,
               void*              suppliedAllocation = 0)  ;
    /**
      AllocInfo destructor.
     */
    ~AllocInfo();

    /**
     @return the name given to the allocation.
     Tested in: AllocInfoTest::getters1 AllocInfoTest::getters2.
     */
    std::string getName() const;

    std::string getSerialName() const;
    unsigned int getSerialNumber() const;

    /**
     @return the starting address of the allocation.
     Tested in: AllocInfoTest::getters1, AllocInfoTest::getters2.
     */
    void * getStart() const;

    /**
     @return the size of the allocation in bytes.
     Tested in: AllocInfoTest::getters1, AllocInfoTest::getters2.
     */
    int getSize() const;

    /**
     @return the type specifier.
     Tested in: AllocInfoTest::getters1, AllocInfoTest::getters2.
     */
    std::string getTypeSpecifierName() const;

    /**
     @return the number of dimensions.
     Tested in: AllocInfoTest::getters1, AllocInfoTest::getters2.
     */
    unsigned int getDimensionsCount() const;

    /**
     @param n index of the dimension.
     @return the size of the nth dimension.
     Tested in: AllocInfoTest::getters1, AllocInfoTest::getters2.
     */
    int getDimensionSize( unsigned int n ) const;

    /**
     @return the DataType for the allocation.
     Tested in: AllocInfoTest::getters1, AllocInfoTest::getters2.
     */
    const DataType * getDataType() const;

    /**
     @return the size of the nth dimension.
     Tested in: AllocInfoTest::getters1, AllocInfoTest::getters2.
     */
    StorageClass::e getStorageClass() const;

    /**
     Clear the every element of the allocation to 0.
     Tested in: AllocInfoTest::clear.
     */
    void clear() const;

    /**
     */
    void* resize( size_t newElementCount );

    /**
     @return true, if the allocation contains the given address. Otherwise return false.
     Tested in: FIXME
     */
    bool contains(void* address) const;

    /**
     */
    bool getVarAccessInfo( LexicalAnalyzer* lexer, VarAccessInfo& varAccessInfo );

    /**
     Add this and the dependencies of this to the vector if they are not already there.
     Tested in: FIXME
     */
    void appendDependencies( MemMgr* memMgr,
                             std::vector<AllocInfo*>& dependencies ) ;

    /**
     Tested in: FIXME
     */
    void appendDependenciesfromComposite( const CompositeDataType * compDataType,
                                          void *address,
                                          MemMgr* memMgr,
                                          std::vector<AllocInfo*>& dependencies) ;

    /**
     Tested in: FIXME
     */
    void appendDependenciesfromArray( const ArrayDataType* arrayDataType,
                                      void* address,
                                      MemMgr* memMgr,
                                      std::vector<AllocInfo*>& dependencies) ;
    /**
     Tested in: FIXME
     */
    std::string toString() const;


    private:

    void initialize( const std::string& varName,
                     const std::string& typeSpecName,
                     unsigned int       dimensionsCount,
                     int *              dimensions,
                     TypeDictionary*    typeDictionary,
                     void*              suppliedAllocation )  ;

    std::string     name;                        /**< ** Object name */
    void*           start;                       /**< ** Address where this allocation starts */
    void*           end;                         /**< ** Address where this allocation ends */

    /* The following three data members describe the type and are used to create the data type below. */
    std::string     typeSpecifierName;           /**< ** Type-specifier */
    unsigned int    dimensionsCount;             /**< ** The number of dimensions */
    int             dimensions[TRICK_MAX_INDEX]; /**< ** Size of each dimension */

    ArrayDataType*  ownDataType;                 /**< **  */
    const DataType* dataType;                    /**< **  */

    StorageClass::e storageClass;                /**< ** Storage class */

    unsigned int serialNumber;
    static unsigned int nextSerialNumber;

    static const std::string localPrefix;
    static const std::string externPrefix;
} ;
#endif
