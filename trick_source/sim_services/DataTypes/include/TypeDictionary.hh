#ifndef TYPE_DICTIONARY_H
#define TYPE_DICTIONARY_H

#include <map>
#include <string>
#include <stdexcept>

class DataType;

/**
 Stores name / typespecifier pairs.
 */
class TypeDictionary {

    public:

    TypeDictionary();

    /**
     Get the DataType of the for the typedef'ed name.
     */
    const DataType* getDataType(std::string typeName);

    /**
     Add a type definiton to the dictionary.
     */
    void addTypeDefinition(std::string name, DataType * typeSpec)
         ;

    /**
     */
    bool validate();

    /**
     Dump the entire TypeDictionary to a std::string.
     */
    std::string toString();

    /**
      Destructor.
     */
    ~TypeDictionary();

    private:
    bool is_valid;
    std::map<std::string, DataType*> typeDictionary;
    std::map<std::string, DataType*>::iterator typeDictionaryIterator;
};
#endif
