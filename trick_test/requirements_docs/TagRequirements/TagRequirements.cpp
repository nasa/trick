#include <iostream>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "Requirement.hh"

const char*usage_info[] = {
"--------------------------------------------------",
"",
"",
"",
"--------------------------------------------------",
};

#define N_USAGE_LINES (sizeof(usage_info)/sizeof(usage_info[0]))
void print_usage(const char* info[], int nlines) {
   int i;
   for (i=0; i<nlines; i++) {
        std::cout << info[i] << std::endl;
   }
}


int main(int argc, const char* argv[]) {

    if (argc <= 1) {
        print_usage(usage_info, N_USAGE_LINES);
        exit(0);
    }

    xmlParserCtxtPtr parser_context;
    xmlDocPtr  doc;
    xmlNode *root_node;

    int valid = 0;

    LIBXML_TEST_VERSION

    // Create a parser context.
    if ((parser_context = xmlNewParserCtxt()) == NULL) {
        std::cerr << "ERROR: Out of Memory" << std::endl;
        std::cerr.flush();
        exit(0);
    } else {
        doc = xmlCtxtReadFile( parser_context, argv[1], NULL, XML_PARSE_RECOVER );

        if (doc != NULL) {

           if (parser_context->valid == 0) {
               std::cerr << "WARNING: XML file \"" << argv[1] << "\"" << " is not valid according to the dtd." << std::endl;
           }
           root_node = xmlDocGetRootElement(doc);

           Requirement* requirement = new Requirement( root_node);
           std::cout << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
           std::cout << "<!DOCTYPE TopLevelReqs PUBLIC \"-//Tricklab//DTD Session V1.0//EN\" \"DTD/TopLevelReqs.dtd\">" << std::endl;
           std::cout << std::endl;

           requirement->writeTagged(0);

           xmlFreeDoc(doc);
        } else {
            std::cerr << "ERROR: Parse of XML file \"" << argv[1] << "\"" << " failed." << std::endl;
        }
    }
}

