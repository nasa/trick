
#ifndef DPM_ATTRIBUTE_HH
#define DPM_ATTRIBUTE_HH

#include <string>
#include <iostream>

/**
 * This class represents key-value pair. This is how XML attributes are stored in DPX.
 * @author John M. Penn
 */
class DPM_attribute {

public:
  /**
   * DPM_attribute constructor.
   * @param Key the identifier for the attribute.
   * @param Value the value of he attribute.
   */
  DPM_attribute(const std::string& Key, const std::string& Value)
    : key(Key), value(Value) {}

  /**
   * Copy constructor.
   */
  DPM_attribute(const DPM_attribute *attr);

  /**
   * If name matches the key value of the attribute return 1, otherwise return 0..
   */
  int key_is(const char *name);

  /**
   * Return a pointer to a copy of the attribute value. The caller shall
   * be responsible for deleting the std::string that is returned.
   */
  std::string *value_of();

  /**
   * Output an xml representation of DPM_attribute.
   * @param s std::ostream to be written.
   * @param attr pointer to the DPX_attribue object to be written.
   */
  friend std::ostream& operator<< (std::ostream& s, const DPM_attribute *attr);

private:
  std::string key;    /**< The name of the attribute.*/
  std::string value;  /**< The value of the attribute.*/

};
#endif
