#ifndef EQPARSE_STACK_H
#define EQPARSE_STACK_H

#ifdef __cplusplus
extern "C" {
#endif

        /*stack def*/
        typedef struct _node {
                char val;
                struct _node *next;
        } node;

        /* stack1 def*/
        typedef struct _node1 {
                double val;
                struct _node1 *next;
        } node1;

#define stack node *

#define stack1 node1 *

#ifdef __cplusplus
}
#endif
#endif
