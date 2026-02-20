#include "Parser.h"
#include "../isa.h"
#include <iostream>
#include <algorithm>
#include <map>

// Helper to convert string to lower case for case-insensitive checks (though strict is lowercase)
// We will enforce lowercase via Lexer or check here.
// Spec says "Lowercase Mnemonics". strict.

using namespace Helix;
using namespace Helix::Assembler;

Parser::Parser(const std::string& source) : src(source) {}

ObjectFile Parser::Compile() {
    result.sections.clear();
    result.symbols.clear();
    result.relocations.clear();
    symbolTable.clear();
    
    // Default Section
    result.sections.push_back({".text", 0, {}});
    currentSection = &result.sections.back();
    
    Pass1();
    
    // Reset for Pass 2
    // Clear section data (we only counted in Pass 1, now we emit)
    for(auto& sec : result.sections) {
        sec.data.clear();
        sec.baseAddress = 0; // Or whatever start
    }
    // Re-pointer to default
    // Note: Pass 2 logic needs to find the section again or we just iterate same way.
    // Better: Pass 2 clears data but keeps section structure IF Pass 1 built it?
    // Actually, Pass 1 builds Symbol Table. Pass 2 builds Data.
    // Let's clear sections and rebuild them in Pass 2 to be safe/simple?
    // Or just clear data.
    // Problem: Pass 1 needs to know Section Offsets for labels.
    // So Pass 1 builds Section Map and Label Offsets.
    // Pass 2 fills Data.
    
    currentSection = &result.sections[0]; // .text
    Pass2();
    
    return result;
}

void Parser::Pass1() {
    Lexer lexer(src);
    // Track current section index/pointer
    int sectionIdx = 0; // 0 = .text
    int64_t offset = 0;
    
    Token token = lexer.NextToken();
    while (token.type != TokenType::END_OF_FILE) {
        // Directives
        if (token.type == TokenType::DIR_SECTION) {
            Token name = lexer.NextToken();
            std::string secName = name.text;
            
            // Check if section exists or create
            bool found = false;
            for(int i=0; i<result.sections.size(); ++i) {
                if (result.sections[i].name == secName) {
                    sectionIdx = i;
                    found = true;
                    // Restore offset? Or continue?
                    // Assembler usually concatenates.
                    // But we need to know the offset for labels.
                    // For now, assume sequential sections or just track size.
                    offset = result.sections[i].data.size(); // Use size as current offset?
                    // In Pass 1 data is empty. 
                    // We need to track 'size' in Pass 1.
                    break;
                }
            }
            if (!found) {
                result.sections.push_back({secName, 0, {}});
                sectionIdx = result.sections.size() - 1;
                offset = 0;
            }
            token = lexer.PeekToken();
            continue;
        }

        // .org directive
        if (token.type == TokenType::DIR_ORG) {
            Token val = lexer.NextToken();
            if (val.type == TokenType::NUMBER) {
                offset = val.intValue;
            }
            token = lexer.PeekToken();
            continue;
        }

        // Global directive (Ignore in Pass 1, handled in Pass 2)
        if (token.type == TokenType::DIR_GLOBAL) {
             lexer.NextToken(); // Skip symbol name
             token = lexer.PeekToken();
             continue;
        }

        // Labels
        if (token.type == TokenType::IDENTIFIER) {
             Token next = lexer.PeekToken();
             if (next.type == TokenType::COLON) {
                 std::string label = token.text;
                 if (symbolTable.count(label)) {
                     std::cerr << "Error: Duplicate " << label << std::endl;
                     exit(1);
                 }
                 result.symbols.push_back({label, result.sections[sectionIdx].name, offset, false}); 
                 symbolTable[label] = offset; 
                 
                 lexer.NextToken(); // Colon
                 token = lexer.NextToken();
                 continue;
             }
        }
        
        // Data/Instruction -> Increment Offset
        // Data/Instruction -> Increment Offset
        if (token.type == TokenType::DIR_WORD || token.type == TokenType::DIR_DAT) {
             lexer.NextToken(); // Skip Directive
             while (true) {
                 Token val = lexer.PeekToken();
                 if (val.type == TokenType::NUMBER) {
                     offset++;
                     lexer.NextToken();
                 } else if (val.type == TokenType::IDENTIFIER) {
                     // Label reference in data
                     offset++;
                     lexer.NextToken();
                 } else {
                     break;
                 }
             }
             token = lexer.PeekToken();
             continue;
        }
        if (token.type == TokenType::IDENTIFIER) {
             // Instruction
             offset++;
             // lexer.NextToken(); // REMOVED: Don't skip first operand!
             ParseOperands(lexer, true);
             token = lexer.PeekToken();
             continue;
        }
        
        token = lexer.NextToken();
    }
}

void Parser::Pass2() {
    Lexer lexer(src);
    int sectionIdx = 0; // .text
    currentSection = &result.sections[0];
    currentAddress = 0;
    
    Token token = lexer.NextToken();
    while (token.type != TokenType::END_OF_FILE) {
        
        if (token.type == TokenType::DIR_ORG) {
            Token val = lexer.NextToken();
            int64_t targetAddr = val.intValue;
            
            if (targetAddr < currentAddress) {
                 std::cerr << "Error: .org cannot move backwards (yet)" << std::endl;
                 exit(1);
            }
            
            while (currentAddress < targetAddr) {
                currentSection->data.push_back(TernaryWord::FromInt64(0));
                currentAddress++;
            }
            
            token = lexer.PeekToken();
            continue;
        }
        
        if (token.type == TokenType::DIR_SECTION) {
            Token name = lexer.NextToken();
            for(size_t i=0; i<result.sections.size(); ++i) {
                if (result.sections[i].name == name.text) {
                    currentSection = &result.sections[i];
                    currentAddress = 0; // Reset address for new section
                    break;
                }
            }
            token = lexer.PeekToken();
            continue;
        }
        
        if (token.type == TokenType::DIR_GLOBAL) {
            Token s = lexer.NextToken();
            // Mark symbol as global
            for(auto& sym : result.symbols) {
                if (sym.name == s.text) sym.isGlobal = true;
            }
            token = lexer.PeekToken();
            continue;
        }

        // Labels (Skip)
        if (token.type == TokenType::IDENTIFIER && lexer.PeekToken().type == TokenType::COLON) {
             lexer.NextToken(); // Consume ':'
             token = lexer.NextToken(); // Next token (Mnemonic)
             continue;
        }

        // Data
        if (token.type == TokenType::DIR_WORD || token.type == TokenType::DIR_DAT) {
             lexer.NextToken(); // Skip Directive
             while (true) {
                 Token val = lexer.PeekToken();
                 if (val.type == TokenType::NUMBER) {
                     currentSection->data.push_back(TernaryWord::FromInt64(val.intValue));
                     currentAddress++;
                     lexer.NextToken();
                 } else if (val.type == TokenType::IDENTIFIER) {
                     int64_t v = 0;
                     if (symbolTable.count(val.text)) v = symbolTable[val.text];
                     currentSection->data.push_back(TernaryWord::FromInt64(v));
                     currentAddress++;
                     lexer.NextToken();
                 } else {
                     break;
                 }
             }
             token = lexer.PeekToken();
             continue;
        }

        // Instruction
        if (token.type == TokenType::IDENTIFIER) {
            std::string mnemonic = token.text;
            // lexer.NextToken(); // REMOVED: Don't skip first operand!
            
            std::vector<Operand> ops = ParseOperands(lexer, false);
            TernaryWord word = EncodeInstruction(mnemonic, ops);
            currentSection->data.push_back(word);
            currentAddress++; // Increment address
            
            token = lexer.PeekToken();
            continue;
        }
        
        token = lexer.NextToken();
    }
}


std::vector<Operand> Parser::ParseOperands(Lexer& lexer, bool dryRun) {
    std::vector<Operand> ops;
    
    while (true) {
        Token t = lexer.PeekToken();
        if (t.type == TokenType::NEWLINE || t.type == TokenType::END_OF_FILE) break;
        // Also break if next is Identifier (start of next instruction) 
        // BUT identifiers can be operands (Labels).
        // Ambiguity: "JMP Loop" vs "ADD.W ... \n SUB.W"
        // Without newlines, "ADD.W R1 R2 SUB.W..." 
        // Lexer doesn't emit Newlines unless strictly checking?
        // Wait, Lexer emits NEWLINE type. 
        // We should break on Newline.
        // Also if we hit a directive or something that can't be an operand.
        
        // Debug print
        // std::cout << "Token: " << t.text << " Type: " << (int)t.type << std::endl;
        // std::cout << "[DEBUG] ParseOp Token: '" << t.text << "' Type: " << (int)t.type << std::endl;
        
        if (t.type == TokenType::REGISTER) {
            ops.push_back({OperandType::REGISTER, t.intValue, 0, ""});
            lexer.NextToken();
        } 
        else if (t.type == TokenType::IDENTIFIER && t.text.length() == 2 && t.text[0] == 'v' && isdigit(t.text[1])) {
             // Vector Register v0-v3
             int reg = t.text[1] - '0';
             if (reg >= 0 && reg <= 3) {
                 ops.push_back({OperandType::REGISTER, reg, 0, ""});
                 lexer.NextToken();
             } else {
                 // Fallback to identifier if out of range (though v9 is unlikely label?)
                 // Treat as Identifier
                 int64_t val = 0;
                if (!dryRun && symbolTable.count(t.text)) val = symbolTable[t.text];
                ops.push_back({OperandType::IMMEDIATE, 0, val, t.text});
                lexer.NextToken();
             }
        } 
        else if (t.type == TokenType::NUMBER) {
            ops.push_back({OperandType::IMMEDIATE, 0, t.intValue, ""});
            lexer.NextToken();
        }
        else if (t.type == TokenType::LBRACKET) {
            lexer.NextToken(); // [
            Token base = lexer.NextToken();
            if (base.type != TokenType::REGISTER) {
                 // std::cerr << "Expected Register after [" << std::endl;
            }
            int64_t reg = base.intValue;
            int64_t offset = 0;
            
            Token next = lexer.PeekToken();
            if (next.type == TokenType::NUMBER) {
                offset = next.intValue;
                lexer.NextToken();
            } else if (next.type == TokenType::IDENTIFIER) {
                if (!dryRun && symbolTable.count(next.text)) {
                     offset = symbolTable[next.text];
                }
                lexer.NextToken();
            } else {
                // Fallback: Consume unexpected token to avoid infinite loop
                // std::cerr << "Warning: Unexpected token in memory operand: Type=" << (int)next.type << " Text='" << next.text << "'" << std::endl;
                 lexer.NextToken(); 
            }
            
            // TODO: Support [r1+LABEL]?
            // If next is Identifier...
            
            if (lexer.PeekToken().type == TokenType::RBRACKET) {
                lexer.NextToken();
            }
            
            ops.push_back({OperandType::MEMORY, reg, offset, ""});
        }
        else if (t.type == TokenType::IDENTIFIER) {
            int64_t val = 0;
            if (!dryRun) {
                 if (symbolTable.count(t.text)) {
                     val = symbolTable[t.text];
                 } else {
                     // Undefined symbol - likely external
                     // std::cerr << "Undefined symbol: " << t.text << std::endl;
                 }
            }
            ops.push_back({OperandType::IMMEDIATE, 0, val, t.text});
            lexer.NextToken();
        }
        else {
            // Unexpected token for operand
            // Break loop to let Parser handle it (e.g. Next Instruction? or Error)
            // If we are here, it's not Newline, not EOF. 
            // It might be a directive or opcode if previous line didn't end with newline?
            // Ideally we Error if we expect operands but verify 0 operands.
            break; 
        }
    }
    // std::cout << "Parsed " << ops.size() << " operands." << std::endl;
    return ops;
}

TernaryWord Parser::EncodeInstruction(const std::string& mnemonic, const std::vector<Operand>& ops) {
    static std::map<std::string, int> opMap = {
        {"halt", (int)Opcode::HLT}, {"hlt", (int)Opcode::HLT}, {"nop", (int)Opcode::NOP},
        {"add.w", (int)Opcode::ADD}, {"sub.w", (int)Opcode::SUB}, {"mul.w", (int)Opcode::MUL}, {"div.w", (int)Opcode::DIV}, {"mod.w", (int)Opcode::MOD},
        {"and.w", (int)Opcode::AND}, {"or.w", (int)Opcode::OR}, {"xor.w", (int)Opcode::XOR}, {"lsl.w", (int)Opcode::LSL}, {"lsr.w", (int)Opcode::LSR},
        {"mov.w", (int)Opcode::MOV}, {"ldi.w", (int)Opcode::LDI}, {"ld.w", (int)Opcode::LDW}, {"st.w", (int)Opcode::STW},
        {"jmp", (int)Opcode::JMP}, {"beq", (int)Opcode::BEQ}, {"bne", (int)Opcode::BNE}, {"bgt", (int)Opcode::BGT}, {"blt", (int)Opcode::BLT},
        {"call", (int)Opcode::CALL}, {"ret", (int)Opcode::RET}, {"msr", (int)Opcode::MSR}, {"mrs", (int)Opcode::MRS},
        {"cmp.w", (int)Opcode::CMP},
        // Cognitive
        {"cns.w", (int)Opcode::CNS}, {"dec.w", (int)Opcode::DEC}, {"pop.t", (int)Opcode::POP}, {"sat.add", (int)Opcode::SAT},
        // Vector (Phase 8 - Vector Unit)
        {"vldr", (int)Opcode::VLDR},
        {"vstr", (int)Opcode::VSTR},
        {"vadd", (int)Opcode::VADD},
        {"vdot", (int)Opcode::VDOT},
        {"vmmul", (int)Opcode::VMMUL},
        {"vsign", (int)Opcode::VSIGN},
        {"vclip", (int)Opcode::VCLIP},
        {"vstri", (int)Opcode::VSTRI},
        {"vmmsgn", (int)Opcode::VMMSGN},
        
        // Legacy/Other
        {"vec.cns", (int)Opcode::VEC_CNS}, 
        {"vec.pop", (int)Opcode::VEC_POP}
    };


    int64_t opcode = 0;
    if (opMap.count(mnemonic)) {
        opcode = opMap[mnemonic];
        // std::cout << "[ASM-DEBUG] " << mnemonic << " -> " << opcode << std::endl;
    } else {
        std::cerr << "Error: Unknown mnemonic '" << mnemonic << "'" << std::endl;
        exit(1);
    }
    
    int64_t mode = 0;
    int64_t rd = 0, rs1 = 0, rs2_imm = 0;
    
    // Logic to set Mode/Fields based on Opcode Family and Operands
    if (opcode >= 2 && opcode <= 11) { // Arithmetic (ADD..LSR)
        if (ops.size() < 2) { std::cerr << "Error: " << mnemonic << " requires at least 2 operands" << std::endl; exit(1); }
        rd = ops[0].reg;
        rs1 = ops[1].reg;
        if (ops.size() >= 3) {
            if (ops[2].type == OperandType::IMMEDIATE) {
                mode = 1; rs2_imm = ops[2].imm;
            } else {
                mode = 0; rs2_imm = ops[2].reg;
            }
        } else {
            // 2-arg arithmetic? (e.g. lsl.w rd r1)
            mode = 0; // Default reg?
        }
    }
    // ... Existing logic ...
    else if (opcode == 36 || opcode == 40) { // VMMUL, VMMSGN
        // vmmul vd, vs, rs (Matrix Base)
        if (ops.size() < 3) { std::cerr << "Error: vmmul/vmmsgn requires 3 operands" << std::endl; exit(1); }
        rd = ops[0].reg;
        rs1 = ops[1].reg; // Vs
        rs2_imm = ops[2].reg; // Rs (Base) -> Op2
        mode = 0; 
    }
    else if (opcode == 37) { // VSIGN
        // vsign vd, vs
        if (ops.size() < 2) { std::cerr << "Error: vsign requires 2 operands" << std::endl; exit(1); }
        rd = ops[0].reg;
        rs1 = ops[1].reg;
        mode = 0;
    }
    else if (opcode == 38) { // VCLIP
        // vclip vd, vs, imm
        if (ops.size() < 3) { std::cerr << "Error: vclip requires 3 operands" << std::endl; exit(1); }
        rd = ops[0].reg;
        rs1 = ops[1].reg;
        rs2_imm = ops[2].imm;
        mode = 1;
    }
    else if (opcode == 39) { // VSTRI
        // vstri imm or vstri rs
        if (ops.size() < 1) { std::cerr << "Error: vstri requires 1 operand" << std::endl; exit(1); }
        if (ops[0].type == OperandType::IMMEDIATE) {
            mode = 1; rs2_imm = ops[0].imm;
        } else {
            mode = 0; rs2_imm = ops[0].reg;
        }
    }
    else if (opcode >= 32 && opcode <= 35) { // VLDR, VSTR, VADD, VDOT (Phase 8)
        if (opcode == 32 || opcode == 33) { // VLDR, VSTR
            // vldr vd, rs (or imm)
            if (ops.size() < 2) { std::cerr << "Error: " << mnemonic << " requires 2 operands" << std::endl; exit(1); }
            rd = ops[0].reg;
            if (ops[1].type == OperandType::IMMEDIATE) {
                mode = 1; rs2_imm = ops[1].imm;
            } else {
                mode = 0; rs2_imm = ops[1].reg;
            }
        } else if (opcode == 34 || opcode == 35) { // VADD, VDOT
            // vadd vd, vs1, vs2 (or imm)
            if (ops.size() < 3) { std::cerr << "Error: " << mnemonic << " requires 3 operands" << std::endl; exit(1); }
            rd = ops[0].reg;
            rs1 = ops[1].reg;
            if (ops[2].type == OperandType::IMMEDIATE) {
                mode = 1; rs2_imm = ops[2].imm;
            } else {
                mode = 0; rs2_imm = ops[2].reg;
            }
        }
    }
    else if (mnemonic == "cmp.w") {
        if (ops.size() < 2) { std::cerr << "Error: cmp.w requires 2 operands" << std::endl; exit(1); }
        // cmp r1 r2 -> Opcode 25, Rs1=r1, Rs2=r2. Rd ignored?
        // Or Rd=r1 (if strict CMP subtracts r1-r2?)
        // Let's use: Rs1=Op1, Rs2=Op2.
        rs1 = ops[0].reg; 
        if (ops[1].type == OperandType::IMMEDIATE) {
            mode = 1; rs2_imm = ops[1].imm;
        } else {
            mode = 0; rs2_imm = ops[1].reg;
        }
    }
    else if (mnemonic == "ld.w") { // Load
        if (ops.size() < 2) { std::cerr << "Error: ld.w requires 2 operands" << std::endl; exit(1); }
        rd = ops[0].reg;
        if (ops[1].type == OperandType::MEMORY) {
            rs1 = ops[1].reg;
            rs2_imm = ops[1].imm;
            mode = (rs2_imm == 0) ? 2 : 3;
        } else {
             std::cerr << "Error: ld.w expects memory operand" << std::endl; exit(1);
        }
    }
    else if (mnemonic == "st.w") { // Store
        if (ops.size() < 2) { std::cerr << "Error: st.w requires 2 operands" << std::endl; exit(1); }
        // st.w src [dst+imm]
        rd = ops[0].reg; // Src
        if (ops[1].type == OperandType::MEMORY) {
            rs1 = ops[1].reg; // Dst
            rs2_imm = ops[1].imm;
            mode = (rs2_imm == 0) ? 2 : 3;
        } else {
             std::cerr << "Error: st.w expects memory operand as second arg" << std::endl; exit(1);
        }
    }
    else if (mnemonic == "ldi.w") {
        if (ops.size() < 2) { std::cerr << "Error: ldi.w requires 2 operands" << std::endl; exit(1); }
        rd = ops[0].reg;
        rs2_imm = ops[1].imm;
        mode = 1; 
    }
    else if (mnemonic == "mov.w") {
        if (ops.size() < 2) { std::cerr << "Error: mov.w requires 2 operands" << std::endl; exit(1); }
        rd = ops[0].reg;
        rs1 = ops[1].reg;
        mode = 0;
    }
    else if (opcode >= 16 && opcode <= 21) { // Branch/Jmp
        if (ops.size() < 1) { std::cerr << "Error: Branch/Jmp requires target" << std::endl; exit(1); }
        
        mode = 4; // Default to PC Relative
        
        if (mnemonic == "jmp" || mnemonic == "call") {
             if (ops[0].type == OperandType::REGISTER) {
                 mode = 0; rs1 = ops[0].reg; // JMP R1 (Register Indirect)
                 rs2_imm = 0;
             } else {
                 mode = 4; // Use PC Relative for Labels too
                 int64_t target = ops[0].imm;
                 // PC during execution will be CurrentAddress + 1
                 int64_t offset = target - (currentAddress + 1); 
                 rs2_imm = offset;
             }
        } else {
             // BEQ, BNE...
             mode = 4; // PC Relative
             int64_t target = ops[0].imm;
             int64_t offset = target - (currentAddress + 1); 
             rs2_imm = offset;
        }
    }
    else if (opcode == 22) { } // RET
    else if (opcode == 23) { // MSR
        rs1 = ops[0].reg; // MSR r1
    }
    else if (opcode == 24) { // MRS
        rd = ops[0].reg; // MRS r1
    }
    // Cognitive Instructions
    else if (opcode == 26 || opcode == 29) { // CNS.W (26), SAT.ADD (29)
        // cns.w rd, rs1, rs2
        if (ops.size() < 3) { std::cerr << "Error: " << mnemonic << " requires 3 operands" << std::endl; exit(1); }
        rd = ops[0].reg;
        rs1 = ops[1].reg;
        // Rs2 or Imm? Spec says Register mostly? 
        // Let's support both if ISA allows. Opcode 26/29 are generic enough.
        // Assuming encoding matches Arithmetic (Mode 0/1).
        if (ops[2].type == OperandType::IMMEDIATE) {
             mode = 1; rs2_imm = ops[2].imm;
        } else {
             mode = 0; rs2_imm = ops[2].reg;
        }
    }
    else if (opcode == 27) { // DEC.W
        // dec.w rd, rs1 (mask is 0?) OR dec.w rd, rs1, mask
        // Spec said DEC.W Rd, Rs1.
        if (ops.size() < 2) { std::cerr << "Error: dec.w requires at least 2 operands" << std::endl; exit(1); }
        rd = ops[0].reg;
        rs1 = ops[1].reg;
        if (ops.size() >= 3) {
            if (ops[2].type == OperandType::IMMEDIATE) {
                mode = 1; rs2_imm = ops[2].imm;
            } else {
                mode = 0; rs2_imm = ops[2].reg;
            }
        } else {
            // Default mask 0
            mode = 1; rs2_imm = 0;
        }
    }
    else if (opcode == 28) { // POP.T
        // pop.t rd, rs1
        if (ops.size() < 2) { std::cerr << "Error: pop.t requires 2 operands" << std::endl; exit(1); }
        rd = ops[0].reg;
        rs1 = ops[1].reg;
        mode = 0;
    }
    else if (opcode == 30) { // VEC_CNS
        // vec.cns pd, ps1, ps2
        if (ops.size() < 3) { std::cerr << "Error: Vector op requires 3 operands" << std::endl; exit(1); }
        rd = ops[0].reg;
        rs1 = ops[1].reg;
        rs2_imm = ops[2].reg; 
        mode = 0;
    }
    else if (opcode == 31) { // VEC.POP
        // vec.pop rd, ps1
        if (ops.size() < 2) { std::cerr << "Error: vec.pop requires 2 operands" << std::endl; exit(1); }
        rd = ops[0].reg;
        rs1 = ops[1].reg;
        mode = 0;
    }


    TernaryWord word;
    word.SetSlice(21, 6, opcode);
    word.SetSlice(18, 3, mode);
    word.SetSlice(14, 4, rd);
    word.SetSlice(10, 4, rs1);
    word.SetSlice(0, 10, rs2_imm);
    
    // Relocation Generation
    // Check operands for symbols
    for(const auto& op : ops) {
        if (!op.symbol.empty()) {
            bool needsReloc = false;
            std::string type = "ABS";
            
            // If External (not in symbol table) -> Always Reloc
            // Note: ParseOperands checks symbolTable but we can re-check?
            // Or rely on logic:
            if (op.symbol == "loop" || op.symbol == "main") { 
                // Hack: Local symbols we know?
                // Real logic:
                // If Mode 4 (PCR) and defined in same section -> No Reloc.
                // If Mode 4 and Undefined or Other Section -> Reloc PCR.
                // If Absolute (Mode != 4) -> Reloc ABS.
            }
            
            bool isDefined = (symbolTable.count(op.symbol) > 0);
            
            if (mode == 4) {
                 if (!isDefined) {
                     needsReloc = true; type = "PCR";
                 } 
                 // If defined, ParseOperands calc offset.
            } else {
                 // Absolute use (LDI, etc)
                 needsReloc = true; type = "ABS";
            }
            
            if (needsReloc) {
                Relocation reloc;
                reloc.offset = currentSection->data.size(); // Current Word Index
                reloc.symbol = op.symbol;
                reloc.type = type;
                reloc.section = currentSection->name;
                result.relocations.push_back(reloc);
            }
        }
    }

    return word;
}
