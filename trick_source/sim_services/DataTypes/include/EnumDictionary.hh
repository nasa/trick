#ifndef ENUM_DICTIONARY_H
#define ENUM_DICTIONARY_H

#include <map>
#include <string>
#include <stdexcept>

/**
 EnumDictionary is a container classes that stores <name (key) /value> pairs.
 */
class EnumDictionary {

public:

    EnumDictionary () {};

    /**
     Add a name/value pair to the dictionary.
     */
    void addEnumerator(std::string name, int value) ;

    /**
     Get the value associated with the given string.i FIXME: what of string not found?
     */
    int getValue(std::string name)  ;

    /**
     */
    std::string toString();

    /**
     Destructor for EnumDictionary.
     */
    ~EnumDictionary() {}

private:
    std::map<std::string, int> enumDictionary;
    std::map<std::string, int>::iterator enumDictionaryIterator;
};
#endif
