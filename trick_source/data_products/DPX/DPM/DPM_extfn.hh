
#ifndef DPM_EXTFN_HH
#define DPM_EXTFN_HH

#include <stdexcept>
#include <string>
#include <vector>
#include <libxml/tree.h>
#include "DPM/DPM_component.hh"
#include "DPM/DPM_inputs.hh"
#include "DPM/DPM_outputs.hh"

/**
 * This class represents an external function.
 * @author John M. Penn
 */
class DPM_extfn : public DPM_component {

public:

  /**
   * XML constructor.
   */
  DPM_extfn(DPM_component *parent, xmlNode *extfn_node) ;

  /**
   * Destructor.
   */
  ~DPM_extfn();

  /**
   * Return the name of the external function. This name is the name of
   * the shared object library (i.e., the .so).
   */
  const char *getFName();

  /**
   * Return the number of inputs that this external function requires.
   */
  int NumberOfInputs();

  /**
   * Return the input variable name at the specified index.
   * @param index this is a super duper index
   */
  const char *getInputVar( unsigned int index);

  /**
   * Return the number of outputs that this external function generates.
   */
  int NumberOfOutputs();

  /**
   * Return the output variable name at the specified index.
   */
  const char *getOutputVar( unsigned int index);

  /**
   * Return the output units specification at the specified index.
   */
  const char *getOutputUnits( unsigned int index);

  /**
   * Determine whether this DPM_extfn object provides the specified output.
   * If it does, return the index of the output whose var_name matches.
   * This may be in the range 0 .. NumberOfOutputs - 1. If an output matching
   * var_name is not found, return -1.
   */
  int index_of(const char* var_name);

  /**
   * Output an XML representation of an DPM_extfn object.
   */
  friend std::ostream& operator<< (std::ostream& s, const DPM_extfn *extfn);

private:
  char *fname;
  DPM_inputs  *inputs;
  DPM_outputs *outputs;

  int Initialize(xmlNode *extfn_elem_node);

};
#endif
