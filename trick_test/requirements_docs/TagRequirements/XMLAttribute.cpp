#include "XMLAttribute.hh"
#include <strings.h>
#include <string.h>

// COPY CONSTRUCTOR
XMLAttribute::XMLAttribute(const XMLAttribute *attr) {
  key   = attr->key;
  value = attr->value;
}

// MEMBER FUNCTION
int XMLAttribute::key_is (const char *name) {
  return (strcmp(name, this->key.c_str()) == 0);
}

// MEMBER FUNCTION
const std::string& XMLAttribute::value_of() {
   return this->value;
}

