#include "XMLComponent.hh"

void XMLComponent::Initialize( xmlNode *Base_node) {

    if ( (Base_node == NULL) ||
         (Base_node->type != XML_ELEMENT_NODE )) {
            std::cerr << "XMLComponent::Initialize: Bad parameters." << std::endl;
    } else {
        xmlAttr *current_attr = NULL;

        /* Get Attributes from the XML parse tree. */
        for (current_attr = Base_node->properties; current_attr ; current_attr = current_attr->next ) {
            if (current_attr->type == XML_ATTRIBUTE_NODE ) {
                // Add the attribute to the list.
                std::string key( (const char *)current_attr->name);
                std::string value( (const char *)current_attr->children->content);
                attribute_list.push_back( new XMLAttribute(key, value) );
            }
        }
    }
}

// XML CONSTRUCTOR
XMLComponent::XMLComponent(xmlNode *Base_node) {

    Initialize( Base_node);
}

// DESTRUCTOR
XMLComponent::~XMLComponent() {
    // destroy each of the attributes in the attribute list
    unsigned int i;
    std::vector<XMLAttribute *>::size_type attr_count = this->attribute_list.size();
    for (i = 0; i<attr_count ; i++) {
        delete this->attribute_list[i];
    }
}

// MEMBER FUNCTION
unsigned int XMLComponent::NumberOfAttributes() {
    return (unsigned int)attribute_list.size();
}

// MEMBER FUNCTION
const char *XMLComponent::AttributeValue( const char *name ) {
    unsigned int i;
    std::vector<XMLAttribute *>::size_type attr_count = this->attribute_list.size();
    for (i=0 ; i<attr_count ; i++) {
        if ( attribute_list[i]->key_is(name) ) {
            return ( attribute_list[i]->value_of().c_str());
        }
    }
    return "";
}
