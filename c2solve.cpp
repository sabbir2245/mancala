#define MANCALA_STANDALONE
#include "2205040.cpp"

static bool s_lastOpponentNearWin = false;
static double s_lastPenalty = 0;

class C2Heuristic {
public:
  static double evaluate(const MancalaBoard &b, int player, bool extraTurn,
                         int captured, const Weights &w) {
    double H_old = w[0] * (b.storageOf(player) - b.oppStorageOf(player)) +
                   w[1] * (b.sideStonesOf(player) - b.oppSideStonesOf(player)) +
                   w[2] * (extraTurn ? 1 : 0) +
                   w[3] * captured;

    int totalStones = 0;
    for (int i = 0; i < 6; ++i) {
      totalStones += b.p1side[i] + b.p2side[i];
    }
    totalStones += b.p1storage + b.p2storage;

    double opponentStorage = b.oppStorageOf(player);
    bool opponentNearWin = (totalStones > 0) && (opponentStorage >= 0.4 * totalStones);
    double P = opponentNearWin ? 15.0 : 0.0;

    s_lastOpponentNearWin = opponentNearWin;
    s_lastPenalty = P;

    return H_old - P;
  }
};

int main() {
  srand((unsigned)time(nullptr));

  MancalaBoard board;
  board.p1side[0] = 10;
  board.p1side[1] = 5;
  board.p1side[2] = 0;
  board.p1side[3] = 8;
  board.p1side[4] = 3;
  board.p1side[5] = 2;
  board.p1storage = 10;

  board.p2side[0] = 4;
  board.p2side[1] = 7;
  board.p2side[2] = 6;
  board.p2side[3] = 0;
  board.p2side[4] = 9;
  board.p2side[5] = 1;
  board.p2storage = 45;

  int player = 1;
  int depth = 4;
  Weights w = {1.0, 0.5, 2.0, 1.0};

  cout << "=== C2: Opponent-Near-Win Penalty ===\n\n";

  AIConfig cfg;
  cfg.name = "c2-ai";
  cfg.heuristic = C2Heuristic::evaluate;
  cfg.weights = w;
  cfg.depth = depth;

  AIPlayer ai(cfg);

  int bestMove = ai.getBestMove(board, player);

  int totalStones = 0;
  for (int i = 0; i < 6; ++i) {
    totalStones += board.p1side[i] + board.p2side[i];
  }
  totalStones += board.p1storage + board.p2storage;

  double opponentStorage = board.oppStorageOf(player);
  bool opponentNearWin = (totalStones > 0) && (opponentStorage >= 0.4 * totalStones);
  double P = opponentNearWin ? 15.0 : 0.0;

  MancalaBoard resultBoard = board;
  int captured = 0;
  bool extra = resultBoard.move(player, bestMove, captured);

  double finalEval = w[0] * (resultBoard.storageOf(player) - resultBoard.oppStorageOf(player)) +
                     w[1] * (resultBoard.sideStonesOf(player) - resultBoard.oppSideStonesOf(player)) +
                     w[2] * (extra ? 1 : 0) +
                     w[3] * captured;
  finalEval -= P;

  cout << "Opponent Storage: " << opponentStorage << "\n";
  cout << "Opponent Near Winning: " << (opponentNearWin ? "Yes" : "No") << "\n";
  cout << "Penalty Applied: " << P << "\n";
  cout << "Selected Move: " << bestMove << "\n";
  cout << "Final Evaluation: " << finalEval << "\n";

  return 0;
}
