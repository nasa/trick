#include <stdlib.h>
#include <iostream>
#include <udunits2.h>
#include <math.h> // for fpclassify
#include <iomanip> // for setprecision
#include <string.h>

#include "trick/VariableReference.hh"
#include "trick/memorymanager_c_intf.h"
#include "trick/wcs_ext.h"
#include "trick/bitfield_proto.h"

// Static variables to be addresses that are known to be the error ref address
// Idk if this is the best way to do this, but it's what john/skraddwelkdh did in the past
// so ¯\_(ツ)_/¯
int Trick::VariableReference::bad_ref_int = 0 ;
int Trick::VariableReference::do_not_resolve_bad_ref_int = 0 ;

Trick::VariableReference::VariableReference(std::string var_name) {

    // get variable attributes from memory manager
    var_info = ref_attributes(var_name.c_str());

    // Handle error cases
    if ( var_info == NULL ) {
        // sendErrorMessage("Variable Server could not find variable %s.\n", var_name);
        printf("Variable Server could not find variable %s.\n", var_name.c_str());

        var_info = make_error_ref(var_name);
    } else if ( var_info->attr ) {
        if ( var_info->attr->type == TRICK_STRUCTURED ) {
            // sendErrorMessage("Variable Server: var_add cant add \"%s\" because its a composite variable.\n", var_name);
            printf("Variable Server: var_add cant add \"%s\" because its a composite variable.\n", var_name.c_str());

            free(var_info);
            var_info = NULL;
            var_info = make_error_ref(var_name);

        } else if ( var_info->attr->type == TRICK_STL ) {
            // sendErrorMessage("Variable Server: var_add cant add \"%s\" because its an STL variable.\n", var_name);
            printf("Variable Server: var_add cant add \"%s\" because its an STL variable.\n", var_name.c_str());

            free(var_info);
            var_info = NULL;
            var_info = make_error_ref(var_name);
        }
    } else {
        // sendErrorMessage("Variable Server: BAD MOJO - Missing ATTRIBUTES.");
        printf("Variable Server: BAD MOJO - Missing ATTRIBUTES.");

        free(var_info);
        var_info = NULL;
        var_info = make_error_ref(var_name);
    }

    // Set up member variables
    // Why are we keeping separate copies of address and size?
    // Maybe don't do that
    address = var_info->address;
    size = var_info->attr->size ;
    deref = false;

    // Deal with weirdness around string vs wstring
    TRICK_TYPE string_type = var_info->attr->type ;

    if ( var_info->num_index == var_info->attr->num_index ) {
        // single value
    } else if ( var_info->attr->index[var_info->attr->num_index - 1].size != 0 ) {
        // Constrained array
        for ( int i = var_info->attr->num_index-1;  i > var_info->num_index-1 ; i-- ) {
            size *= var_info->attr->index[i].size ;
        }
    } else {
        // Unconstrained array
        if ((var_info->attr->num_index - var_info->num_index) > 1 ) {
            printf("Variable Server Error: var_add(%s) requests more than one dimension of dynamic array.\n", var_info->reference);
            printf("Data is not contiguous so returned values are unpredictable.\n") ;
        }
        if ( var_info->attr->type == TRICK_CHARACTER ) {
            string_type = TRICK_STRING ;
            deref = true;
        } else if ( var_info->attr->type == TRICK_WCHAR ) {
            string_type = TRICK_WSTRING ;
        } else {
            deref = true ;
            size *= get_size((char*)address) ;
        }
    }
    // handle strings: set a max buffer size, the copy size may vary so will be set in copy_sim_data
    if (( string_type == TRICK_STRING ) || ( string_type == TRICK_WSTRING )) {
        size = MAX_ARRAY_LENGTH ;
    }

    // Allocate stage and write buffers
    stage_buffer = calloc(size, 1) ;
    write_buffer = calloc(size, 1) ;

    conversion_factor = cv_get_trivial();

    // // std::cout << "Successfully created reference, size " << size << std::endl;
    // Done!
}

Trick::VariableReference::~VariableReference() {
    // // std::cout << "In destructor for VariableReference" << std::endl;
    if (var_info != NULL) {
        // // std::cout << "Nulling out var_info" << std::endl;
        free( var_info );
        var_info = NULL;
    }
    if (stage_buffer != NULL) {
        // // std::cout << "Freeing stage_buffer" << std::endl;
        free (stage_buffer);
        stage_buffer = NULL;
    }
    if (write_buffer != NULL) {
        // // std::cout << "Freeing write_buffer" << std::endl;
        free (write_buffer);
        write_buffer = NULL;
    }
    if (conversion_factor != NULL) {
        cv_free(conversion_factor);
    }

    // // std::cout << "End of destructor" << std::endl;
}

const char* Trick::VariableReference::getName() {
    return var_info->reference;
}

const char* Trick::VariableReference::getUnits() {
    return var_info->attr->units;
}

int Trick::VariableReference::setUnits(char * new_units) {
    if (var_info->units != NULL)  {
        free(var_info->units);
        var_info->units = NULL;
    }
    
    var_info->units = new_units;
}

int Trick::VariableReference::setConversionFactor(cv_converter * new_conversion_factor) {
    if (conversion_factor != NULL)
        cv_free(conversion_factor);
    
    conversion_factor = new_conversion_factor;
}

void Trick::VariableReference::stageValue() {
    // Copy <size> bytes from <address> to staging_point.

    if (var_info->address == &bad_ref_int) {
        REF2 *new_ref = ref_attributes(var_info->reference);
        if (new_ref != NULL) {
            var_info = new_ref;
        }
    }

    // if there's a pointer somewhere in the address path, follow it in case pointer changed
    if ( var_info->pointer_present == 1 ) {
        address = follow_address_path(var_info) ;
        if (address == NULL) {
            std::string save_name(var_info->reference) ;
            free(var_info) ;
            var_info = NULL;
            var_info = make_error_ref(save_name) ;
            address = var_info->address ;
        } else if ( need_validate ) {
            // The address is not NULL.
            // If validate_address is on, check the memory manager if the address falls into
            // any of the memory blocks it knows of.  Don't do this if we have a std::string or
            // wstring type, or we already are pointing to a bad ref.
            if ( (string_type != TRICK_STRING) and
                    (string_type != TRICK_WSTRING) and
                    (var_info->address != &bad_ref_int) and
                    (get_alloc_info_of(address) == NULL) ) {
                std::string save_name(var_info->reference) ;
                free(var_info) ;
                var_info = NULL;
                var_info = make_error_ref(save_name) ;
                address = var_info->address ;
            }
        } else {
            var_info->address = address ;
        }

    }

    // if this variable is a string we need to get the raw character string out of it.
    if (( string_type == TRICK_STRING ) && !deref) {
        std::string * str_ptr = (std::string *)var_info->address ;
        address = (void *)(str_ptr->c_str()) ;
    }

    // if this variable itself is a pointer, dereference it
    if ( deref ) {
        address = *(void**)var_info->address ;
    }

    // handle c++ string and char*
    if ( string_type == TRICK_STRING ) {
        if (address == NULL) {
            size = 0 ;
        } else {
            size = strlen((char*)address) + 1 ;
        }
    }
    // handle c++ wstring and wchar_t*
    if ( string_type == TRICK_WSTRING ) {
        if (address == NULL) {
            size = 0 ;
        } else {
            size = wcslen((wchar_t *)address) * sizeof(wchar_t);
        }
    }
    if(address != NULL) {
        memcpy( stage_buffer , address , size ) ;
    }

    staged = true;

}

static void write_escaped_string( std::ostream& os, const char* s) {
    // std::cout << "Writing escaped string: " << s << std::endl;
    for (int ii=0 ; ii<strlen(s) ; ii++) {
        if (isprint(s[ii])) {
            os << s[ii];
        } else {
            switch ((s)[ii]) {
                case '\n': os << "\\n"; break;
                case '\t': os << "\\t"; break;
                case '\b': os << "\\b"; break;
                case '\a': os << "\\a"; break;
                case '\f': os << "\\f"; break;
                case '\r': os << "\\n"; break;
                case '\v': os << "\\v"; break;
                case '\"': os << "\\\""; break;
                default  : {
                    // Replicating behavior from original vs_format_ascii
                    char temp_s[6];
                    sprintf(temp_s, "\\x%02x", s[ii]);
                    os << temp_s ; 
                    break;
                }
            }
        }
    }
}

void Trick::VariableReference::writeValueAscii( std::ostream& out ) {
    // This is copied and modified from vs_format_ascii
    // There's a lot here that doesn't make sense to me that I need to come back to
    // There seems to be a huge buffer overflow issue in the original.
    // Only strings are checked for length, arrays aren't
    // But using a stream instead should make that better
    // The way that arrays are handled seems weird.

    // std::cout << "Starting writeValueAscii for " << getName() << std::endl;
    int bytes_written = 0;
    void * buf_ptr = write_buffer ;

    while (bytes_written < size) {
        // std::cout << "bytes_written: " << bytes_written << "\tsize: " << size << std::endl;
        bytes_written += var_info->attr->size ;

        // std::cout << "Type: " << var_info->attr->type << std::endl;

        switch (var_info->attr->type) {

        case TRICK_CHARACTER:
            // What does this if statment mean? Need to understand the memory manager more probably
            if (var_info->attr->num_index == var_info->num_index) {
                // Is this trying to convert a char to a double, and then converting it back to a char????
                // Why????
                out << (char)cv_convert_double(conversion_factor, *(char *)buf_ptr);
            } else {
                /* All but last dim specified, leaves a char array */
                write_escaped_string(out, (const char *) buf_ptr);
                bytes_written = size ;
            }
            break;
        case TRICK_UNSIGNED_CHARACTER:
            if (var_info->attr->num_index == var_info->num_index) {
                out << (unsigned char)cv_convert_double(conversion_factor,*(unsigned char *)buf_ptr);
            } else {
                /* All but last dim specified, leaves a char array */
                write_escaped_string(out, (const char *) buf_ptr);
                bytes_written = size ;
            }
            break;

        case TRICK_WCHAR:{
                if (var_info->attr->num_index == var_info->num_index) {
                    out << *(wchar_t *) buf_ptr;
                } else {
                    // convert wide char string char string
                    size_t len = wcs_to_ncs_len((wchar_t *)buf_ptr) + 1 ;
                    // if (len > MAX_VAL_STRLEN) {
                    //     return (-1);
                    // }

                    char temp_buf[len];
                    wcs_to_ncs((wchar_t *) buf_ptr, temp_buf, len);
                    out << temp_buf;
                    bytes_written = size ;
                }
            }
            break;

        case TRICK_STRING:
            if ((char *) buf_ptr != NULL) {
                write_escaped_string(out, (const char *) buf_ptr);
                bytes_written = size ;
            } else {
                out << '\0';
            }
            break;

        case TRICK_WSTRING:
            if ((wchar_t *) buf_ptr != NULL) {
                // convert wide char string char string
                size_t len = wcs_to_ncs_len((wchar_t *)buf_ptr) + 1 ;
                // if (len > MAX_VAL_STRLEN) {
                //     return (-1);
                // }

                char temp_buf[len];
                wcs_to_ncs((wchar_t *) buf_ptr, temp_buf, len);
                out << temp_buf;
                bytes_written = size ;
            } else {
                out << '\0';
            }
            break;

#if ( __linux | __sgi )
        case TRICK_BOOLEAN:
            // out << (unsigned char)cv_convert_double(conversion_factor,*(unsigned char *)buf_ptr);
            if (*(bool *) buf_ptr) {
                out << 1;
            } else {
                out << 0;
            }
            break;
#endif

        case TRICK_SHORT:
            out << (short)cv_convert_double(conversion_factor,*(short *)buf_ptr);
            break;

        case TRICK_UNSIGNED_SHORT:
            out << (unsigned short)cv_convert_double(conversion_factor,*(unsigned short *)buf_ptr);
            break;

        case TRICK_INTEGER:
        case TRICK_ENUMERATED:
#if ( __sun | __APPLE__ )
        case TRICK_BOOLEAN:
#endif
            out << (int)cv_convert_double(conversion_factor,*(int *)buf_ptr);
            break;

        case TRICK_BITFIELD:
            out << (GET_BITFIELD(buf_ptr, var_info->attr->size, var_info->attr->index[0].start, var_info->attr->index[0].size));
            break;

        case TRICK_UNSIGNED_BITFIELD:
            out << (GET_UNSIGNED_BITFIELD(buf_ptr, var_info->attr->size, var_info->attr->index[0].start, var_info->attr->index[0].size));
            break;
        case TRICK_UNSIGNED_INTEGER:
            out << (unsigned int)cv_convert_double(conversion_factor,*(unsigned int *)buf_ptr);
            break;

        case TRICK_LONG: {
            long l = *(long *)buf_ptr;
            if (conversion_factor != cv_get_trivial()) {
                l = (long)cv_convert_double(conversion_factor, l);
            }
            out << l;
            break;
        }

        case TRICK_UNSIGNED_LONG: {
            unsigned long ul = *(unsigned long *)buf_ptr;
            if (conversion_factor != cv_get_trivial()) {
                ul = (unsigned long)cv_convert_double(conversion_factor, ul);
            }
            out << ul;
            break;
        }

        case TRICK_FLOAT:
            out << std::setprecision(8) << cv_convert_float(conversion_factor,*(float *)buf_ptr);
            break;

        case TRICK_DOUBLE:
            // std::cout << "In convert trick double" << std::endl;
            out << std::setprecision(16) << *(double *)buf_ptr;
            // std::cout << "Done with convert trick double" << std::endl;
            break;

        case TRICK_LONG_LONG: {
            long long ll = *(long long *)buf_ptr;
            if (conversion_factor != cv_get_trivial()) {
                ll = (long long)cv_convert_double(conversion_factor, ll);
            }
            out << ll;
            break;
        }

        case TRICK_UNSIGNED_LONG_LONG: {
            unsigned long long ull = *(unsigned long long *)buf_ptr;
            if (conversion_factor != cv_get_trivial()) {
                ull = (unsigned long long)cv_convert_double(conversion_factor, ull);
            }
            out << ull;
            break;
        }

        case TRICK_NUMBER_OF_TYPES:
            out << "BAD_REF";
            break;

        default:{
            break;
        }
        } // end switch

        if (bytes_written < size) {
        // if returning an array, continue array as comma separated values
            out << ",";
            buf_ptr = (void*) ((long)buf_ptr + var_info->attr->size) ;
        }
    } //end while

    if (var_info->units) {
        if ( var_info->attr->mods & TRICK_MODS_UNITSDASHDASH ) {
            out << " {--}";
        } else {
            out << " {" << var_info->units << "}";
        }
    }

}

void Trick::VariableReference::tagAsInvalid () {
    var_info->address = (char*)&bad_ref_int;
    var_info->attr = new ATTRIBUTES() ;
    var_info->attr->type = TRICK_NUMBER_OF_TYPES ;
    var_info->attr->units = (char *)"--" ;
    var_info->attr->size = sizeof(int) ;
}

REF2* Trick::VariableReference::make_error_ref(std::string in_name) {
    REF2* new_ref;
    new_ref = (REF2*)calloc(1, sizeof(REF2));
    new_ref->reference = strdup(in_name.c_str()) ;
    new_ref->units = NULL ;
    new_ref->address = (char *)&bad_ref_int ;
    new_ref->attr = (ATTRIBUTES*)calloc(1, sizeof(ATTRIBUTES)) ;
    new_ref->attr->type = TRICK_NUMBER_OF_TYPES ;
    new_ref->attr->units = (char *)"--" ;
    new_ref->attr->size = sizeof(int) ;
    return new_ref;
}

void Trick::VariableReference::prepareForWrite() {
    void * temp_p = stage_buffer;
    stage_buffer = write_buffer;
    write_buffer = temp_p;

    staged = false;
    write_ready = true;
}

void Trick::VariableReference::writeValueBinary( std::ostream& out ) {

}

