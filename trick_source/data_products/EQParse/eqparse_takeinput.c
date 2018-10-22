/*
 * Author  : Peter Mosbach
 * Date    : August 2003
 * Desc    : Function to take your input char by char & insert it in a stack.
 *           It accept only the desired chars
 */

#include <stdio.h>
#include <string.h>

#include "eqparse_protos.h"


/* Global error number */
extern int eqp_errno;

stack takeinput(stack stk, char *equation, double value)
{
        int len;
        char ch;
        int i;
#ifdef DEBUG
        printf("\nEquation: %s", equation);
        printf("\nValue: %lf", value);
#endif
        operatorcheck(equation);
        if (eqp_errno) {
                return (NULL);
        }
        negcheck(equation);
        funcsub(equation);
        if (eqp_errno) {
                return (NULL);
        }
        i = 0;
        len = strlen(equation);
        for (i = 0; i < len; i++) {
                ch = equation[i];
                if ((ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122)) {
                        stk = push(stk, ch);
                } else if ((ch >= 48 && ch <= 57) ||
                           (ch >= 40 && ch <= 43) ||
                           (ch == 37 || ch == 45 || ch == 47 || ch == 91 ||
                            ch == 93 || ch == 94 || ch == 123 || ch == 125
                            || ch == 46)) {
                        stk = push(stk, ch);
#ifdef DEBUG
                        printf("\nPushing %c onto stack.", ch);
#endif
                } else if ((ch >= (char)0xA2 && ch <= (char)0xCB) || (ch == (char)0xD8) ||
                           (ch == (char)0xCF) || (ch == (char)0xD6)) {
                        stk = push(stk, ch);
#ifdef DEBUG
                        printf("\nPushing %c onto stack.", ch);
#endif
                }
        }
        return (stk);
}
