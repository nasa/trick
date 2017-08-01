
#ifndef DPM_RUN_HH
#define DPM_RUN_HH

#include <stdexcept>
#include <vector>
#include <iostream>
#include <libxml/tree.h>
#include "DPM/DPM_component.hh"

/**
 * This class represents a particular RUN directory.
 * @author John M. Penn
 */
class DPM_run : public DPM_component {

public:
  /**
   * Constructor.
   * @param RunNode is a pointer to an xmlNode whose type is
   *        XML_ELEMENT_NODE and whose name is "run".
   */
  DPM_run(DPM_component *parent, xmlNode *RunNode) ;

  /**
   * Destructor.
   */
  ~DPM_run();

  /**
   * @return the name of the RUN directory.
   */
  const char* getDir();

  /**
   * @return the name of the time variable for this RUN.
   */
  const char* getTimeName();

  /**
   * Output an xml representation of DPM_run.
   * @param s std::ostream to be written.
   * @param run pointer to the DPM_run object to be written.
   */
  friend std::ostream& operator<< (std::ostream& s, const DPM_run *run);

private:
  char * dir;
  const char * time_name;
  int Initialize( xmlNode *var_elem_node);
};
#endif
