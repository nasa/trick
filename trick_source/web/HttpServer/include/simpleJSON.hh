/*************************************************************************
PURPOSE: (Represent Websocket variable server connection.)
LIBRARY DEPENDENCIES:
    ( (../src/WSSession.o))
**************************************************************************/
#ifndef SIMPLEJSON_HH
#define SIMPLEJSON_HH

#include <vector>

class Member {
    public:
    const char* key;
    const char* valText;
    int type;
    Member(const char *k, const char *v, int t);
};

std::vector<Member*> parseJSON( const char *json_s);

#endif
