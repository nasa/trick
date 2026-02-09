
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <Python.h>
#include <stdlib.h>

#include "trick/swig/swig_ref.hh"
#include "trick/swig/swig_double.hh"
#include "trick/swig/swig_int.hh"
#include "trick/swig/swig_convert_units.hh"
#include "trick/MemoryManager.hh"
#include "trick/memorymanager_c_intf.h"
#include "trick/PythonPrint.hh"

#if PY_VERSION_HEX < 0x02050000
#define Py_ssize_t int
#endif

extern Trick::MemoryManager* trick_MM;

swig_ref::swig_ref() : ref() , need_to_deref(false) {
    str_output = NULL ;
}

swig_ref::~swig_ref() {
    if ( ref.attr->type_name ) {
        free((void *)ref.attr->type_name) ;
    }
    if ( ref.attr->units ) {
        free((void *)ref.attr->units) ;
    }
    delete ref.attr ;
    if ( str_output ) {
        free(str_output) ;
    }
}

char * swig_ref::__str__() {
    std::stringstream os ;
    // Check if first dimension is a pointer BEFORE deref_address changes it
    bool is_pointer_dim = (ref.attr->num_index > 0 && ref.attr->index[0].size == 0);

    deref_address() ;
    REF2 ref_copy = ref;

    // Check for NULL pointer
    if ( ref_copy.address == NULL ) {
        if ( str_output ) {
            free(str_output) ;
        }
        str_output = strdup("NULL") ;
        return str_output ;
    }

    // For pointer dimensions with 2+ remaining dimensions, recursively print the structure
    // For 1D arrays (num_index_left == 1), use write_rvalue which handles formatting correctly
    if ( is_pointer_dim && ref_copy.num_index_left > 1 ) {
        // This is a pointer dimension with more dimensions remaining
        int size = get_truncated_size((char *)ref_copy.address);

        os << "[" ;
        for ( int i = 0; i < size; i++ ) {
            if ( i > 0 ) os << ", ";

            // Get element at index i (will be a swig_ref to the next dimension)
            PyObject* elem = __getitem__(i);
            if ( elem != NULL ) {
                // Recursively convert element to string
                PyObject* str_obj = PyObject_Str(elem);
                if ( str_obj != NULL ) {
#if PY_VERSION_HEX >= 0x03000000
                    PyObject* bytes = PyUnicode_AsEncodedString(str_obj, "utf-8", "Error");
                    if ( bytes != NULL ) {
                        os << PyBytes_AS_STRING(bytes);
                        Py_DECREF(bytes);
                    }
#else
                    os << PyString_AsString(str_obj);
#endif
                    Py_DECREF(str_obj);
                }
                Py_DECREF(elem);
            }
        }
        os << "]";
    } else {
        // Use Trick's default printing for all types
        // write_rvalue will handle units display internally
        Trick::PythonPrint::write_rvalue(os , ref_copy.address , ref_copy.attr , 0 , 0, true) ;
    }

    if ( str_output ) {
        free(str_output) ;
    }
    str_output = strdup(os.str().c_str()) ;
    return str_output ;
}

char * swig_ref::__repr__() {
    return(__str__()) ;
}

PyObject * swig_ref::__len__() {
    deref_address() ;
    return PyInt_FromLong((long)ref.attr->index[0].size) ;
}

// If the swig_ref needs is a pointer, dereference it and determine the array size.
void swig_ref::deref_address() {
    if ( need_to_deref ) {
        ref.address = *(void**)ref.address ;
        ref.attr->index[0].size = get_truncated_size((char *)ref.address) ;
        need_to_deref = false ;
    }
}

template <class TYPE>
void * assign_array( PyObject *obj1 , REF2 & ref ) {

    void * argp2 ;
    int ret ;
    TYPE * temp_ptr ;
    unsigned int jj ;


    // convert list to tuple so we only have to deal with tuples in code below.
    if ( PyList_Check(obj1) ) {
        obj1 = PyList_AsTuple(obj1) ;
    }
    unsigned int size = PyTuple_Size(obj1) ;

    temp_ptr = (TYPE *)TMM_declare_var_1d(ref.attr->type_name , size) ;

    for( jj = 0 ; jj < size ; jj++ ) {
        PyObject *o = PyTuple_GetItem( obj1 , jj ) ;
        if (PyFloat_Check(o)) {
            temp_ptr[jj] = (TYPE)(PyFloat_AsDouble(o)) ;
        } else if (PyInt_Check(o)) {
            temp_ptr[jj] = (TYPE)(PyInt_AsLong(o)) ;
        } else if (SWIG_IsOK(SWIG_ConvertPtr(o, &argp2,SWIG_TypeQuery("_p_swig_double"), 0 ))) {
            double new_value ;
            std::string units_ref ;
            swig_double * temp_m = reinterpret_cast<swig_double *>(argp2) ;

            new_value = temp_m->value ;
            units_ref = std::string(ref.attr->units) ;
            ret = convert_united_value( units_ref , temp_m->units , &new_value ) ;
            if ( ret == 0 ) {
                temp_ptr[jj] = (TYPE)new_value ;
            } else {
                return NULL ;
            }
        } else if (SWIG_IsOK(SWIG_ConvertPtr(o, &argp2,SWIG_TypeQuery("_p_swig_int"), 0 ))) {
            long long new_value ;
            std::string units_ref ;
            swig_int * temp_m = reinterpret_cast<swig_int *>(argp2) ;

            new_value = temp_m->value ;
            units_ref = std::string(ref.attr->units) ;
            ret = convert_united_value( units_ref , temp_m->units , &new_value ) ;
            if ( ret == 0 ) {
                temp_ptr[jj] = (TYPE)new_value ;
            } else {
                return NULL ;
            }
        } else {
            std::cout << "\033[31mHERE in swig_ref set_item with list of unknown types!!\033[0m" << std::endl ;
            return NULL ;
        }
    }
    return temp_ptr ;
}

void newVTreeNode( V_TREE **curr_node ) {
    V_TREE * next_node = ( V_TREE * )calloc( 1 , sizeof(V_TREE) ) ;
    (*curr_node)->next = next_node ;
    *curr_node = next_node ;
    (*curr_node)->v_data = ( V_DATA * )calloc( 1 , sizeof(V_DATA) ) ;
}

int setVData( V_DATA * v_data , PyObject * o , REF2 & ref_copy ) {
    void * argp2 ;
    int ret ;

    if (PyFloat_Check(o)) {
        v_data->type = TRICK_DOUBLE ;
        v_data->value.d = PyFloat_AsDouble(o) ;
    } else if (PyInt_Check(o)) {
        v_data->type = TRICK_LONG_LONG ;
        v_data->value.ll = PyInt_AsLong(o) ;
    } else if (SWIG_IsOK(SWIG_ConvertPtr(o, &argp2,SWIG_TypeQuery("_p_swig_double"), 0 ))) {
        double new_value ;
        std::string units_ref ;
        swig_double * temp_m = reinterpret_cast<swig_double *>(argp2) ;

        new_value = temp_m->value ;
        units_ref = std::string(ref_copy.attr->units) ;
        ret = convert_united_value( units_ref , temp_m->units , &new_value ) ;
        if ( ret == 0 ) {
            v_data->value.d = new_value ;
        } else {
            return TRICK_UNITS_CONVERSION_ERROR ;
        }
        v_data->type = TRICK_DOUBLE ;
    } else if (SWIG_IsOK(SWIG_ConvertPtr(o, &argp2,SWIG_TypeQuery("_p_swig_int"), 0 ))) {
        long long new_value ;
        std::string units_ref ;
        swig_int * temp_m = reinterpret_cast<swig_int *>(argp2) ;

        new_value = temp_m->value ;
        units_ref = std::string(ref_copy.attr->units) ;
        ret = convert_united_value( units_ref , temp_m->units , &new_value ) ;
        if ( ret == 0 ) {
            v_data->value.ll = new_value ;
        } else {
            return -1 ;
        }
        v_data->type = TRICK_LONG_LONG ;
    } else {
        //TODO: handle list items of "void *" and other pointer types
        std::cout << "\033[31mHERE in swig_ref set_item with list of unknown types!!\033[0m" << std::endl ;
        return -1 ;
    }
    return 0 ;
}

// Helper function to check if we can reuse existing memory for 2D slice assignment
bool canReuseExisting2DMemory(void *ptr, PyObject *obj, REF2 &ref_copy)
{
    // Check if pointer is NULL
    if (ptr == NULL)
    {
        return false;
    }

    // Check if we have a 2D assignment (list of lists)
    if (!PyTuple_Check(obj) && !PyList_Check(obj))
    {
        return false;
    }

    PyObject *tuple_obj = obj;
    if (PyList_Check(obj))
    {
        tuple_obj = PyList_AsTuple(obj);
    }

    unsigned int size = PyTuple_Size(tuple_obj);
    if (size == 0)
    {
        return false;
    }

    // Check first element is also a list
    PyObject *first_elem = PyTuple_GetItem(tuple_obj, 0);
    if (!PyTuple_Check(first_elem) && !PyList_Check(first_elem))
    {
        return false;
    }

    // At this point we know memory exists and we have a 2D structure
    return true;
}

// Helper function to do element-by-element assignment for 2D arrays
// This avoids reallocating memory and invalidating references
int assign2DInPlace(void *base_ptr, PyObject *obj, REF2 &ref_copy)
{
    int ret;

    // Convert to tuple if needed
    PyObject *outer_tuple = obj;
    if (PyList_Check(obj))
    {
        outer_tuple = PyList_AsTuple(obj);
    }

    unsigned int num_rows = PyTuple_Size(outer_tuple);

    for (unsigned int i = 0; i < num_rows; i++)
    {
        PyObject *row_obj = PyTuple_GetItem(outer_tuple, i);
        PyObject *row_tuple = row_obj;
        if (PyList_Check(row_obj))
        {
            row_tuple = PyList_AsTuple(row_obj);
        }

        unsigned int num_cols = PyTuple_Size(row_tuple);

        // Get pointer to row
        void **row_ptrs = (void **)base_ptr;
        void *row_ptr = row_ptrs[i];

        if (row_ptr == NULL)
        {
            std::cout << "\033[31mError: NULL pointer in row " << i << " during in-place 2D assignment\033[0m"
                      << std::endl;
            return -1;
        }

        // Assign each element in the row
        for (unsigned int j = 0; j < num_cols; j++)
        {
            PyObject *elem = PyTuple_GetItem(row_tuple, j);

            switch (ref_copy.attr->type)
            {
            case TRICK_UNSIGNED_CHARACTER:
                if (PyInt_Check(elem))
                {
                    ((unsigned char *)row_ptr)[j] = (unsigned char)PyInt_AsLong(elem);
                }
                break;
            case TRICK_SHORT:
                if (PyInt_Check(elem))
                {
                    ((short *)row_ptr)[j] = (short)PyInt_AsLong(elem);
                }
                break;
            case TRICK_UNSIGNED_SHORT:
                if (PyInt_Check(elem))
                {
                    ((unsigned short *)row_ptr)[j] = (unsigned short)PyInt_AsLong(elem);
                }
                break;
            case TRICK_INTEGER:
                if (PyInt_Check(elem))
                {
                    ((int *)row_ptr)[j] = (int)PyInt_AsLong(elem);
                }
                break;
            case TRICK_UNSIGNED_INTEGER:
                if (PyInt_Check(elem))
                {
                    ((unsigned int *)row_ptr)[j] = (unsigned int)PyInt_AsLong(elem);
                }
                break;
            case TRICK_LONG:
                if (PyInt_Check(elem))
                {
                    ((long *)row_ptr)[j] = (long)PyInt_AsLong(elem);
                }
                break;
            case TRICK_UNSIGNED_LONG:
                if (PyInt_Check(elem))
                {
                    ((unsigned long *)row_ptr)[j] = (unsigned long)PyInt_AsLong(elem);
                }
                break;
            case TRICK_LONG_LONG:
                if (PyInt_Check(elem))
                {
                    ((long long *)row_ptr)[j] = (long long)PyInt_AsLong(elem);
                }
                break;
            case TRICK_UNSIGNED_LONG_LONG:
                if (PyInt_Check(elem))
                {
                    ((unsigned long long *)row_ptr)[j] = (unsigned long long)PyInt_AsLong(elem);
                }
                break;
            case TRICK_DOUBLE:
                if (PyFloat_Check(elem))
                {
                    ((double *)row_ptr)[j] = PyFloat_AsDouble(elem);
                }
                else if (PyInt_Check(elem))
                {
                    ((double *)row_ptr)[j] = (double)PyInt_AsLong(elem);
                }
                break;
            case TRICK_FLOAT:
                if (PyFloat_Check(elem))
                {
                    ((float *)row_ptr)[j] = (float)PyFloat_AsDouble(elem);
                }
                else if (PyInt_Check(elem))
                {
                    ((float *)row_ptr)[j] = (float)PyInt_AsLong(elem);
                }
                break;
            case TRICK_BOOLEAN:
                if (PyInt_Check(elem))
                {
                    ((bool *)row_ptr)[j] = (bool)PyInt_AsLong(elem);
                }
                break;
            default:
                std::cout << "\033[31mUnsupported type for in-place 2D assignment\033[0m" << std::endl;
                return -1;
            }
        }
    }

    return 0;
}

// Helper function to recursively build V_TREE for nested lists (supports up to 3D)
// depth: current nesting depth (1 = 1D list, 2 = 2D list, 3 = 3D list)
int buildNestedVTree(V_TREE **tree_ptr, PyObject *obj, REF2 &ref_copy, int depth)
{
    V_TREE *first_node, *curr_node;
    PyObject *converted_obj = obj;
    int ret;

    // Convert list to tuple
    if (PyList_Check(obj))
    {
        converted_obj = PyList_AsTuple(obj);
    }

    if (!PyTuple_Check(converted_obj))
    {
        std::cout << "\033[31mExpected list/tuple at depth " << depth << "\033[0m" << std::endl;
        return -1;
    }

    unsigned int size = PyTuple_Size(converted_obj);
    first_node = curr_node = (V_TREE *)calloc(1, sizeof(V_TREE));

    for (unsigned int ii = 0; ii < size; ii++)
    {
        PyObject *elem = PyTuple_GetItem(converted_obj, ii);
        newVTreeNode(&curr_node);

        // Check if this element is another list (more dimensions to go)
        PyObject *check_elem = elem;
        if (PyList_Check(elem))
        {
            check_elem = PyList_AsTuple(elem);
        }

        if (depth > 1 && PyTuple_Check(check_elem))
        {
            ret = buildNestedVTree(&(curr_node->down), elem, ref_copy, depth - 1);
            if (ret != 0)
            {
                return ret;
            }
        }
        else
        {
            // Scalar value (or shouldn't be nested anymore)
            ret = setVData(curr_node->v_data, elem, ref_copy);
            if (ret != 0)
            {
                return ret;
            }
        }
    }

    *tree_ptr = first_node->next;
    free(first_node);
    return 0;
}

int swig_ref::__setitem__( int ii, PyObject * obj1 ) {

    V_TREE v_tree ;
    void * argp2 ;
    int ret ;
    int status ;
    V_TREE *first_node , * curr_node ;

    deref_address() ;
    REF2 ref_copy = ref;

    if ( ii < 0 ) {
        ii += ref_copy.attr->index[0].size ;
    }

    v_tree.v_data = ( V_DATA * )calloc( 1 , sizeof(V_DATA) ) ;

    v_tree.v_data->type = TRICK_INTEGER ;
    v_tree.v_data->value.i = ii ;

    status = trick_MM->ref_dim( &ref_copy , v_tree.v_data ) ;

    // Error in ref_dim
    if ( status != 0 ) {
        return status ;
    }

    v_tree.last = v_tree.next = v_tree.down = NULL ;

    // convert list to tuple so we only have to deal with tuples in code below.
    if ( PyList_Check(obj1) ) {
        obj1 = PyList_AsTuple(obj1) ;
    }

    if (PyFloat_Check(obj1)) {
        v_tree.v_data->type = TRICK_DOUBLE ;
        v_tree.v_data->value.d = PyFloat_AsDouble(obj1) ;
    } else if (PyInt_Check(obj1)) {
        v_tree.v_data->type = TRICK_LONG_LONG ;
        v_tree.v_data->value.ll = PyInt_AsLong(obj1) ;
#if PY_VERSION_HEX >= 0x03000000
    } else if ( PyUnicode_Check(obj1) ) {
        v_tree.v_data->type = TRICK_STRING ;
        PyObject * temp = PyUnicode_AsEncodedString(obj1, "utf-8", "Error ~");
        v_tree.v_data->value.cp = strdup(PyBytes_AS_STRING(temp)) ;
#else
    } else if (PyString_Check(obj1)) {
        v_tree.v_data->type = TRICK_STRING ;
        v_tree.v_data->value.cp = PyString_AsString(obj1) ;
#endif
    } else if (SWIG_IsOK(SWIG_ConvertPtr(obj1, &argp2,SWIG_TypeQuery("_p_swig_double"), 0 ))) {
        swig_double * temp_m = reinterpret_cast< swig_double * >(argp2) ;
        ref_copy.units = (char *)temp_m->units.c_str() ;
        v_tree.v_data->type = TRICK_DOUBLE ;
        v_tree.v_data->value.d = temp_m->value ;
    } else if (SWIG_IsOK(SWIG_ConvertPtr(obj1, &argp2,SWIG_TypeQuery("_p_swig_int"), 0 ))) {
        swig_int * temp_m = reinterpret_cast< swig_int * >(argp2) ;
        ref_copy.units = (char *)temp_m->units.c_str() ;
        v_tree.v_data->type = TRICK_LONG_LONG ;
        v_tree.v_data->value.ll = temp_m->value ;
    } else if ( PyTuple_Check(obj1) ) {
        unsigned int jj ;
        unsigned int size = PyTuple_Size(obj1) ;
        int last_index ;

        // See if we have a list to assign to on the left side
        if ( ref_copy.num_index_left == 0 ) {
            std::cout << "\033[31mCannot assign list to a scalar value\033[0m" << std::endl ;
            return TRICK_PARAMETER_ARRAY_SIZE ;
        }

        //cout << "HERE in swig_ref set_item with list type!! ref->num_index " << ref.num_index << " attr->num_index = " << ref.attr->num_index << endl ;
        //cout << "    last index size = " << ref.attr->index[ref.attr->num_index - 1].size << endl ;
        last_index = ref_copy.attr->index[ref_copy.attr->num_index - 1].size ;

        if ( last_index ) {
            // The LHS is a fixed-size array. Fill out a V_TREE structure with the RHS data to be used with ref_assignment.

            // Check if we need multi-dimensional handling
            // Peek at first element to see if it's a nested list
            PyObject *first_elem = PyTuple_GetItem( obj1 , 0 ) ;
            PyObject *check_elem = first_elem;
            if ( PyList_Check(first_elem) ) {
                check_elem = PyList_AsTuple(first_elem);
            }

            if ( ref_copy.num_index_left > 1 && PyTuple_Check(check_elem) ) {
                // Multi-dimensional assignment for fixed-size arrays (2D or 3D)
                // Use recursive helper to build the tree
                ret = buildNestedVTree( &(v_tree.down), obj1, ref_copy, ref_copy.num_index_left );
                if ( ret != 0 ) {
                    return ret;
                }
            } else {
                // Original 1D assignment code for fixed-size arrays
                first_node = curr_node = ( V_TREE * )calloc( 1 , sizeof(V_TREE) ) ;
                
                for( jj = 0 ; jj < size ; jj++ ) {
                    PyObject *o = PyTuple_GetItem( obj1 , jj ) ;
                    newVTreeNode(&curr_node) ;
                    ret = setVData( curr_node->v_data , o , ref_copy ) ;
                    if ( ret != 0 ) {
                        return ret ;
                    }
                }

                v_tree.down = first_node->next ;
                free(first_node) ;
            }
        } else {
            // The LHS is a pointer dimension.

            // Check if this is a nested list (multi-dimensional)
            PyObject *first_elem = PyTuple_GetItem( obj1 , 0 ) ;
            PyObject *check_elem = first_elem;
            if ( PyList_Check(first_elem) ) {
                check_elem = PyList_AsTuple(first_elem);
            }

            if ( PyTuple_Check(check_elem) && ref_copy.num_index_left > 1 ) {
                // Multi-dimensional assignment to already-allocated pointer structure
                // e.g. 3D pointer array assignment
                // In C++ code, double *** foo_ptr_3d ;
                // In Python input file:
                //     xxx.foo_ptr_3d = [[[...], [...]], [[...], [...]], ...]
                //     Then later, another assignment to a slice of it such as xxx.foo_ptr_3d[0] = [[...], [...]]
                //     (foo_ptr_foo_ptr_3d[0] = [[...],[...]] where foo_ptr_3d was already assigned)
                // Check if we can reuse existing memory (only for 2D case)
                if ( ref_copy.num_index_left == 2 ) {
                    // Get the current pointer value
                    void* existing_ptr = *(void**)ref_copy.address;

                    if ( canReuseExisting2DMemory(existing_ptr, obj1, ref_copy) ) {
                        // Memory exists and dimensions match - do in-place assignment
                        ret = assign2DInPlace(existing_ptr, obj1, ref_copy);
                        if ( ret != 0 ) {
                            return ret;
                        }
                        // Skip ref_assignment since we did in-place update
                        return 0;
                    }
                }

                // Either not 2D, or no existing memory, or dimensions don't match
                // Use V_TREE approach (will allocate new memory)
                ret = buildNestedVTree( &(v_tree.down), obj1, ref_copy, ref_copy.num_index_left );
                if ( ret != 0 ) {
                    return ret;
                }
            } else {
                // 1D case: create a new allocation from the data on the right
                v_tree.v_data->type = TRICK_VOID_PTR ;
                switch (ref_copy.attr->type) {
                    case TRICK_UNSIGNED_CHARACTER:
                        v_tree.v_data->value.vp = assign_array< unsigned char > ( obj1 , ref_copy ) ;
                    break ;
                    case TRICK_SHORT:
                        v_tree.v_data->value.vp = assign_array< short > ( obj1 , ref_copy ) ;
                    break ;
                    case TRICK_UNSIGNED_SHORT:
                        v_tree.v_data->value.vp = assign_array< unsigned short > ( obj1 , ref_copy ) ;
                    break ;
                    case TRICK_INTEGER:
                        v_tree.v_data->value.vp = assign_array< int > ( obj1 , ref_copy ) ;
                    break ;
                    case TRICK_UNSIGNED_INTEGER:
                        v_tree.v_data->value.vp = assign_array< unsigned int > ( obj1 , ref_copy ) ;
                    break ;
                    case TRICK_LONG:
                        v_tree.v_data->value.vp = assign_array< long > ( obj1 , ref_copy ) ;
                    break ;
                    case TRICK_UNSIGNED_LONG:
                        v_tree.v_data->value.vp = assign_array< unsigned long > ( obj1 , ref_copy ) ;
                    break ;
                    case TRICK_LONG_LONG:
                        v_tree.v_data->value.vp = assign_array< long long > ( obj1 , ref_copy ) ;
                    break ;
                    case TRICK_UNSIGNED_LONG_LONG:
                        v_tree.v_data->value.vp = assign_array< unsigned long long > ( obj1 , ref_copy ) ;
                    break ;
                    case TRICK_DOUBLE:
                        v_tree.v_data->value.vp = assign_array< double > ( obj1 , ref_copy ) ;
                    break ;
                    case TRICK_FLOAT:
                        v_tree.v_data->value.vp = assign_array< float > ( obj1 , ref_copy ) ;
                    break ;
                    case TRICK_BOOLEAN:
                        v_tree.v_data->value.vp = assign_array< bool > ( obj1 , ref_copy ) ;
                    break ;
                    default:
                    break ;
                }
                //cout << "\033[33mGoing to set a pointer in swig_ref::set_item!!\033[0m" << endl ;
            }
        }
        //cout << "finished making v_tree" << endl ;
    } else if (SWIG_IsOK(SWIG_ConvertPtr(obj1, &argp2,SWIG_TypeQuery("void *"), 0 ))) {
        //cout << "\033[31mHERE in swig_ref set_item with void *!!\033[0m" << endl ;
        v_tree.v_data->type = TRICK_VOID_PTR ;
        v_tree.v_data->value.vp = reinterpret_cast< void * >(argp2) ;
    } else {
        // Test and see if a the RHS is a compatible pointer to our variable.
#if SWIG_VERSION >= 0x010332
        SwigPyObject * swig_obj = SWIG_Python_GetSwigThis(obj1) ;
#else
        PySwigObject * swig_obj = SWIG_Python_GetSwigThis(obj1) ;
#endif
        if ( swig_obj != NULL ) {
            void *argp1 = NULL ;
            std::string array_type_name = std::string("_p_") + ref.attr->type_name ;
            if ( SWIG_IsOK(SWIG_ConvertPtr(obj1,&argp1,SWIG_TypeQuery(array_type_name.c_str()),SWIG_POINTER_DISOWN))) {
                v_tree.v_data->type = TRICK_VOID_PTR ;
                v_tree.v_data->value.vp = argp1 ;
            } else {
                std::cout << "\033[31mswig_ref::__setitem__ could not convert  " <<
                 SWIG_TypePrettyName(SWIG_Python_GetSwigThis(obj1)->ty) <<
                 " to " << ref.attr->type_name << "\033[0m" << std::endl ;
                return TRICK_SWIG_PARAMETER_INCORRECT_TYPE ;
            }
        } else {
            // Cannot determine what the RHS is.
            std::cout << "\033[31mHERE3 in swig_ref set_item with unknown type\033[0m" << std::endl ;
            return -1 ;
        }
    }

    //cout << "HERE in swig_ref set_item!! " << ii << endl ;
    ret = trick_MM->ref_assignment( &ref_copy , &v_tree ) ;

    free(v_tree.v_data) ;
    curr_node = v_tree.down ;
    while ( curr_node != NULL ) {
        V_TREE * next_node = curr_node->next ;
        free(curr_node->v_data) ;
        free(curr_node) ;
        curr_node = next_node ;
    }
    return(ret) ;
}

template < class VALUE_TYPE >
VALUE_TYPE getValue(void * address , Py_ssize_t index ) {
    VALUE_TYPE * casted_address = ( VALUE_TYPE * ) address ;
    return casted_address[index] ;
}

int copyValue( V_DATA * v_data , REF2 & ref , Py_ssize_t index ) {
    switch (ref.attr->type) {
        case TRICK_CHARACTER:
            v_data->value.ll = getValue< char >(ref.address , index) ;
            v_data->type = TRICK_LONG_LONG ;
            break ;
        case TRICK_UNSIGNED_CHARACTER:
            v_data->value.ll = getValue< unsigned char >(ref.address , index) ;
            v_data->type = TRICK_LONG_LONG ;
            break ;
        case TRICK_SHORT:
            v_data->value.ll = getValue< short >(ref.address , index) ;
            v_data->type = TRICK_LONG_LONG ;
            break ;
        case TRICK_UNSIGNED_SHORT:
            v_data->value.ll = getValue< unsigned short >(ref.address , index) ;
            v_data->type = TRICK_LONG_LONG ;
            break ;
        case TRICK_INTEGER:
            v_data->value.ll = getValue< int >(ref.address , index) ;
            v_data->type = TRICK_LONG_LONG ;
            break ;
        case TRICK_UNSIGNED_INTEGER:
            v_data->value.ll = getValue< unsigned int >(ref.address , index) ;
            v_data->type = TRICK_LONG_LONG ;
            break ;
        case TRICK_LONG:
            v_data->value.ll = getValue< long >(ref.address , index) ;
            v_data->type = TRICK_LONG_LONG ;
            break ;
        case TRICK_UNSIGNED_LONG:
            v_data->value.ll = getValue< unsigned long >(ref.address , index) ;
            v_data->type = TRICK_LONG_LONG ;
            break ;
        case TRICK_LONG_LONG:
            v_data->value.ll = getValue< long long >(ref.address , index) ;
            v_data->type = TRICK_LONG_LONG ;
            break ;
        case TRICK_UNSIGNED_LONG_LONG:
            v_data->value.ll = getValue< unsigned long long >(ref.address , index) ;
            v_data->type = TRICK_LONG_LONG ;
            break ;
        case TRICK_DOUBLE:
            v_data->value.d = getValue< double >(ref.address , index) ;
            v_data->type = TRICK_DOUBLE ;
            break ;
        case TRICK_FLOAT:
            v_data->value.d = getValue< float >(ref.address , index) ;
            v_data->type = TRICK_DOUBLE ;
            break ;
        case TRICK_BOOLEAN:
            v_data->value.ll = getValue< bool >(ref.address , index) ;
            v_data->type = TRICK_LONG_LONG ;
            break ;
        default:
            break ;
    }
    return 0 ;
}

int swig_ref::__setitem__( PyObject * index , PyObject * obj1 ) {

    int ret = 0 ;
    void * argp2 ;

    /* Index types of ints, floats, swig_ints, and swig_doubles sent to __setitem__(int, PyObject *) */
    if ( PyInt_Check(index)) {
        ret = __setitem__((int)PyInt_AsLong(index), obj1) ;
    } else if ( PyFloat_Check(index)) {
        ret = __setitem__((int)PyFloat_AsDouble(index), obj1) ;
    } else if (SWIG_IsOK(SWIG_ConvertPtr(index, &argp2,SWIG_TypeQuery("_p_swig_int"), 0 ))) {
        swig_int * temp_m = reinterpret_cast<swig_int *>(argp2) ;
        ret = __setitem__((int)(temp_m->value), obj1) ;
    } else if (SWIG_IsOK(SWIG_ConvertPtr(index, &argp2,SWIG_TypeQuery("_p_swig_double"), 0 ))) {
        swig_double * temp_m = reinterpret_cast<swig_double *>(argp2) ;
        ret = __setitem__((int)(temp_m->value), obj1) ;
    } else if ( PySlice_Check(index)) {
        Py_ssize_t ii , start , stop , step ;
        int status ;
        // TODO: make sure obj1 is list or tuple.  Handle singular values on RHS

        // get the start, stop, and step value of the slice
#if PY_VERSION_HEX >= 0x03000000
        status = PySlice_GetIndices( index , ref.attr->index[0].size , &start , &stop , &step ) ;
#else
        status = PySlice_GetIndices( (PySliceObject *)index , ref.attr->index[0].size , &start , &stop , &step ) ;
#endif
        if ( status == 0 ) {

            V_TREE v_tree ;
            V_TREE * first_node , * curr_node ;
            unsigned int size ;
            unsigned int expected_slice_size ;

            if ( PyList_Check(obj1) ) {
                obj1 = PyList_AsTuple(obj1) ;
            }
            if ( PyTuple_Check(obj1) ) {
                size = PyTuple_Size(obj1) ;
            } else {
                size = 1 ;
            }

            expected_slice_size = ( stop - start + 1 ) / step ;
            if ( (step == 1) or (expected_slice_size == size) ) {

                // create a dummy first node.  This simplifys code underneath.  We will throw this first node away.
                first_node = curr_node = ( V_TREE * )calloc( 1 , sizeof(V_TREE) ) ;

                // copy the current data before replacement slice
                for ( ii = 0 ; ii < start ; ii++ ) {
                    newVTreeNode(&curr_node) ;
                    copyValue( curr_node->v_data , ref , ii ) ;
                }

                // insert slice data
                if ( step == 1 ) {
                    // normal slice insertion, copy the whole list from RHS to vtree structure.
                    if ( PyTuple_Check(obj1) ) {
#if PY_VERSION_HEX < 0x02050000
                        for( ii = 0 ; ii < (int)size ; ii++ ) {
#else
                        for( ii = 0 ; ii < size ; ii++ ) {
#endif
                            newVTreeNode(&curr_node) ;
                            PyObject *o = PyTuple_GetItem( obj1 , ii ) ;
                            ret = setVData( curr_node->v_data , o , ref ) ;
                        }
                    } else {
                        newVTreeNode(&curr_node) ;
                        ret = setVData( curr_node->v_data , obj1 , ref ) ;
                    }
                } else {
                    // extended slice insertion where step != 1.  Replace each <step> element in existing data
                    for( ii = 0 ; ii < stop - start ; ii++ ) {
                        newVTreeNode(&curr_node) ;
                        if ( ii % step == 0 ) {
                            if ( PyTuple_Check(obj1) ) {
                                PyObject *o = PyTuple_GetItem( obj1 , ii / step) ;
                                ret = setVData( curr_node->v_data , o , ref ) ;
                            } else {
                                ret = setVData( curr_node->v_data , obj1 , ref ) ;
                            }
                        } else {
                            copyValue( curr_node->v_data , ref , ii + start ) ;
                        }
                    }
                }

                // copy the current data after replacement slice
                for ( ii = stop ; ii < ref.attr->index[0].size ; ii++ ) {
                    newVTreeNode(&curr_node) ;
                    copyValue( curr_node->v_data , ref , ii ) ;
                }

                // skip first node because it has no data.
                v_tree.down = first_node->next ;
                free(first_node) ;

                REF2 ref_copy = ref ;
                ret = trick_MM->ref_assignment( &ref_copy , &v_tree ) ;

                // Free all of the allocations made above.
                curr_node = v_tree.down ;
                while ( curr_node != NULL ) {
                    V_TREE * next_node = curr_node->next ;
                    free(curr_node->v_data) ;
                    free(curr_node) ;
                    curr_node = next_node ;
                }
            } else {
                // RHS does not fit in space specified in slice
                ret = TRICK_SWIG_SLICE_ASSIGNMENT ;
            }
        }
    } else {
        /* Can't figure out what the index type was. */
        ret = TRICK_SWIG_UNKNOWN_INDEX_TYPE ;
    }

    return ret ;

}

PyObject * swig_ref::__getitem__(int ii) {

    //cout << "In swig_ref getitem!! " << ii  << " start address = " << ref.address << endl ;
    V_DATA v_data ;
    PyObject * ret = NULL ;
    int jj ;
    int status = 0 ;
    REF2 temp_ref ;

    deref_address() ;
    // convert python negative index to positive index
    temp_ref = ref ;
    if ( ii < 0 ) {
        ii += temp_ref.attr->index[0].size ;
    }

    v_data.type = TRICK_INTEGER ;
    v_data.value.i = ii ;


    /* Do a bounds check before calling ref_dim.  Ref_dim would catch this error too, but it
       prints an error message that throws visitors off from the python error.  Returining NULL
       will cause the input processor to die with an IndexError */
    if ( (int)ii >= temp_ref.attr->index[0].size or ii < 0 ) {
        return NULL ;
    } else {
        status = trick_MM->ref_dim( &temp_ref , &v_data ) ;
        // Error in ref_dim
        if ( status != 0 ) {
            return NULL ;
        }
    }

    if ( temp_ref.num_index_left == 0 ) {
        switch ( temp_ref.attr->type ) {
            case TRICK_CHARACTER: {
                swig_int * t = new swig_int ;
                t->value = *(char *)temp_ref.address ;
                t->units = temp_ref.attr->units ;
                ret = SWIG_NewPointerObj(SWIG_as_voidptr(t), SWIG_TypeQuery("_p_swig_int"), SWIG_POINTER_OWN);
            } break ;
            case TRICK_UNSIGNED_CHARACTER: {
                swig_int * t = new swig_int ;
                t->value = *(unsigned char *)temp_ref.address ;
                t->units = temp_ref.attr->units ;
                ret = SWIG_NewPointerObj(SWIG_as_voidptr(t), SWIG_TypeQuery("_p_swig_int"), SWIG_POINTER_OWN);
            } break ;
            case TRICK_SHORT: {
                swig_int * t = new swig_int ;
                t->value = *(short *)temp_ref.address ;
                t->units = temp_ref.attr->units ;
                ret = SWIG_NewPointerObj(SWIG_as_voidptr(t), SWIG_TypeQuery("_p_swig_int"), SWIG_POINTER_OWN);
            } break ;
            case TRICK_UNSIGNED_SHORT: {
                swig_int * t = new swig_int ;
                t->value = *(unsigned short *)temp_ref.address ;
                t->units = temp_ref.attr->units ;
                ret = SWIG_NewPointerObj(SWIG_as_voidptr(t), SWIG_TypeQuery("_p_swig_int"), SWIG_POINTER_OWN);
            } break ;
            case TRICK_INTEGER: {
                swig_int * t = new swig_int ;
                t->value = *(int *)temp_ref.address ;
                t->units = temp_ref.attr->units ;
                ret = SWIG_NewPointerObj(SWIG_as_voidptr(t), SWIG_TypeQuery("_p_swig_int"), SWIG_POINTER_OWN);
            } break ;
            case TRICK_UNSIGNED_INTEGER: {
                swig_int * t = new swig_int ;
                t->value = *(unsigned int *)temp_ref.address ;
                t->units = temp_ref.attr->units ;
                ret = SWIG_NewPointerObj(SWIG_as_voidptr(t), SWIG_TypeQuery("_p_swig_int"), SWIG_POINTER_OWN);
            } break ;
            case TRICK_LONG: {
                swig_int * t = new swig_int ;
                t->value = *(long *)temp_ref.address ;
                t->units = temp_ref.attr->units ;
                ret = SWIG_NewPointerObj(SWIG_as_voidptr(t), SWIG_TypeQuery("_p_swig_int"), SWIG_POINTER_OWN);
            } break ;
            case TRICK_UNSIGNED_LONG: {
                swig_int * t = new swig_int ;
                t->value = *(unsigned long *)temp_ref.address ;
                t->units = temp_ref.attr->units ;
                ret = SWIG_NewPointerObj(SWIG_as_voidptr(t), SWIG_TypeQuery("_p_swig_int"), SWIG_POINTER_OWN);
            } break ;
            case TRICK_LONG_LONG: {
                swig_int * t = new swig_int ;
                t->value = *(long long *)temp_ref.address ;
                t->units = temp_ref.attr->units ;
                ret = SWIG_NewPointerObj(SWIG_as_voidptr(t), SWIG_TypeQuery("_p_swig_int"), SWIG_POINTER_OWN);
            } break ;
            case TRICK_UNSIGNED_LONG_LONG: {
                swig_int * t = new swig_int ;
                t->value = *(unsigned long long *)temp_ref.address ;
                t->units = temp_ref.attr->units ;
                ret = SWIG_NewPointerObj(SWIG_as_voidptr(t), SWIG_TypeQuery("_p_swig_int"), SWIG_POINTER_OWN);
            } break ;
            case TRICK_DOUBLE: {
                //cout << "swig_ref::__getitem__ returning a swig_double!! " << temp_ref.address << endl ;
                swig_double * t = new swig_double ;
                t->value = *(double *)temp_ref.address ;
                t->units = temp_ref.attr->units ;
                ret = SWIG_NewPointerObj(SWIG_as_voidptr(t), SWIG_TypeQuery("_p_swig_double"), SWIG_POINTER_OWN);
            } break ;
            case TRICK_FLOAT: {
                swig_double * t = new swig_double ;
                t->value = *(float *)temp_ref.address ;
                t->units = temp_ref.attr->units ;
                ret = SWIG_NewPointerObj(SWIG_as_voidptr(t), SWIG_TypeQuery("_p_swig_double"), SWIG_POINTER_OWN);
            } break ;
            case TRICK_BOOLEAN: {
                swig_int * t = new swig_int ;
                t->value = *(bool *)temp_ref.address ;
                t->units = temp_ref.attr->units ;
                ret = SWIG_NewPointerObj(SWIG_as_voidptr(t), SWIG_TypeQuery("_p_swig_int"), SWIG_POINTER_OWN);
            } break ;
            default: {
                std::string type_name ;
                type_name = std::string("_p_") + temp_ref.attr->type_name ;
                // change namespace ':' separators to '_'
                std::replace( type_name.begin() , type_name.end() , ':' , '_' ) ;

                // remove any "*" characters at the end of the type_name
                size_t found = type_name.find(" ") ;
                if ( found != std::string::npos) {
                    type_name.erase(found) ;
                }
                ret = SWIG_NewPointerObj(SWIG_as_voidptr(temp_ref.address), SWIG_TypeQuery(type_name.c_str()), 0);
            } break ;
        }
    } else if ( temp_ref.attr->type == TRICK_STRUCTURED and temp_ref.attr->num_index == 2) {
        // Special case where we are dereferencing a double dimensional structure.
        // We want to return a pointer to the correct structure type, not a swig_ref.
        std::string type_name ;
        void * new_addr ;
        //std::cout << "double dim returing type " << temp_ref.attr->type_name << std::endl ;
        type_name = std::string("_p_") + temp_ref.attr->type_name ;

        // change namespace ':' separators to '_'
        std::replace( type_name.begin() , type_name.end() , ':' , '_' ) ;

        // remove any "*" characters at the end of the type_name
        size_t found = type_name.find(" ") ;
        if ( found != std::string::npos) {
            type_name.erase(found) ;
        }

        // Dereference the pointer if this was a pointer dimension
        if ( temp_ref.attr->index[temp_ref.num_index].size == 0 ) {
            new_addr = *(void**)temp_ref.address ;
        } else {
            new_addr = temp_ref.address ;
        }
        ret = SWIG_NewPointerObj(SWIG_as_voidptr(new_addr), SWIG_TypeQuery(type_name.c_str()), 0);
    } else {
        /*
           Takes care of non final dimensions for all cases.  We have already called ref_dim
           to get to the correct index in the current array. Create a new swig_ref that
           contains the indexed address.  All of the dimensional information will be shifted
           by one in the new swig_ref.
         */

        /* Create a new swig_ref to hold the new dereferenced variable */
        swig_ref * new_ref = new swig_ref ;

        /* Copy temp_ref that holds the indexed address as a result of ref_dim */
        new_ref->ref = temp_ref ;

        /* Create a new attributes instance to because we're going to be modifying it below */
        ATTRIBUTES * new_attr = new ATTRIBUTES() ;

        /* Start with the current attributes we have */
        *new_attr = *temp_ref.attr ;
        new_ref->ref.attr = new_attr ;

        /* make copies of the strings for type_name and units */
        new_ref->ref.attr->type_name = strdup(temp_ref.attr->type_name) ;
        new_ref->ref.attr->units = strdup(temp_ref.attr->units) ;

        // Shift all remaining dimension information to the left
        new_ref->ref.attr->num_index-- ;
        new_ref->ref.num_index-- ;
        for ( jj = 0 ; jj < new_ref->ref.attr->num_index ; jj++ ) {
            new_ref->ref.attr->index[jj].size = new_ref->ref.attr->index[jj + 1].size ;
        }

        // If the current index is a pointer mark this swig_ref as needing to dereference
        if ( new_ref->ref.attr->index[0].size == 0 ) {
            new_ref->need_to_deref = true ;
        }

        ret = SWIG_NewPointerObj(SWIG_as_voidptr(new_ref), SWIG_TypeQuery("_p_swig_ref"), 0);
    }
    return ret ;
}

template < class SWIG_TYPE , class VALUE_TYPE >
PyObject * getValueAsSwigObjectByType( REF2 & temp_ref , Py_ssize_t index , const char * swig_type ) {
    SWIG_TYPE * t = new SWIG_TYPE ;
    VALUE_TYPE * casted_address = ( VALUE_TYPE * ) temp_ref.address ;
    t->value = *(casted_address + index ) ;
    t->units = temp_ref.attr->units ;
    return SWIG_NewPointerObj(SWIG_as_voidptr(t), SWIG_TypeQuery(swig_type), SWIG_POINTER_OWN);
}

PyObject * getValueAsObject( REF2 & temp_ref , Py_ssize_t index ) {
   switch ( temp_ref.attr->type ) {
       case TRICK_CHARACTER:
           return getValueAsSwigObjectByType< swig_int , char >(temp_ref , index , "_p_swig_int") ;
       case TRICK_UNSIGNED_CHARACTER:
           return getValueAsSwigObjectByType< swig_int , unsigned char >(temp_ref , index , "_p_swig_int") ;
       case TRICK_SHORT:
           return getValueAsSwigObjectByType< swig_int , short >(temp_ref , index , "_p_swig_int") ;
       case TRICK_UNSIGNED_SHORT:
           return getValueAsSwigObjectByType< swig_int , unsigned short >(temp_ref , index , "_p_swig_int") ;
       case TRICK_INTEGER:
           return getValueAsSwigObjectByType< swig_int , int >(temp_ref , index , "_p_swig_int") ;
       case TRICK_UNSIGNED_INTEGER:
           return getValueAsSwigObjectByType< swig_int , unsigned int >(temp_ref , index , "_p_swig_int") ;
       case TRICK_LONG:
           return getValueAsSwigObjectByType< swig_int , long >(temp_ref , index , "_p_swig_int") ;
       case TRICK_UNSIGNED_LONG:
           return getValueAsSwigObjectByType< swig_int , unsigned long >(temp_ref , index , "_p_swig_int") ;
       case TRICK_LONG_LONG:
           return getValueAsSwigObjectByType< swig_int , long long >(temp_ref , index , "_p_swig_int") ;
       case TRICK_UNSIGNED_LONG_LONG:
           return getValueAsSwigObjectByType< swig_int , unsigned long long >(temp_ref , index , "_p_swig_int") ;
       case TRICK_DOUBLE :
           return getValueAsSwigObjectByType< swig_double , double >(temp_ref , index , "_p_swig_double") ;
       case TRICK_FLOAT:
           return getValueAsSwigObjectByType< swig_double , float >(temp_ref , index , "_p_swig_double") ;
       case TRICK_BOOLEAN:
           return getValueAsSwigObjectByType< swig_int , bool >(temp_ref , index , "_p_swig_int") ;
       default :
           return NULL ;
   }
   return NULL ;
}

PyObject * swig_ref::__getitem__( PyObject * index ) {

    PyObject * ret = NULL ;
    int status = 0 ;

    deref_address() ;
    REF2 temp_ref = ref ;

    void * argp2 ;

    //std::cout << "In swig_ref::__getitem__( PyObject * index )!!!\n" << std::endl ;

    if ( PyInt_Check(index)) {
        ret = __getitem__((int)PyInt_AsLong(index)) ;
    } else if ( PyFloat_Check(index)) {
        ret = __getitem__((int)PyFloat_AsDouble(index)) ;
    } else if (SWIG_IsOK(SWIG_ConvertPtr(index, &argp2,SWIG_TypeQuery("_p_swig_int"), 0 ))) {
        swig_int * temp_m = reinterpret_cast<swig_int *>(argp2) ;
        ret = __getitem__((int)(temp_m->value)) ;
    } else if (SWIG_IsOK(SWIG_ConvertPtr(index, &argp2,SWIG_TypeQuery("_p_swig_double"), 0 ))) {
        swig_double * temp_m = reinterpret_cast<swig_double *>(argp2) ;
        ret = __getitem__((int)(temp_m->value)) ;
    } else if ( PySlice_Check(index)) {
        Py_ssize_t ii , start , stop , step ;

        // get the start, stop, and step value of the slice
#if PY_VERSION_HEX >= 0x03000000
        status = PySlice_GetIndices( index , temp_ref.attr->index[0].size , &start , &stop , &step ) ;
#else
        status = PySlice_GetIndices( (PySliceObject *)index , temp_ref.attr->index[0].size , &start , &stop , &step ) ;
#endif
        if ( status == 0 ) {
            // only work with final index of array... no pointer dims yet. (Alex)
            if ( temp_ref.num_index_left == 1 ) {
                ret = PyList_New(0) ;
                if ( step > 0 ) {
                    for ( ii = start ; ii < stop ; ii += step ) {
                        PyObject * new_obj = getValueAsObject( temp_ref , ii ) ;
                        if ( new_obj ) {
                            PyList_Append( ret , new_obj ) ;
                        }
                    }
                } else {
                    for ( ii = start ; ii > stop ; ii += step ) {
                        PyObject * new_obj = getValueAsObject( temp_ref , ii ) ;
                        if ( new_obj ) {
                            PyList_Append( ret , new_obj ) ;
                        }
                    }
                }
            } else {
                std::cout << "Array slices currently only work with single dimensional objects" << std::endl ;
            }
        } else {
            std::cout << "Slice parameters do not fit within list length of " << temp_ref.attr->index[0].size << std::endl ;
        }
    } else {
        std::cout << "Unknown object type as array index" << std::endl ;
    }

    return ret ;
}
