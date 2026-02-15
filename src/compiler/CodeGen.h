#pragma once
#include "AST.h"
#include <sstream>
#include <map>
#include <stack>

class CodeGen {
public:
    CodeGen(const Program& program);
    std::string Generate();

private:
    const Program& program;
    std::stringstream asmOut;
    int labelCount = 0;
    
    // Symbol Table: Name -> Offset from FP (R12)
    // Args: Offset > 0 (e.g. FP + 2)
    // Locals: Offset < 0 (e.g. FP - 1)
    std::map<std::string, int> locals;
    int stackOffset = 0; // Tracks local var stack usage per function
    
    void Emit(const std::string& instr);
    void EmitLabel(const std::string& label);
    std::string NewLabel();

    void GenFunction(const FunctionDecl& func);
    void GenStatement(const Statement& stmt);
    void GenBlock(const Block& block);
    void GenExpression(const Expression& expr);
    
    // Visitors
    void Visit(const ReturnStmt& stmt);
    void Visit(const IfStmt& stmt);
    void Visit(const WhileStmt& stmt);
    void Visit(const VarDecl& stmt);
    void Visit(const ExprStmt& stmt);
    
    void Visit(const BinaryExpr& expr);
    void Visit(const UnaryExpr& expr);
    void Visit(const LiteralExpr& expr);
    void Visit(const VarExpr& expr);
    void Visit(const AssignmentExpr& expr);
    void Visit(const CallExpr& expr);
};
