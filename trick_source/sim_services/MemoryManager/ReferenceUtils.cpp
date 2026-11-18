#include "trick/ReferenceUtils.hh"

#include <stdio.h>
#include <string.h>

// Returns the size in bytes of the element type for an STL container.
// For example, for a std::vector<int>, it would return sizeof(int).
static size_t stl_elem_type_size(TRICK_TYPE elem_type)
{
    switch (elem_type)
    {
    case TRICK_CHARACTER:
    case TRICK_UNSIGNED_CHARACTER:
    case TRICK_BOOLEAN:
        return sizeof(char);
    case TRICK_SHORT:
    case TRICK_UNSIGNED_SHORT:
        return sizeof(short);
    case TRICK_INTEGER:
    case TRICK_UNSIGNED_INTEGER:
    case TRICK_ENUMERATED:
        return sizeof(int);
    case TRICK_LONG:
    case TRICK_UNSIGNED_LONG:
        return sizeof(long);
    case TRICK_LONG_LONG:
    case TRICK_UNSIGNED_LONG_LONG:
        return sizeof(long long);
    case TRICK_FLOAT:
        return sizeof(float);
    case TRICK_DOUBLE:
        return sizeof(double);
    case TRICK_STRING:
    case TRICK_WSTRING:
    case TRICK_STRUCTURED:
        return sizeof(void*);
    default:
        return 0;
    }
}

// Check if a reference is to an STL container element.
// Returns true if the reference points to an element within an STL container, false otherwise.
bool Trick::ReferenceUtils::is_stl_ref(const REF2* ref)
{
    if (!ref || !ref->attr)
        return false;
    if (ref->stl_present == 1)
        return true;
    if (ref->attr->type == TRICK_STL && ref->reference)
    {
        size_t len = strlen(ref->reference);
        return (len > 0 && ref->reference[len - 1] == ']');
    }
    return false;
}

// Determine the effective Trick type of a reference.
// If the reference points to an element within an STL container, return the element type.
// Otherwise, return the type of the attribute itself.
TRICK_TYPE Trick::ReferenceUtils::effective_trick_type(const REF2* ref)
{
    if (!ref || !ref->attr)
        return TRICK_NUMBER_OF_TYPES;
    if (ref->attr->type == TRICK_STL && ref->reference)
    {
        size_t len = strlen(ref->reference);
        if (len > 0 && ref->reference[len - 1] == ']')
            return ref->attr->stl_elem_type;
    }
    return ref->attr->type;
}

// Determine the effective size of a reference.
// If the reference points to an element within an STL container, return the size of the element type.
// Otherwise, return the size of the attribute itself.
size_t Trick::ReferenceUtils::effective_trick_size(const REF2* ref)
{
    if (!ref || !ref->attr)
        return 0;
    if (ref->attr->type == TRICK_STL && ref->reference)
    {
        size_t len = strlen(ref->reference);
        if (len > 0 && ref->reference[len - 1] == ']')
            return stl_elem_type_size(ref->attr->stl_elem_type);
    }
    return (size_t)ref->attr->size;
}

// Determine the integer type matching an enum's underlying type.
// The width comes from attr->size and the signedness from the TRICK_ENUM_UNSIGNED flag
// ICG sets in the mods of every ENUM_ATTR entry of an enum with an unsigned underlying type.
TRICK_TYPE Trick::ReferenceUtils::enum_underlying_type(const ATTRIBUTES* attr)
{
    if (!attr)
        return TRICK_INTEGER;
    const ENUM_ATTR* eattr = (const ENUM_ATTR*)attr->attr;
    bool is_unsigned       = (eattr != NULL && (eattr[0].mods & TRICK_ENUM_UNSIGNED));
    switch (attr->size)
    {
    case sizeof(char):
        return is_unsigned ? TRICK_UNSIGNED_CHARACTER : TRICK_CHARACTER;
    case sizeof(short):
        return is_unsigned ? TRICK_UNSIGNED_SHORT : TRICK_SHORT;
    case sizeof(long long):
        return is_unsigned ? TRICK_UNSIGNED_LONG_LONG : TRICK_LONG_LONG;
    default:
        return is_unsigned ? TRICK_UNSIGNED_INTEGER : TRICK_INTEGER;
    }
}

// Determine the effective type of a reference with non-int sized enums resolved to their underlying type.
TRICK_TYPE Trick::ReferenceUtils::effective_data_type(const REF2* ref)
{
    TRICK_TYPE type = effective_trick_type(ref);
    if (type != TRICK_ENUMERATED)
        return type;
    // STL enum elements are sized as int (see stl_elem_type_size) and carry no ENUM_ATTR here.
    // int sized enums stay TRICK_ENUMERATED, read as int, so existing logs keep their type. Plain enums
    // with no negative values have an unsigned int underlying type, so the unsigned flag is not used here.
    if (ref->attr->type == TRICK_STL || ref->attr->size == sizeof(int))
        return TRICK_ENUMERATED;
    return enum_underlying_type(ref->attr);
}

// Format an enum value as a number in the width and signedness of its underlying type.
int Trick::ReferenceUtils::format_enum_value(char* buf, size_t buf_size, const REF2* ref, const void* address)
{
    switch (effective_data_type(ref))
    {
    case TRICK_CHARACTER:
        return snprintf(buf, buf_size, "%d", *((const signed char*)address));
    case TRICK_UNSIGNED_CHARACTER:
        return snprintf(buf, buf_size, "%u", *((const unsigned char*)address));
    case TRICK_SHORT:
        return snprintf(buf, buf_size, "%d", *((const short*)address));
    case TRICK_UNSIGNED_SHORT:
        return snprintf(buf, buf_size, "%u", *((const unsigned short*)address));
    case TRICK_LONG_LONG:
        return snprintf(buf, buf_size, "%lld", *((const long long*)address));
    case TRICK_UNSIGNED_LONG_LONG:
        return snprintf(buf, buf_size, "%llu", *((const unsigned long long*)address));
    default:
        return snprintf(buf, buf_size, "%d", *((const int*)address));
    }
}
