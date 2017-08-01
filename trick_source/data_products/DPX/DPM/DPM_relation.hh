
#ifndef DPM_RELATION_HH
#define DPM_RELATION_HH

#include <stdexcept>
#include <vector>
#include <libxml/tree.h>
#include "DPM/DPM_component.hh"
#include "DPM/DPM_axis.hh"
#include "DPM/DPM_curve.hh"
#include "DPM/DPM_time_constraints.hh"

/**
 * This class represents a relation of one variable (on the X-axis) with one or
 * more other variables (on the Y-axis). This class can used to represent a
 * plot or a table.
 * @author John M. Penn
 */
class DPM_relation : public DPM_component {

public:
  /**
   * Constructor for DPM_relation.
   * @param plot_node a pointer to a libxml2 xmlNode of type XML_ELEMENT_NODE,
   * and whose name is "plot".
   */
  DPM_relation(DPM_component *parent, xmlNode *plot_node) ;
  /**
   * Destructor for DPM_relation.
   */
  ~DPM_relation();

  /**
   * Set the title of the relation
   */
  int setTitle(const char *in_title);

  /**
   * Return the title of the relation, if it has one, otherwise return NULL.
   */
  const char *getTitle();

  /**
   * Return a pointer to the DPM_time_constraints object.
   */
  DPM_time_constraints *getTimeConstraints();

  /**
   * Return the number of axes associated with this relation. Will be either 2 or 3.
   */
  int NumberOfAxes();

  /**
   *
   */
  const char * getXAxisLabel();
  const char * getYAxisLabel();
  const char * getZAxisLabel();

  /**
   *
   */
  const char * getXAxisUnits();
  const char * getYAxisUnits();
  const char * getZAxisUnits();

  const char * getYAxisFormat();

  /**
   *
   */
  int NumberOfCurves();

  /**
   *
   */
  DPM_curve *getCurve(unsigned int index);

  /**
   * Output an xml representation of DPM_relation.
   * @param s std::ostream to be written.
   * @param relation pointer to the DPM_relation object to be written.
   */
  friend std::ostream& operator<< (std::ostream& s, const DPM_relation *relation);

private:
  char  *title;
  DPM_time_constraints *time_constraints;
  DPM_axis *xaxis;
  DPM_axis *yaxis;
  DPM_axis *zaxis;
  std::vector <DPM_curve *> curve_list;

  int Initialize(xmlNode *relation_elem_node);
};
#endif
