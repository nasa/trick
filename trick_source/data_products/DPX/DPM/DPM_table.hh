
#ifndef DPM_TABLE_HH
#define DPM_TABLE_HH

#include <vector>
#include <libxml/tree.h>
#include "DPM/DPM_component.hh"
#include "DPM/DPM_column.hh"
#include "DPM/DPM_time_constraints.hh"

/**
 * This class represents a table.
 * @author John M. Penn
 */
class DPM_table : public DPM_component {

public:
  /**
   * Constructor for DPM_table.
   * @param plot_node a pointer to a libxml2 xmlNode of type XML_ELEMENT_NODE,
   * and whose name is "plot".
   */
  DPM_table(DPM_component *parent, xmlNode *plot_node);
  /**
   * Destructor for DPM_table.
   */
  ~DPM_table();

  /**
   * Return the title of the relation, if it has one, otherwise return NULL.
   */
  const char *getTitle();

  /**
   * Return a pointer to the DPM_time_constraints object.
   */
  DPM_time_constraints *getTimeConstraints();

  /**
   * Return the number of columns within this DPM_table.
   */
  int NumberOfColumns();

  /**
   * Return a pointer to the subordinate column indicated by index.
   */
  DPM_column *getColumn( unsigned int index);

  /**
   * Output an xml representation of DPM_table.
   * @param s std::ostream to be written.
   * @param relation pointer to the DPM_table object to be written.
   */
  friend std::ostream& operator<< (std::ostream& s, const DPM_table *relation);

private:
  char  *title;
  DPM_time_constraints *time_constraints;
  std::vector <DPM_column *> column_list;

  int Initialize(xmlNode *relation_elem_node);
};
#endif
