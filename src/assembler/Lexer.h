#pragma once
#include <string>
#include <vector>
#include <iostream>

enum class TokenType {
    IDENTIFIER, // Labels, Mnemonics
    NUMBER,     // 123, -45, 0xFF
    REGISTER,   // R0-R15, PC, SP
    // Syntax
    COLON,      // :
    DOT,        // .
    LBRACKET,   // [
    RBRACKET,   // ]
    // COMMA removed (Strict syntax: whitespace only)
    
    // Directives
    DIR_ORG,    // .org
    DIR_DAT,    // .dat (Legacy, maybe keep for data?) -> .word in new spec
    DIR_WORD,   // .word
    DIR_SECTION,// .section
    DIR_GLOBAL, // .global
    
    NEWLINE,    // \n
    END_OF_FILE
};

struct Token {
    TokenType type = TokenType::END_OF_FILE;
    std::string text = "";
    int64_t intValue = 0; // For Numbers/Registers
    int line = 0;
    int column = 0;
};

class Lexer {
public:
    Lexer(const std::string& source);
    Token NextToken();
    Token PeekToken(); // Lookahead

private:
    std::string src;
    size_t pos;
    int line;
    int col;

    char Current();
    void Advance();
    void SkipWhitespace();
    
    Token TokenizeIdentifier();
    Token TokenizeNumber();
};
