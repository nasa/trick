#ifndef EQPARSE_PROTO_H
#define EQPARSE_PROTO_H

#include "eqparse_stack.h"

#ifdef __cplusplus
extern "C" {
#endif

        void operatorcheck(char *equation);
        int equationparse(char *str, double value, double *out);
        char *eqperror(int code);
        stack takeinput(stack stk, char *equation, double value);
        double eval(stack * stk, stack1 * no);
        double inverf(double p);
        double add(double x, double y);
        double dif(double x, double y);
        double mul(double x, double y);
        double divide(double x, double y);
        double mod(double x, double y);
        double igamma(double a, double x);
        stack postfix(stack stk);
        int great(char ch1, char ch2);
        int chkvalid(stack * stk);
        void error(char code);
        stack1 cpy_stk1(stack1 * src);
        stack cpy_stk(stack * src);
        stack fillno(stack1 * no, stack stk, double value);
        stack1 revers_stk1(stack1 stk);
        stack revers_stk(stack stk);

/* stack1 operations*/

/* add element to it */
        stack1 push1(stack1 stk, double v);

/* return top element from stack and remove it*/
        stack1 pop1(stack1 stk, double *v);

/* just return top element from stack*/
        void top1(stack1 stk, double *v);

/* free the stack */
        stack1 makenull1(stack1 stk);

/* check if it is empty */
        int empty1(stack1 stk);

/* stack operations*/

/* add element to it */
        stack push(stack stk, char v);

/* return top element from stack and remove it*/
        stack pop(stack stk, char *v);

/* just return top element from stack*/
        void top(stack stk, char *v);

/* free the stack */
        stack makenull(stack stk);

/* check if it is empty */
        int empty(stack stk);

        void negcheck(char *str);

/* substitute functions in the equation and stack */
        void funcsub(char *str);

#ifdef __cplusplus
}
#endif
#endif
