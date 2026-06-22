#include "trick/ReferenceUtils.hh"

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
