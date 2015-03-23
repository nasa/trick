
/*
  This is the 1D array/pointer support code for SWIG
 */

#ifndef SWIG_INT_TEMPLATES_HH
#define SWIG_INT_TEMPLATES_HH

template< class S , typename T > static int convert_and_set( T & output , void * my_argp , std::string to_units ) {
    int ret = 0 ;

    S * temp_m = reinterpret_cast< S * >(my_argp) ;
    if ( temp_m->units.compare("--") ) {
        try {
            Unit converter(temp_m->units.c_str()) ;
            output = (T)converter.Convert_to( temp_m->value , to_units.c_str()) ;
        }
        catch (Unit::CONVERSION_ERROR & ce_err ) {
            PyErr_SetString(PyExc_TypeError,"Units conversion error");
            ret = -1;
        }
    } else {
        output = (T)temp_m->value ;
    }
    return ret ;
}

template<typename T > static int typemap_in_scalar( T & output , PyObject *input , const char * symname ) {

    // SCALAR IN
    std::string temp_name ;
    std::string left_units ;
    void * my_argp ;
    int ret = 0 ;

    temp_name = symname ;
    if ( temp_name.length() > 4 ) {
        temp_name.erase(temp_name.length() - 4) ;
    }
    left_units = Trick::UnitsMap::units_map()->get_units(temp_name) ;
    //std::cerr << "\033[33mleft side units = " << left_units << "\033[00m" << std::endl ;

    if (SWIG_IsOK(SWIG_ConvertPtr(input, &my_argp,SWIG_TypeQuery("_p_swig_int"), 0 ))) {
        ret = convert_and_set< swig_int , T >( output , my_argp , left_units ) ;
    } else if (SWIG_IsOK(SWIG_ConvertPtr(input, &my_argp,SWIG_TypeQuery("_p_swig_double"), 0 ))) {
        ret = convert_and_set< swig_double , T >( output , my_argp , left_units ) ;
    } else if ( PyFloat_Check(input) ) {
        output = (T)PyFloat_AsDouble(input) ;
    } else if ( PyInt_Check(input) ) {
        output = (T)PyInt_AsLong(input) ;
    } else if ( PyString_Check(input) ) {
        // scalar char as a string.  Set the value of the output to the value of the first char.
        if ( PyString_Size(input) == 1 ) {
            char * temp_str = PyString_AsString(input) ;
            output = (T)temp_str[0] ;
        }
    } else {
        ret = -1 ;
    }

    return ret ;
}

template<typename T > static T * typemap_in_1d( PyObject *input , unsigned int out_size, const char * symname ) {

    void * argp2 ;
    unsigned int ii ;
    T * new_array ;
    int ret ;

    // convert list to tuple so we only have to deal with tuples in code below.
    if (PyList_Check(input)) {
        input = PyList_AsTuple(input) ;
    }

    if (PyTuple_Check(input)) {
        unsigned int size = PyTuple_Size(input) ;
        if ( size > out_size ) {
           PyErr_SetString(PyExc_TypeError,"List too long to fit.");
           return NULL ;
        }

        std::string temp_name ;
        std::string left_units ;

        temp_name = symname ;
        if ( temp_name.length() > 4 ) {
            temp_name.erase(temp_name.length() - 4) ;
        }
        left_units = Trick::UnitsMap::units_map()->get_units(temp_name) ;

        new_array = (T *)calloc( out_size , sizeof(T));

        for( ii = 0 ; ii < size ; ii++ ) {
            PyObject *o = PyTuple_GetItem(input,ii) ;
            if (PyFloat_Check(o)) {
                new_array[ii] = (T)PyFloat_AsDouble(o) ;
            } else if (SWIG_IsOK(SWIG_ConvertPtr(o, &argp2,SWIG_TypeQuery("_p_swig_int"), 0 ))) {
                ret = convert_and_set< swig_int , T >( new_array[ii] , argp2 , left_units ) ;
                if ( ret != 0 ) {
                    free(new_array) ;
                    return NULL;
                }
            } else if (SWIG_IsOK(SWIG_ConvertPtr(o, &argp2,SWIG_TypeQuery("_p_swig_double"), 0 ))) {
                ret = convert_and_set< swig_double , T >( new_array[ii] , argp2 , left_units ) ;
                if ( ret != 0 ) {
                    free(new_array) ;
                    return NULL;
                }
            } else if (PyInt_Check(o)) {
                new_array[ii] = (T)PyInt_AsLong(o) ;
            } else {
                PyErr_SetString(PyExc_TypeError,"List must contain numerical values");
                free(new_array) ;
                return NULL;
            }
        }
    } else if ( PyString_Check(input) ) {
        unsigned int size = PyString_Size(input) ;
        char * temp_str = PyString_AsString(input) ;

        if ( size > out_size ) {
           PyErr_SetString(PyExc_TypeError,"List too long to fit.");
           return NULL ;
        }

        new_array = (T *)calloc( out_size , sizeof(T));

        for( ii = 0 ; ii < size ; ii++ ){
            new_array[ii] = (T)temp_str[ii] ;
        }
    } else {
        PyErr_SetString(PyExc_TypeError,"Input must be of type List");
        return NULL;
    }

    return new_array ;
}

template<typename T > static int typemap_in_1dp( PyObject *input , const char * basetype ,
                                                 const char * symname , T ** output ) {
    // INT *
    void * argp2 ;
    int ret ;

    // convert list to tuple so we only have to deal with tuples in code below.
    if (PyList_Check(input)) {
        input = PyList_AsTuple(input) ;
    }

    if (PyTuple_Check(input)) {
        unsigned int size = PyTuple_Size(input) ;
        unsigned int ii = 0;

        std::string temp_name ;
        std::string left_units ;

        temp_name = symname ;
        if ( temp_name.length() > 4 ) {
            temp_name.erase(temp_name.length() - 4) ;
        }
        left_units = Trick::UnitsMap::units_map()->get_units(temp_name) ;

        *output = (T *)TMM_declare_var_1d(basetype , size) ;

        for( ii = 0 ; ii < size ; ii++ ) {
            PyObject *o = PyTuple_GetItem(input,ii) ;
            if (PyFloat_Check(o)) {
                (*output)[ii] = (T)PyFloat_AsDouble(o) ;
            } else if (SWIG_IsOK(SWIG_ConvertPtr(o, &argp2,SWIG_TypeQuery("_p_swig_int"), 0 ))) {
                ret = convert_and_set< swig_int , T >( (*output)[ii] , argp2 , left_units ) ;
                if ( ret != 0 ) {
                    TMM_delete_var_a(*output) ;
                    return -1;
                }
            } else if (SWIG_IsOK(SWIG_ConvertPtr(o, &argp2,SWIG_TypeQuery("_p_swig_double"), 0 ))) {
                ret = convert_and_set< swig_double , T >( (*output)[ii] , argp2 , left_units ) ;
                if ( ret != 0 ) {
                    TMM_delete_var_a(*output) ;
                    return -1;
                }
            } else if (PyInt_Check(o)) {
                (*output)[ii] = (T)PyInt_AsLong(o) ;
            } else {
               PyErr_SetString(PyExc_TypeError,"List must contain int");
               return -1;
            }
        }
    } else if ( SWIG_IsOK(SWIG_ConvertPtr(input, &argp2,SWIG_TypeQuery("_p_swig_ref"), 0)) ) {
        // Array to pointer assignment
        swig_ref * temp_swig_ref = reinterpret_cast< swig_ref * >(argp2);
        if ( temp_swig_ref != NULL ) {
            *output = (T *)temp_swig_ref->ref.address ;
        }
        // package the array address as a void
    } else if ( SWIG_IsOK(SWIG_ConvertPtr(input, &argp2,SWIG_TypeQuery("_p_REF2"), 0)) ) {
        // We have an address coming in, we don't have to do any translation
        REF2 * temp_ref = reinterpret_cast< REF2 * >(argp2) ;
        if ( temp_ref != NULL ){
            *output = (T *)temp_ref->address ;
        }
    } else if ( SWIG_IsOK(SWIG_ConvertPtr(input, &argp2,SWIG_TypeQuery("_p_void"), 0)) ) {
        // We have an address coming in, we don't have to do any translation
        *output = reinterpret_cast< T * >(argp2) ;
    } else {
        if ( !strncmp( basetype , "char" , 4 )) {
            if ( PyString_Check(input) ) {
                *output = (T *)TMM_strdup(PyString_AsString(input)) ;
            } else {
                PyErr_SetString(PyExc_TypeError,"swig_int (char): Input must be of type List, string, or a pointer type");
                return -1;
            }
        } else {
            PyErr_SetString(PyExc_TypeError,"swig_int: Input must be of type List or a pointer type");
            return -1;
        }
    }

    return 0 ;
}

template<typename T, typename baseT > static void * typemap_in_2d( PyObject *input , unsigned int out_dim0, unsigned int out_dim1, const char * symname ) {

    //INT[ANY][ANY] IN

    void * argp2 ;
    unsigned int ii , jj ;
    unsigned int size0, size1 ;
    T * new_array ;
    int ret ;

    // convert list to tuple so we only have to deal with tuples in code below.
    if (PyList_Check(input)) {
        input = PyList_AsTuple(input) ;
    }

    if (PyTuple_Check(input)) {

        size0 = PyTuple_Size(input) ;
        if ( size0 > out_dim0 ) {
           PyErr_SetString(PyExc_TypeError,"Outer list too long to fit.");
           return NULL ;
        }

        std::string temp_name ;
        std::string left_units ;

        temp_name = symname ;
        if ( temp_name.length() > 4 ) {
            temp_name.erase(temp_name.length() - 4) ;
        }
        left_units = Trick::UnitsMap::units_map()->get_units(temp_name) ;

        new_array = (T *)calloc( out_dim0 , sizeof(T));

        for( ii = 0 ; ii < size0 ; ii++ ) {
            PyObject *o = PyTuple_GetItem( input, ii ) ;

            // convert list to tuple so we only have to deal with tuples in code below.
            if (PyList_Check(o)) {
                o = PyList_AsTuple(o) ;
            }

            if (PyTuple_Check(o)) {

                size1 = PyTuple_Size(o) ;
                if ( size1 > out_dim1 ) {
                    PyErr_SetString(PyExc_TypeError,"List too long to fit.");
                    return NULL ;
                }

                for( jj = 0 ; jj < size1 ; jj++ ){

                    PyObject *o2 = PyTuple_GetItem( o , jj ) ;

                    if (PyFloat_Check(o2)) {
                        new_array[ii][jj] = (baseT)PyFloat_AsDouble(o2) ;
                    } else if (SWIG_IsOK(SWIG_ConvertPtr(o2, &argp2, SWIG_TypeQuery("_p_swig_int"), 0 ))) {
                        ret = convert_and_set< swig_int , baseT >( new_array[ii][jj] , argp2 , left_units ) ;
                        if ( ret != 0 ) {
                            free(new_array) ;
                            return NULL;
                        }
                    } else if (SWIG_IsOK(SWIG_ConvertPtr(o2, &argp2, SWIG_TypeQuery("_p_swig_double"), 0 ))) {
                        ret = convert_and_set< swig_double , baseT >( new_array[ii][jj] , argp2 , left_units ) ;
                        if ( ret != 0 ) {
                            free(new_array) ;
                            return NULL;
                        }
                    } else if (PyInt_Check(o2)) {
                        new_array[ii][jj] = (baseT)PyInt_AsLong(o2) ;
                    }
                }
            } else if ( PyString_Check(o) ) {
                unsigned int size = PyString_Size(o) ;
                char * temp_str = PyString_AsString(o) ;

                if ( size > out_dim1 ) {
                   PyErr_SetString(PyExc_TypeError,"String too long to fit.");
                   return NULL ;
                }

                for( jj = 0 ; jj < size ; jj++ ){
                    new_array[ii][jj] = (baseT)temp_str[jj] ;
                }
            } else {
               PyErr_SetString(PyExc_TypeError,"Input must be list of lists");
               return NULL;
            }
        }
    } else {
        PyErr_SetString(PyExc_TypeError,"Input must be of type List");
        return NULL;
    }

    return new_array ;
}

#endif

