
/*
  This is the 1D array/pointer support code for SWIG
 */

%{
#include <iostream>

#include "trick/swig/swig_int.hh"
#include "trick/swig/swig_double.hh"
#include "trick/swig/swig_ref.hh"
#include "trick/swig/PrimitiveAttributesMap.hh"
#include "trick/UnitsMap.hh"
#include "trick/reference.h"
#include "trick/memorymanager_c_intf.h"

#include "trick/swig/swig_int_templates.hh"

%}


%typemap(in) char , unsigned char ,
             short , unsigned short ,
             int , unsigned int ,
             long , unsigned long ,
             long long , unsigned long long ,
             enum SWIGTYPE , bool ,
             double , float {
    int ret ;
    ret = typemap_in_scalar<$1_ltype >( $1 , $input , "$symname") ;
    if ( ret != 0 ) {
        SWIG_exception_fail(SWIG_TypeError,"Right hand side could not be converted proper scalar type");
    }
}

%typemap(out) char , unsigned char ,
              short , unsigned short ,
              int , unsigned int ,
              long , unsigned long ,
              long long , unsigned long long ,
              enum SWIGTYPE , bool {
    // INT OUT
    std::string temp_name ;
    swig_int * t = new swig_int ;
    t->value = (long long)result ;
    temp_name = "$symname" ;
    temp_name.erase(temp_name.length() - 4) ;
    //cout << "swig_int out looking for param " << temp_name << std::endl ;
    t->units = Trick::UnitsMap::units_map()->get_units(temp_name) ;
    //cout << "swig_int out found units " << t->units << std::endl ;
    $result = SWIG_NewPointerObj(SWIG_as_voidptr(t), SWIG_TypeQuery("_p_swig_int"), SWIG_POINTER_OWN);
}

%typemap(out) double , float {
    // DOUBLE OUT
    std::string temp_name ;
    swig_double * t = new swig_double ;
    t->value = (double)result ;
    temp_name = "$symname" ;
    temp_name.erase(temp_name.length() - 4) ;
    //cout << "swig_double out looking for param " << temp_name << std::endl ;
    t->units = Trick::UnitsMap::units_map()->get_units(temp_name) ;
    //cout << "swig_double out found units " << t->units << std::endl ;
    $result = SWIG_NewPointerObj(SWIG_as_voidptr(t), SWIG_TypeQuery("_p_swig_double"), SWIG_POINTER_OWN);
}

%typemap(in) char [] , unsigned char [] ,
             short [] , unsigned short [] ,
             int [] , unsigned int [] ,
             long [] , unsigned long [] ,
             long long [] , unsigned long long [] ,
             enum SWIGTYPE [] , bool [] ,
             double [] , float [] {
    //ARRAY[] IN
    int ret ;
    ret = typemap_in_1dp<$1_basetype>( $input , "$1_basetype", "$symname", &$1) ;
    if ( ret != 0 ) {
        SWIG_exception_fail(SWIG_TypeError,"Right hand side could not be converted to proper array type");
    }
}

%typemap(freearg) char [] , unsigned char [] ,
                   short [] , unsigned short [] ,
                   int [] , unsigned int [] ,
                   long [] , unsigned long [] ,
                   long long [] , unsigned long long [] ,
                   enum SWIGTYPE [] , bool [] ,
                   double [] , float [] {
}

%typemap(in) char [ANY] , unsigned char [ANY] ,
             short [ANY] , unsigned short [ANY] ,
             int [ANY] , unsigned int [ANY] ,
             long [ANY] , unsigned long [ANY] ,
             long long [ANY] , unsigned long long [ANY] ,
             enum SWIGTYPE [ANY] , bool [ANY] ,
             double [ANY] , float [ANY] {
    //ARRAY[ANY] IN
    $1 = typemap_in_1d<$1_basetype>( $input , $1_dim0, "$symname") ;
    if ( $1 == NULL ) {
        SWIG_exception_fail(SWIG_TypeError,"Right hand side could not be converted to proper array type");
    }
}

%typemap(freearg) char [ANY] , unsigned char [ANY] ,
                   short [ANY] , unsigned short [ANY] ,
                   int [ANY] , unsigned int [ANY] ,
                   long [ANY] , unsigned long [ANY] ,
                   long long [ANY] , unsigned long long [ANY] ,
                   enum SWIGTYPE [ANY] , bool [ANY] ,
                   double [ANY] , float [ANY] {
    free($1) ;
}

%typemap(out) char [ANY] , unsigned char [ANY] ,
              short [ANY] , unsigned short [ANY] ,
              int [ANY] , unsigned int [ANY] ,
              long [ANY] , unsigned long [ANY] ,
              long long [ANY] , unsigned long long [ANY] ,
              enum SWIGTYPE [ANY] , bool [ANY] ,
              double [ANY] , float [ANY] {
    std::string temp_name ;

    swig_ref * t = new swig_ref ;

    t->ref.address = (void *)$1;
    t->ref.units = NULL ;

    t->ref.attr = Trick::PrimitiveAttributesMap::attributes_map()->get_attr("$1_basetype") ;
    // PrimitiveAttributes lookup failed. Probably an enum. Create a new attributes based on size of type.
    if ( t->ref.attr == NULL ) {
        t->ref.attr = new ATTRIBUTES() ;
        t->ref.attr->size  = sizeof($1_basetype) ;
        switch ( t->ref.attr->size ) {
            case 1: t->ref.attr->type = TRICK_CHARACTER ; break ;
            case 2: t->ref.attr->type = TRICK_SHORT ; break ;
            case 4: t->ref.attr->type = TRICK_INTEGER ; break ;
            case 8: t->ref.attr->type = TRICK_LONG_LONG ; break ;
            default: t->ref.attr->type = TRICK_INTEGER ; break ;
        }
        t->ref.attr->io  = TRICK_VAR_OUTPUT | TRICK_VAR_INPUT | TRICK_CHKPNT_OUTPUT | TRICK_CHKPNT_INPUT ;
    }

    t->ref.attr->type_name  = strdup("$1_basetype") ;
    t->ref.attr->num_index  = 1 ;
    t->ref.attr->index[0].size  = $1_dim0 ;

    t->ref.create_add_path  = 0 ;
    t->ref.num_index  = 0 ;
    t->ref.num_index_left  = 1 ;
    t->ref.ref_type  = REF_ADDRESS ;

    temp_name = "$symname" ;
    temp_name.erase(temp_name.length() - 4) ;
    t->ref.attr->units = strdup(Trick::UnitsMap::units_map()->get_units(temp_name).c_str()) ;

    $result = SWIG_NewPointerObj(SWIG_as_voidptr(t), SWIG_TypeQuery("_p_swig_ref"), SWIG_POINTER_OWN);
}

%typemap(in) char * , unsigned char * ,
             short * , unsigned short * ,
             int * , unsigned int * ,
             long * , unsigned long * ,
             long long * , unsigned long long * ,
             enum SWIGTYPE * , bool * ,
             double * , float * {
    int ret ;
    ret = typemap_in_1dp<$1_basetype>( $input , "$1_basetype", "$symname", &$1) ;
    if ( ret != 0 ) {
        SWIG_exception_fail(SWIG_TypeError,"Right hand side could not be converted to proper array type");
    }
}

%typemap(memberin) char * {
    $1 = $input ;
}

%typemap(freearg) char * {
}

// For __str__ printout routines
%typemap(out) char * __str__ {
    $result = PyString_FromString($1) ;
}

%typemap(out) char * , unsigned char * ,
              short * , unsigned short * ,
              int * , unsigned int * ,
              long * , unsigned long * ,
              long long * , unsigned long long * ,
              enum SWIGTYPE * , bool * ,
              double * , float * {
    std::string temp_name ;

    swig_ref * t = new swig_ref ;

    t->ref.address = (void *)$1;
    t->ref.units = NULL ;

    t->ref.attr = Trick::PrimitiveAttributesMap::attributes_map()->get_attr("$1_basetype") ;
    // PrimitiveAttributes lookup failed. Probably an enum. Create a new attributes based on size of type.
    if ( t->ref.attr == NULL ) {
        t->ref.attr = new ATTRIBUTES() ;
        t->ref.attr->size  = sizeof($1_basetype) ;
        switch ( t->ref.attr->size ) {
            case 1: t->ref.attr->type = TRICK_CHARACTER ; break ;
            case 2: t->ref.attr->type = TRICK_SHORT ; break ;
            case 4: t->ref.attr->type = TRICK_INTEGER ; break ;
            case 8: t->ref.attr->type = TRICK_LONG_LONG ; break ;
            default: t->ref.attr->type = TRICK_INTEGER ; break ;
        }
        t->ref.attr->io  = TRICK_VAR_OUTPUT | TRICK_VAR_INPUT | TRICK_CHKPNT_OUTPUT | TRICK_CHKPNT_INPUT ;
    }

    t->ref.attr->type_name  = strdup("$1_basetype") ;
    t->ref.attr->num_index  = 1 ;
    t->ref.attr->index[0].size  = get_truncated_size((char *)t->ref.address) ;
    t->ref.attr->index[0].start  = 0 ;

    t->ref.create_add_path  = 0 ;
    t->ref.num_index  = 0 ;
    t->ref.num_index_left  = 1 ;
    t->ref.ref_type  = REF_ADDRESS ;

    temp_name = "$symname" ;
    temp_name.erase(temp_name.length() - 4) ;
    t->ref.attr->units = strdup(Trick::UnitsMap::units_map()->get_units(temp_name).c_str()) ;

    $result = SWIG_NewPointerObj(SWIG_as_voidptr(t), SWIG_TypeQuery("_p_swig_ref"), SWIG_POINTER_OWN);
}

%typemap(in) char [ANY][ANY] , unsigned char [ANY][ANY] ,
             short [ANY][ANY] , unsigned short [ANY][ANY] ,
             int [ANY][ANY] , unsigned int [ANY][ANY] ,
             long [ANY][ANY] , unsigned long [ANY][ANY] ,
             long long [ANY][ANY] , unsigned long long [ANY][ANY] ,
             enum SWIGTYPE [ANY][ANY] , bool [ANY][ANY] ,
             double [ANY][ANY] , float [ANY][ANY] {

    //ARRAY[ANY][ANY] IN
    // The strange looking "+ 0" is required because unconstrained arrays are falling into this rule and level $1_dim0 blank.
    if ( $1_dim0 + 0 > 0 ) {
        $1 = ($1_basetype (*)[$1_dim1])typemap_in_2d<$1_basetype[$1_dim1], $1_basetype>( $input , $1_dim0 + 0, $1_dim1, "$symname") ;
        if ( $1 == NULL ) {
            SWIG_exception_fail(SWIG_TypeError,"Right hand side could not be converted to proper array type");
        }
    } else {
        SWIG_exception_fail(SWIG_TypeError,"Trick SWIG interface code does not support 2d arrays with unconstrained first dim.");
    }
}

// SWIG does not allow us to use "ANY" for the array dimension for this typemap.
%define %int_pointer_to_array(DIM_SIZE)

%typemap(in) char (*)[DIM_SIZE] , unsigned char (*)[DIM_SIZE] ,
             short (*)[DIM_SIZE] , unsigned short (*)[DIM_SIZE] ,
             int (*)[DIM_SIZE] , unsigned int (*)[DIM_SIZE] ,
             long (*)[DIM_SIZE] , unsigned long (*)[DIM_SIZE] ,
             long long (*)[DIM_SIZE] , unsigned long long (*)[DIM_SIZE] ,
             enum SWIGTYPE (*)[DIM_SIZE] , bool (*)[DIM_SIZE] ,
             double (*)[DIM_SIZE] , float (*)[DIM_SIZE] {
    //ARRAY(*)[DIM_SIZE] IN
    void * argp2 ;

    //std::cout << "HERE WITH " << SWIG_TypePrettyName(SWIG_Python_GetSwigThis(obj1)->ty) << std::endl ;
    if ( SWIG_IsOK(SWIG_ConvertPtr($input, &argp2,SWIG_TypeQuery("_p_swig_ref"), 0)) ) {
        // Array to pointer assignment
        swig_ref * temp_swig_ref = reinterpret_cast< swig_ref * >(argp2);
        if ( temp_swig_ref != NULL ) {
            $1 = ($1_basetype (*)[DIM_SIZE])temp_swig_ref->ref.address ;
        }
    } else if ( SWIG_IsOK(SWIG_ConvertPtr($input, &argp2,SWIG_TypeQuery("_p_void"), 0)) ) {
        // We have an address coming in, we don't have to do any translation
        $1 = reinterpret_cast< $1_basetype (*)[DIM_SIZE] >(argp2) ;
    }
}

%enddef

%int_pointer_to_array(1) ;
%int_pointer_to_array(2) ;
%int_pointer_to_array(3) ;
%int_pointer_to_array(4) ;


%typemap(freearg) char [ANY][ANY] , unsigned char [ANY][ANY] ,
             short [ANY][ANY] , unsigned short [ANY][ANY] ,
             int [ANY][ANY] , unsigned int [ANY][ANY] ,
             long [ANY][ANY] , unsigned long [ANY][ANY] ,
             long long [ANY][ANY] , unsigned long long [ANY][ANY] ,
             enum SWIGTYPE [ANY][ANY] , bool [ANY][ANY] ,
             double [ANY][ANY] , float [ANY][ANY] {
    free($1) ;
}

%typemap(out) char [ANY][ANY] , unsigned char [ANY][ANY] ,
              short [ANY][ANY] , unsigned short [ANY][ANY] ,
              int [ANY][ANY] , unsigned int [ANY][ANY] ,
              long [ANY][ANY] , unsigned long [ANY][ANY] ,
              long long [ANY][ANY] , unsigned long long [ANY][ANY] ,
              enum SWIGTYPE [ANY][ANY] , bool [ANY][ANY] ,
              double [ANY][ANY] , float [ANY][ANY] {

    //INT[ANY][ANY] OUT

    std::string temp_name ;

    swig_ref * t = new swig_ref ;

    t->ref.address = (void *)$1;
    t->ref.units = NULL ;

    t->ref.attr = Trick::PrimitiveAttributesMap::attributes_map()->get_attr("$1_basetype") ;
    // PrimitiveAttributes lookup failed. Probably an enum. Create a new attributes based on size of type.
    if ( t->ref.attr == NULL ) {
        t->ref.attr = new ATTRIBUTES() ;
        t->ref.attr->size  = sizeof($1_basetype) ;
        switch ( t->ref.attr->size ) {
            case 1: t->ref.attr->type = TRICK_CHARACTER ; break ;
            case 2: t->ref.attr->type = TRICK_SHORT ; break ;
            case 4: t->ref.attr->type = TRICK_INTEGER ; break ;
            case 8: t->ref.attr->type = TRICK_LONG_LONG ; break ;
            default: t->ref.attr->type = TRICK_INTEGER ; break ;
        }
        t->ref.attr->io  = TRICK_VAR_OUTPUT | TRICK_VAR_INPUT | TRICK_CHKPNT_OUTPUT | TRICK_CHKPNT_INPUT ;
    }

    t->ref.attr->type_name  = strdup("$1_basetype") ;
    t->ref.attr->num_index  = 2 ;
    t->ref.attr->index[0].size  = $1_dim0 ;
    t->ref.attr->index[1].size  = $1_dim1 ;

    t->ref.create_add_path  = 0 ;
    t->ref.num_index  = 0 ;
    t->ref.num_index_left  = 2 ;
    t->ref.ref_type  = REF_ADDRESS ;

    temp_name = "$symname" ;
    temp_name.erase(temp_name.length() - 4) ;
    t->ref.attr->units = strdup(Trick::UnitsMap::units_map()->get_units(temp_name).c_str()) ;

    $result = SWIG_NewPointerObj(SWIG_as_voidptr(t), SWIG_TypeQuery("_p_swig_ref"), SWIG_POINTER_OWN);
}


%typemap(out) char * [ANY] , unsigned char * [ANY] ,
              short * [ANY] , unsigned short * [ANY] ,
              int * [ANY] , unsigned int * [ANY] ,
              long * [ANY] , unsigned long * [ANY] ,
              long long * [ANY] , unsigned long long * [ANY] ,
              enum SWIGTYPE * [ANY] , bool * [ANY] ,
              double * [ANY] , float * [ANY] {

    //INT * [ANY] OUT

    std::string temp_name ;
    $1_basetype dummy ;
    char * type_name = strdup("$1_basetype") ;

    swig_ref * t = new swig_ref ;

    t->ref.address = (void *)$1;
    t->ref.units = NULL ;

    type_name[strlen(type_name) - 2] = '\0' ;

    t->ref.attr = Trick::PrimitiveAttributesMap::attributes_map()->get_attr(type_name) ;
    // PrimitiveAttributes lookup failed. Probably an enum. Create a new attributes based on size of type.
    if ( t->ref.attr == NULL ) {
        t->ref.attr = new ATTRIBUTES() ;
        t->ref.attr->size  = sizeof(*dummy) ;
        switch ( t->ref.attr->size ) {
            case 1: t->ref.attr->type = TRICK_CHARACTER ; break ;
            case 2: t->ref.attr->type = TRICK_SHORT ; break ;
            case 4: t->ref.attr->type = TRICK_INTEGER ; break ;
            case 8: t->ref.attr->type = TRICK_LONG_LONG ; break ;
            default: t->ref.attr->type = TRICK_INTEGER ; break ;
        }
        t->ref.attr->io  = TRICK_VAR_OUTPUT | TRICK_VAR_INPUT | TRICK_CHKPNT_OUTPUT | TRICK_CHKPNT_INPUT ;
    }

    t->ref.attr->type_name  = type_name ;
    t->ref.attr->num_index  = 2 ;
    t->ref.attr->index[0].size  = $1_dim0 ;
    t->ref.attr->index[1].size  =  0 ;

    t->ref.create_add_path  = 0 ;
    t->ref.num_index  = 0 ;
    t->ref.num_index_left  = 2 ;
    t->ref.ref_type  = REF_ADDRESS ;

    temp_name = "$symname" ;
    temp_name.erase(temp_name.length() - 4) ;
    t->ref.attr->units = strdup(Trick::UnitsMap::units_map()->get_units(temp_name).c_str()) ;

    $result = SWIG_NewPointerObj(SWIG_as_voidptr(t), SWIG_TypeQuery("_p_swig_ref"), SWIG_POINTER_OWN);

}

%typemap(in) char ** , unsigned char ** ,
             short ** , unsigned short ** ,
             int ** , unsigned int ** ,
             long ** , unsigned long ** ,
             long long ** , unsigned long long ** ,
             enum SWIGTYPE ** , bool ** ,
             double ** , float ** {

    //INT ** IN
    void * argp2 ;

    if ( SWIG_IsOK(SWIG_ConvertPtr($input, &argp2,SWIG_TypeQuery("_p_swig_ref"), 0)) ) {
        // Array to pointer assignment
        swig_ref * temp_swig_ref = reinterpret_cast< swig_ref * >(argp2);
        if ( temp_swig_ref != NULL ) {
            $1 = ($1_basetype **)temp_swig_ref->ref.address ;
        }
        // package the array address as a void
    } else if ( SWIG_IsOK(SWIG_ConvertPtr($input, &argp2,SWIG_TypeQuery("_p_REF2"), 0)) ) {
        // We have an address coming in, we don't have to do any translation
        REF2 * temp_ref = reinterpret_cast< REF2 * >(argp2) ;
        if ( temp_ref != NULL ){
            $1 = ($1_basetype **)temp_ref->address ;
        }
    } else if ( SWIG_IsOK(SWIG_ConvertPtr($input, &argp2,SWIG_TypeQuery("_p_void"), 0)) ) {
        // We have an address coming in, we don't have to do any translation
        $1 = reinterpret_cast< $1_basetype ** >(argp2) ;
    } else {
        PyErr_SetString(PyExc_TypeError,"$1_basetype **: Input must be a pointer type");
        return NULL;
    }
}

%typemap(out) char ** , unsigned char ** ,
              short ** , unsigned short ** ,
              int ** , unsigned int ** ,
              long ** , unsigned long ** ,
              long long ** , unsigned long long ** ,
              enum SWIGTYPE ** , bool ** ,
              double ** , float ** {

    //INT ** OUT

    std::string temp_name ;

    swig_ref * t = new swig_ref ;

    t->ref.address = (void *)$1;
    t->ref.units = NULL ;

    t->ref.attr = Trick::PrimitiveAttributesMap::attributes_map()->get_attr("$1_basetype") ;
    // PrimitiveAttributes lookup failed. Probably an enum. Create a new attributes based on size of type.
    if ( t->ref.attr == NULL ) {
        t->ref.attr = new ATTRIBUTES() ;
        t->ref.attr->size  = sizeof($1_basetype) ;
        switch ( t->ref.attr->size ) {
            case 1: t->ref.attr->type = TRICK_CHARACTER ; break ;
            case 2: t->ref.attr->type = TRICK_SHORT ; break ;
            case 4: t->ref.attr->type = TRICK_INTEGER ; break ;
            case 8: t->ref.attr->type = TRICK_LONG_LONG ; break ;
            default: t->ref.attr->type = TRICK_INTEGER ; break ;
        }
        t->ref.attr->io  = TRICK_VAR_OUTPUT | TRICK_VAR_INPUT | TRICK_CHKPNT_OUTPUT | TRICK_CHKPNT_INPUT ;
    }

    t->ref.attr->type_name  = strdup("$1_basetype") ;
    t->ref.attr->num_index  = 2 ;
    t->ref.attr->index[0].size  = get_truncated_size((char *)$1) ;
    t->ref.attr->index[1].size  =  0 ;

    t->ref.create_add_path  = 0 ;
    t->ref.num_index  = 0 ;
    t->ref.num_index_left  = 2 ;
    t->ref.ref_type  = REF_ADDRESS ;

    temp_name = "$symname" ;
    temp_name.erase(temp_name.length() - 4) ;
    t->ref.attr->units = strdup(Trick::UnitsMap::units_map()->get_units(temp_name).c_str()) ;

    $result = SWIG_NewPointerObj(SWIG_as_voidptr(t), SWIG_TypeQuery("_p_swig_ref"), SWIG_POINTER_OWN);

}

%typemap(memberin) SWIGTYPE [ANY][ANY][ANY] , SWIGTYPE [ANY][ANY][ANY][ANY] {
    std::cout << "Cannot assign directly to $1_type\n" << std::endl ;
}

%typemap(out) char [ANY][ANY][ANY] , unsigned char [ANY][ANY][ANY] ,
              short [ANY][ANY][ANY] , unsigned short [ANY][ANY][ANY] ,
              int [ANY][ANY][ANY] , unsigned int [ANY][ANY][ANY] ,
              long [ANY][ANY][ANY] , unsigned long [ANY][ANY][ANY] ,
              long long [ANY][ANY][ANY] , unsigned long long [ANY][ANY][ANY] ,
              enum SWIGTYPE [ANY][ANY][ANY] , bool [ANY][ANY][ANY] ,
              double [ANY][ANY][ANY] , float [ANY][ANY][ANY] {

    //DOUBLE[ANY][ANY][ANY] OUT

    std::string temp_name ;

    swig_ref * t = new swig_ref ;

    t->ref.address = (void *)$1;
    t->ref.units = NULL ;

    t->ref.attr = Trick::PrimitiveAttributesMap::attributes_map()->get_attr("$1_basetype") ;
    // PrimitiveAttributes lookup failed. Probably an enum. Create a new attributes based on size of type.
    if ( t->ref.attr == NULL ) {
        t->ref.attr = new ATTRIBUTES() ;
        t->ref.attr->size  = sizeof($1_basetype) ;
        switch ( t->ref.attr->size ) {
            case 1: t->ref.attr->type = TRICK_CHARACTER ; break ;
            case 2: t->ref.attr->type = TRICK_SHORT ; break ;
            case 4: t->ref.attr->type = TRICK_INTEGER ; break ;
            case 8: t->ref.attr->type = TRICK_LONG_LONG ; break ;
            default: t->ref.attr->type = TRICK_INTEGER ; break ;
        }
        t->ref.attr->io  = TRICK_VAR_OUTPUT | TRICK_VAR_INPUT | TRICK_CHKPNT_OUTPUT | TRICK_CHKPNT_INPUT ;
    }

    t->ref.attr->type_name  = strdup("$1_basetype") ;
    t->ref.attr->num_index  = 3 ;
    t->ref.attr->index[0].size  = $1_dim0 ;
    t->ref.attr->index[1].size  = $1_dim1 ;
    t->ref.attr->index[2].size  = $1_dim2 ;

    t->ref.create_add_path  = 0 ;
    t->ref.num_index  = 0 ;
    t->ref.num_index_left  = 3 ;
    t->ref.ref_type  = REF_ADDRESS ;

    temp_name = "$symname" ;
    temp_name.erase(temp_name.length() - 4) ;
    t->ref.attr->units = strdup(Trick::UnitsMap::units_map()->get_units(temp_name).c_str()) ;

    $result = SWIG_NewPointerObj(SWIG_as_voidptr(t), SWIG_TypeQuery("_p_swig_ref"), SWIG_POINTER_OWN);
}

%apply char { int8_t } ;
%apply char [] { int8_t [] } ;
%apply char [ANY] { int8_t [ANY] } ;
%apply char * { int8_t * } ;
%apply char [ANY][ANY] { int8_t [ANY][ANY] } ;
%apply char * [ANY] { int8_t * [ANY] } ;
%apply char ** { int8_t ** } ;
%apply char [ANY][ANY][ANY] { int8_t [ANY][ANY][ANY] } ;

%apply unsigned char { uint8_t } ;
%apply unsigned char [] { uint8_t [] } ;
%apply unsigned char [ANY] { uint8_t [ANY] } ;
%apply unsigned char * { uint8_t * } ;
%apply unsigned char [ANY][ANY] { uint8_t [ANY][ANY] } ;
%apply unsigned char * [ANY] { uint8_t * [ANY] } ;
%apply unsigned char ** { uint8_t ** } ;
%apply unsigned char [ANY][ANY][ANY] { uint8_t [ANY][ANY][ANY] } ;

%apply short { int16_t } ;
%apply short [] { int16_t [] } ;
%apply short [ANY] { int16_t [ANY] } ;
%apply short * { int16_t * } ;
%apply short [ANY][ANY] { int16_t [ANY][ANY] } ;
%apply short * [ANY] { int16_t * [ANY] } ;
%apply short ** { int16_t ** } ;
%apply short [ANY][ANY][ANY] { int16_t [ANY][ANY][ANY] } ;

%apply unsigned short { uint16_t } ;
%apply unsigned short [] { uint16_t [] } ;
%apply unsigned short [ANY] { uint16_t [ANY] } ;
%apply unsigned short * { uint16_t * } ;
%apply unsigned short [ANY][ANY] { uint16_t [ANY][ANY] } ;
%apply unsigned short * [ANY] { uint16_t * [ANY] } ;
%apply unsigned short ** { uint16_t ** } ;
%apply unsigned short [ANY][ANY][ANY] { uint16_t [ANY][ANY][ANY] } ;

%apply int { int32_t } ;
%apply int [] { int32_t [] } ;
%apply int [ANY] { int32_t [ANY] } ;
%apply int * { int32_t * } ;
%apply int [ANY][ANY] { int32_t [ANY][ANY] } ;
%apply int * [ANY] { int32_t * [ANY] } ;
%apply int ** { int32_t ** } ;
%apply int [ANY][ANY][ANY] { int32_t [ANY][ANY][ANY] } ;

%apply unsigned int { uint32_t } ;
%apply unsigned int [] { uint32_t [] } ;
%apply unsigned int [ANY] { uint32_t [ANY] } ;
%apply unsigned int * { uint32_t * } ;
%apply unsigned int [ANY][ANY] { uint32_t [ANY][ANY] } ;
%apply unsigned int * [ANY] { uint32_t * [ANY] } ;
%apply unsigned int ** { uint32_t ** } ;
%apply unsigned int [ANY][ANY][ANY] { uint32_t [ANY][ANY][ANY] } ;

%apply long long { int64_t } ;
%apply long long [] { int64_t [] } ;
%apply long long [ANY] { int64_t [ANY] } ;
%apply long long * { int64_t * } ;
%apply long long [ANY][ANY] { int64_t [ANY][ANY] } ;
%apply long long * [ANY] { int64_t * [ANY] } ;
%apply long long ** { int64_t ** } ;
%apply long long [ANY][ANY][ANY] { int64_t [ANY][ANY][ANY] } ;

%apply unsigned long long { uint64_t } ;
%apply unsigned long long [] { uint64_t [] } ;
%apply unsigned long long [ANY] { uint64_t [ANY] } ;
%apply unsigned long long * { uint64_t * } ;
%apply unsigned long long [ANY][ANY] { uint64_t [ANY][ANY] } ;
%apply unsigned long long * [ANY] { uint64_t * [ANY] } ;
%apply unsigned long long ** { uint64_t ** } ;
%apply unsigned long long [ANY][ANY][ANY] { uint64_t [ANY][ANY][ANY] } ;

%apply unsigned char { u_char } ;
%apply unsigned char [] { u_char [] } ;
%apply unsigned char [ANY] { u_char [ANY] } ;
%apply unsigned char * { u_char * } ;
%apply unsigned char [ANY][ANY] { u_char [ANY][ANY] } ;
%apply unsigned char * [ANY] { u_char * [ANY] } ;
%apply unsigned char ** { u_char ** } ;
%apply unsigned char [ANY][ANY][ANY] { u_char [ANY][ANY][ANY] } ;

%apply unsigned short { u_short } ;
%apply unsigned short [] { u_short [] } ;
%apply unsigned short [ANY] { u_short [ANY] } ;
%apply unsigned short * { u_short * } ;
%apply unsigned short [ANY][ANY] { u_short [ANY][ANY] } ;
%apply unsigned short * [ANY] { u_short * [ANY] } ;
%apply unsigned short ** { u_short ** } ;
%apply unsigned short [ANY][ANY][ANY] { u_short [ANY][ANY][ANY] } ;

%apply unsigned int { u_int } ;
%apply unsigned int [] { u_int [] } ;
%apply unsigned int [ANY] { u_int [ANY] } ;
%apply unsigned int * { u_int * } ;
%apply unsigned int [ANY][ANY] { u_int [ANY][ANY] } ;
%apply unsigned int * [ANY] { u_int * [ANY] } ;
%apply unsigned int ** { u_int ** } ;
%apply unsigned int [ANY][ANY][ANY] { u_int [ANY][ANY][ANY] } ;

%apply unsigned long { u_long } ;
%apply unsigned long [] { u_long [] } ;
%apply unsigned long [ANY] { u_long [ANY] } ;
%apply unsigned long * { u_long * } ;
%apply unsigned long [ANY][ANY] { u_long [ANY][ANY] } ;
%apply unsigned long * [ANY] { u_long * [ANY] } ;
%apply unsigned long ** { u_long ** } ;
%apply unsigned long [ANY][ANY][ANY] { u_long [ANY][ANY][ANY] } ;

%apply long long { quad_t } ;
%apply long long [] { quad_t [] } ;
%apply long long [ANY] { quad_t [ANY] } ;
%apply long long * { quad_t * } ;
%apply long long [ANY][ANY] { quad_t [ANY][ANY] } ;
%apply long long * [ANY] { quad_t * [ANY] } ;
%apply long long ** { quad_t ** } ;
%apply long long [ANY][ANY][ANY] { quad_t [ANY][ANY][ANY] } ;

%apply unsigned long long { u_quad_t } ;
%apply unsigned long long [] { u_quad_t [] } ;
%apply unsigned long long [ANY] { u_quad_t [ANY] } ;
%apply unsigned long long * { u_quad_t * } ;
%apply unsigned long long [ANY][ANY] { u_quad_t [ANY][ANY] } ;
%apply unsigned long long * [ANY] { u_quad_t * [ANY] } ;
%apply unsigned long long ** { u_quad_t ** } ;
%apply unsigned long long [ANY][ANY][ANY] { u_quad_t [ANY][ANY][ANY] } ;

%apply unsigned long { size_t } ;
%apply unsigned long [] { size_t [] } ;
%apply unsigned long [ANY] { size_t [ANY] } ;
%apply unsigned long * { size_t * } ;
%apply unsigned long [ANY][ANY] { size_t [ANY][ANY] } ;
%apply unsigned long * [ANY] { size_t * [ANY] } ;
%apply unsigned long ** { size_t ** } ;
%apply unsigned long [ANY][ANY][ANY] { size_t [ANY][ANY][ANY] } ;

