
#ifndef DPM_CURVE_HH
#define DPM_CURVE_HH

#include <stdexcept>
#include <vector>
#include <libxml/tree.h>
#include "DPM/DPM_component.hh"
#include "DPM/DPM_var.hh"

/**
 * @author John M. Penn
 */
class DPM_curve : public DPM_component {

public:
  /**
   * Constructor for DPM_curve.
   * @param curve_node a pointer to a libxml2 xmlNode of type XML_ELEMENT_NODE,
   * and whose name is "curve".
   */
  DPM_curve(DPM_component *parent, xmlNode *curve_node) ;
  /**
   * Destructor for DPM_curve.
   */
  ~DPM_curve();

  /**
   * Return the number of axes associated with this DPM_curve.
   * Should be either 2 or 3.
   */
  int NumberOfAxes();

  /**
   *
   */
  int NumberOfVarCases();

  /**
   *
   */
  const char * getXVarName(unsigned int case_index);

  /**
   *
   */
  const char * getYVarName(unsigned int case_index);

  /**
   *
   */
  const char * getZVarName(unsigned int case_index);

  /**
   * Return a pointer to the DPM_var object at the given index.
   */
  DPM_var *getXVar(unsigned int index);

  /**
   * Return a pointer to the DPM_var object at the given index.
   */
  DPM_var *getYVar(unsigned int index);

  /**
   * Return a pointer to the DPM_var object at the given index.
   */
  DPM_var *getZVar(unsigned int index);

  /**
   *
   */
  const char * getXCommonName();

  /**
   *
   */
  const char * getYCommonName();

  /**
   *
   */
  const char * getZCommonName();

  /**
   * Output an xml representation of DPM_curve.
   * @param s std::ostream to be written.
   * @param curve pointer to the DPM_curve object to be written.
   */
  friend std::ostream& operator<< (std::ostream& s, const DPM_curve *curve);

private:
  int line_number;
  std::vector <DPM_var *> x_varcase_list;
  std::vector <DPM_var *> y_varcase_list;
  std::vector <DPM_var *> z_varcase_list;

  int Initialize(xmlNode *relation_elem_node);
};
#endif
