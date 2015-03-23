
#ifndef DPM_COMPONENT_HH
#define DPM_COMPONENT_HH

#include <libxml/tree.h>
#include <vector>
#include "DPM/DPM_attribute.hh"

/**
 * This class is the base class for those DPX classes which contain attributes.
 * @author John M. Penn
 */
class DPM_component {

public:
    /**
     * Default constructor.
     */
    DPM_component();

    /**
     * XML constructor.
     */
    DPM_component(DPM_component *parent, xmlNode *element_node);

    /**
     * DPM_component Destructor.
     */
    ~DPM_component();

    /**
     * Add an attribute (key-value) pair to the DPM_component.
     */
    void add_attribute(const char *key, const char* value);

    /**
     * Return the number of attributes contained within this DPM_component.
     */
    int NumberOfAttributes();

    /**
     * Return a pointer to a copy of the DPM_attribute value by name.
     * @param key this is the key to be matched.
     */
    const char *AttributeValue(const char *key);

protected:
    std::vector <DPM_attribute *> attribute_list; /**< This is the list of attributes
                                                associated with an DPM_component. */

private:
    void Initialize( xmlNode *base_node);
    DPM_component * parent;
};
#endif
