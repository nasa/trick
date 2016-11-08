
/*
 * Read data to a device and byteswap if server and client
 * byte orders are different.
 */

#include <pthread.h>

#include "trick/tc.h"
#include "trick/tc_proto.h"
#include "trick/trick_byteswap.h"

static pthread_key_t key ;
static pthread_once_t key_once = PTHREAD_ONCE_INIT ;
static void make_key() {
    pthread_key_create(&key, NULL) ;
}

int tc_read_byteswap(TCDevice * device, char *buffer, int size, ATTRIBUTES * attr)
{
    char local_byteorder;
    int ret = 0;
    struct SwapBuffer * swap_buffer ;

    if (!device) {
        TrickErrorHndlr *temp_error_hndlr = NULL;
        trick_error_report(temp_error_hndlr,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__, "Trying to read_byteswap to a NULL device");
        return (-1);
    }

    pthread_once(&key_once, make_key) ;
    if (( swap_buffer = pthread_getspecific(key)) == NULL ) {
        swap_buffer = malloc(sizeof(struct SwapBuffer)) ;
        swap_buffer->size = 0 ;
        pthread_setspecific(key, swap_buffer) ;
    }
    if ( (unsigned int)size > swap_buffer->size ) {
        unsigned int new_size = (unsigned int)size ;
        if ( new_size % 4 ) {
            new_size = new_size - ( new_size % 4 ) + 4 ;
        }
        swap_buffer->swap_space = malloc(new_size) ;
        swap_buffer->size = new_size ;
    }

    TRICK_GET_BYTE_ORDER(local_byteorder)

    if (device->byte_info[TC_BYTE_ORDER_NDX] != local_byteorder) {
        memset(swap_buffer->swap_space, 0, (size_t) size);
        ret = tc_read(device, (char *) swap_buffer->swap_space, size);
        trick_bswap_buffer(buffer, swap_buffer->swap_space, attr, 0);
        return (ret);
    } else {
        return (tc_read(device, (char *) buffer, size));
    }

    return 0;

}
