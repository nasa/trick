/*
 * $Id: DPM_parse_tree.cpp 3242 2013-09-24 15:15:01Z alin $
 */

#include <DPM/DPM_parse_tree.hh>

int DPM_parse_tree::Initialize(const char *XMLFileName) { 

   int rc;
   xmlParserCtxtPtr parser_context;

   valid = 0;

   if (! XMLFileName) {
       std::cerr << "ERROR: XML file name is NULL." << std::endl;
       std::cerr.flush();
       rc = -1;
   } else {

       LIBXML_TEST_VERSION

       // Create a parser context.
       if ((parser_context = xmlNewParserCtxt()) == NULL) {
           std::cerr << "ERROR: Out of Memory" << std::endl;
           std::cerr.flush();
           exit(0);
       }

       doc = xmlCtxtReadFile( parser_context, XMLFileName, NULL, XML_PARSE_DTDVALID );

       if (doc == NULL) {
           std::cerr << "ERROR: Parse of XML file \"" << XMLFileName << "\"" << " failed." << std::endl;
           std::cerr.flush();
           rc = -1;
       } else if (parser_context->valid == 0) {
           std::cerr << "WARNING: Validation of XML file \"" << XMLFileName << "\"" << " failed." << std::endl;
           std::cerr.flush();
           rc = 0;
       } else {
           valid = 1;
           rc = 0;
       }
       xmlFreeParserCtxt(parser_context);
   }

   return rc;
}


// CONSTRUCTOR
DPM_parse_tree::DPM_parse_tree(const char *XMLFileName) throw (std::invalid_argument) {

    if ( Initialize( XMLFileName) < 0) {
        throw std::invalid_argument("DPM_parse_tree is un-usable.");
    }
}

// DESTRUCTOR
DPM_parse_tree::~DPM_parse_tree() {
    
   if (doc) {
       xmlFreeDoc(doc);
   }
}

// MEMBER FUNCTION
xmlNode *DPM_parse_tree::getRootNode() {

   return ( xmlDocGetRootElement(doc));
}

// MEMBER FUNCTION
int DPM_parse_tree::is_valid() {

   return ( valid);
}







