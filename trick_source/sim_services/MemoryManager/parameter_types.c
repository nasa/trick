// Is this function used anywhere? (Alex 10/27/14)

#include "trick/parameter_types.h"


const char* trick_type_name(TRICK_TYPE type) ;

static struct {
   TRICK_TYPE type;
   const char* name;
} trick_type_name_map[] = {
    {TRICK_VOID,                 "TRICK_VOID"},
    {TRICK_CHARACTER,            "TRICK_CHARACTER"},
    {TRICK_UNSIGNED_CHARACTER,   "TRICK_UNSIGNED_CHARACTER"},
    {TRICK_STRING,               "TRICK_STRING"},
    {TRICK_SHORT,                "TRICK_SHORT"},
    {TRICK_UNSIGNED_SHORT,       "TRICK_UNSIGNED_SHORT"},
    {TRICK_INTEGER,              "TRICK_INTEGER"},
    {TRICK_UNSIGNED_INTEGER,     "TRICK_UNSIGNED_INTEGER"},
    {TRICK_LONG,                 "TRICK_LONG"},
    {TRICK_UNSIGNED_LONG,        "TRICK_UNSIGNED_LONG"},
    {TRICK_FLOAT,                "TRICK_FLOAT"},
    {TRICK_DOUBLE,               "TRICK_DOUBLE"},
    {TRICK_BITFIELD,             "TRICK_BITFIELD"},
    {TRICK_UNSIGNED_BITFIELD,    "TRICK_UNSIGNED_BITFIELD"},
    {TRICK_LONG_LONG,            "TRICK_LONG_LONG"},
    {TRICK_UNSIGNED_LONG_LONG,   "TRICK_UNSIGNED_LONG_LONG"},
    {TRICK_FILE_PTR,             "TRICK_FILE_PTR"},
    {TRICK_BOOLEAN,              "TRICK_BOOLEAN"},
    {TRICK_WCHAR,                "TRICK_WCHAR"},
    {TRICK_WSTRING,              "TRICK_WSTRING"},
    {TRICK_VOID_PTR,             "TRICK_VOID_PTR"},
    {TRICK_ENUMERATED,           "TRICK_ENUMERATED"},
    {TRICK_STRUCTURED,           "TRICK_STRUCTURED"}
};

#define N_PAIRS (sizeof(trick_type_name_map)/sizeof(trick_type_name_map[0]))

const char* trick_type_name(TRICK_TYPE type) {
    unsigned int ii;
    for (ii=0; ii<N_PAIRS; ii++) {
        if ( type == trick_type_name_map[ii].type) {
            return (trick_type_name_map[ii].name);
        }
    }
   return ("UNKNOWN_TYPE");
}
