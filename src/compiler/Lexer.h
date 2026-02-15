#pragma once

#include <string>
#include <vector>
#include <iostream>
#include "../trit_word.h"

// Token Types for Helix C-lite
enum class TokenType {
    // End of File
    END_OF_FILE,

    // Literals
    NUMBER,     // 123, -45
    IDENTIFIER, // myVar, main
    STRING,     // "add r1, r2" (for asm)

    // Keywords
    KW_INT,     // int
    KW_VOID,    // void
    KW_IF,      // if
    KW_ELSE,    // else
    KW_WHILE,   // while
    KW_RETURN,  // return
    KW_ASM,     // asm
    KW_CONST,   // const

    // Operators
    PLUS,       // +
    MINUS,      // -
    STAR,       // *
    SLASH,      // /
    PERCENT,    // %
    AMPERSAND,  // & (Min)
    PIPE,       // | (Max)
    CARET,      // ^ (XOR)
    LSL,        // <<
    LSR,        // >>
    ASSIGN,     // =
    BANG,       // !
    
    // Comparison
    EQ,         // ==
    NEQ,        // !=
    LT,         // <
    GT,         // >
    LTE,        // <=
    GTE,        // >=
    
    // Symbols
    LPAREN,     // (
    RPAREN,     // )
    LBRACE,     // {
    RBRACE,     // }
    LBRACKET,   // [
    RBRACKET,   // ]
    SEMICOLON,  // ;
    COMMA,      // ,
    
    // Error
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string text;
    int line;
    int col;
    int64_t intValue; // For NUMBER

    std::string ToString() const {
        return "Token(" + std::to_string((int)type) + ", '" + text + "')";
    }
};

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> Tokenize();

private:
    std::string src;
    size_t pos;
    int line;
    int col;

    char Peek(int offset = 0) const;
    char Advance();
    bool Match(char expected);
    
    void SkipWhitespace();
    Token ScanToken();
    Token String();
    Token Number();
    Token Identifier();
};
