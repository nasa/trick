/*
 * $Id: MonteVarFixed.cpp $
 */
#include <sstream>

#include "sim_services/MonteCarlo/include/MonteVarFixed.hh"

Trick::MonteVarFixed::MonteVarFixed(std::string in_name, double in_value, std::string in_unit) {
    this->name = in_name;
    std::ostringstream string_stream;
    string_stream << in_value ;
    this->value = string_stream.str();
    this->unit = in_unit;
}

std::string Trick::MonteVarFixed::get_next_value() {
    std::ostringstream string_stream;
    if (unit.empty()) {
        string_stream << name << " = " << value;
    }
    else {
        string_stream << name << " = trick.attach_units(\"" << unit << "\", " << value << ")";
    }
    return string_stream.str();
}
