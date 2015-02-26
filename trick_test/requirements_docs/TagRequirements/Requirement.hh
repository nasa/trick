#ifndef REQUIREMENT_HH
#define REQUIREMENT_HH

#include <stdexcept>
#include <libxml/tree.h>
#include "XMLComponent.hh"

class Requirement : public XMLComponent {

public:
  /**
   * Constructor.
   */
  Requirement(xmlNode * node) throw (std::invalid_argument);

  /**
   * Destructor
   */
  ~Requirement();

   unsigned int derivedRequirementsCount();
   void writeTagged(int indent);
   void writeAttribute( std::string& v, const char* k);

private:
   std::vector <Requirement *> derivedRequirements;

    int Initialize(xmlNode *node);
};
#endif
