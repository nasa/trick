
#include "trick/parameter_types.h"

const char* trickTypeCharString( TRICK_TYPE type, const char* name) {

    const char *type_spec;
    switch (type) {
       case TRICK_VOID:               type_spec = "void"; break;
       case TRICK_CHARACTER:          type_spec = "char"; break;
       case TRICK_UNSIGNED_CHARACTER: type_spec = "unsigned char"; break;
       case TRICK_STRING:             type_spec = "std::string"; break;
       case TRICK_SHORT:              type_spec = "short"; break;
       case TRICK_UNSIGNED_SHORT:     type_spec = "unsigned short"; break;
       case TRICK_INTEGER:            type_spec = "int"; break;
       case TRICK_UNSIGNED_INTEGER:   type_spec = "unsigned int"; break;
       case TRICK_LONG:               type_spec = "long"; break;
       case TRICK_UNSIGNED_LONG:      type_spec = "unsigned long"; break;
       case TRICK_FLOAT:              type_spec = "float"; break;
       case TRICK_DOUBLE:             type_spec = "double"; break;
       case TRICK_BITFIELD:           type_spec = "TRICK_BITFIELD";
       case TRICK_UNSIGNED_BITFIELD:  type_spec = "TRICK_UNSIGNED_BITFIELD";
       case TRICK_LONG_LONG:          type_spec = "long long"; break;
       case TRICK_UNSIGNED_LONG_LONG: type_spec = "unsigned long long"; break;
       case TRICK_FILE_PTR:           type_spec = "FILE*"; break;
       case TRICK_BOOLEAN:            type_spec = "bool"; break;
       case TRICK_WCHAR:              type_spec = "wchar_t"; break;
       case TRICK_WSTRING:            type_spec = "wchar_t*"; break;
       case TRICK_VOID_PTR:           type_spec = "void*"; break;
       case TRICK_ENUMERATED:         type_spec = name; break;
       case TRICK_STRUCTURED:         type_spec = name; break;
       case TRICK_OPAQUE_TYPE:        type_spec = name;  break;
       case TRICK_STL:                type_spec = name;  break;
       default:                       type_spec = "UNKNOWN_TYPE"; break;
    }
    return (type_spec);
}
