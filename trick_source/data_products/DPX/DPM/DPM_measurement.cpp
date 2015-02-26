/*
 * $Id: DPM_measurement.cpp 1842 2011-08-01 00:02:00Z penn $
 */

#include "DPM_measurement.hh"
#include "../../trick_utils/units/include/Unit.hh"

// MEMEBR FUNCTION
int DPM_measurement::Initialize( xmlNode *base_node) {

    if (base_node == NULL) {
         std::cerr << "ERROR: DPM_measurement::Initialize: Bad parameters." << std::endl;
         return -1;
    }
    if ( strcmp( (char *)base_node->name, "measurement") != 0) {
       std::cerr << "ERROR: Expected a <measurement> specification but alas it wasn\'t found." << std::endl;
       return -1;
    }

    /* Process children */
    xmlNode *current_node = base_node->children;

    while (current_node != NULL) {

        if (current_node->type == XML_ELEMENT_NODE) {
            if ( strcmp( (const char *)current_node->name, "var") == 0) {

            // Note that for this "var" element we are ignoring any attributes.
            // So we don't create an DPM_var. We just grab the content part.

            xmlNode *text_node = current_node->children;
            var = strdup( (const char *)text_node->content);

            } else if ( strcmp( (const char *)current_node->name, "units") == 0) {

                xmlNode *text_node = current_node->children;
                unitspec = strdup( (const char *)text_node->content);

            } else {
                std::cerr << "WARNING: <measurement> specification contains an unknown element \"<" <<
                        current_node->name << ">\". Skipping." << std::endl;
            }
        }
        current_node = current_node->next;
    }

    // Validate measurement
    // 1. Measurement requires one var.
    // 2. Measurement requires one units.
    if (var == NULL) {
        std::cerr << "ERROR: <measurement> specification is missing a <var> specification." << std::endl;
        return -1;
    }
    if (unitspec == NULL) {
        std::cerr << "ERROR: <measurement> specification is missing a <units> specification." << std::endl;
        return -1;
    } else {
        // Validate the units.
        Unit* validation_unit;
        try {
            validation_unit = new Unit(unitspec);
        } catch (Unit::CONVERSION_ERROR) {
            std::cerr << "ERROR: <measurement> specification contains an invalid <units> specification: \"" << unitspec << "\"." << std::endl;
            return -1;
        }
        delete validation_unit;
    }
    return 0;
}

// CONSTRUCTOR
DPM_measurement::DPM_measurement(DPM_component *Parent, xmlNode *Base_node) throw (std::invalid_argument)
     : DPM_component (Parent, Base_node) {

     if ( Initialize( Base_node) < 0) {
         throw std::invalid_argument("<measurement> specification is invalid and un-usable.");
     }
}

// DESTRUCTOR
DPM_measurement::~DPM_measurement() {

  if (var) { free(var); }
  if (unitspec) { free(unitspec); }
}

// MEMEBR FUNCTION
const char *DPM_measurement::getVarName() {
    return var;
}

// MEMEBR FUNCTION
const char *DPM_measurement::getUnits() {
    return unitspec;
}

// MEMEBR FUNCTION
std::ostream& operator<< (std::ostream& s, const DPM_measurement *measurement) {
     int i;
     int n = (int)measurement->attribute_list.size();

     s << "<measurement";
     for (i=0 ; i<n ; i++) {
          s << " " << measurement->attribute_list[i];
     }
     s << ">";
     s << std::endl;
     s << measurement->var;
     s << "<units>" << measurement->unitspec << "</units>" << std::endl;
     s << "</measurement>";
     s << std::endl;
     return s;
}
