#ifndef MEMORYMANAGER_C_INTF_H
#define MEMORYMANAGER_C_INTF_H

#include <stdio.h>

#include "trick/parameter_types.h"
#include "trick/attributes.h"
#include "trick/reference.h"
#include "trick/var.h"
#include "trick/io_alloc.h"

#ifdef __cplusplus
extern "C" {
#endif

void* TMM_declare_var( TRICK_TYPE type, const char*class_name, int n_stars, const char* var_name, int n_cdims, int *cdims);
void* TMM_declare_var_1d( const char* elem_decl, int e_elems) ;
void* TMM_declare_var_s( const char* declaration ) ;
void* alloc_type( int e_elems, const char* enh_type_spec) ;

void* TMM_declare_operatornew_var( const char * class_name, unsigned int alloc_size, unsigned int element_size );

void* TMM_declare_ext_var( void* addr, TRICK_TYPE type, const char*class_name, int n_stars, const char* var_name, int n_cdims, int *cdims);
void* TMM_declare_ext_var_1d( void* addr, const char* elem_decl, int e_elems) ;
void* TMM_declare_ext_var_s( void* addr, const char* declaration);


void* TMM_resize_array_a(void *address, int n_cdims, int *cdims);
void* TMM_resize_array_n(const char *name, int n_cdims, int *cdims);
void* TMM_resize_array_1d_a(void* address, int num);
void* TMM_resize_array_1d_n(const char *name, int num);

char* TMM_strdup(char *str) ;

int   TMM_var_exists( const char* var_name);

int   TMM_is_alloced(char *addr) ;

void  TMM_set_debug_level(int level);
void  TMM_reduced_checkpoint(int flag);
void  TMM_hexfloat_checkpoint(int flag);

void  TMM_clear_var_a( void* address);
void  TMM_clear_var_n( const char* var_name );

void  TMM_delete_var_a( void* address);
void  TMM_delete_var_n( const char* var_name );

void  TMM_delete_extern_var_a( void* address);
void  TMM_delete_extern_var_n( const char* var_name );

void  TMM_write_checkpoint( const char* filename) ;
int   TMM_read_checkpoint( const char* filename);
int   TMM_read_checkpoint_from_string( const char* str);

int   TMM_init_from_checkpoint( const char* filename);
int   TMM_add_shared_library_symbols( const char* filename);


REF2* ref_attributes(char* name);
int   ref_var(REF2* R, char* name);
int   get_size(void *addr) ;

void* add_var( TRICK_TYPE type, const char* stype, VAR_DECLARE* var_declare, char* units);
int   add_vars( TRICK_TYPE type, const char* stype, VAR_LIST* var_list, char* units);
void* follow_address_path(REF2 *R) ;
int   ref_allocate(REF2 *R, int num) ;
int   ref_assignment(REF2* R, V_TREE* V);
int   get_truncated_size(void *addr) ;
int   io_get_fixed_truncated_size(char *ptr, ATTRIBUTES * A, char *str, int dims, ATTRIBUTES * left_type) ;
ALLOC_INFO* get_alloc_info_of(void * addr);
ALLOC_INFO* get_alloc_info_at(void * addr);
int set_alloc_name_at(void * addr, const char * name );

void ref_free( REF2 *R ) ;

int get_enumerated(const char* name, V_DATA* v_data) ;

void TMM_add_checkpoint_alloc_dependency(const char * name) ;
#ifdef __cplusplus
}
#endif

#if 1
#define mm_alloc(typespec, num) (typespec*)TMM_declare_var_1d(#typespec,(num))
#else
// Example redefinition
#define mm_alloc(typespec, num) (typespec*)calloc((size_t)(num),sizeof(typespec))
#endif

#define mm_free TMM_delete_var_a
#define mm_realloc TMM_reallocate

#endif
