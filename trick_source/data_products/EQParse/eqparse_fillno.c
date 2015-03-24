/*
 * Author  : Peter Mosbach
 * Date    : August 2003
 * Desc    : Extract the numbers from the input and store in as long
 *           in a long stack
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "eqparse_protos.h"

stack fillno(stack1 * no, stack stk, double value)
{
        int i, j;
        double x;
        char ch;
        char *num;
        char numstr[64];
        char *emptynum;
        char emptynumstr[64];

        stack temp = NULL;
        stack tmp_no = NULL;
        memset(numstr, 0, 64);
        num = numstr;
        memset(emptynumstr, 0, 64);
        emptynum = emptynumstr;

        do {
                i = 0;
                tmp_no = NULL;
                num = strcpy(num, emptynum);
                /*
                 * Increment i while the input is digit & put the
                 * digit in a stack
                 */
                while (!empty((stk = pop(stk, &ch))) &&
                       ((ch >= 48 && ch <= 57) || ch == 46)) {
                        i++;
                        tmp_no = push(tmp_no, ch);
                }
                /* Not last digit or it is operand */
                if (i) {
                        temp = push(temp, 'N');
                        if (!(ch >= 48 && ch <= 57) &&
                            !(ch >= 65 && ch <= 90) &&
                            !(ch >= 97 && ch <= 122) && ch != 46)
                                temp = push(temp, ch);
                        else {
                                i++;
                                tmp_no = push(tmp_no, ch);
                        }
                        /* Convert the input digit to double (note: '0'=48) */
                        tmp_no = revers_stk(tmp_no);
                        for (j = 0; j < i; j++) {
                                tmp_no = pop(tmp_no, &ch);
                                sprintf(num, "%s%c", num, ch);
                        }
                        x = atof(num);
#ifdef DEBUG
                        printf("\nPushing %lf", x);
#endif
                        *no = push1(*no, x);
                } else {
                        if (ch >= 48 && ch <= 57) {
                                *no = push1(*no, ch - 48);
                                temp = push(temp, 'N');
                        } else if ((ch >= 65 && ch <= 90)
                                   || (ch >= 97 && ch <= 122)) {
                                temp = push(temp, 'N');
                                *no = push1(*no, value);
                        } else if (ch == (char)0xCF) {
                                temp = push(temp, 'N');
                                *no = push1(*no, M_PI);
                        } else if (ch == (char)0xD6) {
                                temp = push(temp, 'N');
                                *no = push1(*no, M_E);
                        } else {
                                temp = push(temp, ch);
                        }
                }
        } while (!empty(stk));
        temp = revers_stk(temp);
        *no = revers_stk1(*no);
        return (temp);
}
