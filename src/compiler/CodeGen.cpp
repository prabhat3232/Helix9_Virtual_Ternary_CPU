#include "CodeGen.h"
#include <iostream>

CodeGen::CodeGen(const Program& program) : program(program) {}

std::string CodeGen::Generate() {
    asmOut << "; Helix C-lite Compiled Output\n";
    asmOut << ".ORG 100\n";
    asmOut << "JMP main\n\n"; // Jump to entry point

    // Generate Functions (and Globals - TODO)
    for (const auto& decl : program.declarations) {
        if (decl->GetType() == NodeType::FUNCTION_DECL) {
            GenFunction(*static_cast<FunctionDecl*>(decl.get()));
        }
    }
    
    // Add Halt catch-all
    asmOut << "HaltLoop: HLT\n";
    
    return asmOut.str();
}

void CodeGen::Emit(const std::string& instr) {
    asmOut << "    " << instr << "\n";
}

void CodeGen::EmitLabel(const std::string& label) {
    asmOut << label << ":\n";
}

std::string CodeGen::NewLabel() {
    return "L" + std::to_string(labelCount++);
}

void CodeGen::GenFunction(const FunctionDecl& func) {
    EmitLabel(func.name);
    
    // Prologue
    // PUSH FP (R12)
    Emit("ADD R13, R13, -1"); 
    Emit("STW R12, R13, 0");
    // MOV FP, SP
    Emit("MOV R12, R13");
    
    // Reset Stack Offset for Locals (starts below FP)
    stackOffset = 0;
    locals.clear();
    
    // Register Arguments (R1-R6) to Locals?
    // For v0.1, let's spill args to stack to simplify "VarExpr" lookup
    // Or just map them.
    // Spec: "Args Passed in R1...R6"
    // We can "Alloc" space for them and store them so they are treated as locals.
    for (size_t i = 0; i < func.args.size(); ++i) {
        std::string argName = func.args[i].second;
        stackOffset--; // FP - 1, FP - 2...
        locals[argName] = stackOffset;
        
        // Push Register Arg to Stack
        Emit("ADD R13, R13, -1");
        if (i < 6) {
             Emit("STW R" + std::to_string(i + 1) + ", R13, 0");
        } else {
             // Load from caller's stack (FP + 2 + (i-6))?
             // Not implementing >6 args for now
        }
    }

    GenBlock(*func.body);
    
    // Epilogue (in case of no return)
    Emit("MOV R13, R12"); // Restore SP
    Emit("LDW R12, R13, 0"); // Restore FP
    Emit("ADD R13, R13, 1"); // Pop FP
    Emit("RET");
}

void CodeGen::GenBlock(const Block& block) {
    for (const auto& stmt : block.statements) {
        GenStatement(*stmt);
    }
}

void CodeGen::GenStatement(const Statement& stmt) {
    switch (stmt.GetType()) {
        case NodeType::RETURN_STMT: Visit(static_cast<const ReturnStmt&>(stmt)); break;
        case NodeType::IF_STMT: Visit(static_cast<const IfStmt&>(stmt)); break;
        case NodeType::WHILE_STMT: Visit(static_cast<const WhileStmt&>(stmt)); break;
        case NodeType::VAR_DECL: Visit(static_cast<const VarDecl&>(stmt)); break;
        case NodeType::EXPR_STMT: Visit(static_cast<const ExprStmt&>(stmt)); break;
        default: break;
    }
}

void CodeGen::GenExpression(const Expression& expr) {
    switch (expr.GetType()) {
        case NodeType::BINARY_EXPR: Visit(static_cast<const BinaryExpr&>(expr)); break;
        case NodeType::UNARY_EXPR: Visit(static_cast<const UnaryExpr&>(expr)); break;
        case NodeType::LITERAL_EXPR: Visit(static_cast<const LiteralExpr&>(expr)); break;
        case NodeType::VAR_EXPR: Visit(static_cast<const VarExpr&>(expr)); break;
        case NodeType::ASSIGNMENT_EXPR: Visit(static_cast<const AssignmentExpr&>(expr)); break;
        case NodeType::CALL_EXPR: Visit(static_cast<const CallExpr&>(expr)); break;
    }
}

void CodeGen::Visit(const ReturnStmt& stmt) {
    if (stmt.value) {
        GenExpression(*stmt.value); // Result in R1
    }
    
    // Epilogue
    Emit("MOV R13, R12"); // Restore SP
    Emit("LDW R12, R13, 0"); // Restore FP
    Emit("ADD R13, R13, 1"); // Pop FP
    Emit("RET");
}

void CodeGen::Visit(const IfStmt& stmt) {
    std::string elseLabel = NewLabel();
    std::string endLabel = NewLabel();
    
    GenExpression(*stmt.condition); // R1
    Emit("CMP R1, R0");
    Emit("BEQ " + elseLabel); // If 0, jump to else
    
    GenStatement(*stmt.thenBranch);
    Emit("JMP " + endLabel);
    
    EmitLabel(elseLabel);
    if (stmt.elseBranch) {
        GenStatement(*stmt.elseBranch);
    }
    
    EmitLabel(endLabel);
}

void CodeGen::Visit(const WhileStmt& stmt) {
    std::string loopLabel = NewLabel();
    std::string endLabel = NewLabel();
    
    EmitLabel(loopLabel);
    GenExpression(*stmt.condition);
    Emit("CMP R1, R0");
    Emit("BEQ " + endLabel);
    
    GenStatement(*stmt.body);
    Emit("JMP " + loopLabel);
    
    EmitLabel(endLabel);
}

void CodeGen::Visit(const VarDecl& stmt) {
    stackOffset--;
    locals[stmt.name] = stackOffset;
    
    Emit("ADD R13, R13, -1"); // Alloc Stack
    
    if (stmt.init) {
        GenExpression(*stmt.init); // R1
        Emit("STW R1, R13, 0"); // Store to new local
    }
}

void CodeGen::Visit(const ExprStmt& stmt) {
    GenExpression(*stmt.expr);
}

void CodeGen::Visit(const BinaryExpr& expr) {
    GenExpression(*expr.left); // R1
    
    // Push R1
    Emit("ADD R13, R13, -1");
    Emit("STW R1, R13, 0");
    
    GenExpression(*expr.right); // R1
    
    // Pop to R2
    Emit("LDW R2, R13, 0");
    Emit("ADD R13, R13, 1");
    
    // R1 = R2 (Left) op R1 (Right)
    // Destination R1
    switch (expr.op) {
        case TokenType::PLUS: Emit("ADD R1, R2, R1"); break;
        case TokenType::MINUS: Emit("SUB R1, R2, R1"); break;
        case TokenType::STAR: Emit("MUL R1, R2, R1"); break;
        case TokenType::SLASH: Emit("DIV R1, R2, R1"); break;
        case TokenType::PERCENT: Emit("MOD R1, R2, R1"); break;
        case TokenType::AMPERSAND: Emit("AND R1, R2, R1"); break;
        case TokenType::PIPE: Emit("OR R1, R2, R1"); break;
        case TokenType::CARET: Emit("XOR R1, R2, R1"); break;
        case TokenType::LSL: Emit("LSL R1, R2"); break; 
        case TokenType::LSR: Emit("LSR R1, R2"); break;
        
        // Comparisons: Set R1 to 1 (True) or 0 (False)
        case TokenType::EQ: 
            Emit("CMP R2, R1"); // Compare Left(R2) with Right(R1)
            Emit("BEQ " + NewLabel() + "_True");
            Emit("LDI R1, 0");
            Emit("JMP " + NewLabel() + "_End");
            EmitLabel(NewLabel() + "_True");
            Emit("LDI R1, 1");
            EmitLabel(NewLabel() + "_End");
            break;
            
        case TokenType::NEQ:
            Emit("CMP R2, R1");
            Emit("BNE " + NewLabel() + "_True");
            Emit("LDI R1, 0");
            Emit("JMP " + NewLabel() + "_End");
            EmitLabel(NewLabel() + "_True");
            Emit("LDI R1, 1");
            EmitLabel(NewLabel() + "_End");
            break;

        case TokenType::LT:
            Emit("CMP R2, R1");
            Emit("BLT " + NewLabel() + "_True");
            Emit("LDI R1, 0");
            Emit("JMP " + NewLabel() + "_End");
            EmitLabel(NewLabel() + "_True");
            Emit("LDI R1, 1");
            EmitLabel(NewLabel() + "_End");
            break;

        case TokenType::GT:
            Emit("CMP R2, R1");
            Emit("BGT " + NewLabel() + "_True");
            Emit("LDI R1, 0");
            Emit("JMP " + NewLabel() + "_End");
            EmitLabel(NewLabel() + "_True");
            Emit("LDI R1, 1");
            EmitLabel(NewLabel() + "_End");
            break;
            
       // TODO: LTE, GTE require explicit checks or swapped operands logic
    }
}

void CodeGen::Visit(const UnaryExpr& expr) {
    GenExpression(*expr.right); // Result in R1
    
    if (expr.op == TokenType::MINUS) {
        // R1 = 0 - R1
        Emit("LDI R2, 0");
        Emit("SUB R1, R2, R1");
    } else if (expr.op == TokenType::BANG) {
        // !R1: If R1 == 0 -> 1. Else -> 0.
        Emit("CMP R1, R0"); // Compare with 0
        Emit("BEQ " + NewLabel() + "_True");
        Emit("LDI R1, 0");
        Emit("JMP " + NewLabel() + "_End");
        EmitLabel(NewLabel() + "_True");
        Emit("LDI R1, 1");
        EmitLabel(NewLabel() + "_End");
    }
}


void CodeGen::Visit(const LiteralExpr& expr) {
    Emit("LDI R1, " + std::to_string(expr.value));
}

void CodeGen::Visit(const VarExpr& expr) {
    if (locals.count(expr.name)) {
        int offset = locals[expr.name];
        Emit("LDW R1, R12, " + std::to_string(offset));
    } else {
        std::cerr << "Undefined variable: " << expr.name << std::endl;
        exit(1);
    }
}

void CodeGen::Visit(const AssignmentExpr& expr) {
    GenExpression(*expr.value); // R1
    
    if (locals.count(expr.name)) {
        int offset = locals[expr.name];
        Emit("STW R1, R12, " + std::to_string(offset));
    }
}

void CodeGen::Visit(const CallExpr& expr) {
    // Push Args (Right to Left standard?) 
    // Spec: "R1...R6". CodeGen should fill R1-R6.
    // Evaluating args might clobber R1-R6 if we just compute them.
    // Strategy: Evaluate all args to stack first. Then Pop into registers.
    
    for (const auto& arg : expr.args) {
        GenExpression(*arg); // R1
        Emit("ADD R13, R13, -1");
        Emit("STW R1, R13, 0");
    }
    
    // Pop into R1..R6 (Reverse order)
    int count = expr.args.size();
    for (int i = count - 1; i >= 0; --i) {
        if (i < 6) {
             Emit("LDW R" + std::to_string(i + 1) + ", R13, 0");
             Emit("ADD R13, R13, 1");
        } else {
            // Leave on stack (Excess)
        }
    }
    
    Emit("CALL " + expr.name);
    // Result in R1
    
    // Cleanup Excess Stack? Callee saves?
    // Caller usually cleans args.
    if (count > 6) {
        int excess = count - 6;
        Emit("ADD R13, R13, " + std::to_string(excess));
    }
}
