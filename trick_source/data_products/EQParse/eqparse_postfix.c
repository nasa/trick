/*
 * Author  : Peter Mosbach
 * Date    : August 2003
 * Desc    : Convert from infix to postfix (polish)
 *          (it's a well known algorithm)
 */

#include <stdio.h>

#include "eqparse_protos.h"

stack postfix(stack stk)
{
        stack temp = NULL;
        stack oprnd = NULL;
        char ch, ch2;
        while (!empty(stk)) {
                stk = pop(stk, &ch);
                ch2 = 0;
                if (ch == 'N') {
                        temp = push(temp, ch);
                } else if (ch == (char)0xD8) {
                        temp = push(temp, ch);
                } else if (ch == 42 || ch == 43 || ch == 45 || ch == 47 ||
                           ch == 37 || ch == 94 || (ch >= (char)0xA2 && ch <= (char)0xCB)) {
                        if (!empty(oprnd)) {
                                top(oprnd, &ch2);
                                while (great(ch2, ch) && !empty(oprnd)) {
                                        oprnd = pop(oprnd, &ch2);
                                        temp = push(temp, ch2);
                                        if (!empty(oprnd)) {
                                                top(oprnd, &ch2);
                                        }
                                }
                        }
                        oprnd = push(oprnd, ch);
                } else if (ch == 41 || ch == 93 || ch == 125) {
                        do {
                                if (!empty(oprnd)) {
                                        oprnd = pop(oprnd, &ch2);
                                }
                                if (ch2 != 40) {
                                        temp = push(temp, ch2);
                                }
                        }
                        while (ch2 != 40);
                } else {
                        oprnd = push(oprnd, 40);
                }

        }

        while (!empty(oprnd)) {
                oprnd = pop(oprnd, &ch);
                temp = push(temp, ch);
        }
        temp = revers_stk(temp);

        return (temp);
}


/* Return the precedence 1 if ch1 is first in precedence 0 otherwise */
int great(char ch1, char ch2)
{
        if (ch1 == 40 && ch2 != 40)
                return (0);
        else if (ch1 == '^')
                return (1);
        else if (ch1 >= (char)0xA2 && ch1 <= (char)0xCB)
                return (1);
        else if ((ch1 == '*' || ch1 == '/' || ch1 == '%') &&
                 ch2 != '^' && !(ch2 >= (char)0xA2 && ch2 <= (char)0xCB))
                return (1);
        else if ((ch1 == '+' || ch1 == '-') && (ch2 == '-' || ch2 == '+'))
                return (1);
        else if ((ch1 == '*' || ch1 == '/' || ch1 == '%') &&
                 (ch2 == '/' || ch2 == '*' || ch2 == '%'))
                return (1);
        else
                return (0);
}
