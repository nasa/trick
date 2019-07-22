#include "trick/memorymanager_c_intf.h" // for get_size.
#include "../include/WSSessionVariable.hh"
#include <math.h> // for fpclassify
#include <iomanip> // for setprecision

WSsessionVariable::WSsessionVariable(REF2 * ref ) {
    varInfo = ref;
    address = varInfo->address;
    size = varInfo->attr->size ;
    deref = false;

    TRICK_TYPE string_type = varInfo->attr->type ;

    if ( varInfo->num_index == varInfo->attr->num_index ) {
        // single value
    } else if ( varInfo->attr->index[varInfo->attr->num_index - 1].size != 0 ) {
        // Constrained array
        for ( int i = varInfo->attr->num_index-1;  i > varInfo->num_index-1 ; i-- ) {
            size *= varInfo->attr->index[i].size ;
        }
    } else {
        // Unconstrained array
        if ((varInfo->attr->num_index - varInfo->num_index) > 1 ) {
            printf("Variable Server Error: var_add(%s) requests more than one dimension of dynamic array.\n", varInfo->reference);
            printf("Data is not contiguous so returned values are unpredictable.\n") ;
        }
        if ( varInfo->attr->type == TRICK_CHARACTER ) {
            string_type = TRICK_STRING ;
            deref = true;
        } else if ( varInfo->attr->type == TRICK_WCHAR ) {
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
}

WSsessionVariable::~WSsessionVariable() {
    if (varInfo != NULL) free( varInfo );
}


const char* WSsessionVariable::getName() {
    return varInfo->reference;
}

static void write_quoted_str( std::ostream& os, const char* s) {
    int ii;
    int len = strlen(s);
    os << "\"" ;
    for (ii=0 ; ii<len ; ii++) {
        switch ((s)[ii]) {
        case '\n': os << "\\n"; break;
        case '\t': os << "\\t"; break;
        case '\b': os << "\\b"; break;
        case '\"': os << "\\\""; break;
        default  : os << s[ii] ; break;
        }
    }
    os << "\"" ;
}

void WSsessionVariable::write_value( std::ostream& outs ) {

    switch(varInfo->attr->type) {
        case TRICK_UNSIGNED_CHARACTER:
            outs << std::dec << (int)*(unsigned char*)address ;
        break;
        case TRICK_BOOLEAN:
            if (*(bool*)address) {
                outs << "\"true\"" ;
            } else {
                outs << "\"false\"" ;
            }
        break;
        case TRICK_CHARACTER:
            if (isprint( *(char*)address) ) {
                outs << "'" << *(char*)address << "'" ;
            } else {
                unsigned int ch = *(unsigned char*)address;
                  outs << "'\\x" << std::hex << ch << "'" ;
            }
        break;
        case TRICK_WCHAR:
            outs << std::dec << *(wchar_t*)address;
            break;
        case TRICK_SHORT:
            outs << std::dec << *(short*)address;
            break;
        case TRICK_UNSIGNED_SHORT:
            outs << std::dec << *(unsigned short*)address;
            break;
        case TRICK_INTEGER:
            outs << std::dec << *(int*)address;
            break;
        case TRICK_UNSIGNED_INTEGER:
            outs << std::dec << *(unsigned int*)address;
            break;
        case TRICK_LONG:
            outs << std::dec << *(long*)address;
            break;
        case TRICK_UNSIGNED_LONG:
            outs << std::dec << *(unsigned long*)address;
            break;
        case TRICK_FLOAT:
            if (fpclassify( *(float*)address) != FP_NAN) {
                outs << std::setprecision(8) << *(float*)address;
            } else {
                outs << "NAN";
            }
            break;
        case TRICK_DOUBLE:
            if (fpclassify( *(double*)address) != FP_NAN) {
                outs << std::setprecision(16) << *(double*)address;
            } else {
                outs << "NAN";
            }
            break;
//        case TRICK_BITFIELD: {
//                int sbf = 0;
//                src_addr = (char*)address + offset * (size_t)attr->size;
//                if (attr->size == sizeof(int)) {
//                     sbf = extract_bitfield_any( *(int*)src_addr, attr->size, attr->index[0].start, attr->index[0].size);
//                } else if (attr->size == sizeof(short)) {
//                     sbf = extract_bitfield_any( *(short*)src_addr, attr->size, attr->index[0].start, attr->index[0].size);
//                } else if (attr->size == sizeof(char)) {
//                     sbf = extract_bitfield_any( *(char*)src_addr, attr->size, attr->index[0].start, attr->index[0].size);
//                } else {
//                     message_publish(MSG_ERROR, "Checkpoint Agent INTERNAL ERROR:\n"
//                                                "Unsupported bitfield size (%d) bytes.\n", attr->size) ;
//                }
//                outs << std::dec << sbf;
//            } break;
//        case TRICK_UNSIGNED_BITFIELD: {
//                int bf = 0;
//                src_addr = (char*)address + offset * (size_t)attr->size;
//                if (attr->size == sizeof(int)) {
//                     bf = extract_unsigned_bitfield_any( *(unsigned int*)src_addr, attr->size, attr->index[0].start, attr->index[0].size);
//                } else if (attr->size == sizeof(short)) {
//                     bf = extract_unsigned_bitfield_any( *(unsigned short*)src_addr, attr->size, attr->index[0].start, attr->index[0].size);
//                } else if (attr->size == sizeof(char)) {
//                     bf = extract_unsigned_bitfield_any( *(unsigned char*)src_addr, attr->size, attr->index[0].start, attr->index[0].size);
//                } else {
//                     message_publish(MSG_ERROR, "Checkpoint Agent INTERNAL ERROR:\n"
//                                                "Unsupported bitfield size (%d) bytes.\n", attr->size) ;
//                }
//                outs << std::dec << bf;
//            } break;
        case TRICK_LONG_LONG:
            outs << std::dec << *(long long*)address;
            break;
        case TRICK_UNSIGNED_LONG_LONG:
            outs << std::dec << *(unsigned long long*)address;
            break;
        case TRICK_STRING:
            write_quoted_str(outs, (*(std::string*)address).c_str());
            break;
        default:
            outs << "\"Error\""; // ERROR
            break;
    }
}
