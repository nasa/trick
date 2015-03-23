
#include <string.h>
#include "../include/tc.h"
#include "../include/tc_proto.h"

int tc_dev_copy(TCDevice * dest, TCDevice * src)
{
    /* 
     * If no destination memory or src memory
     */
    if (dest == NULL || src == NULL) {
        return (-1);
    }

    *dest = *src;

    return (0);

}
