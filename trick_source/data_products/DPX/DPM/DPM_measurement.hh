
#ifndef DPM_MEASUREMENT_HH
#define DPM_MEASUREMENT_HH

#include <stdexcept>
#include <vector>
#include <iostream>
#include <libxml/tree.h>
#include "DPM/DPM_component.hh"

/**
 * DPM_measurement is a variable with units.
 * @author John M. Penn
 */
class DPM_measurement : public DPM_component {

public:
  /**
   * Constructor.
   * @param measurement_node this is a pointer to an xmlNode whose type is
   *        XML_ELEMENT_NODE and whose name is "measurement".
   */
  DPM_measurement(DPM_component *parent, xmlNode *measurement_node) ;

  /**
   * Destructor.
   */
  ~DPM_measurement();

  /**
   * Get the variable name for this measurement.
   */
  const char *getVarName();

  /**
   * Get the units specification for this measurement.
   */
  const char *getUnits();

  /**
   * Output an xml representation of DPM_var.
   * @param s std::ostream to be written.
   * @param measurement pointer to the DPM_var object to be written.
   */
  friend std::ostream& operator<< (std::ostream& s, const DPM_measurement *measurement);

private:
  char * var;
  char * unitspec;

  int Initialize( xmlNode *measurement_node);
};
#endif
