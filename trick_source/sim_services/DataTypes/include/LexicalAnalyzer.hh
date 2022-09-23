#ifndef LEXICAL_ANALYZER_HH
#define LEXICAL_ANALYZER_HH

#include <string>
#include <sstream>

namespace Token {
    enum e {
        Error        = 0,
        EndOfFile    = 1,
        Identifier   = 2,
        Void         = 3,
        Boolean      = 4,
        Char         = 5,
        Short        = 6,
        Int          = 7,
        Long         = 8,
        WideChar     = 9,
        Signed       = 10,
        Unsigned     = 11,
        Float        = 12,
        Double       = 13,
        LeftParen    = 14,
        RightParen   = 15,
        LeftBracket  = 16,
        RightBracket = 17,
        Asterisk     = 18,
        Integer      = 19
    };
};

class LexicalAnalyzer {

    public:
    LexicalAnalyzer();
    void load( const std::string& s );
    Token::e getToken();
    Token::e nextToken();
    Token::e matchToken(Token::e expectedToken );
    std::string getText();
    Token::e token;

    private:
    char nextChar;
    std::stringstream inStream;
    std::string lexemeText;
};

#endif
