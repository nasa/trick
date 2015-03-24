#include <iostream>

#include "sim_services/MonteCarlo/include/MonteVarCalculated.hh"
#include "sim_services/MemoryManager/include/memorymanager_c_intf.h"
#include "sim_services/Message/include/message_proto.h"
#include "sim_services/Message/include/message_type.h"

Trick::MonteVarCalculated::MonteVarCalculated(std::string in_name, std::string in_unit) {
    this->name = in_name;
    this->unit = in_unit;

    ref2 = ref_attributes((char *)name.c_str());
    if (ref2 == NULL) {
        message_publish(MSG_ERROR, "Monte : MonteVarCalculated could not find parameter %s.\n", name.c_str()) ;
    }
}

std::string Trick::MonteVarCalculated::get_next_value() {
    char buffer[128];
    if (ref2 != NULL) {
        switch (ref2->attr->type) {
            case TRICK_CHARACTER:
            case TRICK_UNSIGNED_CHARACTER:
                sprintf(buffer, "%d", *(char *)ref2->address);
                value = buffer;
                break;
            case TRICK_SHORT:
            case TRICK_UNSIGNED_SHORT:
                sprintf(buffer, "%d", *(short *)ref2->address);
                value = buffer;
                break;
            case TRICK_INTEGER:
            case TRICK_UNSIGNED_INTEGER:
            case TRICK_LONG:
            case TRICK_UNSIGNED_LONG:
                sprintf(buffer, "%ld", *(long *)ref2->address);
                value = buffer;
                break;
            case TRICK_LONG_LONG:
            case TRICK_UNSIGNED_LONG_LONG:
                sprintf(buffer, "%lld", *(long long *)ref2->address);
                value = buffer;
                break;
            case TRICK_FLOAT:
                sprintf(buffer, "%.10g", *(float *)ref2->address);
                value = buffer;
                sprintf(buffer, "trick.unhex_float(0x%lx)", *(long *)ref2->address);
                break;
            case TRICK_DOUBLE:
                sprintf(buffer, "%.10g", *(double *)ref2->address);
                value = buffer;
                sprintf(buffer, "struct.unpack(\"!d\", binascii.unhexlify(\"%016llx\"))[0]",
                  *(long long *)ref2->address);
                break;
            default:
                sprintf(buffer, "#Unsupported value type %d", ref2->attr->type) ;
                break ;
        }

        if (unit.empty()) {
            return name + std::string(" = ") + std::string(buffer) + std::string(" # ") + std::string(value);
        } else {
            return name + std::string(" = trick.attach_units(\"") + unit + std::string("\", ") + std::string(buffer) +
              std::string(") # ") + std::string(value);
        }
    }
    return NULL;
}
