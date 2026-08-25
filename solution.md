# Solution — Heuristic Modifications to 2205040.cpp

Each problem below shows the modified heuristic function and any required changes to TunedWeights or AIConfig.

---

## Problem C3 — Extra Turn Chain Bonus

**Change:** Add storage comparison bonus when `extraTurn` is true.

### Modified Heuristic Functions:

**heuristic3** (line 197-201):
```cpp
static double heuristic3(const MancalaBoard &b, int player, bool extraTurn,
                         int, const Weights &w) {
    double base = w[0] * storageDiff(b, player) + w[1] * sideDiff(b, player) +
                  w[2] * (extraTurn ? 1 : 0);
    double bonus = (extraTurn && b.storageOf(player) > b.oppStorageOf(player)) ? 10.0 : 0.0;
    return base + bonus;
}
```

**heuristic4** (line 203-207):
```cpp
static double heuristic4(const MancalaBoard &b, int player, bool extraTurn,
                         int captured, const Weights &w) {
    double base = w[0] * storageDiff(b, player) + w[1] * sideDiff(b, player) +
                  w[2] * (extraTurn ? 1 : 0) + w[3] * captured;
    double bonus = (extraTurn && b.storageOf(player) > b.oppStorageOf(player)) ? 10.0 : 0.0;
    return base + bonus;
}
```

**heuristic5** (line 233-238):
```cpp
static double heuristic5(const MancalaBoard &b, int player, bool extraTurn,
                         int, const Weights &w) {
    double base = w[0] * storageDiff(b, player) +
                  w[1] * stonesCloseToStorage(b, player) +
                  w[2] * (extraTurn ? 1 : 0);
    double bonus = (extraTurn && b.storageOf(player) > b.oppStorageOf(player)) ? 10.0 : 0.0;
    return base + bonus;
}
```

**heuristic6** (line 240-245):
```cpp
static double heuristic6(const MancalaBoard &b, int player, bool,
                         int captured, const Weights &w) {
    double base = w[0] * winProximity(b, player) +
                  w[1] * sideDiff(b, player) +
                  w[2] * captured;
    double bonus = (extraTurn && b.storageOf(player) > b.oppStorageOf(player)) ? 10.0 : 0.0;
    return base + bonus;
}
```

**TunedWeights:** No changes needed.

---

## Problem C4 — Endgame Urgency Multiplier

**Change:** Multiply H_old by M when total stones ≤ 20.

### Modified Heuristic Functions:

**heuristic1** (line 185-189):
```cpp
static double heuristic1(const MancalaBoard &b, int player,
                         bool, int,
                         const Weights &w) {
    double old = storageDiff(b, player);
    double totalStones = b.sideStonesOf(player) + b.oppSideStonesOf(player) +
                       b.storageOf(player) + b.oppStorageOf(player);
    double multiplier = (totalStones <= 20) ? 1.5 : 1.0;
    return old * multiplier;
}
```

**heuristic2** (line 191-195):
```cpp
static double heuristic2(const MancalaBoard &b, int player,
                         bool, int,
                         const Weights &w) {
    double old = w[0] * storageDiff(b, player) + w[1] * sideDiff(b, player);
    double totalStones = b.sideStonesOf(player) + b.oppSideStonesOf(player) +
                       b.storageOf(player) + b.oppStorageOf(player);
    double multiplier = (totalStones <= 20) ? 1.5 : 1.0;
    return old * multiplier;
}
```

**heuristic3** (line 197-201):
```cpp
static double heuristic3(const MancalaBoard &b, int player, bool extraTurn,
                         int, const Weights &w) {
    double old = w[0] * storageDiff(b, player) + w[1] * sideDiff(b, player) +
                 w[2] * (extraTurn ? 1 : 0);
    double totalStones = b.sideStonesOf(player) + b.oppSideStonesOf(player) +
                         b.storageOf(player) + b.oppStorageOf(player);
    double multiplier = (totalStones <= 20) ? 1.5 : 1.0;
    return old * multiplier;
}
```

**heuristic4** (line 203-207):
```cpp
static double heuristic4(const MancalaBoard &b, int player, bool extraTurn,
                         int captured, const Weights &w) {
    double old = w[0] * storageDiff(b, player) + w[1] * sideDiff(b, player) +
                 w[2] * (extraTurn ? 1 : 0) + w[3] * captured;
    double totalStones = b.sideStonesOf(player) + b.oppSideStonesOf(player) +
                         b.storageOf(player) + b.oppStorageOf(player);
    double multiplier = (totalStones <= 20) ? 1.5 : 1.0;
    return old * multiplier;
}
```

**heuristic5** (line 233-238):
```cpp
static double heuristic5(const MancalaBoard &b, int player, bool extraTurn,
                         int, const Weights &w) {
    double old = w[0] * storageDiff(b, player) +
                 w[1] * stonesCloseToStorage(b, player) +
                 w[2] * (extraTurn ? 1 : 0);
    double totalStones = b.sideStonesOf(player) + b.oppSideStonesOf(player) +
                         b.storageOf(player) + b.oppStorageOf(player);
    double multiplier = (totalStones <= 20) ? 1.5 : 1.0;
    return old * multiplier;
}
```

**heuristic6** (line 240-245):
```cpp
static double heuristic6(const MancalaBoard &b, int player, bool,
                         int captured, const Weights &w) {
    double old = w[0] * winProximity(b, player) +
                 w[1] * sideDiff(b, player) +
                 w[2] * captured;
    double totalStones = b.sideStonesOf(player) + b.oppSideStonesOf(player) +
                         b.storageOf(player) + b.oppStorageOf(player);
    double multiplier = (totalStones <= 20) ? 1.5 : 1.0;
    return old * multiplier;
}
```

**TunedWeights:** No changes needed.

---

## Problem C5 — Opponent Storage Warning Penalty

**Change:** Subtract P based on opponent storage % of total stones.

### Modified Heuristic Functions:

**heuristic2** (line 191-195):
```cpp
static double heuristic2(const MancalaBoard &b, int player,
                         bool, int,
                         const Weights &w) {
    double totalStones = b.sideStonesOf(player) + b.oppSideStonesOf(player) +
                       b.storageOf(player) + b.oppStorageOf(player);
    double oppStoragePct = b.oppStorageOf(player) / totalStones;
    double penalty = 0.0;
    if (oppStoragePct >= 0.5) penalty = 20.0;
    else if (oppStoragePct >= 0.4) penalty = 10.0;
    else if (oppStoragePct >= 0.3) penalty = 5.0;
    return w[0] * storageDiff(b, player) + w[1] * sideDiff(b, player) - penalty;
}
```

**heuristic3** (line 197-201):
```cpp
static double heuristic3(const MancalaBoard &b, int player, bool extraTurn,
                         int, const Weights &w) {
    double totalStones = b.sideStonesOf(player) + b.oppSideStonesOf(player) +
                       b.storageOf(player) + b.oppStorageOf(player);
    double oppStoragePct = b.oppStorageOf(player) / totalStones;
    double penalty = 0.0;
    if (oppStoragePct >= 0.5) penalty = 20.0;
    else if (oppStoragePct >= 0.4) penalty = 10.0;
    else if (oppStoragePct >= 0.3) penalty = 5.0;
    return w[0] * storageDiff(b, player) + w[1] * sideDiff(b, player) +
           w[2] * (extraTurn ? 1 : 0) - penalty;
}
```

**heuristic4** (line 203-207):
```cpp
static double heuristic4(const MancalaBoard &b, int player, bool extraTurn,
                         int captured, const Weights &w) {
    double totalStones = b.sideStonesOf(player) + b.oppSideStonesOf(player) +
                       b.storageOf(player) + b.oppStorageOf(player);
    double oppStoragePct = b.oppStorageOf(player) / totalStones;
    double penalty = 0.0;
    if (oppStoragePct >= 0.5) penalty = 20.0;
    else if (oppStoragePct >= 0.4) penalty = 10.0;
    else if (oppStoragePct >= 0.3) penalty = 5.0;
    return w[0] * storageDiff(b, player) + w[1] * sideDiff(b, player) +
           w[2] * (extraTurn ? 1 : 0) + w[3] * captured - penalty;
}
```

**heuristic5** (line 233-238):
```cpp
static double heuristic5(const MancalaBoard &b, int player, bool extraTurn,
                         int, const Weights &w) {
    double totalStones = b.sideStonesOf(player) + b.oppSideStonesOf(player) +
                       b.storageOf(player) + b.oppStorageOf(player);
    double oppStoragePct = b.oppStorageOf(player) / totalStones;
    double penalty = 0.0;
    if (oppStoragePct >= 0.5) penalty = 20.0;
    else if (oppStoragePct >= 0.4) penalty = 10.0;
    else if (oppStoragePct >= 0.3) penalty = 5.0;
    return w[0] * storageDiff(b, player) +
           w[1] * stonesCloseToStorage(b, player) +
           w[2] * (extraTurn ? 1 : 0) - penalty;
}
```

**heuristic6** (line 240-245):
```cpp
static double heuristic6(const MancalaBoard &b, int player, bool,
                         int captured, const Weights &w) {
    double totalStones = b.sideStonesOf(player) + b.oppSideStonesOf(player) +
                       b.storageOf(player) + b.oppStorageOf(player);
    double oppStoragePct = b.oppStorageOf(player) / totalStones;
    double penalty = 0.0;
    if (oppStoragePct >= 0.5) penalty = 20.0;
    else if (oppStoragePct >= 0.4) penalty = 10.0;
    else if (oppStoragePct >= 0.3) penalty = 5.0;
    return w[0] * winProximity(b, player) +
           w[1] * sideDiff(b, player) +
           w[2] * captured - penalty;
}
```

**TunedWeights:** No changes needed.

---

## Problem C6 — Side Dominance Bonus

**Change:** Add D when player's side stones > opponent's AND player's storage > opponent's.

### Modified Heuristic Functions:

**heuristic1** (line 185-189):
```cpp
static double heuristic1(const MancalaBoard &b, int player,
                         bool, int,
                         const Weights &w) {
    double base = storageDiff(b, player);
    double sideDominance = (b.sideStonesOf(player) > b.oppSideStonesOf(player) &&
                           b.storageOf(player) > b.oppStorageOf(player)) ? 8.0 : 0.0;
    return base + sideDominance;
}
```

**heuristic2** (line 191-195):
```cpp
static double heuristic2(const MancalaBoard &b, int player,
                         bool, int,
                         const Weights &w) {
    double base = w[0] * storageDiff(b, player) + w[1] * sideDiff(b, player);
    double sideDominance = (b.sideStonesOf(player) > b.oppSideStonesOf(player) &&
                           b.storageOf(player) > b.oppStorageOf(player)) ? 8.0 : 0.0;
    return base + sideDominance;
}
```

**heuristic3** (line 197-201):
```cpp
static double heuristic3(const MancalaBoard &b, int player, bool extraTurn,
                         int, const Weights &w) {
    double base = w[0] * storageDiff(b, player) + w[1] * sideDiff(b, player) +
                  w[2] * (extraTurn ? 1 : 0);
    double sideDominance = (b.sideStonesOf(player) > b.oppSideStonesOf(player) &&
                           b.storageOf(player) > b.oppStorageOf(player)) ? 8.0 : 0.0;
    return base + sideDominance;
}
```

**heuristic4** (line 203-207):
```cpp
static double heuristic4(const MancalaBoard &b, int player, bool extraTurn,
                         int captured, const Weights &w) {
    double base = w[0] * storageDiff(b, player) + w[1] * sideDiff(b, player) +
                  w[2] * (extraTurn ? 1 : 0) + w[3] * captured;
    double sideDominance = (b.sideStonesOf(player) > b.oppSideStonesOf(player) &&
                           b.storageOf(player) > b.oppStorageOf(player)) ? 8.0 : 0.0;
    return base + sideDominance;
}
```

**heuristic5** (line 233-238):
```cpp
static double heuristic5(const MancalaBoard &b, int player, bool extraTurn,
                         int, const Weights &w) {
    double base = w[0] * storageDiff(b, player) +
                  w[1] * stonesCloseToStorage(b, player) +
                  w[2] * (extraTurn ? 1 : 0);
    double sideDominance = (b.sideStonesOf(player) > b.oppSideStonesOf(player) &&
                           b.storageOf(player) > b.oppStorageOf(player)) ? 8.0 : 0.0;
    return base + sideDominance;
}
```

**heuristic6** (line 240-245):
```cpp
static double heuristic6(const MancalaBoard &b, int player, bool,
                         int captured, const Weights &w) {
    double base = w[0] * winProximity(b, player) +
                  w[1] * sideDiff(b, player) +
                  w[2] * captured;
    double sideDominance = (b.sideStonesOf(player) > b.oppSideStonesOf(player) &&
                           b.storageOf(player) > b.oppStorageOf(player)) ? 8.0 : 0.0;
    return base + sideDominance;
}
```

**TunedWeights:** No changes needed.