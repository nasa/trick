
#ifndef DPM_INPUTS_HH
#define DPM_INPUTS_HH

#include <stdexcept>
#include <string>
#include <vector>
#include <libxml/tree.h>
#include "DPM/DPM_var.hh"
#include "DPM/DPM_component.hh"

/**
 * DPM_inputs is an DPM_component which represents a list of variable names
 * required for an external function.
 * @author John M. Penn
 */
class DPM_inputs : public DPM_component {

public:

  /**
   * Constructor.
   */
  DPM_inputs(DPM_component *parent, xmlNode *inputs_node) ;

  /**
   * Destructor.
   */
  ~DPM_inputs();

  /**
   * Return the number of variables associated with these DPM_inputs.
   */
  int NumberOfInputs();

  /**
   * Return the variable name associated with the given index.
   * @param index Which of the variable names to return. Range is 0 ..
   * NumberOfVariables - 1. If index is outside of this range, NULL will be returned.
   */
  const char *getVarName(unsigned int index);

  /**
   * Output an XML representation of an DPM_inputs object.
   * @param s is the std::ostream to which this function writes.
   * @param inputs is a pointer to the DPM_inputs object whose representation
   * is to be written.
   */
  friend std::ostream& operator<< (std::ostream& s, const DPM_inputs *inputs);

private:
  std::vector <DPM_var *> var_list;
  int Initialize( xmlNode *inputs_elem_node);
};
#endif
