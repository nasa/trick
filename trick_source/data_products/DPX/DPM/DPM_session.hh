
#ifndef DPM_SESSION_HH
#define DPM_SESSION_HH

#include <stdexcept>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "DPM/DPM_component.hh"
#include "DPM/DPM_run.hh"
#include "DPM_time_constraints.hh"

/**
 * This class represents a description of a Trick DPX Session file.
 * @author John M. Penn
 */
class DPM_session : public DPM_component {

public:

  enum Device {TERMINAL, PRINTER, FILE};

  DPM_time_constraints *time_constraints;
  std::vector <DPM_run *> run_list;
  std::vector <char *> product_file_list;

  /**
   * XML Constructor.
   */
  DPM_session(DPM_component *parent, xmlNode *SessionNode) ;

  /**
   * Destructor
   */
  ~DPM_session();

  /**
   * Output an XML representation of an DPM_session object.
   * @param s is the std::ostream to which this function writes.
   * @param session is a pointer to the DPM_session object whose representation
   * is to be written.
   */
  friend std::ostream& operator<< (std::ostream& s, const DPM_session *session);

  int setDevice(char* device);
  Device getDevice();
  char* getDeviceFileName();


private:
  int Initialize(xmlNode *SessionNode);

  Device sessionDevice;
  char* deviceFileName;
};
#endif
