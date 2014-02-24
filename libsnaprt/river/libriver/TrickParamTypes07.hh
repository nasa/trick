#ifndef TRICKPARAMTYPES07
#define TRICKPARAMTYPES07
#include "TrickParamTypes.hh"

// Trick 07 parameter types for fields in .trk files
class TrickParamTypes07: public TrickParamTypes
{
  public:
    TrickParamTypes07(int swap): TrickParamTypes(swap) {}
    double toValue(void* address, int type, int size);
    int doubleType() const { return TRK_DOUBLE; }

    enum {
        TRK_CHARACTER           = 0,   /* (char) */
        TRK_UNSIGNED_CHARACTER  = 1,   /* (unsigned char) */
        TRK_STRING              = 2,   /* (char *) */
        TRK_SHORT               = 3,   /* (short) */
        TRK_UNSIGNED_SHORT      = 4,   /* (unsigned short) */
        TRK_INTEGER             = 5,   /* (int) */
        TRK_UNSIGNED_INTEGER    = 6,   /* (unsigned int) */
        TRK_LONG                = 7,   /* (long) */
        TRK_UNSIGNED_LONG       = 8,   /* (unsigned long) */
        TRK_FLOAT               = 9,   /* (float) */
        TRK_DOUBLE             = 10,   /* (double) */
        TRK_BITFIELD           = 11,   /* (signed int : 1) */
        TRK_UNSIGNED_BITFIELD  = 12,   /* (unsigned int : 1) */
        TRK_LONG_LONG          = 13,   /* (long long) */
        TRK_UNSIGNED_LONG_LONG = 14,   /* (long long) */
        TRK_FILE_PTR           = 15,   /* (file *) */
        TRK_TRICK_VOID         = 16,   /* (void) */
        TRK_BOOLEAN            = 17,   /* (c++ boolean) */
        TRK_COMPLX             = 18,   /* (complex) */
        TRK_DBL_COMPLX         = 19,   /* (double prec complex) */
        TRK_TRICK_REF          = 20,   /* (trick ref pointer) */
        TRK_VOID_PTR           = 99,    /* an arbitrary address */
        TRK_F_STANDARD         = 100,
        TRK_C_STANDARD         = 101,
        TRK_ENUMERATED         = 102,
        TRK_STRUCTURED         = 103,
        TRK_BIT_FIELD          = 104,
        TRK_TYPEDEF            = 105,
    } PARAM_TYPES;
};

#endif
