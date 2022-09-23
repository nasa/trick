
#ifndef BST_H
#define BST_H

#include <stdio.h>

#define RIGHTLEAF 1   /* get right inner leaf in delete */
#define LEFTLEAF 0    /* get left inner leaf in delete */


#ifdef __cplusplus
extern "C" {
#endif

typedef struct bstnode
{
    struct bstnode *left;    /* points to left child */
    struct bstnode *right;   /* points to right child */
    struct bstnode *parent;  /* points to parent */
    void *info;              /* points to the data contained in the node */
}bstNode;                    /* bst node structure */


typedef struct
{
    bstNode *root;                                /* points to the root node */
    int nodes;                                    /* number of nodes in the tree */
    short deltype;                                /* determines left or right leaf deletion strategy */
    int (*compare)(void *left,void *right);       /* compare function implemented by user of bst */
                                              /* return 0 if left and right are identical */
                                              /* else return -1 if left <= right and 1 if left > right */
    void (*print_node)(FILE *output,void *info);  /* print function to be implemented by user */
    void (*traverse_node)(void *info);            /* traverse function to be implemented by user */
    void (*delete_node)(void *info);              /* delete function to be implemented by user */
    void (*malloc_fail_handler)(void);            /* malloc fail function to be implemented by user */
    short init;
}BST;


/* function prototypes */

bstNode *bstFind(void *info,BST *bst);           /* find a node that contains info */
void *bstGetInfo(bstNode *node);                 /* get the info pointer of a node */
bstNode *bstGetLeft(bstNode *node);              /* get the left node of a node */
bstNode *bstGetRight(bstNode *node);             /* get the right node of a node */
bstNode *bstGetParent(bstNode *node);            /* get the parent node of a node */
void *bstDelete(bstNode *node,BST *bst);         /* delete a node from the tree */
void bstDeleteNodes (BST * bst, bstNode * node);
int bstGetCount(BST *bst);                       /* get the number of nodes in the tree */
bstNode* bstInsert(void *info,BST *bst);         /* insert a new node into the tree */
void bstPrintAscending(FILE *output, BST *bst);  /* print the nodes in ascending order */
void bstPrintDescending(FILE *output, BST *bst); /* print the nodes in descending order */
void bstInit(BST *bst);                          /* intialize the BST structure */

void bstTraverseTree(BST *);
void bstDeleteTree(BST *);

/* override the default functions with a user supplied function */
void bstSetPrintFunc(BST *bst,void (*printfunc)(FILE *output,void *info));
void bstSetTraverseFunc(BST *bst,void (*traversefunc)(void *info));
void bstSetDeleteFunc(BST *bst,void (*deletefunc)(void *info));
void bstSetCompareFunc(BST *bst,int (*compfunc)(void *left,void *right));

/* override the default malloc failure handler with a user supplied handler */
void bstSetMallocFailHandler(BST *bst,void (*malloc_fail_handler)(void));

void DefPrintProc (FILE * output, void *info) ;
void DefTraverseProc (void *info) ;
void DefDeleteProc (void *info) ;
void DefHandlerProc (void) ;

#ifdef __cplusplus
}
#endif

#endif


