#pragma once
#include <vector>
#include <memory>
#include "Lexer.h"
#include "AST.h"

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<Program> Parse();

private:
    std::vector<Token> tokens;
    int pos;

    Token Peek(int offset = 0) const;
    Token Advance();
    bool Match(TokenType type);
    bool Check(TokenType type) const;
    Token Consume(TokenType type, const std::string& message);

    // Grammar
    std::unique_ptr<FunctionDecl> ParseFunction();
    std::unique_ptr<Statement> ParseStatement();
    std::unique_ptr<Block> ParseBlock();
    std::unique_ptr<Statement> ParseVarDecl();
    std::unique_ptr<Statement> ParseIf();
    std::unique_ptr<Statement> ParseWhile();
    std::unique_ptr<Statement> ParseReturn();
    
    // Expressions (Precedence Climbing)
    std::unique_ptr<Expression> ParseExpression();
    std::unique_ptr<Expression> ParseAssignment();
    std::unique_ptr<Expression> ParseBinary(int minPrec);
    std::unique_ptr<Expression> ParseUnary();
    std::unique_ptr<Expression> ParsePrimary();
    
    int GetPrecedence(TokenType type);
};
