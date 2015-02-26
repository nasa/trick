#ifndef XMLATTRIBUTE_HH
#define XMLATTRIBUTE_HH

#include <string>
#include <iostream>

class XMLAttribute {

public:
  /**
   * XMLAttribute constructor.
   * @param Key the identifier for the attribute.
   * @param Value the value of he attribute.
   */
  XMLAttribute(const std::string& Key, const std::string& Value)
    : key(Key), value(Value) {}

  /**
   * Copy constructor.
   */
  XMLAttribute(const XMLAttribute *attr);

  /**
   * If name matches the key value of the attribute return 1, otherwise return 0..
   */
  int key_is(const char *name);

  /**
   * Return a pointer to a copy of the attribute value. The caller shall
   * be responsible for deleting the string that is returned.
   */
  const std::string& value_of();

private:
  std::string key;    /**< The name of the attribute.*/
  std::string value;  /**< The value of the attribute.*/
};
#endif
