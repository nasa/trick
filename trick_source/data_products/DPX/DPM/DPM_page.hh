
#ifndef DPM_PAGE_HH
#define DPM_PAGE_HH

#include <string>
#include <vector>
#include <libxml/tree.h>
#include "DPM/DPM_component.hh"
#include "DPM/DPM_relation.hh"

/**
 * This class represents a "page" of plots. A page is meant to be displayed in it's own window.
 * Consists of:
 * - A title
 * - Time constraints for the subordinate plots.
 * - List of plots that are contained within the page.
 * @author John M. Penn
 */
class DPM_page : public DPM_component {

public:
  /**
   * Constructor.
   * @param page_node this is a pointer to an xmlNode whose type is
   *        XML_ELEMENT_NODE and whose name is "page".
   */
  DPM_page(DPM_component *parent, xmlNode *page_node);
  /**
   * Destructor.
   */
  ~DPM_page();

  /**
   * Return the title of the page, if it has one, otherwise return NULL.
   */
  const char *getTitle();

  /**
   * Return a pointer to the DPM_time_constraints object.
   */
  DPM_time_constraints *getTimeConstraints();

  /**
   * Return the number of relations within this DPM_page.
   */
  int NumberOfRelations();

  /**
   * Return a pointer to the subordinate relation indicated by index.
   */
  DPM_relation *getRelation( unsigned int index);

  /**
   * Output an xml representation of DPM_page.
   * @param s std::ostream to be written.
   * @param page pointer to the DPM_page object to be written.
   */
  friend std::ostream& operator<< (std::ostream& s, const DPM_page *page);

private:
  char * title;
  DPM_time_constraints *time_constraints;
  std::vector <DPM_relation *> relation_list;

  int Initialize( xmlNode *base_node);
};
#endif
