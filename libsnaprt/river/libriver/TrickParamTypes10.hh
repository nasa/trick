#ifndef TRICKPARAMTYPES10
#define TRICKPARAMTYPES10
#include "TrickParamTypes.hh"

// Trick 10 parameter types for fields in .trk files
class TrickParamTypes10: public TrickParamTypes
{
  public:
    TrickParamTypes10(int swap): TrickParamTypes(swap) {}
    double toValue(void* address, int type, int size);
    int doubleType() const { return TRK_DOUBLE; }

    enum {
        TRK_VOID                = 0,   /* No type */
        TRK_CHARACTER           = 1,   /* (char) */
        TRK_UNSIGNED_CHARACTER  = 2,   /* (unsigned char) */
        TRK_STRING              = 3,   /* (char *) */
        TRK_SHORT               = 4,   /* (short) */
        TRK_UNSIGNED_SHORT      = 5,   /* (unsigned short) */
        TRK_INTEGER             = 6,   /* (int) */
        TRK_UNSIGNED_INTEGER    = 7,   /* (unsigned int) */
        TRK_LONG                = 8,   /* (long) */
        TRK_UNSIGNED_LONG       = 9,   /* (unsigned long) */
        TRK_FLOAT              = 10,   /* (float) */
        TRK_DOUBLE             = 11,   /* (double) */
        TRK_BITFIELD           = 12,   /* (signed int : 1) */
        TRK_UNSIGNED_BITFIELD  = 13,   /* (unsigned int : 1) */
        TRK_LONG_LONG          = 14,   /* (long long) */
        TRK_UNSIGNED_LONG_LONG = 15,   /* (long long) */
        TRK_FILE_PTR           = 16,   /* (file *) */
        TRK_BOOLEAN            = 17,   /* (c++ boolean) */
        TRK_WCHAR              = 18,   /* (wchar_t) */
        TRK_WSTRING            = 19,   /* (wchar_t *) */
        TRK_VOID_PTR           = 20,   /* an arbitrary address */
        TRK_ENUMERATED         = 21,   /* User defined type (enumeration) */
        TRK_STRUCTURED         = 22,   /* User defined type (struct/class) */
    } PARAM_TYPES;
};

#endif
