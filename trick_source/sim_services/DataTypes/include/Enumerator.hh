#ifndef ENUMERATOR_H
#define ENUMERATOR_H

#include <string>

class Enumerator {

public:
    Enumerator(std::string name, int value);
    Enumerator( const Enumerator& original);
    std::string getName();
    int getValue();
    std::string toString();

private:
    std::string name;
    int value;
};
#endif
