#pragma once
#include <string>
#include <vector>
#include <memory>
#include "Lexer.h"

// AST Node Types
enum class NodeType {
    PROGRAM,
    FUNCTION_DECL,
    VAR_DECL,
    BLOCK,
    RETURN_STMT,
    IF_STMT,
    WHILE_STMT,
    EXPR_STMT,
    BINARY_EXPR,
    UNARY_EXPR,
    CALL_EXPR,
    LITERAL_EXPR,
    VAR_EXPR,
    ASSIGNMENT_EXPR
};

struct ASTNode {
    virtual ~ASTNode() = default;
    virtual NodeType GetType() const = 0;
};

// Expressions
struct Expression : public ASTNode {};

struct LiteralExpr : public Expression {
    int64_t value;
    LiteralExpr(int64_t v) : value(v) {}
    NodeType GetType() const override { return NodeType::LITERAL_EXPR; }
};

struct VarExpr : public Expression {
    std::string name;
    VarExpr(const std::string& n) : name(n) {}
    NodeType GetType() const override { return NodeType::VAR_EXPR; }
};

struct BinaryExpr : public Expression {
    TokenType op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    BinaryExpr(TokenType o, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
        : op(o), left(std::move(l)), right(std::move(r)) {}
    NodeType GetType() const override { return NodeType::BINARY_EXPR; }
};

struct UnaryExpr : public Expression {
    TokenType op;
    std::unique_ptr<Expression> right;
    UnaryExpr(TokenType o, std::unique_ptr<Expression> r)
        : op(o), right(std::move(r)) {}
    NodeType GetType() const override { return NodeType::UNARY_EXPR; }
};

struct CallExpr : public Expression {
    std::string name;
    std::vector<std::unique_ptr<Expression>> args;
    CallExpr(const std::string& n, std::vector<std::unique_ptr<Expression>> a)
        : name(n), args(std::move(a)) {}
    NodeType GetType() const override { return NodeType::CALL_EXPR; }
};

struct AssignmentExpr : public Expression {
    std::string name;
    std::unique_ptr<Expression> value;
    AssignmentExpr(const std::string& n, std::unique_ptr<Expression> v)
        : name(n), value(std::move(v)) {}
    NodeType GetType() const override { return NodeType::ASSIGNMENT_EXPR; }
};

// Statements
struct Statement : public ASTNode {};

struct ReturnStmt : public Statement {
    std::unique_ptr<Expression> value;
    ReturnStmt(std::unique_ptr<Expression> v) : value(std::move(v)) {}
    NodeType GetType() const override { return NodeType::RETURN_STMT; }
};

struct ExprStmt : public Statement {
    std::unique_ptr<Expression> expr;
    ExprStmt(std::unique_ptr<Expression> e) : expr(std::move(e)) {}
    NodeType GetType() const override { return NodeType::EXPR_STMT; }
};

struct Block : public Statement {
    std::vector<std::unique_ptr<Statement>> statements;
    Block(std::vector<std::unique_ptr<Statement>> s) : statements(std::move(s)) {}
    NodeType GetType() const override { return NodeType::BLOCK; }
};

struct IfStmt : public Statement {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> thenBranch;
    std::unique_ptr<Statement> elseBranch;
    IfStmt(std::unique_ptr<Expression> c, std::unique_ptr<Statement> t, std::unique_ptr<Statement> e)
        : condition(std::move(c)), thenBranch(std::move(t)), elseBranch(std::move(e)) {}
    NodeType GetType() const override { return NodeType::IF_STMT; }
};

struct WhileStmt : public Statement {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> body;
    WhileStmt(std::unique_ptr<Expression> c, std::unique_ptr<Statement> b)
        : condition(std::move(c)), body(std::move(b)) {}
    NodeType GetType() const override { return NodeType::WHILE_STMT; }
};

struct VarDecl : public Statement {
    std::string type; // "int"
    std::string name;
    std::unique_ptr<Expression> init;
    VarDecl(const std::string& t, const std::string& n, std::unique_ptr<Expression> i)
        : type(t), name(n), init(std::move(i)) {}
    NodeType GetType() const override { return NodeType::VAR_DECL; }
};

struct FunctionDecl : public ASTNode {
    std::string returnType;
    std::string name;
    std::vector<std::pair<std::string, std::string>> args; // type, name
    std::unique_ptr<Block> body;
    FunctionDecl(const std::string& rt, const std::string& n, 
                 std::vector<std::pair<std::string, std::string>> a, std::unique_ptr<Block> b)
        : returnType(rt), name(n), args(std::move(a)), body(std::move(b)) {}
    NodeType GetType() const override { return NodeType::FUNCTION_DECL; }
};

struct Program : public ASTNode {
    std::vector<std::unique_ptr<ASTNode>> declarations; // Functions or Globals
    Program(std::vector<std::unique_ptr<ASTNode>> d) : declarations(std::move(d)) {}
    NodeType GetType() const override { return NodeType::PROGRAM; }
};
