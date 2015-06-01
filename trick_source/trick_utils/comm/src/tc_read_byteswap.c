
/*
 * Read data to a device and byteswap if server and client
 * byte orders are different.
 */

#include "trick/tc.h"
#include "trick/tc_proto.h"

int tc_read_byteswap(TCDevice * device, char *buffer, int size, ATTRIBUTES * attr)
{
    char local_byteorder;
    int ret = 0;
    static char swap[65536];

    if (!device) {
        TrickErrorHndlr *temp_error_hndlr = NULL;
        trick_error_report(temp_error_hndlr,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__, "Trying to read_byteswap to a NULL device");
        return (-1);
    }

    TRICK_GET_BYTE_ORDER(local_byteorder)

        if (device->byte_info[TC_BYTE_ORDER_NDX] != local_byteorder) {
        memset(swap, 0, (size_t) size);
        ret = tc_read(device, (char *) swap, size);
        trick_bswap_buffer(buffer, swap, attr, 0);
        return (ret);
    } else {
        return (tc_read(device, (char *) buffer, size));
    }

    return 0;

}
