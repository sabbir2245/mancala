#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <functional>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

class MancalaBoard {
public:
    int p1side[6];
    int p1storage;
    int p2side[6];
    int p2storage;

    MancalaBoard() {
        for (int i = 0; i < 6; ++i) {
            p1side[i] = 4;
            p2side[i] = 4;
        }
        p1storage = 0;
        p2storage = 0;
    }

    bool move(int player, int binIndex, int& capturedStonesOut) {
        capturedStonesOut = 0;
        if (binIndex < 0 || binIndex > 5) return false;

        int stones = (player == 1) ? p1side[binIndex] : p2side[binIndex];
        if (stones == 0) return false;

        if (player == 1) p1side[binIndex] = 0;
        else p2side[binIndex] = 0;

        int currPos = (player == 1) ? binIndex : (binIndex + 7);

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

        bool extraTurn = (player == 1 && currPos == 6) || (player == 2 && currPos == 13);

        if (player == 1 && currPos >= 0 && currPos < 6 && p1side[currPos] == 1) {
            int oppPos = 5 - currPos;
            if (p2side[oppPos] > 0) {
                capturedStonesOut = p1side[currPos] + p2side[oppPos];
                p1storage += capturedStonesOut;
                p1side[currPos] = 0;
                p2side[oppPos] = 0;
            }
        } else if (player == 2 && currPos >= 7 && currPos < 13 && p2side[currPos - 7] == 1) {
            int oppPos = 5 - (currPos - 7);
            if (p1side[oppPos] > 0) {
                capturedStonesOut = p2side[currPos - 7] + p1side[oppPos];
                p2storage += capturedStonesOut;
                p2side[currPos - 7] = 0;
                p1side[oppPos] = 0;
            }
        }

        checkAndApplyGameEnd();
        return extraTurn;
    }

    bool isGameOver() const {
        int sumP1 = 0, sumP2 = 0;
        for (int i = 0; i < 6; ++i) {
            sumP1 += p1side[i];
            sumP2 += p2side[i];
        }
        return sumP1 == 0 || sumP2 == 0;
    }

    vector<int> getValidMoves(int player) const {
        vector<int> moves;
        const int* side = (player == 1) ? p1side : p2side;
        for (int i = 0; i < 6; ++i)
            if (side[i] > 0) moves.push_back(i);
        return moves;
    }

    int storageOf(int player) const { return player == 1 ? p1storage : p2storage; }
    int oppStorageOf(int player) const { return player == 1 ? p2storage : p1storage; }

    int sideStonesOf(int player) const {
        const int* side = (player == 1) ? p1side : p2side;
        int total = 0;
        for (int i = 0; i < 6; ++i) total += side[i];
        return total;
    }

    int oppSideStonesOf(int player) const {
        return sideStonesOf(player == 1 ? 2 : 1);
    }

private:
    void checkAndApplyGameEnd() {
        int sumP1 = 0, sumP2 = 0;
        for (int i = 0; i < 6; ++i) {
            sumP1 += p1side[i];
            sumP2 += p2side[i];
        }
        if (sumP1 == 0 || sumP2 == 0) {
            p1storage += sumP1;
            p2storage += sumP2;
            for (int i = 0; i < 6; ++i) {
                p1side[i] = 0;
                p2side[i] = 0;
            }
        }
    }
};

using Weights = array<double, 4>;
using HeuristicFunc = function<double(const MancalaBoard&, int, bool, int, const Weights&)>;

namespace Heuristics {

double storageDiff(const MancalaBoard& b, int player) {
    return b.storageOf(player) - b.oppStorageOf(player);
}

double sideDiff(const MancalaBoard& b, int player) {
    return b.sideStonesOf(player) - b.oppSideStonesOf(player);
}

double heuristic1(const MancalaBoard& b, int player, bool /*extraTurn*/, int /*captured*/, const Weights& /*w*/) {
    return storageDiff(b, player);
}

double heuristic2(const MancalaBoard& b, int player, bool /*extraTurn*/, int /*captured*/, const Weights& w) {
    return w[0] * storageDiff(b, player) + w[1] * sideDiff(b, player);
}

double heuristic3(const MancalaBoard& b, int player, bool extraTurn, int /*captured*/, const Weights& w) {
    return w[0] * storageDiff(b, player) + w[1] * sideDiff(b, player) + w[2] * (extraTurn ? 1 : 0);
}

double heuristic4(const MancalaBoard& b, int player, bool extraTurn, int captured, const Weights& w) {
    return w[0] * storageDiff(b, player) + w[1] * sideDiff(b, player)
         + w[2] * (extraTurn ? 1 : 0) + w[3] * captured;
}

struct Entry {
    string name;
    HeuristicFunc func;
    Weights weights;
};

vector<Entry> registry() {
    return {
        {"heuristic-1", heuristic1, {1, 0, 0, 0}},
        {"heuristic-2", heuristic2, {1, 1, 0, 0}},
        {"heuristic-3", heuristic3, {1, 1, 5, 0}},
        {"heuristic-4", heuristic4, {1, 1, 5, 3}}
    };
}

}

struct AIConfig {
    string name;
    HeuristicFunc heuristic;
    Weights weights;
    int depth;
};

class AIPlayer {
public:
    AIConfig config;

    AIPlayer(const AIConfig& cfg) : config(cfg) {}

    double alphaBeta(MancalaBoard board, int depth, double alpha, double beta,
                      int currentPlayer, int rootPlayer, bool leadingExtraTurn, int leadingCaptured) {
        if (depth == 0 || board.isGameOver())
            return config.heuristic(board, rootPlayer, leadingExtraTurn, leadingCaptured, config.weights);

        vector<int> moves = board.getValidMoves(currentPlayer);
        if (moves.empty())
            return config.heuristic(board, rootPlayer, leadingExtraTurn, leadingCaptured, config.weights);

        bool maximizing = (currentPlayer == rootPlayer);
        double best = maximizing ? -1e18 : 1e18;

        for (int m : moves) {
            MancalaBoard child = board;
            int captured = 0;
            bool extra = child.move(currentPlayer, m, captured);
            int nextPlayer = extra ? currentPlayer : (currentPlayer == 1 ? 2 : 1);

            double score = alphaBeta(child, depth - 1, alpha, beta, nextPlayer, rootPlayer, extra, captured);

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

    int getBestMove(const MancalaBoard& board, int player) {
        vector<int> moves = board.getValidMoves(player);
        if (moves.empty()) return -1;

        int bestMove = moves[0];
        double bestScore = -1e18;
        int tieCount = 0;

        for (int m : moves) {
            MancalaBoard child = board;
            int captured = 0;
            bool extra = child.move(player, m, captured);
            int nextPlayer = extra ? player : (player == 1 ? 2 : 1);

            double score = alphaBeta(child, config.depth - 1, -1e18, 1e18, nextPlayer, player, extra, captured);

            if (score > bestScore) {
                bestScore = score;
                bestMove = m;
                tieCount = 1;
            } else if (score == bestScore) {
                tieCount++;
                if (rand() % tieCount == 0) bestMove = m;
            }
        }
        return bestMove;
    }
};

struct MatchResult {
    int winner;
    int p1storage;
    int p2storage;
};

class GameSimulator {
public:
    static MatchResult playMatch(AIPlayer& p1, AIPlayer& p2, int startingPlayer) {
        MancalaBoard board;
        int currentPlayer = startingPlayer;

        while (!board.isGameOver()) {
            AIPlayer& active = (currentPlayer == 1) ? p1 : p2;
            int chosenMove = active.getBestMove(board, currentPlayer);
            if (chosenMove == -1) break;

            int captured = 0;
            bool extra = board.move(currentPlayer, chosenMove, captured);
            if (!extra) currentPlayer = (currentPlayer == 1) ? 2 : 1;
        }

        MatchResult result;
        result.p1storage = board.p1storage;
        result.p2storage = board.p2storage;
        if (board.p1storage > board.p2storage) result.winner = 1;
        else if (board.p2storage > board.p1storage) result.winner = 2;
        else result.winner = 0;
        return result;
    }

    struct ExperimentStats {
        int p1wins = 0;
        int p2wins = 0;
        int draws = 0;
    };

    static ExperimentStats runExperiment(int totalGames, const AIConfig& cfg1, const AIConfig& cfg2) {
        ExperimentStats stats;
        for (int g = 0; g < totalGames; ++g) {
            AIPlayer p1(cfg1);
            AIPlayer p2(cfg2);
            int starter = (g % 2 == 0) ? 1 : 2;
            MatchResult r = playMatch(p1, p2, starter);
            if (r.winner == 1) stats.p1wins++;
            else if (r.winner == 2) stats.p2wins++;
            else stats.draws++;
        }
        return stats;
    }
};

int main() {
    srand((unsigned)time(nullptr));

    int GAMES_PER_MATCHUP = 100;
    int SEARCH_DEPTH = 6;

    vector<Heuristics::Entry> heuristics = Heuristics::registry();

    vector<AIConfig> configs;
    for (auto& h : heuristics)
        configs.push_back({h.name, h.func, h.weights, SEARCH_DEPTH});

    ofstream out("result.txt");
    out << "Mancala Heuristic Round-Robin Results\n";
    out << "Games per matchup: " << GAMES_PER_MATCHUP << ", Search depth: " << SEARCH_DEPTH << "\n\n";

    cout << "Mancala Heuristic Round-Robin Results\n";
    cout << "Games per matchup: " << GAMES_PER_MATCHUP << ", Search depth: " << SEARCH_DEPTH << "\n\n";

    for (size_t i = 0; i < configs.size(); ++i) {
        for (size_t j = i + 1; j < configs.size(); ++j) {
            auto stats = GameSimulator::runExperiment(GAMES_PER_MATCHUP, configs[i], configs[j]);

            // need to show the output in the format: "heuristic-1 vs heuristic-2 -> wins: 60 / 40, draws: 0"  h1 win perc: 80% h2 win perc: 20% draws: 0%

            string line = configs[i].name + " vs " + configs[j].name +
                          " -> wins: " + to_string(stats.p1wins) +
                          " / " + to_string(stats.p2wins) +
                          ", draws: " + to_string(stats.draws) + 
                         ", " + configs[i].name + " win perc: " + to_string((stats.p1wins * 100) / GAMES_PER_MATCHUP) + "%" +
                         ", " + configs[j].name + " win perc: " + to_string((stats.p2wins * 100) / GAMES_PER_MATCHUP) + "%" +
                         ", draws perc: " + to_string((stats.draws * 100) / GAMES_PER_MATCHUP) + "%"    

                          ;

            cout << line << "\n";
            out << line << "\n";
        }

       
        
    }

    out.close();
    cout << "\nResults saved to result.txt\n";
    return 0;
}
