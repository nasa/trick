/*
    PURPOSE:
        (Memory Manager.)
*/

#ifndef MEMORYMANAGEMENT_HH
#define MEMORYMANAGEMENT_HH

#include <map>
#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <stdexcept>
#include <pthread.h>
#include <functional>

#include "trick/attributes.h"
#include "trick/reference.h"
#include "trick/parameter_types.h"
#include "trick/io_alloc.h"
#include "trick/mm_error.h"
#include "trick/var.h"

#include "trick/CheckPointAgent.hh"

// forward declare the units converter types used by ref_assignment
union cv_converter ;
struct ut_system ;

namespace Trick {

    typedef std::map<void*, ALLOC_INFO*, std::greater<void*> > ALLOC_INFO_MAP;
    typedef std::map<void*, ALLOC_INFO*, std::greater<void*> >::const_iterator ALLOC_INFO_MAP_ITER ;
    typedef std::map<std::string, ALLOC_INFO*> VARIABLE_MAP;
    typedef std::map<std::string, ALLOC_INFO*>::const_iterator VARIABLE_MAP_ITER ;
    typedef std::map<std::string, ENUM_ATTR*> ENUMERATION_MAP;

/**
  The Memory Manager provides memory-resource administration services.
  To provide these services, it tracks information about chunks of memory.

  These chunks of memory can come from one of two places:
  @li You can ask the Memory Manager to allocate memory according a
      declaration [using declare_var()].
  @li You can provide an address to memory that you've somehow acquired,
      together with a declaration of what's at that address
      [using declare_extern_var()].

  By tracking the address and declaration information, the Memory Manager
  it is able, on request, to write a human-readable representation of those memory
  resources to a file [using write_checkpoint()]. The memory resources can be later
  restored from this checkpoint file using read_checkpoint() or init_from_checkpoint().

  It can also return addresses of named subcomponents of memory resources.

 */
    class MemoryManager {

        public:

            /**
             Memory Manager constructor.
             @par NOTE:
             Only one instance of the MemoryManager is allowed to exist per process.
             */
            MemoryManager() ;

            /**
             Memory Manager destructor.
             */
            ~MemoryManager();

            /**
             Allocate a contiguous region of memory as specified by an
             @subpage ADEF_Syntax "allocation declaration string."

             @param declaration - a \ref definition_of_type_specifier "type specifier"
              followed by zero or more asterisks, followed by an optional name,
              followed by zero or more bracketed integers.

             @return - an address to the allocated memory or NULL on failure.

             */
            void* declare_var( const char* declaration);

            /**
             Allocate an anonymous, one dimensional array. The elements of the array are specified by the
             enhanced-type-specifier. The length of the array is specified by @b n_elems.

             @param enh_type_spec - type specifier followed by zero or more asterisks.
             @param n_elems - The number of items of the given type to allocate.
             @return - an address to the allocated memory or NULL on failure.
             */
            void* declare_var( const char* enh_type_spec, int n_elems);

            /**
             This is the general version of declare_var(), which allocates a contiguous
             region of memory, for a (named or anonymous) variable, of the specified type
             and dimension.

             @par NOTE:
             An allocation (variable) may by named (@b var_name != "") or anonymous
             (@b var_name == ""). Named allocations are checkpointed using their given name.
             Anonymous allocations are checkpointed with generated names of the form:
             @c trick_temp_#, where @c # is an integer.

             @param type - TRICK_TYPE.
             @param class_name - class or struct name if @b type is TRICK_STRUCTURED,
                                 otherwise @b class_name should be "".
             @param n_stars - number of asterisks in the variable declaration.
             @param var_name - (optional) name of the allocation. ="" for anonymous allocations.
             @param n_cdims - number of constrained/fixed dimensions. =0 for unarrayed variables.
             @param cdims - array of dimension sizes.
             @return - an address to the allocated memory or NULL on failure.
             */
            void* declare_var(TRICK_TYPE type, std::string class_name, int n_stars, std::string var_name, int n_cdims, int *cdims);

            /**
             Alex's desecration of the cathedral.
             This is the version of declare_var() used when we override a class specific new/delete.
             It allocates the number of bytes requested by new, and tracks an anonymous allocation
             corresponding to this memory.

             @param class_name - class or struct name if @b type is TRICK_STRUCTURED,
                                 otherwise @b class_name should be "".
             @param alloc_size - number of bytes requested by new
             @param element_size - size of underlying class
             @return - an address to the allocated memory or NULL on failure.
             */
            void* declare_operatornew_var( std::string class_name , unsigned int alloc_size , unsigned int element_size );

            /**
             This is a convenience version of declare_extern_var(), that declares an external variable
             as specified by an @subpage ADEF_Syntax "allocation declaration string."

             @param declaration - a type_specifier followed by zero or more asterisks, followed by an
              optional name, followed by zero or more bracketed integers.

             @return - an address to the declared object or NULL on failure.
             */
             void* declare_extern_var( void* address, const char * declaration);

            /**
             This is a convenience version of declare_extern_var(), that declares a
             one dimensional array. The elements of the array are specified by a
             type-specifier followed by zero or more asterisks. The length of the array
             is specified by @b n_elems.

             @param address - address of the external object.
             @param element_definition - type specifier followed by zero or more asterisks, followed by an
              optional name, followed by zero or more bracketed integers.
             @param n_elems - number of items of the given type to allocate.
             @return - an address to the declared object or NULL on failure.
             */
             void* declare_extern_var( void* address, const char *element_definition, int n_elems);

            /**
             Tell the Memory Manager about a memory object that it did not allocate.
             This makes it possible for the Memory Manager to dump (checkpoint) the object's contents.

             @param address - The address of the external object.
             @param type - TRICK_TYPE.
             @param class_name - class or struct name if @b type is TRICK_STRUCTURED,
                                 otherwise @b class_name should be "".
             @param n_stars - number of asterisks in the variable declaration.
             @param var_name - name of the declared object.
             @param n_cdims - number of constrained/fixed dimensions. =0 for unarrayed variables.
             @param cdims - array of dimension sizes.
             @return - an address to the declared object or NULL on failure.

             @attention
             The checkpoint is restorable if and only if:
             - the @b name used in the checkpoint file is that of a declared variable.
             - the io specification in the underlying ATTRIBUTES allows it.
             */
             void* declare_extern_var( void* address,
                                        TRICK_TYPE type,
                                        std::string class_name,
                                        int n_stars,
                                        std::string var_name,
                                        int n_cdims,
                                        int *cdims);

            /**
             Resize multi-dimensional array by address.
             @param address - address of array to be resized.
             @param n_cdims - number of constrained dimensions. This must match the number
                              of "fixed" dimensions of the existing array.
             @param cdims - array of new dimension sizes.
             @return - a pointer to the resized array or NULL on failure.
             */
            void* resize_array( void *address, int  n_cdims, int  *cdims);

            /**
             Resize one-dimensional array by address.
             @param address - address of array to be resized.
             @param num - number of elements in 1 dimensional array.
             @return - a pointer to the resized array or NULL on failure.
             */
            void* resize_array( void* address, int num);

            /**
             Resize multi-dimensional array by name.
             @param name - name of array to be resized.
             @param n_cdims - number of constrained dimensions. This must match the number
                              of "fixed" dimensions of the existing array.
             @param cdims - array of new dimension sizes.
             @return - a pointer to the resized array or NULL on failure.
             */
            void* resize_array( const char* name, int n_cdims, int *cdims);

            /**
             Resize one-dimensional array by name.
             @param name - name of array to be resized.
             @param num - number of elements in 1 dimensional array.
             @return - a pointer to the resized array or NULL on failure.
             */
            void* resize_array( const char* name, int num);

            /**
             Duplicate the given character string.
             @param s - character string to be duplicated.
             @return a duplicate character string.
             @todo Could this be made more general to duplicate a variable?
             */
            char* mm_strdup( const char* s);

            /**
             Test whether the given variable_name is in use .
             @return 0 = FALSE, 1 = TRUE.
             */
            int var_exists(std::string var_name);

            /**
             Determine the size of the given declaration.
             @param declaration - a \ref definition_of_type_specifier "type specifier"
              followed by zero or more asterisks, followed by an optional name,
              followed by zero or more bracketed integers.

             @return - the size in bytes of the memory that would be allocated by the by the given declaration or 0 on failure.
             */
            size_t sizeof_type( const char* declaration);

            /**
             Test whether the given address is in Trick managed memory.
             @param address - The address to be tested.
             @return 0 = FALSE, 1 = TRUE.
             */
            int is_alloced( void* address);

            /**
             Set the verbosity of MemoryManager debug messages.
             @param level - 0 = No debug messages. > 0 print debug messages.
             */
             void set_debug_level( int level);

            /**
             Indicate whether a checkpoint should represent arrays and their values as one
             assignment or as individual assignments to each of its elements.
             @param flag - true: Arrays and their values are represented as multiple assignments, one for each array element.
                           false: (default) Arrays and their values are compactly represented as one assignment.
             */
             void set_expanded_arrays( bool flag);

            /**
             Indicate whether a checkpoint should include assignments to variables whose values are 0.
             @param flag - true: (default) Assignments are created only for non zero-valued variables.
                           false: Assignments are created for all variables, regardless of their value.
             */
             void set_reduced_checkpoint( bool flag);

            /**
             Indicate whether a checkpoint should represent floating point values as "hexfloat" values.
             Hexfloat format preserves the precision of floating point values, but isn't readable by
             sane human beings.
             @param flag - false: Assignments are created for all variables, regardless of their value.
                           true: Assignments are created only for non zero-valued variables.
             */
             void set_hexfloat_checkpoint( bool flag);

            /**
             Set the value(s) of the variable at the given address to 0, 0.0, NULL, false or "", as appropriate for the type.
             @param address - The address of the variable to be cleared.
             */
            void clear_var( void* address);

            /**
             Set the value(s) of the variable with the given name to 0, 0.0, NULL, false or "", as appropriate for the type.
             @param var_name - The name of the variable to be cleared.
             */
            void clear_var( const char* var_name);

            /**
             Set the value(s) of all variables to 0, 0.0, NULL, false or "", as appropriate for the type.
             */
            void clear_all_vars();

            /**
             Forget about the variable at the given address and deallocate the memory associated with it.
             @param address - the address of the variable.
             @return 0 = SUCCESS, 1 = FAILURE
             */
            int delete_var(void* address, bool destroy = true);

            /**
             Forget about the variable with the given name and deallocate the memory associated with it.
             @param var_name - the address of the variable.
             @return 0 = SUCCESS, 1 = FAILURE
             */
            int delete_var(const char* var_name);

            /**
             Forget about the external variable at the given address. DOES NOT attempt to deallocate the
             memory at the given address.
             @param address - the address of the external variable.
             @return 0 = SUCCESS, 1 = FAILURE
             */
            int delete_extern_var(void* address);

            /**
             Forget about the external variable with the given name. DOES NOT attempt to deallocate the
             memory at the given address.
             @param var_name - the address of the external variable.
             @return 0 = SUCCESS, 1 = FAILURE
             */
            int delete_extern_var(const char* var_name);

            /**
             Checkpoint all allocations known to the MemoryManager to the given stream.
             @param out_s output stream.
             */
            void write_checkpoint( std::ostream& out_s);

            /**
             Checkpoint all allocations known to the MemoryManager to a file.
             @param filename  Name of file to be written.
             */
            void write_checkpoint( const char* filename);

            /**
             Checkpoint the named variable (allocation) and it dependencies to the given stream.
             @param out_s output stream.
             @param var_name  Variable name.
             */
            void write_checkpoint( std::ostream& out_s, const char* var_name);

            /**
             Checkpoint the named variable (allocation) and it dependencies to a file.
             @param filename  Checkpoint file.
             @param var_name  Variable name.
             */
            void write_checkpoint( const char* filename, const char* var_name);

            /**
             Checkpoint the named variables and their dependencies to a stream.
             @param out_s output stream.
             @param var_name_list List of variable names.
             */
            void write_checkpoint( std::ostream& out_s, std::vector<const char*>& var_name_list);

            /**
             Checkpoint the named variables and their dependencies to a file.
             @param filename output file name.
             @param var_name_list List of variable names.
             */
            void write_checkpoint( const char* filename, std::vector<const char*>& var_name_list);

            /**
             Restore a checkpoint from the given stream.
             @param in_s - input stream.
             */
            int read_checkpoint( std::istream* in_s, bool do_restore_stls=false);

            /**
             Read a checkpoint from the file of the given name.
             @param filename - name of the checkpoint file to be read.
             */
            int read_checkpoint( const char* filename);

            /**
             Read a checkpoint from the given string.
             @param s - string containing the checkpoint info.
             */
            int read_checkpoint_from_string( const char* s );

            /**
             Delete all TRICK_LOCAL variables and clear all TRICK_EXTERN variables. Then read
             and restore the checkpoint from the given stream.
             @param in_s - input stream.
             */
            int init_from_checkpoint( std::istream* in_s);

            /**
             Delete all TRICK_LOCAL variables and clear all TRICK_EXTERN variables. Then read
             and restore the checkpoint of the given filename.
             */
            int init_from_checkpoint( const char* filename);

            /**
             Deallocate the memory for all TRICK_LOCAL variables and then forget about them.
             Clear the memory for all TRICK_EXTERN variables.
             Does not attempt to deallocate external memory.
             */
            void reset_memory();

            /**
             Find and initialize the ATTRIBUTES of the named type. Then assign these
             (child) ATTRIBUTES to the attr field of the given (parent) ATTRIBUTES structure.
             @param type_name - user defined type name.
             @param attr      - pointer to parent attributes structure.
             @return 0 on success, otherwise failure.
             */
            int add_attr_info(const std::string& type_name , ATTRIBUTES* attr , const char * file_name = "None" , unsigned int line_num = 0 ) ;

            /**
             Adds a template name translation to our map.  The base template type is mangled to the io_src name
             @param template_name - base template type name
             @param attr_name - mangled attribute name
             @return 0 on success, otherwise failure.
             */
            int add_template_name_trans(std::string template_name , std::string attr_name) ;

            /**
             Add a variable.

             @param type - TRICK_TYPE.
             @param class_name - name of class or struct, if type == TRICK_STRUCTURED.
             @param var_declare - variable declarator.
             @param units - units associated with the variable.

             @todo this needs a better description.
             @todo get rid of the units arg. It belongs as an assignment arg.
             */
            void* add_var( TRICK_TYPE   type,
                           const char*  class_name,
                           VAR_DECLARE* var_declare,
                           char*        units);

            /**
             Add a list of variables.
             @param type - TRICK_TYPE.
             @param class_name - name of class or struct, if type == TRICK_STRUCTURED.
             @param var_list - variable declaration.
             @param units - units associated with the variable.
             @todo get rid of the units arg. It belongs as an assignment arg.
             */
            int add_vars( TRICK_TYPE  type,
                          const char* class_name,
                          VAR_LIST*   var_list,
                          char*       units);

            /**
             Allocates memory for the variable specified by R. Note that R->address
             contains an address of (pointer to) a pointer variable and R-attr contains
             the ATTRIBUTES of the variable to which the pointer variable points. If size
             is 0 then num items of the size specified by R->attr->size will be allocated.
             If the size is not 0, the num items of the size sizeof( void*) will be allocated.
             @param R Parameter reference attributes.
             @param num Number of items to allocate.
             @return 0 on success, otherwise failure.
             */
            int ref_allocate( REF2* R, int num ) ;

            /**
             Generate and return a REF2 reference object for the named variable.
             Caller is responsible for freeing the returned REF2 object (using free() from stdlib.h ).
             @param name - fully qualified variable name.
             @return pointer to REF2 object, or NULL on failure.
             */
            REF2 *ref_attributes( const char* name);

            /**
             @param address - Address for which a name reference is needed.
             @return a name reference for the given address.
             */
            std::string ref_name_from_address (void *address) ;

            /**
             Assigns values from the given value list to the memory locations
             referenced by R.
             @param R Reference to the variable to which values are to be assigned.
             @param V Tree of values to be assigned to the variable referenced by R.
             @return 0 on success, otherwise failure.
             */
            int ref_assignment( REF2* R, V_TREE* V);

            /**
             Populate the REF2 object pointed to by R with the attributes and address
             of the named variable.
             @param R - pointer to the REF2 object to be populated.
             @param name - variable name.
             @return 0 on success, otherwise failure.
             @todo rename to ref_base.
             */
            int ref_var( REF2* R, char* name);

            /**
             Updates R, a reference to an arrayed object, to a reference
             to the indexed sub-element of that arrayed object.
             @param R reference to the arrayed object of which V identifies a sub-element.
             @param V index of a sub-element the structured object referenced by R.
             @todo V_DATA arg needs to be changed to an int.
             */
            int ref_dim( REF2* R, V_DATA* V);

            /**
             Updates R, a reference to a structured object, to a reference
             to the named sub-element of that structured object.
             @param R reference to the structured object of which @b name names a sub-element.
             @param name name of a sub-element the structured object referenced by R.
             */
            int ref_name(REF2 * R, char *name);

            /**
             Return the value associated with the name.
             @param name - name of enumeration value.
             @param v_data - value associated with the name.
             */
            int get_enumerated(const char* name, V_DATA* v_data);

            /**
             Return the number of array elements in the allocation.
             @param addr Address.
             */
            int get_size(void *addr);

            /**
             Return the number of array elements in the allocation following ptr.
             @param addr Address.
             */
            int get_truncated_size(void *addr);

            /**
             @todo does this work?
             */
            int io_get_fixed_truncated_size(char *ptr, ATTRIBUTES * A, char *str, int dims, ATTRIBUTES * left_type);

            /**
             Get information for the allocation containing the specified address.
             @param addr The Address.
             */
            ALLOC_INFO* get_alloc_info_of( void* addr);

            /**
             Get information for the allocation starting at the specified address.
             @param addr The Address.
             */
            ALLOC_INFO* get_alloc_info_at( void* addr);

            /**
             Names an allocation in the ALLOC_INFO map to the incoming name.
             @param addr The Address.
             @param name The Name.
             */
            int set_name_at( void* addr, const char * name);

            /**
             Adds a name of an allocation to the dependency list of allocations that are to be checkpointed.
             This call is used by standard template library functions that create new allocations and
             dependencies during the checkpoint process.
             @param name The name of the allocation.
             */
            void add_checkpoint_alloc_dependency(const char * name);

            /**
             Opens a handle to the shared library file.  The handles are used to look for io_src functions.
             @param file_name The name of the file to open.
             */
            int add_shared_library_symbols( const char * file_name );


            std::vector<void*> dlhandles ; /**< ** dynamic loader handle left open for all dl (dlsym) calls */

            /**
             Get the current MemoryManager's CheckPointAgent
             */
            CheckPointAgent * get_CheckPointAgent();

            /**
             Set the MemoryManager's CheckPointAgent to a user-specified CheckPointAgent.
             @param agent pointer to the user-specified CheckPointAgent.
             */
            void set_CheckPointAgent( CheckPointAgent* agent);

            /**
             Set the MemoryManager's CheckPointAgent to the default, "Classic" CheckPointAgent.
             */
            void reset_CheckPointAgent();

            /**
            Write the contents of the variable at the given address, as described by the
            given attributes to the given stream.
            @param out_s - output stream.
            @param address - address of the variable.
            @param attr - data type attributes of the variable.
             */
            void write_var( std::ostream& out_s, void* address, ATTRIBUTES* attr);

            /**
            Write the contents of the variable at the given address, as described by the
            given attributes to the given stream.
            @param out_s - output stream.
            @param alloc_info - pointer to the ALLOC_INFO record for this variable.
             */
            void write_var( std::ostream& out_s, ALLOC_INFO* alloc_info );

            /**
            Write the contents of the variable with the given name to the given stream.
            In other words, checkpoint a single variable to a file.
            @param out_s - output stream.
            @param var_name - Name of the variable.
             */
            void write_var( std::ostream& out_s, const char* var_name );

            /**
            Write the contents of the variable at the given address to the given stream.
            In other words, checkpoint a single variable to a file.
            @param out_s - output stream.
            @param address - address of the variable.
             */
            void write_var( std::ostream& out_s, void* address );

            /**
             @attention This function is not meant for general use. Use write_var or write_checkpoint instead.

             Write the contents of the composite variable (struct|union|class) at the given address to the given stream.
             */
            void write_composite_var( std::ostream& out_s, void* address, ATTRIBUTES* attr_list);

            /**
             @attention This function is not meant for general use. Use write_var or write_checkpoint instead.

             Write the contents of the arrayed variable at the given address to the given stream.
             */
            void write_array_var( std::ostream& out_s, void* address, ATTRIBUTES* attr, int curr_dim, int offset);

            /**
             Make a string representation of a declaration.
             */
            std::string make_decl_string(TRICK_TYPE type, std::string class_name, int n_stars, std::string var_name, int n_cdims, int *cdims);

            /**
             Return the size of the specified user-defined type.
             @param user_type_name The name of a user-defined type.
             @return The the size (in bytes) of the user-defined type.
             */
            size_t io_src_sizeof_user_type(const char* user_type_name);

            ALLOC_INFO_MAP_ITER alloc_info_map_begin() { return alloc_info_map.begin() ; } ;
            ALLOC_INFO_MAP_ITER alloc_info_map_end() { return alloc_info_map.end() ; } ;

            VARIABLE_MAP_ITER variable_map_begin() { return variable_map.begin() ; } ;
            VARIABLE_MAP_ITER variable_map_end() { return variable_map.end() ; } ;

            int debug_level; /**< -- Debug level */
            static void emitMessage( std::string s);
            static void emitError( std::string s);
            static void emitWarning( std::string s);

            void write_JSON_alloc_info( std::ostream& s, ALLOC_INFO *alloc_info) ;
            void write_JSON_alloc_list( std::ostream& s, int start_ix, int num) ;

        private:

            static int instance_count;          /**< -- Number of instances of MemoryManager. Not allowed to exceed 1.*/
            const char* local_anon_var_prefix;  /**< -- Temporary-variable-name prefix. */
            const char* extern_anon_var_prefix; /**< -- Temporary-variable-name prefix. */
            CheckPointAgent* currentCheckPointAgent; /**< ** currently active Check point agent. */
            CheckPointAgent* defaultCheckPointAgent; /**< ** the classic Check point agent. */

            bool reduced_checkpoint;    /**< -- true = Don't write zero valued variables in the checkpoint. false= Write all values. */
            bool hexfloat_checkpoint;   /**< -- true = Represent floating point values as hexidecimal to preserve precision. false= Normal. */
            bool expanded_arrays;       /**< -- true = array element values are set in separate assignments. */

            ALLOC_INFO_MAP  alloc_info_map;  /**< ** Map of <address, ALLOC_INFO*> key-value pairs for each of the managed allocations. */
            VARIABLE_MAP    variable_map;    /**< ** Map of <name, ALLOC_INFO*> key-value pairs for each named-allocations. */
            ENUMERATION_MAP enumeration_map; /**< ** Enumeration map. */
            pthread_mutex_t mm_mutex;        /**< ** Mutex to control access to memory manager maps */

            int alloc_info_map_counter ;     /**< ** counter to assign unique ids to allocations as they are added to map */
            int extern_alloc_info_map_counter ; /**< ** counter to assign unique ids to allocations as they are added to map */

            std::vector<ALLOC_INFO*> dependencies; /**< ** list of allocations used in a checkpoint. */
            std::vector<ALLOC_INFO*> stl_dependencies; /**< ** list of allocations known to be STL checkpoint allocations */

            void execute_checkpoint( std::ostream& out_s );

            /**
             Walks through allocation and allocates space for STLs
             FIXME: I NEED DOCUMENTATION!
             */
            void get_stl_dependencies( ALLOC_INFO* alloc_info );
            void get_stl_dependencies_in_class( std::string name, char* address, ATTRIBUTES* attr) ;
            void get_stl_dependencies_in_arrayed_class( std::string name,
             char* address, ATTRIBUTES* attr, int curr_dim, int offset) ;
            void get_stl_dependencies_in_intrinsic( std::string name,
             void* address, ATTRIBUTES* attr, int curr_dim, int offset) ;

            /**
             Walks through allocations and restores STLs
             FIXME: I NEED DOCUMENTATION!
             */
            void restore_stls( ALLOC_INFO* alloc_info );
            void restore_stls_in_class( std::string name, char* address, ATTRIBUTES* attr) ;
            void restore_stls_in_arrayed_class( std::string name,
             char* address, ATTRIBUTES* attr, int curr_dim, int offset) ;
            void restore_stls_in_intrinsic( std::string name,
             void* address, ATTRIBUTES* attr, int curr_dim, int offset) ;

            /**
             FIXME: I NEED DOCUMENTATION!
             */
            void get_alloc_deps_in_allocation( ALLOC_INFO* alloc_info );
            /**
             FIXME: I NEED DOCUMENTATION!
             */
            void get_alloc_deps_in_allocation( const char* var_name );
            /**
             FIXME: I NEED DOCUMENTATION!
             */
            void get_alloc_deps_in_class( char* address, ATTRIBUTES* attr);
            /**
             FIXME: I NEED DOCUMENTATION!
             */
            void get_alloc_deps_in_arrayed_class( char* address, ATTRIBUTES* attr, int curr_dim, int offset);
            /**
             FIXME: I NEED DOCUMENTATION!
             */
            void get_alloc_deps_in_intrinsic( void* address, ATTRIBUTES* attr, int curr_dim, int offset);

            std::set< std::string > primitive_types ; /**< ** Names of primitive types.  Used in add_attr_info */
            std::map< std::string , std::string > template_name_map ; /**< ** Templates names => mangled attr names */

            /**
             Assign the value(s) in v_tree to the variable at the given address.
             The object at the address may be a singleton or an array of up to
             eight dimensions. If v_tree is NULL, then assign nil values.

             @param base_addr - Base address of the variable in which data is being assigned.
             @param attr - Attributes of the variable.
             @param curr_dim - The dimension of interest in the variable.
             @param offset - Offset to one of the elements in the dimension of interest.
             @param v_tree - RHS data to be assigned.
             @param cf - units conversion function.
             */
            int assign_recursive(void* base_addr, ATTRIBUTES* attr, int curr_dim, int offset, V_TREE* v_tree, cv_converter * cf);

            /**
             Copy the common elements from one array to another. The arrays must be of the same dimension,
             but may be different sizes in one or more of those dimensions. Said another way, we are copying
             "overlapping" array elements.

             @param s_base - source base address.
             @param d_base - destination base address.
             @param s_cdims - sizes of constrained dimensions of the source array.
             @param d_cdims - sizes of constrained dimensions of the destination array.
             @param size - sizeof an individual array element.
             @param n - number of array dimensions. s_cdims and d_cdims are both this size.
             @param cur - current dimension.
             @param s_off - source offset in the current dimension.
             @param d_off - destination offset in the current dimension.
             */
            void recursive_array_copy( void* s_base, void* d_base, int* s_cdims, int* d_cdims, size_t size, int n, int cur, int s_off, int d_off);

            /**
             Convenience version of recursive_array_copy.

             @param s_base - source base address.
             @param d_base - destination base address.
             @param s_cdims - sizes of constrained dimensions of the source array.
             @param d_cdims - sizes of constrained dimensions of the destination array.
             @param size - sizeof an individual array element.
             @param n - number of array dimensions. s_cdims and d_cdims are both this size.
             */
            void array_copy( void* s_base, void* d_base, int* s_cdims, int* d_cdims, size_t size, int n);

            /**
             {Trick_type, class_name, n_stars} ==> {sub_attr, size}
             */
            int get_type_attributes( TRICK_TYPE& type, std::string user_type_name, int n_stars, ATTRIBUTES*& sub_attr, int& size);

            /**
             Create reference attributes from the the given ALLOC_INFO record.
             @param alloc_info pointer to the ALLOC_INFO record.
             */
            ATTRIBUTES* make_reference_attr( ALLOC_INFO* alloc_info);

            /**
             Delete/free the given reference attributes.
             */
            void free_reference_attr( ATTRIBUTES* reference_attr);

            /**
             Allocate one or more instances of the named class.
             @param class_name The name of the class to allocate.
             @param num The number of instances to allocate.
             @return The address of the allocation, or NULL on failure.
             */
            void* io_src_allocate_class(const char* class_name, int num);

            /**
             Call the default destructor for one or more instances of the named class.
             @param alloc_info The alloc_info struct that contains the address and type to delete.
             */
            void io_src_destruct_class(ALLOC_INFO * alloc_info);

            /**
             Call the proper class/struct delete for the address given in the ALLOC_INFO struct.
             @param alloc_info The alloc_info struct that contains the address and type to delete.
             */
            void io_src_delete_class(ALLOC_INFO * alloc_info);

            /**
             FIXME: I NEED DOCUMENTATION!
             */
            void clear_rvalue( void* base_address, ATTRIBUTES* attr, int curr_dim, int offset);
            /**
             FIXME: I NEED DOCUMENTATION!
             */
            void clear_class( char *address, ATTRIBUTES * A);
            /**
             FIXME: I NEED DOCUMENTATION!
             */
            void clear_arrayed_class( char* address, ATTRIBUTES* A, int curr_dim, int offset);
            /**
             FIXME: I NEED DOCUMENTATION!
             */
            void debug_write_alloc_info( ALLOC_INFO *alloc_info);

            /**
             Returns a pointer to the udunits system we are using.
             */
            ut_system * get_unit_system() ;

    }; // endof class MemoryManager

} // endof namespace Trick

#ifndef SWIG
extern Trick::MemoryManager* trick_MM;
#endif

#endif

