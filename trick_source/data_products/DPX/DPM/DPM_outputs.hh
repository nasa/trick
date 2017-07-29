
#ifndef DPM_OUTPUTS_HH
#define DPM_OUTPUTS_HH

#include <stdexcept>
#include <string>
#include <vector>
#include <libxml/tree.h>
#include "DPM/DPM_component.hh"
#include "DPM/DPM_measurement.hh"

/**
 * DPM_outputs is an DPM_component which represents a list of outputs produced
 * by an external function. Each output is represented by an DPM_measurement
 * which consists of a name and a units specifier.
 * @author John M. Penn
 */
class DPM_outputs : public DPM_component {

public:
  /**
   * XML based constructor.
   */
  DPM_outputs(DPM_component *parent, xmlNode *outputs_node) ;

  /**
   * Destructor.
   */
  ~DPM_outputs();

  /**
   * Get the number of outputs contained in this DPM_outputs object.
   */
  int NumberOfOutputs();

  /**
   * Get the varible name at the given index.
   */
  const char *getVarName( unsigned int index);

  /**
   * Get the units specification at the given index.
   */
  const char *getUnits( unsigned int index);

  /**
   * Output an XML representation of an DPM_outputs object.
   * @param s is the std::ostream to which this function writes.
   * @param outputs is a pointer to the DPM_outputs object whose representation
   * is to be written.
   */
  friend std::ostream& operator<< (std::ostream& s, const DPM_outputs *outputs);

private:
  std::vector <DPM_measurement *> measurement_list;

  int Initialize(xmlNode *outputs_elem_node);
};
#endif
