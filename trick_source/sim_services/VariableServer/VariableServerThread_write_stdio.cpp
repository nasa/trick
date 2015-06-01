
#include "trick/VariableServer.hh"
#include "trick/variable_server_message_types.h"
#include "trick/tc_proto.h"

int Trick::VariableServerThread::write_stdio(int stream, std::string text) {

    char header[16] ;
    sprintf(header, "%-2d %1d %8d\n" , VS_STDIO, stream , (int)text.length()) ;
    tc_write(&connection , (char *) header , strlen(header)) ;
    tc_write(&connection , (char *) text.c_str() , text.length()) ;
    return 0 ;
}
