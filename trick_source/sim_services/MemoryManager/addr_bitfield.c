#include "trick/attributes.h"
#include "trick/bitfield_proto.h"

/* Who is using this? (Alex 04/16/12) */

void addr_bitfield(ATTRIBUTES *A, ATTRIBUTES *B, int field_count) {
        // A is last non-bit field in struct
        // B is bit field to process
        static int bitfield_offset, bitfield_bits_used ;
        static int misalign_bytes, prev_item_size;
        int used_bytes, ii, dims;
        int offset, item_size, item_bits ;
        int bitfield_max, bitfield_align;
        #define BITS_PER_BYTE 8

        // a null bitfield means 0 length - go to next bit container
        // field_count will be the container size in this case
        if (B==NULL) {
                bitfield_offset = bitfield_offset - (bitfield_offset % field_count) + field_count ;
                bitfield_bits_used = 0 ;
                prev_item_size = field_count;
                return ;
        }

        // offset is where bitfield group will start in struct
        if (A==NULL) {
                offset = 0 ;
        }
        else {
            // compute offset from previous struct field
            dims = 1;
            if (A->num_index > 0) {
                for (ii = 0; ii<A->num_index; ii++) dims *= A->index[ii].size;
            }
            offset = A->offset + (A->size * dims);
        }
        if (field_count==1) {
        // start of bitfield group
            bitfield_offset = offset ;
            bitfield_bits_used = 0 ;
            misalign_bytes = 0;
        }
        // backward compatibility: if started on even boundary, treat short same as int
        // current sim users expect shorts to be addressed and returned as ints
        // (i.e. any bitfield declared as short should not span a short boundary)
        // GOOD: short x:8; short y:4; short z:4;
        // BAD:  short x:8; short y:6; short z:6;
        if ((B->size==sizeof(short)) && (bitfield_offset-misalign_bytes) % 2 == 0) {
            B->size = sizeof(int);
        }
        item_size = B->size ;
        item_bits = B->index[0].size ;
        bitfield_max = item_size * BITS_PER_BYTE ;
        if (field_count==1) {
        // when you start an int/short not on an int/short boundary, account for it as used bits
            misalign_bytes = (bitfield_offset % item_size);
            bitfield_bits_used = misalign_bytes * BITS_PER_BYTE;
            bitfield_offset -= misalign_bytes;
            prev_item_size = item_size;
        }
        if (item_size != prev_item_size) {
        // go to next bit container if bitfield different declared size than previous
            used_bytes = (bitfield_bits_used / BITS_PER_BYTE) + ((bitfield_bits_used % BITS_PER_BYTE) > 0);
            bitfield_offset += used_bytes;
            bitfield_align = (item_size > 1) && (bitfield_offset % 2); // on even boundary if not char
            bitfield_offset += bitfield_align;
            bitfield_bits_used = 0;
            misalign_bytes = 0;
        } else {
        // same declared size
            if (item_bits + bitfield_bits_used > bitfield_max) {
            // go to next bit container if it won't fit here
                bitfield_offset += item_size ;
                bitfield_bits_used = bitfield_max < bitfield_bits_used ? bitfield_bits_used-bitfield_max : 0 ;
                misalign_bytes = 0;
            }
        }
        B->offset = bitfield_offset ;
        B->index[0].start = bitfield_max - bitfield_bits_used - item_bits ;
        bitfield_bits_used += item_bits ;
        prev_item_size = item_size;
}

