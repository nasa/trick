
#ifndef GP_UTILITIES_HH
#define GP_UTILITIES_HH

#include "gp_view_page_node.hh"

using namespace std;

int filterPostScriptBW( const char *psName );
//int filterPostScriptColor( const char *psName, const char *backgroundColor_,
//                           const char *foregroundColor_ );
int filterPostScriptColor( const char *psName, GPViewPageNode* gp_view_page_node );
int printPS( std::string psName );
int printPS( const char *psName );

#endif
