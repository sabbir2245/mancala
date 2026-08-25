Time: 25 Minutes Marks: 10
Section C2 — Opponent-Near-Win Penalty
Modify your existing heuristic so that the AI becomes more defensive when the opponent is close to
winning.
Let:
H_new = H_old − P
Where:
● P = 15 if the opponent's storage contains at least 40% of the total stones.
● Otherwise, P = 0.
Requirements:
1. Modify the evaluation function to apply the penalty when the opponent is near winning.
2. Run the search on a given initial state.
3. Print:
● Opponent Storage
● Opponent Near Winning: Yes/No
● Penalty Applied
● Selected Move
● Final Evaluation