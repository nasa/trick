
#ifndef DPM_AXIS_HH
#define DPM_AXIS_HH

#include <stdexcept>
#include <vector>
#include <libxml/tree.h>
#include "DPM/DPM_component.hh"
#include "DPM/DPM_var.hh"

enum DPM_axis_type { DPM_AXIS_NONE,
                     DPM_AXIS_X,
                     DPM_AXIS_Y,
                     DPM_AXIS_Z
                   } ;

/**
 * @brief Represents one axis of an DPM_relation.
 * @author John M. Penn
 */
/**
 * DPM_axis is an DPM_component which represents one axis of an DPM_relation.
 * It consists of:
 * - A label (optional).
 * - A specification of the units.
 * - A list of variables names.
 * - attributes (as a result of also being an DPM_component).
 */
class DPM_axis : public DPM_component {

public:
  /**
   * XML based constructor.
   * @param axis_node this is a pointer to an xmlNode whose type is
   *        XML_ELEMENT_NODE and whose name is either "xaxis" or "yaxis".
   */
  DPM_axis(DPM_component *parent, xmlNode *axis_node) ;

  /**
   * Destructor.
   */
  ~DPM_axis();

  /**
   * Return a pointer to the label if present, Null otherwise;
   */
  const char *getLabel();

  /**
   * Return a pointer to the units specification.
   */
  const char *getUnits();

  /**
   * Output an XML representation of an DPM_axis object.
   * @param s is the std::ostream to which this function writes.
   * @param axis is a pointer to the DPM_axis object whose representation
   * is to be written.
   */
  friend std::ostream& operator<< (std::ostream& s, const DPM_axis *axis);

private:
  char*  label;
  char*  unitspec;
  DPM_axis_type axis_type;

  int Initialize(xmlNode *base_node);
};
#endif
