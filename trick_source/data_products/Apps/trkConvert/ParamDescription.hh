#ifndef PARAM_DESCRIPTION_H
#define PARAM_DESCRIPTION_H

#include <stdio.h>

struct ParamDescription {

    char* parameterName;
    char* unitsName;
    int32_t dataType;
    int32_t dataSize;

    ParamDescription(){}
    ParamDescription(FILE* fp);
};
#endif
