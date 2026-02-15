#include "Lexer.h"
#include <cctype>
#include <map>

Lexer::Lexer(const std::string& source) : src(source), pos(0), line(1), col(1) {}

std::vector<Token> Lexer::Tokenize() {
    std::vector<Token> tokens;
    while (pos < src.length()) {
        SkipWhitespace();
        if (pos >= src.length()) break;
        
        Token token = ScanToken();
        if (token.type != TokenType::UNKNOWN) {
            tokens.push_back(token);
        }
    }
    tokens.push_back({TokenType::END_OF_FILE, "", line, col, 0});
    return tokens;
}

char Lexer::Peek(int offset) const {
    if (pos + offset >= src.length()) return '\0';
    return src[pos + offset];
}

char Lexer::Advance() {
    char c = src[pos++];
    if (c == '\n') {
        line++;
        col = 1;
    } else {
        col++;
    }
    return c;
}

void Lexer::SkipWhitespace() {
    while (true) {
        char c = Peek();
        if (std::isspace(c)) {
            Advance();
        } else if (c == '/') {
            if (Peek(1) == '/') {
                // Single line comment
                while (Peek() != '\n' && Peek() != '\0') Advance();
            } else if (Peek(1) == '*') {
                // Multi line comment
                Advance(); Advance(); // Eat /*
                while (!(Peek() == '*' && Peek(1) == '/') && Peek() != '\0') {
                    Advance();
                }
                if (Peek() != '\0') {
                    Advance(); Advance(); // Eat */
                }
            } else {
                return;
            }
        } else {
            return;
        }
    }
}

Token Lexer::ScanToken() {
    char c = Peek();
    int startCol = col; // Capture starting column
    
    // Identifiers & Keywords
    if (std::isalpha(c) || c == '_') return Identifier();
    
    // Numbers
    if (std::isdigit(c)) return Number(); // Handle negative numbers differently (operator)
    
    // String
    if (c == '"') return String();
    
    Advance(); // Consume the character
    
    switch (c) {
        case '(': return {TokenType::LPAREN, "(", line, startCol};
        case ')': return {TokenType::RPAREN, ")", line, startCol};
        case '{': return {TokenType::LBRACE, "{", line, startCol};
        case '}': return {TokenType::RBRACE, "}", line, startCol};
        case '[': return {TokenType::LBRACKET, "[", line, startCol};
        case ']': return {TokenType::RBRACKET, "]", line, startCol};
        case ';': return {TokenType::SEMICOLON, ";", line, startCol};
        case ',': return {TokenType::COMMA, ",", line, startCol};
        
        case '+': return {TokenType::PLUS, "+", line, startCol};
        case '-': return {TokenType::MINUS, "-", line, startCol}; // Unary handled in parser
        case '*': return {TokenType::STAR, "*", line, startCol};
        case '/': return {TokenType::SLASH, "/", line, startCol};
        case '%': return {TokenType::PERCENT, "%", line, startCol};
        case '^': return {TokenType::CARET, "^", line, startCol};
        
        case '&': return {TokenType::AMPERSAND, "&", line, startCol};
        case '|': return {TokenType::PIPE, "|", line, startCol};
        
        case '=':
            if (Peek() == '=') { Advance(); return {TokenType::EQ, "==", line, startCol}; }
            return {TokenType::ASSIGN, "=", line, startCol};
        
        case '!':
            if (Peek() == '=') { Advance(); return {TokenType::NEQ, "!=", line, startCol}; }
            return {TokenType::BANG, "!", line, startCol};

        case '<':
            if (Peek() == '=') { Advance(); return {TokenType::LTE, "<=", line, startCol}; }
            if (Peek() == '<') { Advance(); return {TokenType::LSL, "<<", line, startCol}; }
            return {TokenType::LT, "<", line, startCol};
            
        case '>':
            if (Peek() == '=') { Advance(); return {TokenType::GTE, ">=", line, startCol}; }
            if (Peek() == '>') { Advance(); return {TokenType::LSR, ">>", line, startCol}; }
            return {TokenType::GT, ">", line, startCol};
            
        default:
            return {TokenType::UNKNOWN, std::string(1, c), line, startCol};
    }
}

Token Lexer::String() {
    Advance(); // Eat opening "
    std::string text = "";
    while (Peek() != '"' && Peek() != '\0') {
        text += Advance();
    }
    if (Peek() == '"') Advance(); // Eat closing "
    return {TokenType::STRING, text, line, col};
}

Token Lexer::Number() {
    std::string text = "";
    while (std::isdigit(Peek())) {
        text += Advance();
    }
    return {TokenType::NUMBER, text, line, col, std::stoll(text)};
}

Token Lexer::Identifier() {
    std::string text = "";
    while (std::isalnum(Peek()) || Peek() == '_') {
        text += Advance();
    }
    
    static std::map<std::string, TokenType> keywords = {
        {"int", TokenType::KW_INT},
        {"void", TokenType::KW_VOID},
        {"if", TokenType::KW_IF},
        {"else", TokenType::KW_ELSE},
        {"while", TokenType::KW_WHILE},
        {"return", TokenType::KW_RETURN},
        {"asm", TokenType::KW_ASM},
        {"const", TokenType::KW_CONST}
    };
    
    if (keywords.count(text)) {
        return {keywords[text], text, line, col};
    }
    return {TokenType::IDENTIFIER, text, line, col};
}
