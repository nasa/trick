/*
PURPOSE:
    (Data structure typedef for parameter attributes used by executive IO
        routines and filled by the interface code generator.)

REFERENCE:
    (((Bailey, R.W)
      (Trick Simulation Environment Developer's Guide - Beta Release)
      (NASA:JSC #......)
      (JSC / Engineering Directorate / Automation and Robotics Division)
      (February 1991) ()))

PROGRAMMERS:
    ((Robert W. Bailey) (LinCom Corp) (September 1990) (Initial Release.))
*/

#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#include <stddef.h>

#define MAX_INDEX 8

#ifdef __cplusplus
extern "C" {
#endif

/* ADDR_BF is analagous to offsetof for bit field parameters */
#define ADDR_BF(X,Y,W) ((offsetof(X,Y)+(sizeof(((X*)0)->Y)+(W*4))))
#define ADDR_BF0(W) (W*4)
#define GET_SIZE(X)  ( (X == 0)? 0 : (*((int*)((char*)X-8))) )
#define REF_LEVEL 16       /* Maximum level for nested data structure names */

typedef enum {
    Language_C,     /* -- ANSI C */
    Language_CPP,   /* -- C Plus Plus */
    Language_Ada    /* -- Ada */
} Language;

typedef struct {
    int size;       /* -- index for first array element */
    int start;      /* -- index for first array element */
} INDEX;

typedef struct ATTRIBUTES_tag {

    char *name;       /* -- Name of Parameter at current level */
    char *type_name;  /* -- Parameter type name */
    char *units;      /* -- Units of Measurement */
    char *alias;      /* -- Name alias for parameter */
    char *des;        /* -- Parameter description */
    int io;           /* -- Input / Output processing restrictions,
                            0=no processing, 
                            1=output, 
                            2=input, 
                            3=both */
    int visited;      /* -- Flag to check if attr struct visited. 
                            Used by sie and db. */
    int type;         /* -- Type of parameter */
    int size;         /* -- Data type size in bytes */
    double range_min; /* -- Minimum allowable value for parameter */
    double range_max; /* -- Minimum allowable value for parameter */
    Language language; /* -- Native language of parameter */
    int reference;    /* -- 0 = No 1 = C++ reference variable */
    unsigned long offset; /* -- Offset in bytes of this param from
                                beginning of data structure */
    char *attr;       /* -- Address to next level parameter 
                            attributes, either ATTRIBUTES* or 
                            ENUM_ATTR* */
    int num_index;    /* -- Number of array indices for the param */
    INDEX index[8];   /* -- Array index info */

} ATTRIBUTES;

typedef struct {
    char *label;    /* --  Enumeration label */
    int value;      /* --  Enumeration value */
} ENUM_ATTR;

typedef struct {

    char *name;       /* -- Name of Parameter at current level */
    char *type_name;  /* -- Parameter type name */
    char *units;      /* -- Units of Measurement */
    char *alias;      /* -- Name alias for parameter */
    char *des;        /* -- Parameter description */
    char *attr;       /* -- Address to next level param attributes,
                            either ATTRIBUTES* or ENUM_ATTR* */
    double range_min; /* -- Minimum allowable value for parameter */
    double range_max; /* -- Minimum allowable value for parameter */
    int size;         /* -- Size of each data item in BITS */
    int offset;       /* -- Offset from beginning of record to 
                            data, in BITS */
    char type;        /* -- Type of parameter */
    char io;          /* -- Input / Output processing restrictions,
                            0=no processing, 
                            1=output, 
                            2=input, 
                            3=both */
    char num_index;   /* -- Number of array indices for the param */
    INDEX index[8];   /* -- Array index info */

} ADA_ATTR;


typedef struct {

    /* Parsed information from input file */
    int level;                  /* -- Level of nested struct ref */
    char reference[512];        /* -- Main buf for var reference */
    char name[REF_LEVEL][80];   /* -- Ref name for each level */
    int dim[REF_LEVEL];         /* -- Ref array dimensions 
                                      for each level */
    int index[REF_LEVEL][MAX_INDEX];  /* -- Array element for each 
                                            dimension */
    char units[32];             /* -- Units as specified in 
                                      input runstream */
    Language language;          /* -- Language syntax used in 
                                      input file */

    /* Parameter attributes */
    int attr_level;             /* -- Current ref name level in 
                                      IO routines */
    char *address;              /* -- Address of the specified ref*/
    int index_lim[REF_LEVEL][MAX_INDEX];  /* -- Max size, each of 
                                                ref's dims */
    ATTRIBUTES attr;            /* -- Parameter attributes */

} REF;

#ifdef __cplusplus
}
#endif
#endif
