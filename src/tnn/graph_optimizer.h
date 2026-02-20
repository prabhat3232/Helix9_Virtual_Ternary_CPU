#pragma once
#include "helix_runtime.h"
#include <vector>
#include <memory>
#include "../memory.h"

namespace Helix {

enum class IROpType {
    LOAD_WEIGHTS,
    VMMUL,
    VSIGN,
    VMMSGN, // Fused
    VCLIP
};

struct GraphNode {
    IROpType type;
    int64_t input_addr;
    int64_t weight_addr; // (If applicable)
    int64_t output_addr;
    int dim_input;
    int dim_output;
    int64_t imm_val; // For scaling/clipping limits
    
    std::string debug_name;
    
    GraphNode* prev_node = nullptr;
    GraphNode* next_node = nullptr;

    GraphNode() : input_addr(0), weight_addr(0), output_addr(0), dim_input(0), dim_output(0), imm_val(0) {}
};

class GraphCompiler {
public:
    // Pass 1: Translate TNNModel semantic layers into Intermediate Representation
    static std::vector<GraphNode> BuildIR(const TNNModel& model);
    
    // Pass 2: Scans GraphNode list and fuses compatible nodes (e.g. VMMUL -> VSIGN = VMMSGN)
    static std::vector<GraphNode> OptimizeIR(const std::vector<GraphNode>& ir);
    static bool CanFuse(const GraphNode& a, const GraphNode& b);
    
    // Pass 3: Iterates through IR and determines exactly where in memory (0x2000+) tensors will live.
    static void PlanMemory(std::vector<GraphNode>& ir, TernaryMemory& mem, const TNNModel& model, int64_t input_base_addr = 0x1000, int64_t tensor_base_addr = 0x2000);
    
    // Pass 4: Translates IR + Memory Plan into Helix9 Binary Opcodes and flashes them to 0x4000
    static void GenerateProgram(const std::vector<GraphNode>& ir, TernaryMemory& mem, int64_t program_base = 0x4000);
};

} // namespace Helix
