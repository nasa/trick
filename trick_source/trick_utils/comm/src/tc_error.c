
/*
 * Turn error reporting On/Off
 */

#include "trick/tc.h"
#include "trick/tc_proto.h"

int tc_error(TCDevice * device, int on_off)
{
    size_t size;

    if (!device) {
        TrickErrorHndlr *temp_error_hndlr = NULL;
        trick_error_report(temp_error_hndlr,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__, "Trying to set up error handler on NULL device");
        return (-1);
    }

    /* If there is no error handler, initialize one */
    if (device->error_handler == NULL) {
        size = sizeof(TrickErrorHndlr);
        device->error_handler = (TrickErrorHndlr *) malloc(size);
        trick_error_init(device->error_handler, NULL, NULL, TRICK_ERROR_ALL);
    }

    /* Now turn on/off */
    if (on_off == 1) {
        device->error_handler->report_level = TRICK_ERROR_ALL;
    } else {
        device->error_handler->report_level = TRICK_ERROR_SILENT;
    }

    return 0;

}
