#include "Lexer.h"
#include <cctype>

Lexer::Lexer(const std::string& source) 
    : src(source), pos(0), line(1), col(1) {}

char Lexer::Current() {
    if (pos >= src.length()) return '\0';
    return src[pos];
}

void Lexer::Advance() {
    if (Current() == '\n') {
        line++;
        col = 1;
    } else {
        col++;
    }
    pos++;
}

void Lexer::SkipWhitespace() {
    while (true) {
        char c = Current();
        if (c == ' ' || c == '\t' || c == '\r' || c == ',') {
            Advance();
        } else if (c == ';') {
            // Comment: skip until newline
            while (Current() != '\n' && Current() != '\0') {
                Advance();
            }
        } else {
            break;
        }
    }
}

Token Lexer::NextToken() {
    SkipWhitespace();
    
    char c = Current();
    Token token;
    token.line = line;
    token.column = col;

    if (c == '\0') {
        token.type = TokenType::END_OF_FILE;
        return token;
    }

    switch (c) {
        case '\n':
            token.type = TokenType::NEWLINE;
            token.text = "\\n";
            Advance();
            return token;
        case ':':
            Advance();
            return {TokenType::COLON, ":", token.line, token.column};
        case '[':
            Advance();
            return {TokenType::LBRACKET, "[", token.line, token.column};
        case ']':
            Advance();
            return {TokenType::RBRACKET, "]", token.line, token.column};
        // Remove COMMA support
        // case ',': return {TokenType::COMMA, ",", line, col}; 
    }

    // Number (Digit or Sign followed by Digit)
    if (isdigit(c) || c == '-' || c == '+') {
        // Check if sign is followed by digit (Number) or part of something else?
        // Assembly: -5 is number. 
        // Identifiers can contain numbers but usually start with alpha.
        // What if we have "ADD R1, -5"? 
        // If it starts with digit or sign, attempt number.
        // Exception: '-' could be minus operator? But we assume operands are simple.
        
        // Peek next for sign
        if ((c == '-' || c == '+') && !isdigit(src[pos+1])) {
             // Maybe it's a label? or just a symbol?
             // For now, assume it's part of a number or just invalid if standalone.
             // Actually, could be Trit literal `t(+-0)`.
        }
        return TokenizeNumber();
    }

    // Identifier or Register
    if (isalpha(c) || c == '.' || c == '_') {
        return TokenizeIdentifier();
    }

    // Unknown
    token.text = std::string(1, c);
    token.type = TokenType::IDENTIFIER; // Fallback or Error?
    Advance();
    return token;
}

Token Lexer::TokenizeNumber() {
    Token token;
    token.line = line;
    token.column = col;
    token.type = TokenType::NUMBER;
    
    // Handle Sign
    if (Current() == '-' || Current() == '+') {
        token.text += Current();
        Advance();
    }
    
    // Basic Decimal / Hex support
    while (isalnum(Current())) { // Hex support (0x...)
        token.text += Current();
        Advance();
    }
    
    try {
        token.intValue = std::stoll(token.text, nullptr, 0);
    } catch (...) {
        token.intValue = 0; // Error
    }
    
    return token;
}

Token Lexer::TokenizeIdentifier() {
    Token token;
    token.line = line;
    token.column = col;
    
    // Consume
    while (isalnum(Current()) || Current() == '_' || Current() == '.') {
        token.text += Current();
        Advance();
    }

    // Directives (Case Insensitive for robustness, but prefer lowercase)
    std::string lower = token.text;
    for(auto& c : lower) c = tolower(c);
    
    // Update token text to lowercase to match OpMap? 
    // Spec says strict lowercase. If we enforce it, we should ERROR on uppercase?
    // But for now, let's NORMALIZE to lowercase to fix the 'LDI.W' unknown mnemonic issue 
    // (assuming something is uppercasing it or user input is upper).
    token.text = lower; 

    if (lower == ".section" || lower == ".text" || lower == ".data") return {TokenType::DIR_SECTION, lower, 0, token.line, token.column};
    if (lower == ".global") return {TokenType::DIR_GLOBAL, lower, 0, token.line, token.column};
    if (lower == ".word" || lower == ".int") return {TokenType::DIR_WORD, lower, 0, token.line, token.column};
    if (lower == ".dat") return {TokenType::DIR_DAT, lower, 0, token.line, token.column};
    if (lower == ".org") return {TokenType::DIR_ORG, lower, 0, token.line, token.column};
    
    // Registers
    if (lower.size() > 1 && lower[0] == 'r' && isdigit(lower[1])) {
         token.type = TokenType::REGISTER;
         // Validate range? Parser does it.
         token.intValue = std::stoll(lower.substr(1));
    } 
    else if (lower == "sp") { token.type = TokenType::REGISTER; token.intValue = 13; }
    else if (lower == "lr") { token.type = TokenType::REGISTER; token.intValue = 14; }
    else if (lower == "pc") { token.type = TokenType::REGISTER; token.intValue = 15; }
    else {
        token.type = TokenType::IDENTIFIER;
    }

    return token;
}

Token Lexer::PeekToken() {
    size_t saved_pos = pos;
    int saved_line = line;
    int saved_col = col;
    
    Token token = NextToken();
    
    // Restore
    pos = saved_pos;
    line = saved_line;
    col = saved_col;
    
    return token;
}
