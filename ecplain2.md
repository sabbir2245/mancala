# Mancala AI — Code Explanation

## 1. Overview

This program implements a Mancala game where two AI players play against each other.

The program:

- Implements the Mancala board and game rules.
- Implements 6 different heuristic functions.
- Uses Minimax with Alpha-Beta pruning.
- Uses move ordering to improve Alpha-Beta pruning.
- Allows different search depths.
- Runs multiple games between every pair of heuristics.
- Records the results in `result.txt`.

The main purpose is to compare different heuristic functions and see which one performs better.

---

# 2. Required Libraries

```cpp
#include <algorithm>
#include <array>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

These libraries provide:

algorithm → sort(), max(), min()
array → array<double, 4>
cstdlib → rand(), srand()
ctime → time()
fstream → writing results to a file
functional → function<>
iostream → console output
string → strings
vector → dynamic arrays
using namespace std;

This allows us to use names such as vector, string, and cout without writing std::.

3. MancalaBoard Class
class MancalaBoard {
public:
    int p1side[6];
    int p1storage;
    int p2side[6];
    int p2storage;

The board has:

6 pits for Player 1
6 pits for Player 2
1 storage/pot for Player 1
1 storage/pot for Player 2

Initially every pit contains 4 stones.

So each player starts with:

4 4 4 4 4 4

and:

storage = 0
4. Constructor
MancalaBoard() {
    for (int i = 0; i < 6; ++i) {
        p1side[i] = 4;
        p2side[i] = 4;
    }

    p1storage = 0;
    p2storage = 0;
}

This creates the initial Mancala board.

There are:

6 × 4 = 24

stones on each side.

Therefore there are 48 stones in total.

5. Making a Move

The main game logic is inside:

bool move(int player, int binIndex, int &capturedStonesOut)

The function:

Checks whether the selected pit is valid.
Removes all stones from that pit.
Distributes the stones around the board.
Skips the opponent's storage.
Checks for an extra turn.
Checks for a capture.
Checks whether the game has ended.

The return value is:

true  → player gets another turn
false → turn changes
6. Validating the Move
if (binIndex < 0 || binIndex > 5)
    return false;

There are only 6 pits, numbered:

0 1 2 3 4 5

Therefore anything outside this range is invalid.

Then the number of stones is obtained:

int stones = (player == 1)
                 ? p1side[binIndex]
                 : p2side[binIndex];

If the pit is empty:

if (stones == 0)
    return false;

the move cannot be made.

7. Removing Stones from the Selected Pit
if (player == 1)
    p1side[binIndex] = 0;
else
    p2side[binIndex] = 0;

All stones are removed from the selected pit before distribution begins.

8. Board Position Representation

The board is represented as 14 positions:

Player 1 pits
0 1 2 3 4 5

Player 1 storage
6

Player 2 pits
7 8 9 10 11 12

Player 2 storage
13

The starting position is:

int currPos = (player == 1) ? binIndex : (binIndex + 7);

This converts the player's pit number into the common 14-position representation.

9. Distributing Stones

The program repeatedly moves to the next position:

currPos = (currPos + 1) % 14;

The % 14 makes the board circular.

For example:

12 → 13 → 0 → 1
10. Skipping the Opponent's Storage

Player 1 should not place stones in Player 2's storage.

if (player == 1 && currPos == 13)
    continue;

Similarly, Player 2 skips Player 1's storage:

if (player == 2 && currPos == 6)
    continue;
11. Adding Stones

If the position is one of Player 1's pits:

if (currPos < 6)
    p1side[currPos]++;

If it is Player 1's storage:

else if (currPos == 6)
    p1storage++;

For Player 2:

else if (currPos < 13)
    p2side[currPos - 7]++;

And Player 2's storage:

else if (currPos == 13)
    p2storage++;
12. Extra Turn

After distributing all stones:

bool extraTurn =
    (player == 1 && currPos == 6) ||
    (player == 2 && currPos == 13);

A player gets another turn if their final stone lands in their own storage.

For example:

Player 1 → final stone in position 6

means Player 1 gets another turn.

13. Capturing Stones

Mancala allows a player to capture stones when their final stone lands in an empty pit on their own side.

For Player 1:

if (player == 1 &&
    currPos >= 0 &&
    currPos < 6 &&
    p1side[currPos] == 1)

The opposite pit is calculated using:

int oppPos = 5 - currPos;

For example:

Player 1 pit: 0
Opposite pit: 5

Player 1 pit: 1
Opposite pit: 4

Player 1 pit: 2
Opposite pit: 3

If the opposite pit contains stones, they are captured.

capturedStonesOut =
    p1side[currPos] + p2side[oppPos];

p1storage += capturedStonesOut;

The same process is performed for Player 2.

14. Checking the End of the Game
bool isGameOver() const

calculates the total number of stones remaining on each player's side.

return sumP1 == 0 || sumP2 == 0;

If either player's side becomes empty, the game ends.

15. Applying the Remaining Stones

When the game ends:

p1storage += sumP1;
p2storage += sumP2;

Any stones still remaining on the board are moved into their respective storage.

Then all pits are cleared.

16. Getting Valid Moves
vector<int> getValidMoves(int player) const

checks all 6 pits and returns the pits that contain at least one stone.

For example:

Board:

0 4 0 2 0 3

The valid moves are:

1, 3, 5
17. Helper Functions

The board provides several helper functions.

Player storage
int storageOf(int player) const

Returns the number of stones in the player's storage.

Opponent storage
int oppStorageOf(int player) const

Returns the opponent's storage.

Stones on the player's side
int sideStonesOf(int player) const

Adds the stones in the player's six pits.

Opponent side stones
int oppSideStonesOf(int player) const

Returns the total stones on the opponent's side.

18. Weights
typedef array<double, 4> Weights;

A heuristic can use up to four different weights.

For example:

{1.0, 0.3, 2.0, 0}

can mean:

1.0 → storage difference
0.3 → side difference
2.0 → extra-turn bonus
0   → capture bonus

Different heuristics use different combinations.

19. Heuristic Function Type
typedef function<double(
    const MancalaBoard &,
    int,
    bool,
    int,
    const Weights &
)> HeuristicFunc;

This defines a common format for all heuristic functions.

The parameters represent:

MancalaBoard → current board
int          → player being evaluated
bool         → whether an extra turn occurred
int          → number of captured stones
Weights      → heuristic weights

The function returns a double score.

20. TunedWeights
struct TunedWeights

contains the weights used by the different heuristics.

For example:

static Weights getHeuristic3Weights() {
    return {1.0, 0.3, 2.0, 0};
}

This means heuristic 3 uses:

Storage difference × 1.0
Side difference    × 0.3
Extra turn         × 2.0
Capture            × 0
21. Heuristics Class

The Heuristics class contains six different evaluation functions.

The AI uses these functions to decide which board position is better.

22. Heuristic 1
static double heuristic1(...)
{
    return storageDiff(b, player);
}

This is the simplest heuristic.

It only considers the difference between the player's storage and the opponent's storage.

For example:

Player 1 storage = 10
Player 2 storage = 6

Then:

score = 10 - 6 = 4

A positive value is good for the player being evaluated.

23. Heuristic 2
return w[0] * storageDiff(b, player)
     + w[1] * sideDiff(b, player);

This considers two things:

Storage difference
Difference in stones remaining on the sides

The weights determine how important each factor is.

24. Heuristic 3

Heuristic 3 adds an extra-turn bonus.

return w[0] * storageDiff(b, player)
     + w[1] * sideDiff(b, player)
     + w[2] * (extraTurn ? 1 : 0);

If the player gets an extra turn:

extraTurn = true

then the bonus is added.

25. Heuristic 4

Heuristic 4 additionally considers captured stones.

return w[0] * storageDiff(b, player)
     + w[1] * sideDiff(b, player)
     + w[2] * (extraTurn ? 1 : 0)
     + w[3] * captured;

So it considers:

Storage difference
+
Side difference
+
Extra turn
+
Captured stones
26. Stones Close to Storage
static double stonesCloseToStorage(...)

This tries to estimate how many stones are relatively close to reaching the player's storage.

For every pit:

int binsToStorage = 5 - i;

The closer the pit is to storage, the fewer positions the stones need to travel.

This value is used by heuristic 5.

27. Win Proximity
static double winProximity(...)

This tries to measure how close each player is to reaching a large storage value.

The code calculates:

double myDist = 24.0 - myStorage;
double oppDist = 24.0 - oppStorage;

Then it uses the inverse distance:

(1.0 / myDist) - (1.0 / oppDist)

The idea is:

Closer to winning → larger score
Further from winning → smaller score
28. Heuristic 5

Heuristic 5 uses:

Storage difference
Stones close to storage
Extra turn

The formula is:

return w[0] * storageDiff(b, player)
     + w[1] * stonesCloseToStorage(b, player)
     + w[2] * (extraTurn ? 1 : 0);
29. Heuristic 6

Heuristic 6 uses:

Win proximity
Side difference
Captured stones

The formula is:

return w[0] * winProximity(b, player)
     + w[1] * sideDiff(b, player)
     + w[2] * captured;
30. Heuristic Registry

The registry() function stores all six heuristics in one vector.

return {
    {"heuristic-1", heuristic1, ...},
    {"heuristic-2", heuristic2, ...},
    ...
};

Each entry contains:

Name
Function
Weights

This makes it easy for the experiment code to loop through all heuristics.

31. AIConfig
struct AIConfig {
    string name;
    HeuristicFunc heuristic;
    Weights weights;
    int depth;
};

This stores everything needed to configure an AI player.

For example:

name      = heuristic-3
heuristic = heuristic3
weights   = {1.0, 0.3, 2.0, 0}
depth     = 6
32. AIPlayer
class AIPlayer

is responsible for making decisions.

It contains:

AIConfig config;

which determines:

Which heuristic is used.
Which weights are used.
How deep the search goes.
33. Move Ordering
vector<pair<int, double>> getOrderedMoves(...)

Before Alpha-Beta searches the moves, the program tries to order promising moves first.

The temporary move score is:

if (extra)
    score += 10;

if (captured > 0)
    score += captured * 2;

score += (m + 1) * 0.5;

The ordering gives priority to:

Extra turns
Captures
Slight preference based on pit position

Then:

sort(...)

sorts the moves from highest priority to lowest priority.

The purpose is not to determine the actual move.

It is mainly intended to make Alpha-Beta pruning more effective.

34. Alpha-Beta Search

The main search function is:

double alphaBeta(...)

It implements Minimax with Alpha-Beta pruning.

The two important values are:

alpha → best value MAX can guarantee
beta  → best value MIN can guarantee
35. Terminal Condition

The search stops when:

if (depth == 0 || board.isGameOver())

This means either:

The maximum search depth was reached.
The game has ended.

At that point the heuristic evaluates the position:

return config.heuristic(...);
36. Maximizing and Minimizing Players
bool maximizing = (currentPlayer == rootPlayer);

The AI that is currently making the decision is the maximizing player.

The opponent is the minimizing player.

For the maximizing player:

best = max(best, score);

For the minimizing player:

best = min(best, score);
37. Handling Extra Turns

After making a move:

bool extra = child.move(currentPlayer, m, captured);

the next player is determined by:

int nextPlayer =
    extra
        ? currentPlayer
        : (currentPlayer == 1 ? 2 : 1);

If there is an extra turn, the same player continues.

Otherwise the turn switches.

38. Alpha Update

For the maximizing player:

alpha = max(alpha, best);

Alpha represents the best result the maximizing player has found so far.

39. Beta Update

For the minimizing player:

beta = min(beta, best);

Beta represents the best result the minimizing player has found so far.

40. Alpha-Beta Pruning

The important condition is:

if (beta <= alpha)
    break;

When this happens, the remaining moves do not need to be searched.

They cannot affect the final decision.

This is what makes Alpha-Beta pruning faster than normal Minimax.

41. Choosing the Best Move
int getBestMove(...)

checks every legal move.

For each move:

Copy the board.
Make the move.
Check for an extra turn.
Run Alpha-Beta.
Compare the returned score with the current best score.

The move with the highest score is selected.

42. Random Tie Breaking

If two moves have exactly the same score:

else if (score == bestScore)

the program randomly chooses between them.

if (rand() % tieCount == 0)
    bestMove = m;

This prevents the AI from always selecting the same move when several moves are equally good.

43. MatchResult
struct MatchResult {
    int winner;
    int p1storage;
    int p2storage;
};

This stores the result of one game.

It records:

Winner
Player 1's storage
Player 2's storage
44. GameSimulator

The GameSimulator class is responsible for playing games and running experiments.

45. Playing One Match
static MatchResult playMatch(...)

The function:

Creates a new Mancala board.
Chooses the starting player.
Repeatedly asks the active AI for a move.
Applies the move.
Handles extra turns.
Continues until the game ends.
Determines the winner.
46. Alternating the Starting Player

In the experiment:

int starter = (g % 2 == 0) ? 1 : 2;

Game 0:

Player 1 starts

Game 1:

Player 2 starts

Game 2:

Player 1 starts

and so on.

This is important because the starting player can have an advantage.

Alternating the starting player makes the comparison fairer.

47. Running an Experiment
static ExperimentStats runExperiment(
    int totalGames,
    const AIConfig &cfg1,
    const AIConfig &cfg2)

This function makes two AI players and makes them play multiple games.

For each game:

AIPlayer p1(cfg1);
AIPlayer p2(cfg2);

Then the result is recorded as:

Player 1 win
Player 2 win
Draw
48. Experiment Statistics
struct ExperimentStats {
    int p1wins = 0;
    int p2wins = 0;
    int draws = 0;
};

These values keep track of the results.

49. Main Function

The program starts from:

int main()

First, the random number generator is initialized:

srand((unsigned)time(nullptr));

This allows different random tie-breaking between executions.

50. Number of Games
int GAMES_PER_MATCHUP = 10;

Each pair of heuristics plays 10 games.

For example:

heuristic-1 vs heuristic-2

plays 10 games.

Then:

heuristic-1 vs heuristic-3

plays another 10 games.

And so on.

51. Search Depths
vector<int> depths = {4, 6, 8};

The experiment is performed using three different search depths:

Depth 4
Depth 6
Depth 8

Higher depth means the AI looks further ahead.

However, higher depth also requires more computation.

52. Creating AI Configurations

For every heuristic:

configs.push_back({
    h.name,
    h.func,
    h.weights,
    depth
});

A configuration is created using the current search depth.

For example:

heuristic-4
depth = 6

becomes one AI configuration.

53. Round-Robin Tournament

The nested loops:

for (size_t i = 0; i < configs.size(); ++i) {
    for (size_t j = i + 1; j < configs.size(); ++j) {

make every pair of heuristics play against each other.

With 6 heuristics, the number of unique matchups is:

6 × 5 / 2 = 15

So for each depth there are 15 matchups.

There are 3 depths:

15 × 3 = 45 matchups

Each matchup contains 10 games:

45 × 10 = 450 games

So the complete experiment runs approximately 450 games.

54. Calculating Win Percentage

The program calculates:

(stats.p1wins * 100) / GAMES_PER_MATCHUP

For example, if an AI wins 7 out of 10 games:

7 × 100 / 10 = 70%

The same calculation is performed for the second AI.

55. Output

The results are printed to the terminal:

cout << line << "\n";

They are also saved to:

ofstream out("result.txt");

The output file is:

result.txt
56. Example Result

The output can look like:

=== Search Depth: 4 ===

heuristic-1 vs heuristic-2 -> wins: 4 / 6, draws: 0,
heuristic-1 win perc: 40%, heuristic-2 win perc: 60%

This means the first heuristic won 4 games and the second heuristic won 6 games.

57. Overall Program Flow

The overall flow is:

main()
   |
   v
Load all heuristics
   |
   v
For each search depth
   |
   v
Create AI configurations
   |
   v
Choose every pair of heuristics
   |
   v
Run 10 games
   |
   v
Each game:
   |
   +--> Create Mancala board
   |
   +--> AI chooses move
   |       |
   |       +--> Move ordering
   |       |
   |       +--> Alpha-Beta search
   |               |
   |               +--> Minimax
   |               |
   |               +--> Heuristic evaluation
   |
   +--> Apply move
   |
   +--> Handle extra turn
   |
   +--> Continue until game ends
   |
   v
Record winner
   |
   v
Calculate win percentage
   |
   v
Write results to result.txt
58. Main Concepts Used

The important AI concepts demonstrated by this program are:

Minimax

The AI assumes:

I will choose the best move for myself.
My opponent will choose the best move for themselves.

Therefore the AI tries to maximize its score while assuming the opponent minimizes it.

Alpha-Beta Pruning

Alpha-Beta removes branches of the search tree that cannot affect the final decision.

This reduces the number of positions that need to be evaluated.

Heuristic Evaluation

When the AI cannot search all the way to the end of the game, it estimates how good the current board is.

The six heuristics use different information:

Heuristic	Main Factors
Heuristic 1	Storage difference
Heuristic 2	Storage + side stones
Heuristic 3	Storage + side stones + extra turn
Heuristic 4	Storage + side stones + extra turn + captures
Heuristic 5	Storage + stones close to storage + extra turn
Heuristic 6	Win proximity + side stones + captures
59. Why Multiple Heuristics Are Used

Different heuristics can make the AI behave differently.

For example:

Heuristic 1

only cares about the current storage difference.

Meanwhile:

Heuristic 4

also considers captures and extra turns.

The experiment tries to determine which evaluation strategy produces better gameplay.

60. Why Different Search Depths Are Tested

The same heuristic may perform differently at different depths.

For example:

Depth 4

searches fewer future moves and is faster.

Depth 8

searches further into the future but requires much more computation.

Testing:

4, 6, 8

allows the performance of the heuristics to be compared under different search depths.

61. Summary

This program is essentially a Mancala AI tournament system.

Its architecture can be summarized as:

MancalaBoard
     ↓
Game Rules
     ↓
AIPlayer
     ↓
Move Ordering
     ↓
Minimax + Alpha-Beta
     ↓
Heuristic Evaluation
     ↓
Best Move
     ↓
GameSimulator
     ↓
Multiple Games
     ↓
Heuristic Comparison

The final purpose is not just to play Mancala, but to experimentally compare different heuristic evaluation strategies using Minimax and Alpha-Beta pruning.