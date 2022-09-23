
#include <string.h>
#include "trick/tc.h"
#include "trick/tc_proto.h"

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
