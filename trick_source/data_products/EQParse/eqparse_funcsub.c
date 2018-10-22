/*
 * Author  : Peter Mosbach
 * Date    : August 2003
 * Desc    : Substitute functions in the equation and stack
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "eqparse_protos.h"

/* Global error number */
extern int eqp_errno;

void funcsub(char *str)
{
        int ii;
        char ch1, func;
        char *temp;
        char *equation;
        int offset, counter, start, len, i, tstart, j, tlen, comp, test;

#define NUM_FUNCTIONS 38
        char function_names[NUM_FUNCTIONS][32] = {
                "abs", "acosh", "acos", "asinh", "asin",
                "atanh", "atan2", "atan", "besj0", "besj1",
                "besy0", "besy1", "ceil", "cosh",
                "cos", "div", "erfc", "erf", "exp", "floor", "gamma",
                "int", "inverf", "invnorm", "lgamma", "log10",
                "log", "norm", "rand", "sgn", "sinh", "sin", "sqrt",
                "tanh", "tan", "PI", "M_PI", "M_E"
        };

        char *function_symbols = (char *)
         "\xA2\xA4\xA3\xA7\xA6"
         "\xA9\xAA\xA8\xAB\xAC"
         "\xAD\xAE\xAF\xB2"
         "\xB1\xCB\xB4\xB3\xB5\xB6\xB7"
         "\xBB\xBC\xBD\xBE\xC0"
         "\xBF\xC1\xC2\xC4\xC6\xC5\xC7"
         "\xC9\xC8\xCF\xCF\xD6";

        temp = (char *) calloc(2 * strlen(str), sizeof(char));

        equation = str;

        /* Reset global errno */
        eqp_errno = 0;

#ifdef DEBUG
        printf("\n%s", equation);
#endif
        len = strlen(equation);
        comp = 0;
        for (test = 0; test < (len - 1); test++) {
                if ((isalpha(equation[test])) &&
                    ((isalpha(equation[test + 1]))
                     || (equation[test + 1] == '_'))) {
                        comp = 1;
                        break;
                }
        }
        while (comp == 1) {
                start = 0;
                comp = 0;
                len = strlen(equation);
                i = 0;
                while (i == 0) {
                        if ((isalpha(equation[start])) &&
                            ((isalpha(equation[start + 1])) ||
                             (equation[start + 1] == '_'))) {
                                i = 1;
                        } else {
                                temp[start] = equation[start];
                                start++;
                        }
                }
                ch1 = equation[start];
                for (ii = 0; ii < NUM_FUNCTIONS; ii++) {
                        if (!strncmp(function_names[ii],
                                     &equation[start],
                                     strlen(function_names[ii]))) {
                                func = function_symbols[ii];
                                break;
                        }
                }
                if (ii == NUM_FUNCTIONS) {
                        /* Didn't find a function match */
                        if (isalpha(equation[start + 1])) {
                                /* Syntax error since variables must be one char */
                                eqp_errno = 14;
                                free(temp);
                                return;
                        } else {
                                /* We have a variable */
                                func = 0;       /* Character is a variable */
                                temp[start] = equation[start];
                        }
                }

                offset = strlen(function_names[ii]);

                if (func == (char)0xAA || func == (char)0xCB || func == (char)0xB9) {
                        tstart = start;
                        start = start + offset;
                        temp[tstart] = '(';
                        tstart++;
                        ch1 = equation[start];
                        while (ch1 != ',') {
                                if (ch1 == '\0') {
                                        /* No second argument found */
                                        eqp_errno = 17;
                                        free(temp);
                                        return;
                                }
                                temp[tstart] = equation[start];
                                tstart++;
                                start++;
                                ch1 = equation[start];
                        }
                        temp[tstart] = ')';
                        tstart++;
                        temp[tstart] = func;
                        tstart++;
                        temp[tstart] = '(';
                        tstart++;
                        start++;
                        counter = 1;
                        do {
                                ch1 = equation[start];
                                if (ch1 == '\0') {
                                        /* Unmatched parethesis */
                                        eqp_errno = 1;
                                        free(temp);
                                        return;
                                }
                                temp[tstart] = equation[start];
                                if (ch1 == '(')
                                        counter++;
                                if (ch1 == ')')
                                        counter--;
                                tstart++;
                                start++;
                        } while (counter != 0);
                        temp[tstart] = ')';
                        tstart++;
                } else if (func == (char)0xCF) {
                        tstart = start;
                        temp[tstart] = func;
                        tstart++;
                        if (equation[start] == 'P')
                                start += 2;
                        else
                                start += 4;
                } else if (func == (char)0xD6) {
                        tstart = start;
                        temp[tstart] = func;
                        tstart++;
                        start += 3;
                } else if (func != 0) {
                        tstart = start;
                        start = start + offset;
                        temp[tstart] = '(';
                        tstart++;
                        counter = 0;
                        do {
                                ch1 = equation[start];
                                if (ch1 == '\0') {
                                        /* Unmatched parethesis */
                                        eqp_errno = 1;
                                        free(temp);
                                        return;
                                }
                                temp[tstart] = equation[start];
                                if (ch1 == 40)
                                        counter++;
                                if (ch1 == 41)
                                        counter--;
                                tstart++;
                                start++;
                        } while (counter != 0);
                        temp[tstart] = func;
                        tstart++;
                        temp[tstart] = 0xD8;
                        tstart++;
                        temp[tstart] = ')';
                        tstart++;
                } else {
                        tstart = start;
                }
                while ((temp[tstart] = equation[start]) != '\0') {
                        tstart++;
                        start++;
                }
                strcpy(equation, temp);
                tlen = strlen(equation);
#ifdef DEBUG
                printf("\n%s", equation);
#endif

                for (j = 0; j < (tlen - 1); j++) {
                        if ((isalpha(equation[j]))
                            && ((isalpha(equation[j + 1]))
                                || (equation[j + 1] == '_'))) {
                                comp = 1;
                                break;
                        }
                }
        }

        free(temp);
}
