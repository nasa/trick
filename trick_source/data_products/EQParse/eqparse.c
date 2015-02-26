/*
 * Author  : Peter Mosbach
 * Date    : August 2003
 * Desc    : This is THE function a user will call to evaluate an
 *           expression.  The function will return 0, on success.
 *           On error, use eqperror() to get error message with return code.
 */

#include <stdio.h>
#include "eqparse_protos.h"

/* Global error number */
extern int eqp_errno;

int equationparse(char *str, double value, double *value_out)
{

#define EQP_FREE_STACKS \
	 makenull(input) ;\
	 makenull1(numbers)

        double value_in;

        stack input = NULL;
        stack1 numbers = NULL;

        if (str[0] == '\0') {
                eqp_errno = 22;
                EQP_FREE_STACKS;
                return (eqp_errno);
        }

        /* Always reset error to no error */
        eqp_errno = 0;

        input = takeinput(input, str, value);
        if (eqp_errno) {
                EQP_FREE_STACKS;
                return (eqp_errno);
        }
        if (input) {
                input = revers_stk(input);
                input = fillno(&numbers, input, value);
                if (chkvalid(&input)) {
                        /* Take input */
                        input = postfix(input);

                } else {
                        EQP_FREE_STACKS;
                        return (eqp_errno);
                }
        }

        /* Save off copy of value input in case of error */
        value_in = value;

        /* Evaluate them */
        *value_out = eval(&input, &numbers);

        if (eqp_errno) {
                /* If there is an error, just return value passed in */
                *value_out = value_in;
        }

        EQP_FREE_STACKS;

        return (eqp_errno);
}
