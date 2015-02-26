#include "Requirement.hh"
#include <algorithm>
#include <functional>
#include <string.h>

int Requirement::Initialize(xmlNode *root_node) { 

    int rc = 0;
    if (strcmp((char*) root_node->name, "requirement") == 0) {

        /* Process children */
        xmlNode *current_node = root_node->children;

        while (current_node != NULL) {

            if (current_node->type == XML_ELEMENT_NODE) {

                if ( strcmp( (char *)current_node->name, "requirement") == 0) {
                    try {

                        Requirement* requirement = new Requirement(current_node);
                        derivedRequirements.push_back(requirement);

                    } catch (std::invalid_argument) {
                        std::cerr << "ERROR: <requirement> specification." << std::endl;
                    }
                } else {
                    std::cerr << "WARNING: unknown element \"<" << current_node->name << ">\". Skipping." << std::endl;
                }
            }
            current_node = current_node->next;
        }
    } else {
        std::cerr << "ERROR: Expected <requirement> component but alas it wasn\'t found." << std::endl;
        rc = -1;
    }
    return rc;
}

// CONSTRUCTOR
Requirement::Requirement(xmlNode *Base_node) throw (std::invalid_argument)
    : XMLComponent(Base_node) {

    if ( Initialize( Base_node) < 0) {
        throw std::invalid_argument("Requirement is un-usable.");
    }
}


// DESTRUCTOR
Requirement::~Requirement() {
}

unsigned int Requirement::derivedRequirementsCount() {
    return (unsigned int)derivedRequirements.size();
}

unsigned int hash( const char * s ) {
   unsigned int h = 31;
   while (*s) {
       h = h * 101 + (unsigned int)*s;
       s++;
   }
   return h;
}


// Strip all of the extraneous white space from a string.
std::string normalize( std::string& s) {

    int state = 0;
    std::string result_s;

    for (std::string::iterator ii = s.begin() ; ii < s.end() ; ii++) {

       if (state == 0) {
           if (!isspace(*ii)) {
               result_s += *ii;
               state = 1;
           }
       } else {
           if (isspace(*ii)) {
              result_s += ' ';
              state = 0;
           } else {
              result_s += *ii;
           }
       }
    }
    return result_s;
}

void Requirement::writeAttribute( std::string& v, const char* k) {

    std::string out_v;
    std::cout << " " << k << " = \"" ;
    for (std::string::iterator ii = v.begin() ; ii < v.end() ; ii++) {
        if (*ii == '"' ) { 
            out_v += "&quot;";
        } else if (*ii == '\'') {
            out_v += "&apos;";
        } else {
            out_v += *ii;
        }   
    }   
    std::cout << out_v << "\"" ;
}

/*
void Requirement::writeStatement( std::string& s) {

    std::string out_s;
    std::cout << " statement = \"" ;
    for (std::string::iterator ii = s.begin() ; ii < s.end() ; ii++) {
        if (*ii == '"' ) {
            out_s += "&quot;";
        } else if (*ii == '\'') {
            out_s += "&apos;";
        } else {
            out_s += *ii;
        }
    }
    std::cout << out_s << "\"" ;
}
*/

void Requirement::writeTagged(int indent) {

    for (int ii = 0; ii < indent; ii++) { std::cout << " "; }

    std::cout << "<requirement";

    std::string statement = AttributeValue("statement");

    std::string normalized_statement = normalize(statement);
    writeAttribute(normalized_statement, "statement");

	std::string note = AttributeValue("note");
    if(note[0] != '\0') {
        std::string normalized_note = normalize(note);
        writeAttribute(normalized_note, "note");
    }

    std::string verify = AttributeValue("verify");
    if(verify[0] != '\0') {
        std::string normalized_verify = normalize(verify);
        writeAttribute(normalized_verify, "verify");
    }

    unsigned int tag = hash(normalized_statement.c_str());
    std::cout << " tag = \"" << tag << "\"";

    unsigned int numberOfDerivedRequirements = derivedRequirementsCount();

    if (numberOfDerivedRequirements == 0) {
        std::cout << "/>" << std::endl;
    } else {
        std::cout << ">" << std::endl;
        for (unsigned int ii = 0; ii < numberOfDerivedRequirements; ii++) {
            Requirement* dreq = derivedRequirements[ii];
            dreq->writeTagged(indent+3);
        }

        for (int ii = 0; ii < indent; ii++) { std::cout << " "; }
        std::cout << "</requirement>" << std::endl;
    }
}

