#ifndef COMPOSITE_VALUE_H
#define COMPOSITE_VALUE_H

#include "Value.hh"
#include <vector>

/**
 CompositeValue is a Value composed of Value (s).
 */
class CompositeValue : public Value {

    public:

    /**
     Destroy this CompositeValue.
     */
    ~CompositeValue();

    /**
     Add a member to this CompositeValue.
     */
    void addMember(Value* value);

    /**
     Get the number of members in this CompositeValue.
     @return the number of members in this CompositeValue.
     */
    int  getMemberCount();

    /**
     Get reference to the component Value at the given index.
     */
    Value*& operator[] (const int index);

    /**
     Print this CompositeValue to a stream.
     */
    void  print(std::ostream &s);

    /**
     Get a string representation of this CompositeValue.
     */
    std::string toString();

    private:
    std::vector<Value*> member_list;
};
#endif
