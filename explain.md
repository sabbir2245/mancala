Mancala AI Implementation - Detailed Explanation
Table of Contents

    Introduction

    Mancala Game Rules

    Adversarial Search Theory

    Alpha-Beta Pruning

    Heuristic Evaluation Functions

    Code Structure and Implementation

    Experimental Framework

Introduction

This document provides a comprehensive explanation of a Mancala game implementation with AI players using adversarial search techniques. The code implements multiple AI players with different heuristic evaluation functions and compares their performance through round-robin tournaments at various search depths.

The implementation demonstrates:

    Complete Mancala game rules

    Multiple heuristic evaluation functions

    Alpha-beta pruning for efficient game tree search

    Move ordering for improved pruning efficiency

    Experimental framework for comparing AI strategies

Mancala Game Rules
Board Setup

Mancala is a two-player board game played on a board with:

    12 small pits (6 on each side)

    2 larger pits called "stores" (one for each player)

Initial setup:

    Each small pit contains 4 stones

    Both stores start with 0 stones

Game Layout (From Player 1's Perspective)
text

Player 2's Store (p2storage)  ← Index 13
Player 2's Side (indices 7-12)
  [7] [8] [9] [10] [11] [12]
Player 1's Side (indices 0-5)
  [0] [1] [2] [3] [4] [5]
Player 1's Store (p1storage)  ← Index 6

Turn Mechanics

A player's turn consists of:

    Selecting a pit on their side with at least one stone

    Sowing: Removing all stones from that pit and distributing them counter-clockwise, one per pit

    Special rules apply based on where the last stone lands

Sowing Rules

    Skip opponent's store: When sowing, you skip the opponent's store

    Land in your own store: You get an extra turn

    Land in an empty pit on your side: If the last stone lands in an empty pit on your side, and the opposite pit on the opponent's side has stones, you capture both the last stone and all stones in the opposite pit

Game End

The game ends when all pits on one player's side are empty. The remaining stones on the other player's side are added to their store. The player with more stones in their store wins.
Adversarial Search Theory
What is Adversarial Search?

Adversarial search is a problem-solving technique used in games with two or more players where the opponents have conflicting goals. It's fundamental to game-playing AI and forms the basis of most game engines.
Key Concepts
1. Game Tree

A game tree represents all possible game states and moves. The tree starts at the current state (root), with each level representing a player's turn, and branches representing possible moves.
text

                    [Root State]
                   /     |     \
              Move 1  Move 2  Move 3
               /         |         \
          State 1    State 2    State 3
           /  \       /  \       /  \
        ...   ...  ...   ...  ...   ...

2. Minimax Algorithm

Minimax is the foundational algorithm for adversarial search. It assumes both players play optimally:

    Maximizing Player (AI): Tries to maximize the score

    Minimizing Player (Opponent): Tries to minimize the score

The algorithm explores the game tree recursively:

    At MAX nodes (AI's turn), choose the move with the highest value

    At MIN nodes (Opponent's turn), choose the move with the lowest value

python

def minimax(state, depth, maximizing_player):
    if depth == 0 or game_over:
        return evaluate(state)
    
    if maximizing_player:
        value = -∞
        for each move in get_moves():
            value = max(value, minimax(state_after_move, depth-1, False))
        return value
    else:
        value = +∞
        for each move in get_moves():
            value = min(value, minimax(state_after_move, depth-1, True))
        return value

3. Depth-Limited Search

Since game trees can be enormous (branching factor × depth), we limit the search depth and use a heuristic evaluation function to estimate the value of non-terminal states.
Alpha-Beta Pruning
The Problem

Minimax requires exploring the entire game tree up to the given depth. With a branching factor of ~6-8 and depth of 8, we could explore 6^8 ≈ 1.7 million nodes, making it computationally expensive.
The Solution: Alpha-Beta Pruning

Alpha-beta pruning eliminates branches that cannot possibly affect the final decision, dramatically reducing the search space.
Key Concepts

    α (alpha): The best (highest) value found so far for the MAX player

    β (beta): The best (lowest) value found so far for the MIN player

Pruning Rules

    If α ≥ β: Prune the remaining branches

    MAX node: Update α = max(α, child_value)

    MIN node: Update β = min(β, child_value)

Example
text

         MAX
        /    \
       /      \
     MIN      MIN
    /  \      /  \
   3   5     2   Pruned!
   
At MIN node 1: returns 3 (min of 3,5)
At MIN node 2: sees first child value = 2
Since α = 3 (from MAX) and β = 2 (from MIN)
β ≤ α, so prune the second child!

Benefits

    Optimal: Still returns the same result as minimax

    Efficient: Can reduce search space by up to 50% with good move ordering

    Scalable: Allows deeper search with same computational resources

Implementation Details
cpp

double alphaBeta(MancalaBoard board, int depth, double alpha, double beta,
                 int currentPlayer, int rootPlayer) {
    if (depth == 0 || board.isGameOver())
        return heuristic(board);
    
    // Move ordering for better pruning
    auto orderedMoves = getOrderedMoves(board, currentPlayer);
    
    bool maximizing = (currentPlayer == rootPlayer);
    double best = maximizing ? -1e18 : 1e18;
    
    for (auto [move, _] : orderedMoves) {
        // Make move and recurse
        double score = alphaBeta(new_board, depth-1, alpha, beta, nextPlayer);
        
        if (maximizing) {
            best = max(best, score);
            alpha = max(alpha, best);
        } else {
            best = min(best, score);
            beta = min(beta, best);
        }
        
        // Prune if alpha >= beta
        if (beta <= alpha)
            break;
    }
    return best;
}

Move Ordering

The effectiveness of alpha-beta pruning heavily depends on move ordering. If good moves are explored first:

    At MAX nodes: Higher-valued moves cause more pruning

    At MIN nodes: Lower-valued moves cause more pruning

The code implements move ordering based on:

    Extra turns (bonus +10)

    Captured stones (bonus +2 per stone)

    Pit position (bonus +0.5 per index)

Heuristic Evaluation Functions
Purpose

Since the search cannot explore to the end of the game, we need a function to evaluate game states and estimate their value.
Heuristic 1: Simple Storage Difference
cpp

evaluate = storageDiff(board, player)

    Formula: Player's stones in store - Opponent's stones in store

    Strength: Simple, fast, captures the primary goal

    Weakness: Ignores strategic considerations

Heuristic 2: Storage + Side Stones
cpp

evaluate = 1.0 * storageDiff + 0.5 * sideDiff

    Formula: Storage difference + weighted side stone difference

    Strength: Considers both immediate score and future potential

    Weakness: Side stones can be misleading (might favor the opponent)

Heuristic 3: Add Extra Turn Bonus
cpp

evaluate = 1.0 * storageDiff + 0.3 * sideDiff + 2.0 * extraTurn

    Formula: Heuristic 2 + bonus for getting an extra turn

    Strength: Values the strategic importance of extra turns

    Weakness: Extra turn bonus might be overvalued or undervalued

Heuristic 4: Include Captured Stones
cpp

evaluate = 1.0 * storageDiff + 0.3 * sideDiff + 1.5 * extraTurn + 1.0 * captured

    Formula: Heuristic 3 + bonus for captured stones

    Strength: Rewards immediate capture opportunities

    Weakness: Doesn't consider which stones were captured

Heuristic 5: Stones Close to Storage
cpp

evaluate = 1.0 * storageDiff + 0.2 * stonesCloseToStorage + 3.0 * extraTurn

    Formula: Storage difference + proximity bonus + extra turn bonus

    Concept: Stones closer to storage are more valuable as they'll reach the store sooner

    Strength: Forward-looking, considers positional advantage

    Weakness: Proximity calculation is heuristic and might not always be accurate

Heuristic 6: Win Proximity
cpp

evaluate = 2.0 * winProximity + 0.3 * sideDiff + 1.5 * captured

    Formula: Inverse distance to winning + side difference + captured stones

    Concept: Focuses on how close each player is to winning

    Strength: Directly considers the end game

    Weakness: Can be overly aggressive, ignoring strategic positioning

Understanding the Weight Values

All heuristics use tuned weights that were likely determined through experimentation:

    Heuristic 1: {1.0, 0, 0, 0} - Only storage matters

    Heuristic 2: {1.0, 0.5, 0, 0} - Storage more important than side stones

    Heuristic 3: {1.0, 0.3, 2.0, 0} - Extra turn is highly valued

    Heuristic 4: {1.0, 0.3, 1.5, 1.0} - Balanced approach

    Heuristic 5: {1.0, 0.2, 3.0, 0} - Extra turns are very important

    Heuristic 6: {2.0, 0.3, 1.5, 0} - Win proximity is weighted heavily

Code Structure and Implementation
1. MancalaBoard Class

The core game state implementation.
cpp

class MancalaBoard {
public:
    int p1side[6];     // Player 1's pits
    int p1storage;     // Player 1's store
    int p2side[6];     // Player 2's pits  
    int p2storage;     // Player 2's store

Key Methods:
move(int player, int binIndex, int &capturedStonesOut)

Executes a move and updates the board state:

    Validates the move

    Removes stones from the selected pit

    Sows stones counter-clockwise

    Applies capture rules if applicable

    Checks for game end

    Returns whether the player gets an extra turn

checkAndApplyGameEnd()

    Called after every move

    Checks if either player's side is empty

    If game over, adds remaining stones to stores

    Resets all pits to 0

getValidMoves(int player)

    Returns all pit indices on the specified player's side that have stones

    Used for both move generation and move ordering

2. Heuristics Class

Contains all heuristic evaluation functions and their registry.
cpp

class Heuristics {
    static double heuristic1(...) { return storageDiff(b, player); }
    static double heuristic2(...) { /* weighted sum */ }
    // ... more heuristics
    
    struct Entry {
        string name;
        HeuristicFunc func;
        Weights weights;
    };
    
    static vector<Entry> registry() { /* returns all heuristics */ }
};

Design Pattern: This uses a registry pattern to maintain a collection of heuristics that can be easily iterated over for experiments.
3. AIPlayer Class

Implements the AI search algorithm.
cpp

class AIPlayer {
    AIConfig config;  // Contains heuristic, weights, and depth
    
    vector<pair<int, double>> getOrderedMoves(board, player) {
        // Scores moves for better alpha-beta pruning
        // Orders by: extra turns > captured stones > position
    }
    
    double alphaBeta(board, depth, alpha, beta, currentPlayer, rootPlayer, ...) {
        // Main search function with alpha-beta pruning
        // Uses move ordering for efficiency
    }
    
    int getBestMove(board, player) {
        // Returns the best move found by alpha-beta search
        // Handles ties randomly
    }
};

4. GameSimulator Class

Manages game play and experiments.
cpp

class GameSimulator {
    static MatchResult playMatch(AIPlayer &p1, AIPlayer &p2, int startingPlayer) {
        // Plays a complete game between two AIs
        // Alternates turns until game over
    }
    
    static ExperimentStats runExperiment(int totalGames, cfg1, cfg2) {
        // Runs multiple games with configurable number
        // Alternates starting player
        // Collects statistics
    }
};

5. Main Function
cpp

int main() {
    int GAMES_PER_MATCHUP = 10;  // Games between each heuristic pair
    vector<int> depths = {4, 6, 8};  // Search depths to test
    
    // Load all heuristics from registry
    vector<Heuristics::Entry> heuristics = Heuristics::registry();
    
    // Round-robin tournament:
    for (int depth : depths) {
        for (int i = 0; i < configs.size(); ++i) {
            for (int j = i + 1; j < configs.size(); ++j) {
                // Run games and collect results
            }
        }
    }
}

Experimental Framework
Round-Robin Tournament

The code implements a round-robin tournament where:

    Each heuristic is tested against every other heuristic

    Tests are run at depths 4, 6, and 8

    Each matchup plays GAMES_PER_MATCHUP games (10 in the current version)

    Starting player alternates between games

Metrics Collected

For each matchup:

    Wins: Number of games won by each player

    Draws: Games ending in a tie

    Win Percentage: Wins / Total Games × 100

Output Format
text

heuristic-1 vs heuristic-2 -> wins: 4 / 6, draws: 0, 
heuristic-1 win perc: 40%, heuristic-2 win perc: 60%

Analysis Tips

When analyzing results, consider:

    Search Depth: Deeper search generally improves performance but increases computation

    Heuristic Components: Which features (storage, side stones, extra turns, captures) are most important?

    Performance Consistency: Does a heuristic perform well against all opponents?

    Computational Cost: Faster heuristics might allow deeper search

Technical Considerations
Performance Optimizations

    Pass by const reference for board objects in heuristic functions

    Move ordering improves alpha-beta pruning effectiveness

    Early termination in alpha-beta when β ≤ α

    Copy-on-write: Board is copied only when needed for search

Memory Management

    Board state is small (arrays of 6 ints × 2 + 2 ints)

    Recursive search creates copies, but they're on the stack

    No dynamic allocation in the search loop

Potential Improvements

    Transposition tables to cache already-evaluated states

    Iterative deepening to use time more effectively

    Quiescence search to evaluate noisy positions more accurately

    Parallel search using multiple cores

    Opening book for common positions

    Endgame database for precise evaluation in late game

Common Pitfalls

    Integer division in win percentage calculation: (p1wins * 100) / totalGames - ensure totalGames is not 0

    Random number seeding: Only seeded once at program start

    Board copying: Ensure the copy constructor works correctly

    Recursion depth: Could overflow for very deep searches (not an issue here)

Conclusion

This Mancala AI implementation demonstrates the application of:

    Game theory through adversarial search

    Optimization techniques via alpha-beta pruning

    Machine learning concepts through heuristic weight tuning

    Software engineering patterns like registry pattern

The code provides a solid foundation for experimenting with and comparing different AI strategies in the Mancala domain. The round-robin tournament framework allows systematic evaluation of which heuristics perform best at different search depths.
Future Research Directions

    Reinforcement Learning: Learn heuristic weights through self-play

    Monte Carlo Tree Search: Different search paradigm for large state spaces

    Neural Networks: Replace hand-crafted heuristics with learned evaluations

    Game Tree Complexity: Study the exact branching factor and search efficiency

Appendix: Quick Reference
Board Indices
text

Index 0-5:   Player 1's pits
Index 6:     Player 1's store
Index 7-12:  Player 2's pits
Index 13:    Player 2's store

Key Constants

    Initial stones per pit: 4

    Total stones per player: 24

    Total stones in game: 48

    Maximum stones per pit: Variable (up to 48)

Heuristic Summary Table
Heuristic	Components	Weight Vector	Key Feature
H1	Storage	{1,0,0,0}	Pure storage
H2	Storage + Side	{1,0.5,0,0}	Balanced
H3	Storage + Side + Extra	{1,0.3,2,0}	Extra turn bonus
H4	Storage + Side + Extra + Capture	{1,0.3,1.5,1}	Full feature set
H5	Storage + Proximity + Extra	{1,0.2,3,0}	Positional awareness
H6	Win proximity + Side + Capture	{2,0.3,1.5,0}	Endgame focused