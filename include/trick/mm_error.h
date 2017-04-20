#ifndef MM_ERROR_H
#define MM_ERROR_H
/*
    PURPOSE: ( Memory Manager Error Types )
*/
    typedef enum {
        MM_OK,
        MM_ASSIGNMENT_LIST,
        MM_ERROR,
        MM_FILE_STREAM,
        MM_FILE_STREAM_HIGH_ARG,
        MM_INIT_GT_512_LINE,
        MM_INVALID_UNITS,
        MM_MAX_ERROR,
        MM_PARAMETER_ARRAY_DIM,
        MM_PARAMETER_ARRAY_SIZE,
        MM_PARAMETER_TYPE,
        MM_STRUCT_NOT_ASSIGNABLE,
        MM_SYNTAX_ERROR,
        MM_TOO_MANY_ARRAY_DIMENSIONS,
        MM_UNCLOSED_COMMENT,
        MM_UNDEFINED_REFERENCE,
        MM_UNTERMINATED_STRING
    } MM_ERROR_TYPE ;

#endif
