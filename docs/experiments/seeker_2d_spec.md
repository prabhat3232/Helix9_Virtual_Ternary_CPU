# 2D Seeker Experiment Specification (v1.0)

## Overview
A 2D Grid World where "Seeker" agents must navigate to find Food (Energy) and avoid Poison (Damage). This experiment validates the full Cognitive Runtime stack: Agent Lifecycle, Scheduler, Memory Isolation (Senses), Stability flux, and Reward reinforcement.

## Environment
*   **Dimensions**: 32x32 Grid (Wraparound / Toroidal).
*   **Entities**:
    *   **Empty**: 0
    *   **Food**: +1 (Green)
    *   **Poison**: -1 (Red)
    *   **Agent**: ID > 10 (Blue)
*   **Dynamics**:
    *   Food spawns randomly every N ticks.
    *   Poison spawns randomly (less frequent).
    *   Agents consume Food (Gain Health + Reward).
    *   Agents hit Poison (Lose Health + Negative Reward).
    *   Agents decay health slowly (Hunger).

## Agent Interface
The Agent interacts with the world via **Memory Pages** managed by the Kernel.

### 1. Input Page (Senses) - Page ID: `input_page_start`
The Environment writes 9 words to this page every tick (before Agent runs).
*   **Offset 0**: Local Cell (Under agent).
*   **Offset 1**: North
*   **Offset 2**: North-East
*   **Offset 3**: East
*   **Offset 4**: South-East
*   **Offset 5**: South
*   **Offset 6**: South-West
*   **Offset 7**: West
*   **Offset 8**: North-West

**Encoding**:
*   `+1`: Food
*   `-1`: Poison
*   `0`: Empty/Wall
*   `>1`: Other Agent

### 2. Output Page (Actions) - Page ID: `output_page_start`
The Agent writes 1 word to Offset 0 indicating desired move.
*   **0**: Stay
*   **1**: North
*   **2**: East
*   **3**: South
*   **4**: West
*   (All other values ignored/Stay)

### 3. Internal State (Brain) - Page ID: `belief_page_start`
Agents use this page to map Inputs to Outputs.
*   **Simple Hebbian**:
    *   Weights[Direction][Neighbor]
    *   `Score[Dir] = Sum(Input[Neighbor] * Weight[Dir][Neighbor])`
    *   Pick Dir with Max Score.
    *   **Learning**:
        *   If Reward > 0: Strengthen weights that matched Input.
        *   If Reward < 0: Weaken weights that matched Input.

## Execution Loop
1.  **Env Update**:
    *   Spawn/Despawn items.
    *   Write Inputs to all Agent Input Pages (Kernel `WriteSecure`).
2.  **Kernel Tick**:
    *   Scheduler runs Agents (Round-Robin).
    *   Agents Read Input -> Compute -> Write Output.
    *   Stability Monitor calculates Flux.
3.  **Physics Resolution**:
    *   Read Agent Output Pages.
    *   Move Agents (if valid).
    *   Check Collisions (Food/Poison).
4.  **Reward Injection**:
    *   If Food: `Kernel::Reward(Agent, +10)`.
    *   If Poison: `Kernel::Reward(Agent, -10)`.
    *   If Starve: `Kernel::Reward(Agent, -1)`.

## Metrics
*   **Survival Time**: Average ticks before death.
*   **Efficiency**: Food collected / Steps taken.
*   **Learning Curve**: Survival Time vs Generation.
