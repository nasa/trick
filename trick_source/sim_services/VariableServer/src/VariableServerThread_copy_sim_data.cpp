
#include <iostream>
#include <string.h>

#include "sim_services/VariableServer/include/VariableServer.hh"
#include "sim_services/MemoryManager/include/memorymanager_c_intf.h" // for follow_address_path

int Trick::VariableServerThread::copy_sim_data() {

    unsigned int ii ;
    VariableReference * curr_var ;

    if ( vars.size() == 0 ) {
        return 0 ;
    }

    if ( pthread_mutex_trylock(&copy_mutex) == 0 ) { 

        for ( ii = 0 ; ii < vars.size() ; ii++ ) {
            curr_var = vars[ii] ;

            // if this variable is unresolved, try to resolve it
            if ( retry_bad_ref ) {
                if (curr_var->ref->address == &bad_ref_int) {
                    REF2 *new_ref = ref_attributes(const_cast<char*>(curr_var->ref->reference));
                    if (new_ref != NULL) {
                        curr_var->ref = new_ref;
                    }
                }
            }

            // if there's a pointer somewhere in the address path, follow it in case pointer changed
            if ( curr_var->ref->pointer_present == 1 ) {
                curr_var->address = follow_address_path(curr_var->ref) ;
                curr_var->ref->address = curr_var->address ;
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
            memcpy( curr_var->buffer_in , curr_var->address , curr_var->size ) ;
        }

        retry_bad_ref = false ;

        // Indicate that sim data has been written and is now ready in the buffer_in's of the vars variable list.
        var_data_staged = true;
        packets_copied++ ;

        pthread_mutex_unlock(&copy_mutex) ;
    }


    return (0) ;

}

