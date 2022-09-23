/*
 * Author  : Peter Mosbach
 * Date    : August 2003
 * Desc    : Check to make sure no illegal characters for operators
 *           are in string
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "eqparse_protos.h"

/* Global error number */
extern int eqp_errno;

void operatorcheck(char *equation)
{
        int i, j, counter;
        char ch;
        int len = strlen(equation);
        for (i = 0; i < len; i++) {
                ch = equation[i];
                if ((ch == '%') || (ch >= '(' && ch <= '9') ||
                    (ch >= 'A' && ch <= 'Z') ||
                    (ch == '[') || (ch == ']') || (ch == '^') || (ch == '_') ||
                    (ch >= 'a' && ch <= 'z') || (ch == '{') || (ch == '}') ||
                    (ch >= (char)0xA2 && ch <= (char)0xCB) ||
                    (ch == (char)0xD8) || (ch == (char)0xCF) || (ch == (char)0xD6) || (ch == ' ')) {
                        if (ch == '.') {
                                if ((len < 2) || (i == (len - 1))) {
                                        eqp_errno = 15;
                                        return;
                                } else if (!isdigit(equation[i + 1])) {
                                        eqp_errno = 15;
                                        return;
                                }
                        }
                        if (ch == '_') {
                                if ((len < 3) || (i > (len - 2)) || (i == 0)) {
                                        eqp_errno = 15;
                                        return;
                                }
                                if (equation[i - 1] != 'M') {
                                        eqp_errno = 15;
                                        return;
                                }
                                if ((equation[i + 1] != 'P') &&
                                    (equation[i + 1] != 'E')) {
                                        eqp_errno = 15;
                                        return;
                                }
                                if (equation[i + 1] == 'P') {
                                        if ((len < 4) || (i > (len - 3))
                                            || (i == 0)) {
                                                eqp_errno = 15;
                                                return;
                                        } else if ((equation[i - 1] != 'M') ||
                                                   (equation[i + 1] != 'P') ||
                                                   (equation[i + 2] != 'I')) {
                                                eqp_errno = 15;
                                                return;
                                        }
                                }
                                if (equation[i + 1] == 'E') {
                                        if ((equation[i - 1] != 'M') ||
                                            (equation[i + 1] != 'E')) {
                                                eqp_errno = 15;
                                                return;
                                        }
                                }
                        }
                        if (ch == ',') {
                                if ((len < 8) || (i < 5) || (i > (len - 3))) {
                                        eqp_errno = 15;
                                        return;
                                }
                                j = i;
                                counter = 1;
                                while (counter != 0) {
                                        if (equation[j] == ')') {
                                                counter++;
                                        }
                                        if (equation[j] == '(') {
                                                counter--;
                                        }
                                        if (counter == 0) {
                                                break;
                                        }
                                        if (j < 3) {
                                                eqp_errno = 15;
                                                return;
                                        }
                                        j--;
                                }
                                if ((i - j) <= 1) {
                                        eqp_errno = 15;
                                        return;
                                }
                                if ((equation[j - 1] != 'v') &&
                                    (equation[j - 1] != 'a') &&
                                    (equation[j - 1] != '2')) {
                                        eqp_errno = 15;
                                        return;
                                }
                                if (equation[j - 1] == 'v') {
                                        if ((equation[j - 2] != 'i') ||
                                            (equation[j - 3] != 'd')) {
                                                eqp_errno = 15;
                                                return;
                                        }
                                } else if (equation[j - 1] == '2') {
                                        if ((len < 10) || (i < 7)
                                            || (i > (len - 3))) {
                                                eqp_errno = 15;
                                                return;
                                        }
                                        if ((equation[j - 2] != 'n') ||
                                            (equation[j - 3] != 'a') ||
                                            (equation[j - 4] != 't') ||
                                            (equation[j - 5] != 'a')) {
                                                eqp_errno = 15;
                                                return;
                                        }
                                }
                        }
                } else {
                        eqp_errno = 15;
                        break;
                }
        }
}
