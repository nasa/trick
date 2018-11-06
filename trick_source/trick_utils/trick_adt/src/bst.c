/* ////////////// BST.CPP ////////////////// */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "trick/bst.h"

/* locate the node that contains the data pointed to by info */

bstNode *bstFind(void *info, BST * bst)
{
    bstNode *current;

    if (bst == NULL) {
        fprintf(stderr, "bst is not allocated");
        return NULL;
    }
    if (info == NULL) {
        fprintf(stderr, "info is invalid pointer");
        return NULL;
    }

    if (!bst->init) {
        bstInit(bst);
        return (NULL);
    }

    current = bst->root;
    while (current != NULL) {
        int compval;
        compval = bst->compare(current->info, info);    /* compare the current node with info */
        if (compval <= -1)             /* if the node's data was less than info */
            current = current->right;  /* search in the right subtree */
        else if (compval >= 1)         /* if the node's data was greater than info */
            current = current->left;   /* search inthe left subtree */
        else
            break;                     /* otherwise, the data equals info */
    }
    return current;                    /* returns null if not found, or the node that equals info */
}


/* returns a pointer to the data pointed to by node */

void *bstGetInfo(bstNode * node)
{
    if (node == NULL) {
        fprintf(stderr, "Queue is empty");
        return NULL;
    }

    return node->info;
}

/* gets the node to the left of node */

bstNode *bstGetLeft(bstNode * node)
{
    if (node == NULL) {
        fprintf(stderr, "Queue is empty");
        return NULL;
    }

    return node->left;
}


/* gets the node to the right of node */

bstNode *bstGetRight(bstNode * node)
{

    if (node == NULL) {
        fprintf(stderr, "Queue is empty");
        return NULL;
    }

    return node->right;
}


/* gets the parent of the node pointed to by node */

bstNode *bstGetParent(bstNode * node)
{

    if (node == NULL) {
        fprintf(stderr, "Queue is empty");
        return NULL;
    }
    return node->parent;
}


/* delete a node from the bst */

void *bstDelete(bstNode * node, BST * bst)
{
    bstNode *current;
    void *ret;
    bstNode **parentspointer;


    if (bst == NULL) {
        fprintf(stderr, "bst is not allocated");
        return NULL;
    }
    if (node == NULL) {
        fprintf(stderr, "node is invalid pointer");
        return NULL;
    }

    if (!bst->init) {
        bstInit(bst);
        return (node);
    }
    bst->nodes--;                      /* decrement the node counter */
    if (node && node->parent != NULL) {        /* if node is not the root, *//* then get the address of the parent's pointer to node */
        if (node->parent->left == node)
            parentspointer = &(node->parent->left);
        else
            parentspointer = &(node->parent->right);
    } else
        parentspointer = &(bst->root);

    if (node->left == NULL) {          /* if there is no left subtree of node */
        *parentspointer = node->right;
        if (node->right != NULL)
            node->right->parent = node->parent; /* link up parent pointer if a right tree exists */
    } else if (node->right == NULL) {  /* if right is null, but not left then link up with left subtree */
        *parentspointer = node->left;
        node->left->parent = node->parent;
    } else if (bst->deltype == RIGHTLEAF) {     /* neither left nor right is null */
        bst->deltype = 1 - bst->deltype;        /* toggle deltype between 1 and 0 */
        current = node->right;         /* get inner child from right side */
        if (current->left == NULL) {   /* if left is null then inner child is found */
            *parentspointer = current;
            current->parent = node->parent;
            current->left = node->left;
            current->left->parent = current;

        } else {
            while (current->left != NULL)       /* go as far left as possible */
                current = current->left;
            current->parent->left = current->right;     /* replace the deleted node with inner child */
            if (current->right != NULL)
                current->right->parent = current->parent;
            *parentspointer = current;
            current->parent = node->parent;
            current->left = node->left;
            current->right = node->right;
            current->left->parent = current;
            current->right->parent = current;
        }
    } else {                           /* using left leaf deletion to keep from getting unbalanced due to deletions */

        bst->deltype = 1 - bst->deltype;        /* toggle deltype back to right leaf deletion */
        current = node->left;
        if (current->right == NULL) {  /* do the same right leaf deletion except in other direction */
            *parentspointer = current;
            current->parent = node->parent;
            current->right = node->right;
            current->right->parent = current;

        } else {
            while (current->right != NULL)
                current = current->right;
            current->parent->right = current->left;
            if (current->left != NULL)
                current->left->parent = current->parent;
            *parentspointer = current;
            current->parent = node->parent;
            current->left = node->left;
            current->right = node->right;
            current->right->parent = current;
            current->left->parent = current;
        }
    }
    ret = node->info;                  /* return a pointer to the data contained in the node */
    free(node);                        /* free memory associated with the deleted node */
    return ret;
}


/* returns the number of nodes in the tree */

int bstGetCount(BST * bst)
{

    if (bst == NULL) {
        fprintf(stderr, "Queue is empty");
        return -1;
    }

    if (!bst->init) {
        bstInit(bst);
        return (0);
    }
    return bst->nodes;
}


/* insert a node into the tree */

bstNode *bstInsert(void *info, BST * bst)
{
    bstNode *newNode;
    bstNode *current;
    int done = 0;


    if (bst == NULL) {
        fprintf(stderr, "bst is not allocated");
        return NULL;
    }
    if (info == NULL) {
        fprintf(stderr, "info is invalid pointer");
        return NULL;
    }

    if (bst && !bst->init) {
        bstInit(bst);
    }
    newNode = (bstNode *) malloc(sizeof(bstNode));      /* allocate memory for the node */
    if (newNode == NULL) {
        bst->malloc_fail_handler();    /* call the malloc fail handler if memory is not allocated */
        return NULL;
    }
    newNode->info = info;              /* initialize fields of the new node */
    newNode->left = NULL;
    newNode->right = NULL;
    bst->nodes++;                      /* increment the node counter */
    if (bst->root == NULL) {           /* check if this is the only node in the tree */
        bst->root = newNode;
        newNode->parent = NULL;
    } else {                           /* else find a location to insert the node */

        current = bst->root;
        do {
            int compval;
            done = 0;
            compval = bst->compare(info, current->info);
            if (compval >= 0) {        /* new node is greater than the current node */
                if (current->right == NULL) {   /* if right is null then a location has been found *//* for the new node */
                    current->right = newNode;
                    newNode->parent = current;
                    done = 1;
                }
                current = current->right;       /* else go right */
            } else {                   /* the new node is less than the current node */

                if (current->left == NULL) {    /* if left is null then put new node to the left */
                    current->left = newNode;
                    newNode->parent = current;
                    done = 1;
                }
                current = current->left;        /* else go left */
            }
        } while (!done);               /* keep searching until a location for new node has been found */
    }
    return newNode;
}

/* print the nodes of the tree in ascending order (using a non-recursive method) */

void bstPrintAscending(FILE * output, BST * bst)
{
    bstNode *current;
    int done = 0;
    int st = 2;


    if (output == NULL && bst == NULL) {
        fprintf(stderr, "Queue is empty");
        return;
    }

    if (!bst->init) {
        bstInit(bst);
        return;
    }
    current = bst->root;               /* start at the root */
    if (current == NULL)
        return;
    while (!done) {
        switch (st) {                  /* switch based upon the current state of traversing the tree */
            case 1:                   /* go up to the parent of current node */
                {
                    current = current->parent;
                    if (current->parent == NULL)
                        done = 1;
                    else if (current == current->parent->left)
                        st = 3;
                }
                break;
            case 2:                   /* traverse left until null is found */
                {

                    while (current->left != NULL)
                        current = current->left;
                    st = 4;

                }
                break;
            case 3:                   /* go to the parent of current node */
                {
                    current = current->parent;
                    st = 4;
                }
                break;
            case 4:                   /* print the current node, then figure out where to go */
                {
                    bst->print_node(output, current->info);
                    if (current->right == NULL) {
                        if (current->parent == NULL)
                            done = 1;
                        else if (current->parent->left == current)
                            st = 3;
                        else
                            st = 1;
                    } else {
                        current = current->right;
                        st = 2;
                    }
                }
                break;
        }
    }
}


/* print the nodes of the tree in ascending order (using a non-recursive method) */

void bstTraverseTree(BST * bst)
{
    bstNode *current;
    int done = 0;
    int st = 2;


    if (bst == NULL) {
        fprintf(stderr, "Queue is empty");
        return;
    }

    if (!bst->init) {
        bstInit(bst);
        return;
    }
    current = bst->root;               /* start at the root */
    if (current == NULL)
        return;
    while (!done) {
        switch (st) {                  /* switch based upon the current state of traversing the tree */
            case 1:                   /* go up to the parent of current node */
                {
                    current = current->parent;
                    if (current->parent == NULL)
                        done = 1;
                    else if (current == current->parent->left)
                        st = 3;
                }
                break;
            case 2:                   /* traverse left until null is found */
                {

                    while (current->left != NULL)
                        current = current->left;
                    st = 4;

                }
                break;
            case 3:                   /* go to the parent of current node */
                {
                    current = current->parent;
                    st = 4;
                }
                break;
            case 4:                   /* do something to the current node, then figure out where to go */
                {
                    bst->traverse_node(current->info);
                    if (current->right == NULL) {
                        if (current->parent == NULL)
                            done = 1;
                        else if (current->parent->left == current)
                            st = 3;
                        else
                            st = 1;
                    } else {
                        current = current->right;
                        st = 2;
                    }
                }
                break;
        }
    }
}

void bstDeleteNodes(BST * bst, bstNode * node)
{
    if (node->left != NULL) {
        bstDeleteNodes(bst, node->left);
    }
    if (node->right != NULL) {
        bstDeleteNodes(bst, node->right);
    }
    bst->delete_node(node->info);
    free(node);
}

void bstDeleteTree(BST * bst)
{


    if (bst == NULL) {
        fprintf(stderr, "Queue is empty");
        return;
    }

    if (!bst->init) {
        bstInit(bst);
        return;
    }

    bstDeleteNodes(bst, bst->root);

    bst->root = NULL;
    bst->nodes = 0;
    bst->deltype = 0;
}

/* default printing function, will be overridden by the user */

void DefPrintProc(FILE * output, void *info)
{
    (void) output;                     /* unused */
    (void) info;                       /* unused */
}

/* default traverse function, will be overridden by the user */

void DefTraverseProc(void *info)
{
    (void) info;                       /* unused */
}

/* default delete function, will be overridden by the user */

void DefDeleteProc(void *info)
{
    (void) info;                       /* unused */
}

/* default malloc fail handler... can be overridden by user */

void DefHandlerProc(void)
{
    fprintf(stderr, "\nOut of memory... Exiting\n");
    exit(EXIT_FAILURE);
}



/* initialize the BST structure, must be called first before any other bst function */

void bstInit(BST * bst)
{
    bst->root = NULL;
    bst->nodes = 0;
    bst->deltype = 0;
    bst->init = 1;
    bst->print_node = DefPrintProc;
    bst->traverse_node = DefTraverseProc;
    bst->delete_node = DefDeleteProc;
    bst->malloc_fail_handler = DefHandlerProc;
}


/* override the default printing function with a user defined print function */

void bstSetPrintFunc(BST * bst, void (*printfunc) (FILE * output, void *info))
{
    bst->print_node = printfunc;
}

/* override the default traverse function with a user defined print function */

void bstSetTraverseFunc(BST * bst, void (*traversefunc) (void *info))
{
    bst->traverse_node = traversefunc;
}

/* override the default traverse function with a user defined print function */

void bstSetDeleteFunc(BST * bst, void (*deletefunc) (void *info))
{
    bst->delete_node = deletefunc;
}

/* override the default compare function with a user defined compare function */

void bstSetCompareFunc(BST * bst, int (*compfunc) (void *left, void *right))
{
    bst->compare = compfunc;
}


/* override the malloc fail handler with the user defined handler function */

void bstSetMallocFailHandler(BST * bst, void (*malloc_fail_handler) (void))
{
    bst->malloc_fail_handler = malloc_fail_handler;
}
