/*
 * $Id: MonteVarExecCode.cpp $
 */
#include <sstream>
#include <iomanip>

#include "trick/MonteVarExecCode.hh"

Trick::MonteVarExecCode::MonteVarExecCode(std::string in_name) {
    this->name = in_name;
}

std::string Trick::MonteVarExecCode::describe_variable()
{
    std::stringstream ss;

    ss << "#NAME:\t\t" << this->name << "\n"
       << "#TYPE:\t\tExecCode\n" 
       << "#UNIT:\t\tNone" << "\n"
       << "#VALUE:\t\tNone" << "\n";

    return ss.str();
}

std::string Trick::MonteVarExecCode::get_next_value() {
    std::ostringstream string_stream;
    string_stream << "execfile('" << name << "')";
    return string_stream.str();
}
