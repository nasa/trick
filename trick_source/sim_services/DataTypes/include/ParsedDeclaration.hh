#ifndef PARSED_DECLARATION_HH
#define PARSED_DECLARATION_HH

#include <string>
#include <iostream>
#include "LexicalAnalyzer.hh"

#define MAX_DIMS 16

class ParsedDeclaration {

    public:

    ParsedDeclaration( std::string s );
    std::string getTypeSpecifier();
    std::string getVariableName();
    unsigned int getNumberOfDimensions();
    int getDimensionSize( unsigned int index);

    private:
    bool parseTypeSpecifier();
    bool parseDeclaration();
    bool parseDeclarator();
    bool parseDirectDeclarator();

    bool match(Token::e expected_token);

    Token::e nextToken;
    LexicalAnalyzer lexer;

    std::string typeSpec;
    std::string varName;
    unsigned int ndims;
    int dims[MAX_DIMS];

};

#endif
