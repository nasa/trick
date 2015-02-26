#ifndef MEMMGR_HH
#define MEMMGR_HH

#include <vector>
#include <map>
#include <string>
#include <pthread.h>

class AllocInfo;
class DataType;
class TypeDictionary;
class ChkPtAgent;
class ClassicChkPtAgent;

class MemMgr {

    public:
    MemMgr();

    void* declare_var( const std::string& typeSpecName, const std::string& varName, unsigned int DimensionsCount, int *Dimensions, void* suppliedAllocation = 0);
    void* declare_var( const std::string& declaration, void* suppliedAllocation = 0);
    void* declare_var( const std::string& typeSpecName, int n, void* suppliedAllocation = 0);
    void* declare_var( const std::string& typeSpecName, const std::string& varName, int n, void* suppliedAllocation = 0);

//    void* declare_extern_var( void* address, const char * declaration);
//    void* declare_extern_var( void* address, const char *element_definition, int n_elems);
//    void* declare_extern_var( void* address, TRICK_TYPE type, std::string class_name, int n_stars, std::string var_name, int n_cdims, int *cdims);

    void* resize_var( void* address, size_t newElementCount);
    void* resize_var( const std::string& name, size_t newElementCount);

//    char* mm_strdup( const char* s);

    bool var_exists( const std::string& var_name);
//    size_t sizeof_type( const char* declaration);
//    int is_alloced( void* address);
//    void set_debug_level( int level);
//    void set_reduced_checkpoint(bool flag);
//    void set_hexfloat_checkpoint(bool flag);
    void clear_var( void* address);
    void clear_var( const std::string& var_name);
    void clear_all_vars();

//    int delete_var(void* address, bool destroy = true);
//    int delete_var(const char* var_name);
//    int delete_extern_var(void* address);
//    int delete_extern_var(const char* var_name);

//    void write_var( std::ostream& out_s, const char* var_name );
//    void write_var( std::ostream& out_s, void* address );

    void write_checkpoint( std::ostream& out_s);
    void write_checkpoint( const std::string& filename);
    void write_checkpoint( std::ostream& out_s, const std::string& variableName );
    void write_checkpoint( const std::string& filename, const std::string& variableName);
    void write_checkpoint( std::ostream& out_s, std::vector<const char*>& variableNameList);
    void write_checkpoint( const std::string& filename, std::vector<const char*>& variableNameList);
    void write_checkpoint(std::ostream& outStream, std::vector<AllocInfo*>& dependencies);

//    int read_checkpoint( std::istream* in_s);
//    int read_checkpoint( const char* filename);
//    int read_checkpoint_from_string( const char* s );

//    int init_from_checkpoint( std::istream* in_s);
//    int init_from_checkpoint( const char* filename);

//    void reset_memory();

//    int add_attr_info(std::string& type_name , ATTRIBUTES* attr , const char * file_name = "None" , unsigned int line_num = 0 ) ;
//    int add_template_name_trans(std::string template_name , std::string attr_name) ;
//    void* add_var( TRICK_TYPE   type, const char*  class_name, VAR_DECLARE* var_declare, char* units);
//    int add_vars( TRICK_TYPE  type, const char* class_name, VAR_LIST*   var_list, char*       units);
//    int ref_allocate( REF2* R, int num ) ;
//    REF2 *ref_attributes( const char* name);
//    int ref_assignment( REF2* R, V_TREE* V);
//    int ref_var( REF2* R, char* name);
//    int ref_dim( REF2* R, V_DATA* V);
//    int ref_name(REF2 * R, char *name);
//    int get_enumerated(const char* name, V_DATA* v_data); (see EnumDictionary.hh)
//    int get_size(char *ptr);
//    int get_truncated_size(char *ptr);
//    int io_get_fixed_truncated_size(char *ptr, ATTRIBUTES * A, char *str, int dims, ATTRIBUTES * left_type);
    AllocInfo* getAllocInfoOf( void* address );
    AllocInfo* getAllocInfoAt( void* address );
    AllocInfo* getAllocInfoNamed( const std::string& name );
    const DataType* getDataType(std::string typeName);
//    int add_shared_library_symbols( const char * file_name );
//    CheckPointAgent * get_CheckPointAgent();
//    void set_CheckPointAgent( CheckPointAgent* agent);
//    void reset_CheckPointAgent();
//    void write_class( std::ostream& out_s, char *address, ATTRIBUTES * A);
//    void write_arrayed_class( std::ostream& out_s, char* address, ATTRIBUTES* A, int curr_dim, int offset);
//    size_t io_src_sizeof_user_type(const char* user_type_name);

    private:
    unsigned int debugLevel;

    ChkPtAgent* currentCheckPointAgent;
    ClassicChkPtAgent* defaultCheckPointAgent;

    bool reducedCheckpoint;
    bool hexfloatCheckpoint;
    bool compactArraysCheckpoint;

    TypeDictionary* typeDictionary;

    pthread_mutex_t allocInfoMapMutex;

    std::map<void*, AllocInfo*> allocInfoByAddressMap;
    std::map<std::string, AllocInfo*> allocInfoByNameMap;

};
#endif
