# Similar Problems — Heuristic Modifications to 2205040.cpp

Each problem below describes a modification to the evaluation function. The core change is always in the **Heuristic function** (lines ~185–260) and/or the **TunedWeights struct** (lines ~151–171).

---

## Problem C3 — Extra Turn Chain Bonus

**Statement:** Modify your heuristic so that the AI values moves that lead to extra turns more aggressively when the board state is favorable. Let:

H_new = H_old + B

Where:
- B = 10 if the move grants an extra turn AND the player's storage is greater than the opponent's storage.
- Otherwise, B = 0.

**Main part to change:** The heuristic function that receives `extraTurn` and computes `H_old`. Add a conditional bonus based on storage comparison when `extraTurn` is true.

---

## Problem C4 — Endgame Urgency Multiplier

**Statement:** Modify your heuristic so that the AI plays more aggressively when fewer total stones remain on the board. Let:

H_new = H_old × M

Where:
- M = 1.5 if total stones on both sides (including storages) ≤ 20.
- Otherwise, M = 1.0.

**Main part to change:** The heuristic function. Compute `totalStones` and apply a multiplier to `H_old` based on the threshold. This changes the scale of evaluation near game end.

---

## Problem C5 — Opponent Storage Warning Penalty

**Statement:** Modify your heuristic so that the AI penalizes itself more harshly when the opponent's storage is close to winning, but with a graduated scale instead of a fixed penalty. Let:

H_new = H_old − P

Where:
- P = 5 if opponent storage ≥ 30% of total stones.
- P = 10 if opponent storage ≥ 40% of total stones.
- P = 20 if opponent storage ≥ 50% of total stones.
- Otherwise, P = 0.

**Main part to change:** The heuristic function. Compute `totalStones`, check `opponentStorage` against multiple thresholds, and apply the appropriate penalty. This replaces the single-threshold logic from C2.

---

## Problem C6 — Side Dominance Bonus

**Statement:** Modify your heuristic so that the AI values having more stones on its side of the board relative to the opponent. Let:

H_new = H_old + D

Where:
- D = 8 if the player's side stones > opponent's side stones AND the player's storage > opponent's storage.
- D = 0 otherwise.

**Main part to change:** The heuristic function. Compare `sideStonesOf(player)` vs `oppSideStonesOf(player)` and `storageOf(player)` vs `oppStorageOf(player)`. Apply bonus only when both conditions hold.

---

## Summary of What Changes Per Problem

| Problem | Heuristic Function | TunedWeights | AIConfig | Other |
|---------|-------------------|--------------|----------|-------|
| C3      | Add extraTurn + storage bonus | — | — | — |
| C4      | Add totalStones multiplier | — | — | — |
| C5      | Add multi-threshold penalty | — | — | — |
| C6      | Add side + storage comparison bonus | — | — | — |
