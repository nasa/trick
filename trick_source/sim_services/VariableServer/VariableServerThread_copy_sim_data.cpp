
#include <iostream>
#include <string.h>

#include "trick/VariableServer.hh"
#include "trick/memorymanager_c_intf.h"
#include "trick/exec_proto.h"

int Trick::VariableServerThread::copy_sim_data() {

    unsigned int ii ;
    VariableReference * curr_var ;

    if ( vars.size() == 0 ) {
        return 0 ;
    }

    if ( pthread_mutex_trylock(&copy_mutex) == 0 ) {

        // Get the simulation time we start this copy
        time = (double)exec_get_time_tics() / exec_get_time_tic_value() ;

        for ( ii = 0 ; ii < vars.size() ; ii++ ) {
            curr_var = vars[ii] ;

            // if this variable is unresolved, try to resolve it
            if (curr_var->ref->address == &bad_ref_int) {
                REF2 *new_ref = ref_attributes(curr_var->ref->reference);
                if (new_ref != NULL) {
                    curr_var->ref = new_ref;
                }
            }

            // if there's a pointer somewhere in the address path, follow it in case pointer changed
            if ( curr_var->ref->pointer_present == 1 ) {
                curr_var->address = follow_address_path(curr_var->ref) ;
                if (curr_var->address == NULL) {
                    std::string save_name(curr_var->ref->reference) ;
                    free(curr_var->ref) ;
                    curr_var->ref = make_error_ref(save_name) ;
                    curr_var->address = curr_var->ref->address ;
                } else if ( validate_address ) {
                    // The address is not NULL.
                    // If validate_address is on, check the memory manager if the address falls into
                    // any of the memory blocks it knows of.  Don't do this if we have a std::string or
                    // wstring type, or we already are pointing to a bad ref.
                    if ( (curr_var->string_type != TRICK_STRING) and
                         (curr_var->string_type != TRICK_WSTRING) and
                         (curr_var->ref->address != &bad_ref_int) and
                         (get_alloc_info_of(curr_var->address) == NULL) ) {
                        std::string save_name(curr_var->ref->reference) ;
                        free(curr_var->ref) ;
                        curr_var->ref = make_error_ref(save_name) ;
                        curr_var->address = curr_var->ref->address ;
                    }
                } else {
                    curr_var->ref->address = curr_var->address ;
                }

            }

            // if this variable is a string we need to get the raw character string out of it.
            if (( curr_var->string_type == TRICK_STRING ) && !curr_var->need_deref) {
                std::string * str_ptr = (std::string *)curr_var->ref->address ;
                curr_var->address = (void *)(str_ptr->c_str()) ;
            }

            // if this variable itself is a pointer, dereference it
            if ( curr_var->need_deref) {
                curr_var->address = *(void**)curr_var->ref->address ;
            }

            // handle c++ string and char*
            if ( curr_var->string_type == TRICK_STRING ) {
                if (curr_var->address == NULL) {
                    curr_var->size = 0 ;
                } else {
                    curr_var->size = strlen((char*)curr_var->address) + 1 ;
                }
            }
            // handle c++ wstring and wchar_t*
            if ( curr_var->string_type == TRICK_WSTRING ) {
                if (curr_var->address == NULL) {
                    curr_var->size = 0 ;
                } else {
                    curr_var->size = wcslen((wchar_t *)curr_var->address) * sizeof(wchar_t);
                }
            }
            if(curr_var->address != NULL) {
                memcpy( curr_var->buffer_in , curr_var->address , curr_var->size ) ;
            }
        }

        // Indicate that sim data has been written and is now ready in the buffer_in's of the vars variable list.
        var_data_staged = true;
        packets_copied++ ;

        pthread_mutex_unlock(&copy_mutex) ;
    }


    return (0) ;

}

