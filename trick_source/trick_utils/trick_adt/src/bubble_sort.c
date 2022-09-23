/*********************************** TRICK HEADER **************************
PURPOSE:
   (Generalized bi-directional bubble sort for a linked list.)
REFERENCES:
   ((NONE))
ASSUMPTIONS AND LIMITATIONS:
   ((This routines uses an inefficient bi-directional bubble sort.))
CLASS:
   (scheduled)
LIBRARY DEPENDENCY:
   ((bubble_sort.o))
PROGRAMMERS:
   (((Edwin Z. Crues) (LinCom) (10 Oct 97) (Trickified for RGPS work)))
***************************************************************************/

#include "trick/dllist.h"
#include <stdio.h>

int bubble_sort(                /* RETURN: -- Always returns zero. */
                   DLLIST * list)
{                                      /* INOUT: -- Linked list to sort. */
    /* Declare list reordering flag. */
    int reordered;

    /* Declare list position parameters. */
    DLLPOS top;
    DLLPOS prev;
    DLLPOS current;
    DLLPOS next;
    DLLPOS bottom;

    /* Pointers to PRN list entry. */
    void *prev_prn;
    void *this_prn;
    void *next_prn;

    /* Make sure that list is not null. */

    if (list == NULL) {
        fprintf(stderr, "List is NULL");
        return -1;
    }

    /* Make sure that comparison function is not null. */

    if (list->compare == NULL) {
        fprintf(stderr, "Comparison function is NULL");
        return -1;
    }

    /* Mark bounds and position for start of linked list sort. */
    top = DLL_GetHeadPosition(list);
    bottom = DLL_GetTailPosition(list);

    /* Loop through the entire linked list. */
    while (top != bottom) {

        /* Initialize the sort parameters. */
        current = top;
        reordered = 0;

        /* Go down list, sorting in ascending order. */
        while (current != bottom) {

            next = current;
            this_prn = DLL_GetNext(&next, list);
            next_prn = DLL_GetAt(next, list);

            /* Compare current PRN to next in list. */
            if ((*(list->compare)) (this_prn, next_prn)) {

                /* Swap the entries. */
                DLL_SetAt(current, next_prn, list);
                DLL_SetAt(next, this_prn, list);

                /* Mark the list as reordered. */
                reordered = 1;

            }

            /* Move to the next position in the list. */
            current = next;

        }

        /* Check the reordered flag. */
        if (!reordered) {
            /* If the list was not reordered, we're done. */
            return (0);
        } else {
            /* Reset the reordered flag for the reverse pass. */
            reordered = 0;
        }

        /* Move the bottom of the sort list up. */
        DLL_GetPrev(&bottom, list);

        /* Make sure the current position is the new bottom of the list. */
        current = bottom;

        /* Go up list, sorting in ascending order. */
        while (current != top) {

            prev = current;
            this_prn = DLL_GetPrev(&prev, list);
            prev_prn = DLL_GetAt(prev, list);

            /* Compare current PRN to previous in list. */
            if ((*(list->compare)) (prev_prn, this_prn)) {

                /* Swap the entries. */
                DLL_SetAt(current, prev_prn, list);
                DLL_SetAt(prev, this_prn, list);

                /* Mark the list as reordered. */
                reordered = 1;

            }

            /* Move to the previous position in the list. */
            current = prev;

        }

        /* If the list was not reordered, we're done. */
        if (!reordered) {
            return (0);
        }

        /* Move the top of the sort list down. */
        DLL_GetNext(&top, list);

    }

    return (0);

}
