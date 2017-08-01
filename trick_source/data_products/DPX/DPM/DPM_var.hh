
#ifndef DPM_VAR_HH
#define DPM_VAR_HH

#include <stdexcept>
#include <vector>
#include <iostream>
#include <libxml/tree.h>
#include "DPM/DPM_component.hh"

/**
 * This class represents a variable and it's associated attributes.
 * @author John M. Penn
 */
class DPM_var:public DPM_component {

    public:

    /**
     * Constructor.
     * @param name Name of the variable.
     */
    DPM_var(const char *name);

    /**
     * Constructor.
     * @param VarNode is a pointer to an xmlNode whose type is
     *        XML_ELEMENT_NODE and whose name is "var".
     */
    DPM_var(DPM_component * parent, xmlNode * VarNode) ;

    /**
     * Destructor.
     */
    ~DPM_var();

    /**
     * Return the name of the variable.
     */
    const char     *getName();

    /**
     * Return the part of the variable name right of the last
     * period or the name if there are no periods.
     */
    const char     *getShortName();

    /**
     * Output an xml representation of DPM_var.
     * @param s std::ostream to be written.
     * @param var pointer to the DPM_var object to be written.
     */
    friend std::ostream & operator << (std::ostream & s, const DPM_var * var);

    private:
    char           *name;
    int             Initialize(xmlNode * var_elem_node);
};
#endif
