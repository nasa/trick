
/*
 * Write data to a device and byteswap if server and client
 * byte orders are different.
 */

#include "trick/tc.h"
#include "trick/tc_proto.h"
#include "trick/trick_byteswap.h"

int tc_write_byteswap(TCDevice * device, char *buffer, int size, ATTRIBUTES * attr)
{
    char local_byteorder;
    int ret = 0;
    static char swap[65536];

    if (!device) {
        TrickErrorHndlr *temp_error_hndlr = NULL;
        trick_error_report(temp_error_hndlr,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__, "Trying to write_byteswap to a NULL device");
        return (-1);
    }

    TRICK_GET_BYTE_ORDER(local_byteorder)

        if (device->byte_info[TC_BYTE_ORDER_NDX] != local_byteorder) {
        memset(swap, 0, (size_t) size);
        trick_bswap_buffer(swap, buffer, attr, 1);
        ret = tc_write(device, (char *) swap, size);
        return (ret);
    } else {
        return (tc_write(device, (char *) buffer, size));
    }

    return 0;

}
