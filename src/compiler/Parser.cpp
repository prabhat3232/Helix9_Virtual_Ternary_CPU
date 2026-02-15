#include "Parser.h"
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), pos(0) {}

Token Parser::Peek(int offset) const {
    if (pos + offset >= tokens.size()) return tokens.back();
    return tokens[pos + offset];
}

Token Parser::Advance() {
    if (pos < tokens.size()) pos++;
    return tokens[pos - 1];
}

bool Parser::Match(TokenType type) {
    if (Check(type)) {
        Advance();
        return true;
    }
    return false;
}

bool Parser::Check(TokenType type) const {
    return Peek().type == type;
}

Token Parser::Consume(TokenType type, const std::string& message) {
    if (Check(type)) return Advance();
    // Error handling: Simple panic for now
    std::cerr << "Parser Error: " << message << " at " << Peek().ToString() << std::endl;
    exit(1);
}

std::unique_ptr<Program> Parser::Parse() {
    std::vector<std::unique_ptr<ASTNode>> decls;
    while (!Check(TokenType::END_OF_FILE)) {
        // Lookahead to distinguish Function vs Global Var checking "TypeName Identifier ("
        // Simple heuristic: if follows '(', it's function.
        // Actually, could be int a; or int func();
        // Peek(2) should be '('. 'int' 'name' '('
        if (Check(TokenType::KW_INT) || Check(TokenType::KW_VOID)) {
            if (Peek(1).type == TokenType::IDENTIFIER && Peek(2).type == TokenType::LPAREN) {
                decls.push_back(ParseFunction());
            } else {
                // Global Var (using Statement parser for VarDecl, but need to cast/wrap)
                // For simplified AST, Program holds Nodes.
                decls.push_back(ParseVarDecl());
            }
        } else {
             std::cerr << "Unexpected token at top level: " << Peek().ToString() << std::endl;
             Advance();
        }
    }
    return std::make_unique<Program>(std::move(decls));
}

std::unique_ptr<FunctionDecl> Parser::ParseFunction() {
    std::string type = Advance().text; // int/void
    std::string name = Consume(TokenType::IDENTIFIER, "Expected function name").text;
    Consume(TokenType::LPAREN, "Expected '('");
    
    std::vector<std::pair<std::string, std::string>> args;
    if (!Check(TokenType::RPAREN)) {
        do {
            std::string argType = Consume(TokenType::KW_INT, "Expected type").text;
            std::string argName = Consume(TokenType::IDENTIFIER, "Expected arg name").text;
            args.push_back({argType, argName});
        } while (Match(TokenType::COMMA));
    }
    Consume(TokenType::RPAREN, "Expected ')'");
    
    std::unique_ptr<Block> body = ParseBlock();
    return std::make_unique<FunctionDecl>(type, name, args, std::move(body));
}

std::unique_ptr<Block> Parser::ParseBlock() {
    Consume(TokenType::LBRACE, "Expected '{'");
    std::vector<std::unique_ptr<Statement>> stmts;
    while (!Check(TokenType::RBRACE) && !Check(TokenType::END_OF_FILE)) {
        stmts.push_back(ParseStatement());
    }
    Consume(TokenType::RBRACE, "Expected '}'");
    return std::make_unique<Block>(std::move(stmts));
}

std::unique_ptr<Statement> Parser::ParseStatement() {
    if (Check(TokenType::LBRACE)) return ParseBlock();
    if (Match(TokenType::KW_IF)) return ParseIf();
    if (Match(TokenType::KW_WHILE)) return ParseWhile();
    if (Match(TokenType::KW_RETURN)) return ParseReturn();
    if (Check(TokenType::KW_INT)) return ParseVarDecl();
    
    // Expr Statement
    std::unique_ptr<Expression> expr = ParseExpression();
    Consume(TokenType::SEMICOLON, "Expected ';'");
    return std::make_unique<ExprStmt>(std::move(expr));
}

std::unique_ptr<Statement> Parser::ParseVarDecl() {
    std::string type = Consume(TokenType::KW_INT, "Expected 'int'").text;
    std::string name = Consume(TokenType::IDENTIFIER, "Expected variable name").text;
    std::unique_ptr<Expression> init = nullptr;
    if (Match(TokenType::ASSIGN)) {
        init = ParseExpression();
    }
    Consume(TokenType::SEMICOLON, "Expected ';'");
    return std::make_unique<VarDecl>(type, name, std::move(init));
}

std::unique_ptr<Statement> Parser::ParseIf() {
    Consume(TokenType::LPAREN, "Expected '(' after if");
    std::unique_ptr<Expression> cond = ParseExpression();
    Consume(TokenType::RPAREN, "Expected ')' after condition");
    std::unique_ptr<Statement> thenBranch = ParseStatement();
    std::unique_ptr<Statement> elseBranch = nullptr;
    if (Match(TokenType::KW_ELSE)) {
        elseBranch = ParseStatement();
    }
    return std::make_unique<IfStmt>(std::move(cond), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<Statement> Parser::ParseWhile() {
    Consume(TokenType::LPAREN, "Expected '(' after while");
    std::unique_ptr<Expression> cond = ParseExpression();
    Consume(TokenType::RPAREN, "Expected ')' after condition");
    std::unique_ptr<Statement> body = ParseStatement();
    return std::make_unique<WhileStmt>(std::move(cond), std::move(body));
}

std::unique_ptr<Statement> Parser::ParseReturn() {
    std::unique_ptr<Expression> value = nullptr;
    if (!Check(TokenType::SEMICOLON)) {
        value = ParseExpression();
    }
    Consume(TokenType::SEMICOLON, "Expected ';'");
    return std::make_unique<ReturnStmt>(std::move(value));
}

std::unique_ptr<Expression> Parser::ParseExpression() {
    return ParseAssignment();
}

std::unique_ptr<Expression> Parser::ParseAssignment() {
    std::unique_ptr<Expression> expr = ParseBinary(0); // parse lowest precedence first
    
    if (Match(TokenType::ASSIGN)) {
        // LHS must be var
        if (expr->GetType() != NodeType::VAR_EXPR) {
             std::cerr << "Invalid assignment target." << std::endl;
             exit(1);
        }
        std::unique_ptr<Expression> value = ParseAssignment(); // Right assoc
        // Transform VarExpr to name for AssignmentExpr
        std::string name = static_cast<VarExpr*>(expr.get())->name;
        return std::make_unique<AssignmentExpr>(name, std::move(value));
    }
    return expr;
}

int Parser::GetPrecedence(TokenType type) {
    switch (type) {
        case TokenType::PIPE: return 1;      // | (Max)
        case TokenType::CARET: return 2;     // ^ (XOR)
        case TokenType::AMPERSAND: return 3; // & (Min)
        case TokenType::EQ:
        case TokenType::NEQ: return 4;
        case TokenType::LT:
        case TokenType::GT:
        case TokenType::LTE:
        case TokenType::GTE: return 5;
        case TokenType::LSL:
        case TokenType::LSR: return 6;
        case TokenType::PLUS:
        case TokenType::MINUS: return 7;
        case TokenType::STAR:
        case TokenType::SLASH:
        case TokenType::PERCENT: return 8;
        default: return 0;
    }
}

std::unique_ptr<Expression> Parser::ParseBinary(int minPrec) {
    std::unique_ptr<Expression> left = ParseUnary(); // Parse higher precedence (Unary/Primary)
    
    while (true) {
        int prec = GetPrecedence(Peek().type);
        if (prec < minPrec) break;
        
        TokenType op = Advance().type;
        std::unique_ptr<Expression> right = ParseBinary(prec + 1); // Left associative
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Expression> Parser::ParseUnary() {
    if (Match(TokenType::MINUS) || Match(TokenType::BANG)) {
        TokenType op = tokens[pos-1].type;
        std::unique_ptr<Expression> right = ParseUnary();
        return std::make_unique<UnaryExpr>(op, std::move(right));
    }
    return ParsePrimary();
}

std::unique_ptr<Expression> Parser::ParsePrimary() {
    if (Match(TokenType::NUMBER)) {
        return std::make_unique<LiteralExpr>(tokens[pos-1].intValue);
    }
    if (Match(TokenType::IDENTIFIER)) {
        if (Check(TokenType::LPAREN)) { // Function Call
             std::string name = tokens[pos-1].text;
             Advance(); // (
             std::vector<std::unique_ptr<Expression>> args;
             if (!Check(TokenType::RPAREN)) {
                 do {
                     args.push_back(ParseExpression());
                 } while (Match(TokenType::COMMA));
             }
             Consume(TokenType::RPAREN, "Expected ')'");
             return std::make_unique<CallExpr>(name, std::move(args));
        }
        return std::make_unique<VarExpr>(tokens[pos-1].text);
    }
    if (Match(TokenType::LPAREN)) {
        std::unique_ptr<Expression> expr = ParseExpression();
        Consume(TokenType::RPAREN, "Expected ')'");
        return expr;
    }
    
    std::cerr << "Unexpected token in expression: " << Peek().ToString() << std::endl;
    exit(1);
    return nullptr;
}
