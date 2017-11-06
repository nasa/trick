/*
 * Author  : Peter Mosbach (adapted from internet src)
 * Date    : August 2003
 * Desc    : Stack funtions for equation parser.  There are two
 *           stacks.  A stack for longs and a stack for chars.
 */

#include <stdio.h>
#include <stdlib.h>

#include "eqparse_protos.h"

/* Global error number */
extern int eqp_errno;

/* Reverse the stack of char */
stack revers_stk(stack stk)
{
        char c;
        stack temp = NULL;
        while (!empty(stk)) {
                stk = pop(stk, &c);
                temp = push(temp, c);
        }
        return (temp);
}

/* Reverse the stack of long*/
stack1 revers_stk1(stack1 stk)
{
        double c;
        stack1 temp = NULL;
        while (!empty1(stk)) {
                stk = pop1(stk, &c);
                temp = push1(temp, c);
        }
        return (temp);
}


/* Copy long stack*/
stack1 cpy_stk1(stack1 * src)
{
        stack1 dest = NULL;
        stack1 temp = NULL;

        double ch;

        while (!empty1(*src)) {
                *src = pop1(*src, &ch);
                temp = push1(temp, ch);
                dest = push1(dest, ch);
        }
        temp = revers_stk1(temp);
        dest = revers_stk1(dest);
        *src = temp;
        return (dest);
}

/* copy char stack*/
stack cpy_stk(stack * src)
{
        stack dest = NULL;
        stack temp = NULL;
        stack rtemp;

        char ch;

        while (!empty(*src)) {
                *src = pop(*src, &ch);
                temp = push(temp, ch);
                dest = push(dest, ch);
        }
        rtemp = revers_stk(temp);
        dest = revers_stk(dest);
        *src = rtemp;
        return (dest);
}

stack push(stack stk, char v)
{
        stack temp;

        temp = (stack) malloc(sizeof(node));
        if (temp != NULL) {
                temp->val = v;

                temp->next = (struct _node *) stk;
        } else {
                eqp_errno = 12;
        }
        return (temp);

}


stack pop(stack stk, char *v)
{
        stack temp;

        if (stk == NULL) {
                eqp_errno = 13;
        } else {
                *v = stk->val;

                temp = stk;

                stk = (stack) stk->next;

                free(temp);

        }
        return (stk);
}



void top(stack stk, char *v)
{
        if (stk == NULL) {
                eqp_errno = 13;
        } else {
                *v = stk->val;
        }

}


stack makenull(stack stk)
{

        stack temp;

        if (stk == NULL) {
                return (NULL);
        }

        do {
                temp = stk;
                stk = (stack) stk->next;
                free(temp);

        } while (stk != NULL);

        return (NULL);

}

int empty(stack stk)
{
        if (stk == NULL) {
                return (1);
        } else {
                return (0);
        }
}



stack1 push1(stack1 stk, double v)
{
        stack1 temp;

        temp = (stack1) malloc(sizeof(node1));
        if (temp != NULL) {
                temp->val = v;

                temp->next = (struct _node1 *) stk;
        } else {
                eqp_errno = 12;
        }
        return (temp);

}


stack1 pop1(stack1 stk, double *v)
{
        stack1 temp;

        if (stk == NULL) {
                eqp_errno = 13;
        } else {
                *v = stk->val;

                temp = stk;

                stk = (stack1) stk->next;

                free(temp);

        }
        return (stk);
}



void top1(stack1 stk, double *v)
{
        if (stk == NULL) {
                eqp_errno = 13;
        } else {
                *v = stk->val;
        }

}


stack1 makenull1(stack1 stk)
{

        stack1 temp;

        while (stk != NULL) {
                temp = stk;

                stk = (stack1) stk->next;

                free(temp);
        }
        return (NULL);

}

int empty1(stack1 stk)
{
        if (stk == NULL) {
                return (1);
        } else {
                return (0);
        }
}
