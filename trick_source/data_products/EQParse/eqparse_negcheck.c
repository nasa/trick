/*
 * Author  : Peter Mosbach
 * Date    : August 2003
 * Desc    : Function used to distinguish negative numbers
 *           from subtraction operator.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eqparse_protos.h"

void negcheck(char *str)
{
        char *temp, *equation;
        int len, i, pt, tpt;

        temp = (char *) calloc(2 * strlen(str), sizeof(char));

        equation = str;
        len = strlen(equation);
        pt = 1;
        tpt = 1;
        temp[0] = equation[0];
        for (i = 1; i < len; i++) {
                if ((equation[pt] == '-') && (equation[pt - 1] == '(')) {
                        temp[tpt] = '0';
                        tpt++;
                        temp[tpt] = '-';
                        tpt++;
                        pt++;
                } else {
                        temp[tpt] = equation[pt];
                        tpt++;
                        pt++;
#ifdef DEBUG
                        printf("\n%s", temp);
#endif
                }
        }
#ifdef DEBUG
        printf("\n%s", temp);
#endif
        strcpy(str, temp);

        free(temp);
}
