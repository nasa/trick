#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits>

#include "trick/MonteVarFile.hh"
#include "trick/message_proto.h"
#include "trick/exec_proto.h"

Trick::MonteVarFile::MonteVarFile(std::string in_name, std::string in_file_name, unsigned int in_column, std::string in_unit) : input_file_stream(NULL)
{
    this->name = in_name;
    this->column = in_column;
    this->unit = in_unit;

    set_file_name(in_file_name);
    buffer = new char[4096];
}

Trick::MonteVarFile::~MonteVarFile()
{
    delete input_file_stream;
    delete buffer;
}

// Composite the various properties of this MonteVarFile.
std::string Trick::MonteVarFile::describe_variable()
{
    std::stringstream ss;

    ss << "#NAME:\t\t" << this->name << "\n"
       << "#TYPE:\t\tFILE\n"
       << "#UNIT:\t\t" << this->unit << "\n"
       << "#FILE:\t\t" << this->file_name << "\n"
       << "#COLUMN:\t" << this->column << "\n";

    return ss.str();
}

std::string Trick::MonteVarFile::get_next_value()
{
    if (input_file_stream->good())
    {
        // Skip the comments and empty lines in the data file.
        do
        {
            input_file_stream->getline(buffer, 4096);

            if(input_file_stream->eof())
            {
                input_file_stream->close();
                return "EOF";
            }
        }
        while(buffer[0] == '#' || buffer[0] == '\0');

        // Count the number of columns in the input file.
        char *token;
        unsigned int ntokens = 0;
        char temp_str[4096];
        strcpy(temp_str, buffer);
        token = strtok(temp_str, " \t");
        while (token != NULL)
        {
            token = strtok(NULL, " \t");
            ntokens++;
        }

        // Verify the input column number is valid.
        if ((column == 0) || (column > ntokens))
        {
            char string[100];
            sprintf(string, "Trick:MonteVarFile An invalid column number %d, valid column numbers are 1 - %d", column, ntokens);
            exec_terminate_with_return(-1, __FILE__, __LINE__, string);
        }

        // Get the next value.
        strcpy(temp_str, buffer);
        token = strtok(temp_str, " \t");

        for(unsigned int i = 1; i < column; i++)
        {
            // Iterate through each token in the temp_str.
            if(token != NULL)
            {
                token = strtok(NULL, " \t");
            }
        }

        // Return the value as a string.
        this->value = token;
        std::stringstream ss;

        if(unit.empty())
        {
            ss << this->name << " = " << token;
        }
        else
        {
            ss << this->name << " = " << "trick.attach_units(\"" << unit << "\", " << token << ")";
        }

        return ss.str();
    }
    char string[100];
    sprintf(string, "Trick:MonteVarFile the input file \"%s\" is not open for reading", file_name.c_str());
    exec_terminate_with_return(-1, __FILE__, __LINE__, string);

    return NULL;
}

void Trick::MonteVarFile::set_file_name(std::string in_file_name)
{
    delete input_file_stream;

    input_file_stream = new std::ifstream(in_file_name.c_str(), std::ifstream::in);
    if (input_file_stream->fail())
    {
        std::stringstream string_stream;

        string_stream << "Error: " << strerror(errno) << std::endl
                      << "       Trick:MonteVarFile input file \"" << in_file_name << "\" failed to open";

        exec_terminate_with_return(-1, __FILE__, __LINE__, string_stream.str().c_str());
    }
    this->file_name = in_file_name;
}

void Trick::MonteVarFile::set_column(unsigned int in_column)
{
    this->column = in_column;
}
