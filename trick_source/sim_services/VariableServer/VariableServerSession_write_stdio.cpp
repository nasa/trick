#include <sstream>

#include "trick/VariableServerSession.hh"
#include "trick/tc_proto.h"
#include "trick/variable_server_message_types.h"

int Trick::VariableServerSession::write_stdio(int stream, std::string text)
{
    std::stringstream outstream;

    outstream << VS_STDIO << " " << stream << " " << (int)text.length() << "\n";
    outstream << text;

    _connection->write(outstream.str());

    return 0;
}
