#include "../src/cpu.h"
#include <iostream>
#include <fstream>
#include <random>
#include <iomanip>

// Configuration
const int MAX_TICKS = 100;
const int GOAL_POS = 10;
const int START_POS = 0;
const double NOISE_PROB = 0.3; // 30% Noise

int main() {
    std::cout << "--- Helix9 1D Seeker Experiment ---" << std::endl;
    
    // 1. Setup CPU & Memory
    TernaryMemory mem;
    // Load seeker.ht (Assumes it's linked/loaded at 0)
    // We need to link it first? Or just load object?
    // Cpu starts at 0.
    // Let's assume we link it to seeker.hx
    if (!mem.LoadExecutable("seeker.hx")) {
        std::cerr << "Failed to load seeker.hx" << std::endl;
        return 1;
    }
    
    Cpu cpu(mem);
    
    // 2. Setup Logging
    std::ofstream log("seeker_metrics.csv");
    log << "Tick,Pos,Goal,Input,Belief,Confidence,Action,Sparsity\n";
    
    // 3. Simulation State
    int agent_pos = START_POS;
    
    // Random Noise
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    
    // 4. Loop
    for (int t = 0; t < MAX_TICKS; t++) {
        // --- ENV: GENERATE INPUT ---
        int direction = 0;
        if (agent_pos < GOAL_POS) direction = 1;
        else if (agent_pos > GOAL_POS) direction = -1;
        
        // Apply Noise
        if (dis(gen) < NOISE_PROB) {
            direction = 0; // Signal Drop
        }
        
        // Write Input to 0x3100
        mem.Write(TernaryWord::FromInt64(12544), TernaryWord::FromInt64(direction));
        
        // --- AGENT: RUN CYCLE ---
        // Run until Halt. 
        // We need a way to run until next HALT.
        // Cpu::Run() runs N cycles.
        // We can loop Cpu::Step() until halted?
        cpu.halted = false;
        while (!cpu.halted) {
             cpu.Step();
        }
        
        // --- ENV: READ ACTION ---
        TernaryWord action_word = mem.Read(TernaryWord::FromInt64(12800)); // 0x3200
        int64_t action = action_word.ToInt64();
        
        // Helper: Belief & Confidence for logging
        TernaryWord belief = mem.Read(TernaryWord::FromInt64(12288)); // 0x3000
        TernaryWord conf_word = mem.Read(TernaryWord::FromInt64(13056)); // 0x3300
        int64_t conf = conf_word.ToInt64();
        
        // Update Position
        agent_pos += action;
        
        // Log
        log << t << "," << agent_pos << "," << GOAL_POS << "," 
            << direction << "," << belief.ToInt64() << "," 
            << conf << "," << action << "," 
            << "0" // Todo: Calculate Sparsity
            << "\n";
            
        std::cout << "Tick " << t << ": Pos=" << agent_pos 
                  << " In=" << direction 
                  << " Act=" << action 
                  << " Belief=" << belief.ToInt64() 
                  << " Conf=" << conf << std::endl;
                  
        if (agent_pos == GOAL_POS) {
            std::cout << ">>> GOAL REACHED! <<<" << std::endl;
            // Don't break, let's see if it holds position
        }
    }
    
    log.close();
    return 0;
}
