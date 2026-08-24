Here is a Markdown guide containing 5 common lab modification challenges designed specifically around your Mancala Minimax/Alpha-Beta codebase, along with detailed code solutions.
Mancala Codebase Extension Lab Challenges
Challenge 1: Implement Dynamic Depth (Iterative Deepening with Time Limits)

Problem: In real competition, fixed-depth search can cause timeouts or underutilize remaining turn time. Modify the search engine to use Iterative Deepening Search (IDS) with a configurable time limit (e.g., 50 milliseconds per turn) instead of a fixed search depth.

Solution:
Add <chrono> based time-checking to alphaBeta and wrap getBestMove in an iterative loop:
C++

#include <chrono>

class AIPlayer {
public:
    AIConfig config;
    chrono::time_point<chrono::steady_clock> startTime;
    double timeLimitMs = 50.0; // 50ms time budget per move
    bool timeOut = false;

    // Modified Alpha-Beta checking time limit
    double alphaBeta(MancalaBoard board, int depth, double alpha, double beta,
                     int currentPlayer, int rootPlayer, bool leadingExtraTurn,
                     int leadingCaptured) {
        
        // Time check
        auto now = chrono::steady_clock::now();
        if (chrono::duration<double, milli>(now - startTime).count() >= timeLimitMs) {
            timeOut = true;
            return 0; // Abort early
        }

        if (depth == 0 || board.isGameOver())
            return config.heuristic(board, rootPlayer, leadingExtraTurn, leadingCaptured, config.weights);

        vector<int> moves = board.getValidMoves(currentPlayer);
        if (moves.empty())
            return config.heuristic(board, rootPlayer, leadingExtraTurn, leadingCaptured, config.weights);

        bool maximizing = (currentPlayer == rootPlayer);
        double best = maximizing ? -1e18 : 1e18;
        auto orderedMoves = getOrderedMoves(board, currentPlayer);

        for (auto [m, f] : orderedMoves) {
            MancalaBoard child = board;
            int captured = 0;
            bool extra = child.move(currentPlayer, m, captured);
            int nextPlayer = extra ? currentPlayer : (currentPlayer == 1 ? 2 : 1);

            double score = alphaBeta(child, depth - 1, alpha, beta, nextPlayer, rootPlayer, extra, captured);
            if (timeOut) return 0; // Return immediately on timeout

            if (maximizing) {
                best = max(best, score);
                alpha = max(alpha, best);
            } else {
                best = min(best, score);
                beta = min(beta, best);
            }
            if (beta <= alpha) break;
        }
        return best;
    }

    // Iterative Deepening move selector
    int getBestMoveIDS(const MancalaBoard &board, int player, double maxTimeMs) {
        timeLimitMs = maxTimeMs;
        startTime = chrono::steady_clock::now();
        timeOut = false;

        int currentBestMove = board.getValidMoves(player)[0];
        
        // Iteratively search from depth 1 onwards until time runs out
        for (int d = 1; d <= 20; ++d) {
            int moveAtDepth = -1;
            double bestScore = -1e18;
            auto orderedMoves = getOrderedMoves(board, player);

            for (auto [m, f] : orderedMoves) {
                MancalaBoard child = board;
                int captured = 0;
                bool extra = child.move(player, m, captured);
                int nextPlayer = extra ? player : (player == 1 ? 2 : 1);

                double score = alphaBeta(child, d - 1, -1e18, 1e18, nextPlayer, player, extra, captured);

                if (timeOut) break; // Use results from previous completed depth

                if (score > bestScore) {
                    bestScore = score;
                    moveAtDepth = m;
                }
            }

            if (!timeOut && moveAtDepth != -1) {
                currentBestMove = moveAtDepth; // Update only if full depth was evaluated safely
            } else {
                break;
            }
        }
        return currentBestMove;
    }
};

Challenge 2: Transposition Table Integration (State Caching)

Problem: The tree search re-evaluates identical board states reached through different move permutations. Add a Transposition Table using std::unordered_map to store computed minimax values and speed up search.

Solution:
Create a custom hash/state-key generator for MancalaBoard and check table hits inside alphaBeta:
C++

#include <unordered_map>

struct BoardStateKey {
    array<int, 6> p1;
    array<int, 6> p2;
    int p1store, p2store;
    int currentPlayer;

    bool operator==(const BoardStateKey &other) const {
        return p1 == other.p1 && p2 == other.p2 && 
               p1store == other.p1store && p2store == other.p2store && 
               currentPlayer == other.currentPlayer;
    }
};

// Hash function for state key
namespace std {
    template<> struct hash<BoardStateKey> {
        size_t operator()(const BoardStateKey& k) const {
            size_t h = 0;
            for(int x : k.p1) h ^= hash<int>()(x) + 0x9e3779b9 + (h << 6) + (h >> 2);
            for(int x : k.p2) h ^= hash<int>()(x) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= hash<int>()(k.p1store) + hash<int>()(k.p2store) + hash<int>()(k.currentPlayer);
            return h;
        }
    };
}

enum TTFlag { EXACT, LOWERBOUND, UPPERBOUND };

struct TTEntry {
    int depth;
    double value;
    TTFlag flag;
};

// Transposition Table global/class member
unordered_map<BoardStateKey, TTEntry> transpositionTable;

BoardStateKey makeKey(const MancalaBoard &b, int player) {
    BoardStateKey k;
    for(int i=0; i<6; ++i) { k.p1[i] = b.p1side[i]; k.p2[i] = b.p2side[i]; }
    k.p1store = b.p1storage; k.p2store = b.p2storage;
    k.currentPlayer = player;
    return k;
}

// Integrated TT Lookup in alphaBeta
double alphaBetaWithTT(MancalaBoard board, int depth, double alpha, double beta,
                        int currentPlayer, int rootPlayer, bool leadingExtra, int leadingCap, const AIConfig &config) {
    
    double alphaOrig = alpha;
    BoardStateKey key = makeKey(board, currentPlayer);

    if (transpositionTable.count(key) && transpositionTable[key].depth >= depth) {
        TTEntry entry = transpositionTable[key];
        if (entry.flag == EXACT) return entry.value;
        else if (entry.flag == LOWERBOUND) alpha = max(alpha, entry.value);
        else if (entry.flag == UPPERBOUND) beta = min(beta, entry.value);

        if (alpha >= beta) return entry.value;
    }

    if (depth == 0 || board.isGameOver())
        return config.heuristic(board, rootPlayer, leadingExtra, leadingCap, config.weights);

    vector<int> moves = board.getValidMoves(currentPlayer);
    bool maximizing = (currentPlayer == rootPlayer);
    double best = maximizing ? -1e18 : 1e18;

    for (int m : moves) {
        MancalaBoard child = board;
        int captured = 0;
        bool extra = child.move(currentPlayer, m, captured);
        int nextPlayer = extra ? currentPlayer : (currentPlayer == 1 ? 2 : 1);

        double score = alphaBetaWithTT(child, depth - 1, alpha, beta, nextPlayer, rootPlayer, extra, captured, config);

        if (maximizing) best = max(best, score);
        else best = min(best, score);

        if (maximizing) alpha = max(alpha, best);
        else beta = min(beta, best);

        if (beta <= alpha) break;
    }

    // Store in TT
    TTEntry entry;
    entry.value = best;
    entry.depth = depth;
    if (best <= alphaOrig) entry.flag = UPPERBOUND;
    else if (best >= beta) entry.flag = LOWERBOUND;
    else entry.flag = EXACT;

    transpositionTable[key] = entry;
    return best;
}

Challenge 3: Alternate Board Rules (Avalanche Mancala / Multi-Lap Sowing)

Problem: Modify the move logic inside MancalaBoard to support Avalanche Mancala rules: If the last stone drops into a non-empty pit on your side, you pick up all stones from that pit and continue sowing seamlessly.

Solution:
Update the movement algorithm inside MancalaBoard::move to include an avalanche loop:
C++

bool moveAvalanche(int player, int binIndex, int &capturedStonesOut) {
    capturedStonesOut = 0;
    if (binIndex < 0 || binIndex > 5) return false;

    int stones = (player == 1) ? p1side[binIndex] : p2side[binIndex];
    if (stones == 0) return false;

    if (player == 1) p1side[binIndex] = 0;
    else p2side[binIndex] = 0;

    int currPos = (player == 1) ? binIndex : (binIndex + 7);

    while (true) {
        while (stones > 0) {
            currPos = (currPos + 1) % 14;

            if (player == 1 && currPos == 13) continue;
            if (player == 2 && currPos == 6) continue;

            if (currPos < 6) p1side[currPos]++;
            else if (currPos == 6) p1storage++;
            else if (currPos < 13) p2side[currPos - 7]++;
            else if (currPos == 13) p2storage++;

            stones--;
        }

        // Check extra turn landing condition
        if ((player == 1 && currPos == 6) || (player == 2 && currPos == 13)) {
            checkAndApplyGameEnd();
            return true; // Extra turn granted
        }

        // Check if landed in a pit on player's own side with > 1 stones (Avalanche trigger)
        bool landedOwnPit = (player == 1 && currPos >= 0 && currPos < 6 && p1side[currPos] > 1) ||
                            (player == 2 && currPos >= 7 && currPos < 13 && p2side[currPos - 7] > 1);

        if (landedOwnPit) {
            // Pick up new batch of stones and continue sowing loop
            if (player == 1) {
                stones = p1side[currPos];
                p1side[currPos] = 0;
            } else {
                stones = p2side[currPos - 7];
                p2side[currPos - 7] = 0;
            }
        } else {
            break; // Stop sowing if landed in store, empty pit, or opponent pit
        }
    }

    // Process normal capture rules for non-avalanche end
    if (player == 1 && currPos >= 0 && currPos < 6 && p1side[currPos] == 1) {
        int oppPos = 5 - currPos;
        if (p2side[oppPos] > 0) {
            capturedStonesOut = p1side[currPos] + p2side[oppPos];
            p1storage += capturedStonesOut;
            p1side[currPos] = 0; p2side[oppPos] = 0;
        }
    } else if (player == 2 && currPos >= 7 && currPos < 13 && p2side[currPos - 7] == 1) {
        int oppPos = 5 - (currPos - 7);
        if (p1side[oppPos] > 0) {
            capturedStonesOut = p2side[currPos - 7] + p1side[oppPos];
            p2storage += capturedStonesOut;
            p2side[currPos - 7] = 0; p1side[oppPos] = 0;
        }
    }

    checkAndApplyGameEnd();
    return false;
}

Challenge 4: Genetic Algorithm Weight Tuning

Problem: Replace the static heuristic weights in TunedWeights with an automated optimization script. Write a genetic optimization loop that evolves Weights via tournament selection and mutation.

Solution:
Create a candidate genome wrapper and an evolutionary training loop:
C++

#include <random>

struct Individual {
    Weights weights;
    int score = 0;
};

// Generate random weight configuration
Weights randomWeights() {
    static mt19937 rng(1337);
    uniform_real_distribution<double> dist(0.0, 3.0);
    return {dist(rng), dist(rng), dist(rng), dist(rng)};
}

// Mutate weight array
Weights mutate(const Weights &w) {
    static mt19937 rng(time(nullptr));
    normal_distribution<double> noise(0.0, 0.2);
    Weights newW = w;
    for(int i = 0; i < 4; ++i) {
        newW[i] = max(0.0, newW[i] + noise(rng));
    }
    return newW;
}

Weights evolveBestWeights(HeuristicFunc func, int generations = 10, int popSize = 8) {
    vector<Individual> pop(popSize);
    for (auto &ind : pop) ind.weights = randomWeights();

    for (int gen = 0; gen < generations; ++gen) {
        // Reset scores
        for (auto &ind : pop) ind.score = 0;

        // Round Robin evaluations at depth 4
        for (int i = 0; i < popSize; ++i) {
            for (int j = i + 1; j < popSize; ++j) {
                AIConfig cfg1{"p1", func, pop[i].weights, 4};
                AIConfig cfg2{"p2", func, pop[j].weights, 4};

                auto stats = GameSimulator::runExperiment(2, cfg1, cfg2);
                pop[i].score += stats.p1wins * 3 + stats.draws;
                pop[j].score += stats.p2wins * 3 + stats.draws;
            }
        }

        // Sort by fitness
        sort(pop.begin(), pop.end(), [](const Individual &a, const Individual &b) {
            return a.score > b.score;
        });

        cout << "Gen " << gen << " Best Score: " << pop[0].score 
             << " | Weights: [" << pop[0].weights[0] << ", " << pop[0].weights[1] << "]\n";

        // Breed top half to replace bottom half
        for (int i = popSize / 2; i < popSize; ++i) {
            pop[i].weights = mutate(pop[i - popSize / 2].weights);
        }
    }
    return pop[0].weights;
}

Challenge 5: Comprehensive Experimentation Framework Export

Problem: Extend main() to export tournament results directly into CSV format, tracking move counts, average game length, total captures, and win percentages per heuristic pairing for plotting.

Solution:
Wrap game simulation statistics in an extended metrics struct and generate standard CSV files:
C++

struct ExtendedMatchResult {
    MatchResult match;
    int totalMoves = 0;
    int totalCaptures = 0;
};

ExtendedMatchResult playDetailedMatch(AIPlayer &p1, AIPlayer &p2, int startingPlayer) {
    MancalaBoard board;
    int currentPlayer = startingPlayer;
    ExtendedMatchResult res;

    while (!board.isGameOver()) {
        AIPlayer &active = (currentPlayer == 1) ? p1 : p2;
        int chosenMove = active.getBestMove(board, currentPlayer);
        if (chosenMove == -1) break;

        int captured = 0;
        bool extra = board.move(currentPlayer, chosenMove, captured);
        
        res.totalMoves++;
        res.totalCaptures += captured;

        if (!extra) currentPlayer = (currentPlayer == 1) ? 2 : 1;
    }

    res.match.p1storage = board.p1storage;
    res.match.p2storage = board.p2storage;
    res.match.winner = (board.p1storage > board.p2storage) ? 1 : 
                       (board.p2storage > board.p1storage ? 2 : 0);
    return res;
}

void exportExperimentCSV(string filename, int depth, int gamesPerMatchup) {
    ofstream csv(filename);
    csv << "Depth,Heuristic1,Heuristic2,H1_Wins,H2_Wins,Draws,AvgMoves,AvgCaptures\n";

    auto heuristics = Heuristics::registry();
    for (size_t i = 0; i < heuristics.size(); ++i) {
        for (size_t j = i + 1; j < heuristics.size(); ++j) {
            AIConfig cfg1{heuristics[i].name, heuristics[i].func, heuristics[i].weights, depth};
            AIConfig cfg2{heuristics[j].name, heuristics[j].func, heuristics[j].weights, depth};

            int h1Wins = 0, h2Wins = 0, draws = 0;
            int totalMovesSum = 0, totalCapturesSum = 0;

            for (int g = 0; g < gamesPerMatchup; ++g) {
                AIPlayer p1(cfg1), p2(cfg2);
                int starter = (g % 2 == 0) ? 1 : 2;
                
                auto res = playDetailedMatch(p1, p2, starter);
                totalMovesSum += res.totalMoves;
                totalCapturesSum += res.totalCaptures;

                if (res.match.winner == 1) h1Wins++;
                else if (res.match.winner == 2) h2Wins++;
                else draws++;
            }

            csv << depth << "," << cfg1.name << "," << cfg2.name << ","
                << h1Wins << "," << h2Wins << "," << draws << ","
                << (double)totalMovesSum / gamesPerMatchup << ","
                << (double)totalCapturesSum / gamesPerMatchup << "\n";
        }
    }
    csv.close();
    cout << "Exported experiment details to " << filename << endl;
}

Would you like help combining any of these modules into your existing codebase?
