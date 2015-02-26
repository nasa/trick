/** 
 @file
 $Id: RefParseContext.cpp 2977 2013-05-13 20:20:08Z alin $
*/
#include <stdio.h>
#include <stdlib.h>
#include "sim_services/MemoryManager/include/RefParseContext.hh"

/** 
 Constructor 
 */ RefParseContext::RefParseContext( Trick::MemoryManager *in_mem_mgr, std::istream* in_is = &std::cin ) {

   // Set verif input and echo input flags.
   verify_input = 0;
   echo_input = 1;
   reference_attr = NULL ;

   // Initialize the scanner
   init_scanner();

   this->is = in_is;
   this->mem_mgr = in_mem_mgr;

}

/** 
 Destructor 
 */
RefParseContext::~RefParseContext() {
    destroy_scanner();
}

