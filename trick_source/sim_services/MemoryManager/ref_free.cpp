/*
   PURPOSE: (Frees allocated memory within a REF2 structure. Does not free the REF2 itself)
   PROGRAMMERS: (((Alex Lin) (NASA) (5/13)))
*/

#include <stdlib.h>

#include "trick/reference.h"
#include "trick/dllist.h"

extern "C" void ref_free(REF2 * ref) {
    if ( ref ) {
        if ( ref->address_path ) {
            DLLPOS pos = DLL_GetHeadPosition(ref->address_path);
            DLLPOS prev = NULL;
            while (pos != NULL) {
                prev = pos;
                // The address nodes were allocated with "new"
                delete (ADDRESS_NODE *)DLL_GetNext(&pos, ref->address_path);
                DLL_RemoveAt(prev, ref->address_path);
            }
            DLL_Delete(ref->address_path) ;
        }

        // The reference string was allocated with strdup.
        if ( ref->reference ) {
            free(ref->reference) ;
            ref->reference = NULL;
        }

        // Free units string
        if (ref->units) {
            free(ref->units);
            ref->units = NULL;
        }

        // Free ref_attr, however, not freeing the shared attr
        if (ref->ref_attr) {
            free(ref->ref_attr);
            ref->ref_attr = NULL;
        }
    }
    return ;
}
