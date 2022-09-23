#ifndef VAR_ACCESS_INFO_HH
#define VAR_ACCESS_INFO_HH

class DataType;
class BitFieldStructMember;

struct VarAccessInfo {
    bool isBitField;
    void* address;
    const DataType* dataType;
    const BitFieldStructMember* bitFieldStructMember;
};
#endif
