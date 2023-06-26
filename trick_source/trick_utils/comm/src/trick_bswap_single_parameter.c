
#include <stdio.h>

#include "trick/attributes.h"
#include "trick/parameter_types.h"
#include "trick/trick_byteswap.h"
#include "trick/tc_proto.h"

/*
 * tofrom: 1 = to.
 * Use 1 (to) before writing data. Converts to the other endian
 *
 * tofrom: 0 = from.
 * Use 0 (from) after reading data. Converts from the other endian
 */

void *trick_bswap_single_parameter(void *out, void *in, ATTRIBUTES * attr, int tofrom)
{

    int i, j;
    unsigned short ts;
    unsigned int ti;
    double td;
    int num;
    unsigned int mask;
    int shift;
    int local_order;

    i = 0;
    TRICK_GET_BYTE_ORDER(local_order);

    if ((attr[i].name != NULL) && (attr[i].name[0] != '\0')) {

        num = 1;
        for (j = 0; j < attr[i].num_index; j++) {
            num *= attr[i].index[j].size;
        }

        if (attr[i].type == TRICK_STRUCTURED) {

            for (j = 0; j < num; j++) {
                trick_bswap_buffer((char *) ((char *) out + attr[i].offset + j * attr[i].size),
                                   (char *) ((char *) in + attr[i].offset + j * attr[i].size),
                                   (ATTRIBUTES *) attr[i].attr, tofrom);
            }

        } else if (attr[i].type == TRICK_BITFIELD || attr[i].type == TRICK_UNSIGNED_BITFIELD) {

            ti = *(int *) ((char *) in );

            /* swap this word if this is incoming data */
            if (tofrom == 0) {
                ti = trick_byteswap_int((int) ti);
            }

            if ((local_order == TRICK_LITTLE_ENDIAN && tofrom == 1) || (local_order == TRICK_BIG_ENDIAN && tofrom == 0)) {

                /* Make a mask of "size" bits and shift it to the correct bit position */
                mask =
                    (~(0xffffffff << attr[i].index[0].size)) << (32 - attr[i].index[0].start - attr[i].index[0].size);
                ti &= mask;

                /* Calculate shift to new location */
                shift = (2 * attr[i].index[0].start) + attr[i].index[0].size - 32;
            } else {

                /* Make a mask of "size" bits and shift it to the correct bit position */
                mask =
                    (~(0xffffffff >> attr[i].index[0].size)) >> (32 - attr[i].index[0].start - attr[i].index[0].size);
                ti &= mask;

                /* Calculate shift to new location */
                shift = 32 - (2 * attr[i].index[0].start) - attr[i].index[0].size;
            }
            if (shift > 0) {
                ti = ti << shift;
            } else {
                ti = ti >> -shift;
            }

            /* Mask in the new bits */
            *(int *) ((char *) out ) = ti;

        } else {

            switch (attr[i].size) {

                case 1:
                    for (j = 0; j < num; j++) {
                        *(unsigned char *) ((char *) out + j) = *(unsigned char *) ((char *) in + j);
                    }
                    break;
                case 2:
                    for (j = 0; j < num; j++) {
                        ts = *(unsigned short *) ((char *) in + j * 2);
                        *(unsigned short *) ((char *) out + j * 2) = trick_byteswap_short((short) ts);
                    }
                    break;
                case 4:
                    for (j = 0; j < num; j++) {
                        ti = *(unsigned int *) ((char *) in + j * 4);
                        *(unsigned int *) ((char *) out + j * 4) = trick_byteswap_int((int) ti);
                    }
                    break;
                case 8:
                    for (j = 0; j < num; j++) {
                        td = *(double *) ((char *) in + j * 8);
                        *(double *) ((char *) out + j * 8) = trick_byteswap_double(td);
                    }
                    break;
            }
        }
        i++;
    }

    return ((void *) out);
}
