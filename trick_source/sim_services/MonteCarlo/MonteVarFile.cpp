#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <limits>

#include "trick/MonteVarFile.hh"
#include "trick/message_proto.h"
#include "trick/exec_proto.h"

Trick::MonteVarFile::MonteVarFile(std::string in_name, std::string in_file_name, unsigned int in_column, std::string in_unit) {
    this->name = in_name;
    this->file_name = in_file_name;
    this->column = in_column;
    this->unit = in_unit;

    input_file_stream = new std::ifstream(file_name.c_str(), std::ifstream::in);
    if (input_file_stream->fail()) { 
        char string[100];
        sprintf(string, "Trick:MonteVarFile the input file \"%s\" failed to open", file_name.c_str());
        exec_terminate_with_return(-1, __FILE__, __LINE__, string);
   }
    buffer = new char[4096];
}

std::string Trick::MonteVarFile::get_next_value() {
    if (input_file_stream->good()) {
        double file_value;
        char *current_position = buffer;
        do {
            input_file_stream->getline(buffer, 4096);
            file_value = strtold(buffer, &current_position);
            if (input_file_stream->eof()) {
                input_file_stream->close();
                return "EOF";
            }
        } while (file_value == 0 && current_position == buffer && input_file_stream->good());

        //  Count the number of columns in the input file
        char* token;
        unsigned int ntokens = 0;
        char temp_str[4096];
        strcpy(temp_str, buffer) ;
        token = strtok( temp_str, " \t" );
        while ( token != NULL ) {
            token = strtok( NULL, " \t" );
            ntokens++;
        }

        // Verify the input column number is valid 
        if ( (column == 0) || (column > ntokens) ) {
            char string[100];
            sprintf(string, "Trick:MonteVarFile An invalid column number %d, valid column numbers are 1 - %d", column, ntokens);
            exec_terminate_with_return(-1, __FILE__, __LINE__, string);
        }

        if (current_position != buffer) {
            for (unsigned int i = 1; i < column; ++i) {
                file_value = strtold(current_position, &current_position);
            }

            std::stringstream string_stream;
            string_stream.precision(std::numeric_limits<double>::digits10);
            string_stream << file_value ;
            value = string_stream.str();
            string_stream.str("");
            if (unit.empty()) {
                string_stream << name << " = " << file_value ;
            }
            else {
                string_stream << name << " = " << "trick.attach_units(\"" << unit << "\", " << file_value
                  << ")";
            }
            return string_stream.str() ;
        }
    }
    char string[100];
    sprintf(string, "Trick:MonteVarFile the input file \"%s\" is not open for reading", file_name.c_str());
    exec_terminate_with_return(-1, __FILE__, __LINE__, string);

    return NULL;
}
