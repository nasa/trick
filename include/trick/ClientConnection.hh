/************************************************************************
PURPOSE: (Abstract base class for a connection to a client. Should
be used by variable server and web server connections. )
LIBRARY DEPENDENCIES:
    () )
**************************************************************************/

#include <iostream>
#include <string>

class ClientConnection {
    
    virtual int initialize() = 0;
    virtual int write (std::string& message) = 0;
    virtual int read  (std::string& message) = 0;

};
