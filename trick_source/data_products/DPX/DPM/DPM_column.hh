
#ifndef DPM_COLUMN_HH
#define DPM_COLUMN_HH

#include <stdexcept>
#include <vector>
#include <libxml/tree.h>
#include "DPM/DPM_component.hh"
#include "DPM/DPM_var.hh"

/**
 * @brief Represents one column of an DPM_relation.
 * @author John M. Penn
 */
/**
 * DPM_column is an DPM_component which represents one column of an DPM_relation.
 * It consists of:
 * - A label (optional).
 * - A specification of the units.
 * - A list of variables names.
 * - attributes (as a result of also being an DPM_component).
 */
class DPM_column : public DPM_component {

public:
  /**
   * XML based constructor.
   * @param column_node this is a pointer to an xmlNode whose type is
   *        XML_ELEMENT_NODE and whose name is "column".
   */
  DPM_column(DPM_component *parent, xmlNode *column_node) ;

  /**
   * Destructor.
   */
  ~DPM_column();

  /**
   * Return a pointer to the label if present, Null otherwise;
   */
  const char *getLabel();

  /**
   * Return a pointer to the units specification.
   */
  const char *getUnits();

  /**
   * Return a pointer to the units specification.
   */
  DPM_var *getVar();

  /**
   * Output an XML representation of an DPM_column object.
   * @param s is the std::ostream to which this function writes.
   * @param column is a pointer to the DPM_column object whose representation
   * is to be written.
   */
  friend std::ostream& operator<< (std::ostream& s, const DPM_column *column);

private:
  char    * label;
  char    * unitspec;
  DPM_var * column_var;
  int Initialize(xmlNode *base_node);
};
#endif
