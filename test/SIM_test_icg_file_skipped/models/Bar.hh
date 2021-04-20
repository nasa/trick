// @trick_parse{everything}
#include "Foo.hh"
#include "trick/memorymanager_c_intf.h"

class Bar {
    public:
    Foo foo;
    int o;

    Bar():o(777){}

    int test_bar_member_o_correct_offset() {
        char ref_name[] = "testfsSimObject.bar.o";
        REF2* ref = ref_attributes(ref_name);
        if(*((int*)ref->address) == o) {
            return 0;
        }
        else {
            return 1;
        }
    }
};
