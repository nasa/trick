
/*
 * Set the time in seconds that a blocking read will wait before timing out
 */

#include "trick/tc.h"
#include "trick/tc_proto.h"

int tc_set_blockio_timeout_limit(TCDevice * device, double limit)
{

    if (!device) {
        TrickErrorHndlr *temp_error_hndlr = NULL;
        trick_error_report(temp_error_hndlr,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__,
                           "Trying to tc_set_blockio_timeout_limit " "to a NULL device");
        return (-1);
    }

    trick_error_report(device->error_handler,
                       TRICK_ERROR_ALL, __FILE__, __LINE__,
                       "set blockio_timeout_limit for client "
                       "(ID = %d  tag = %s) limit = %f\n", device->client_id, device->client_tag, limit);

    device->blockio_limit = limit;

    return 0;

}
