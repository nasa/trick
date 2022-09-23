/*
 * Author  : Peter Mosbach
 * Date    : August 2003
 * Desc    : Check if the inputs are valid and set eqperrno if not.
 *
 */

#include <stdio.h>
#include "eqparse_protos.h"

int chkvalid(stack * stk)
{

#define EQP_CHKVALID_FREE \
	 makenull(temp) ; \
	 makenull(brace)

        int counter = 0;
        char ch, br;
        stack brace = NULL;
        stack temp = NULL;
        temp = cpy_stk(stk);
        while (!empty(temp)) {
                br = 0;
                temp = pop(temp, &ch);
                if (ch == 'N') {
                        counter++;
                } else if (ch == (char)0xD8) {
                        counter++;
                } else if (ch == 42 || ch == 43 || ch == 45 || ch == 47 ||
                         ch == 37 || ch == 94 || (ch >= (char)0xA2 && ch <= (char)0xCB)) {
                        counter--;
                } else if (ch == ')') {
                        if (brace) {
                                brace = pop(brace, &br);
                        }
                        if (br != '(') {
                                if (br == '[') {
                                        error(1);
                                } else if (br == '{') {
                                        error(2);
                                } else {
                                        error(5);
                                }
                                EQP_CHKVALID_FREE;
                                return (0);
                        }
                } else if (ch == ']') {
                        if (brace) {
                                brace = pop(brace, &br);
                        }
                        if (br != '[') {
                                if (br == '(') {
                                        error(0);
                                } else if (br == '{') {
                                        error(2);
                                } else {
                                        error(6);
                                }
                                EQP_CHKVALID_FREE;
                                return (0);
                        }
                } else if (ch == '}') {
                        if (brace) {
                                brace = pop(brace, &br);
                        }
                        if (br != '{') {
                                if (br == '(') {
                                        error(0);
                                } else if (br == '[') {
                                        error(1);
                                } else {
                                        error(7);
                                }
                                EQP_CHKVALID_FREE;
                                return (0);
                        }
                } else
                        brace = push(brace, ch);

        }
        if (!empty(brace)) {
                brace = pop(brace, &ch);
                if (ch != '(') {
                        if (ch == '{') {
                                error(2);
                        } else {
                                error(1);
                        }
                } else {
                        error(0);
                }
                EQP_CHKVALID_FREE;
                return (0);
        }

        EQP_CHKVALID_FREE;

        if (counter > 1) {
                /* Operand without operator */
                error(3);
                return (0);
        } else if (counter < 1) {
                /* Operator without operand */
                error(4);
                return (0);
        } else {
                return (1);
        }
}
