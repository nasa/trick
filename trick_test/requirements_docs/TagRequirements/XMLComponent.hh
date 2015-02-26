#ifndef XMLCOMPONENT_HH
#define XMLCOMPONENT_HH

#include <libxml/tree.h>
#include <vector>
#include "XMLAttribute.hh"

class XMLComponent {

public:

    XMLComponent() {}
    /**
     * XML constructor.
     */
    XMLComponent(xmlNode *element_node);

    /**
     * XMLComponent Destructor.
     */
    ~XMLComponent();

    /**
     * Return the number of attributes contained within this XMLComponent.
     */
    unsigned int NumberOfAttributes();

    /**
     * Return a pointer to a copy of the Attribute value by name. 
     * @param key this is the key to be matched.
     */
    const char *AttributeValue(const char *key);

protected:
    std::vector <XMLAttribute *> attribute_list; /**< This is the list of attributes
                                                associated with an XMLComponent. */

private:
    void Initialize( xmlNode *base_node);
};
#endif
