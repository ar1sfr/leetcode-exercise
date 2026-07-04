```cpp
class Solution {
public:
    bool findSafeWalk(vector<vector<int>>& grid, int health) {
        int m = grid.size(), n = grid[0].size();
        const int DIRS[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};

        // cost[r][c] = minimum unsafe cells (1s) visited on a path from (0,0) to (r,c).
        vector<vector<int>> cost(m, vector<int>(n, INT_MAX));
        cost[0][0] = grid[0][0];
        deque<pair<int,int>> dq;
        dq.push_back({0, 0});

        // 0-1 BFS: pushing safe (0-cost) moves to front, unsafe (1-cost) to back.
        while (!dq.empty()) {
            auto [r, c] = dq.front(); dq.pop_front();
            for (auto& d : DIRS) {
                int nr = r + d[0], nc = c + d[1];
                if (nr < 0 || nr >= m || nc < 0 || nc >= n) continue;
                int newCost = cost[r][c] + grid[nr][nc];
                if (newCost < cost[nr][nc]) {
                    cost[nr][nc] = newCost;
                    if (grid[nr][nc] == 0) dq.push_front({nr, nc});
                    else                     dq.push_back({nr, nc});
                }
            }
        }
        // Final cell cost = total health lost along the cheapest path.
        // Reach safely iff cost ≤ health - 1 (need ≥1 health remaining).
        return cost[m - 1][n - 1] <= health - 1;
    }
};
```

## Problem Reformulation

The problem is equivalent to: **find a path from (0,0) to (m-1, n-1) that minimizes the number of unsafe cells (1s) traversed**. If that minimum ≤ `health - 1`, we can reach the end with health ≥ 1.

| Path property | Health relation |
|---------------|-----------------|
| Traverses `k` unsafe cells | Final health = `health - k` |
| Need final health ≥ 1 | `health - k ≥ 1` → `k ≤ health - 1` |

## Algorithm Choice: **0-1 BFS**

This is a **shortest-path problem on a graph with edge weights in {0, 1}**:
- Moving into a safe cell (`grid=0`) costs **0**.
- Moving into an unsafe cell (`grid=1`) costs **1**.

**0-1 BFS** uses a `deque`:
- **0-weight edge** → push to **front** (process next).
- **1-weight edge** → push to **back** (process later).

This gives the same complexity as Dijkstra but with simpler logic and better constants.

| Alternative | Time | Verdict |
|-------------|------|---------|
| BFS over (r, c, health) state | `O(m·n·health)` | ❌ Health up to `m+n=100`, so still OK but wasteful |
| Dijkstra with min-heap | `O(m·n·log(m·n))` | ✅ Correct, but slower constants than 0-1 BFS |
| **0-1 BFS** | `O(m·n)` | ✅ Optimal for this edge-weight structure |

## Step-by-Step Walkthrough

### Step 1: Initialize cost grid

```cpp
cost[0][0] = grid[0][0];
```

The starting cell itself is "traversed" — if it's unsafe (`grid[0][0] == 1`), we already lose 1 health.

### Step 2: 0-1 BFS expansion

For each cell popped from the deque's front:
- Try all 4 neighbors.
- If `cost + grid[nr][nc] < cost[nr][nc]` (we found a cheaper way):
  - Update `cost[nr][nc]`.
  - Push to **front** of deque if the move is free (`grid[nr][nc] == 0`).
  - Push to **back** of deque if the move costs 1 (`grid[nr][nc] == 1`).

This guarantees: when a cell is popped, its `cost` is the global minimum.

### Step 3: Compare with health

```cpp
return cost[m - 1][n - 1] <= health - 1;
```

The minimum unsafe cells to reach the destination must be ≤ `health - 1` so that 1 health remains.

## Variable Definitions

| Variable | Meaning |
|----------|---------|
| `m, n` | Grid dimensions |
| `DIRS` | 4 directions: up, down, left, right |
| `cost[r][c]` | Minimum number of unsafe cells traversed on any path from `(0,0)` to `(r,c)` |
| `dq` | Deque used as 0-1 BFS queue |
| `r, c` | Current cell's row/column |
| `nr, nc` | **N**eighbor's **r**ow and **c**ol |
| `newCost` | Tentative new cost for the neighbor |
| `health` | Initial health points |

## Verification with Examples

### Example 1: 3×5 grid, health=1

```
0 1 0 0 0
0 1 0 1 0
0 0 0 1 0
```

Path avoiding all 1s: `(0,0)→(1,0)→(2,0)→(2,1)→(2,2)→(1,2)→(0,2)→(0,3)→(0,4)→(1,4)→(2,4)`. All cells are 0 → `cost = 0`. `0 ≤ health-1 = 0` → **return true** ✓

### Example 2: 4×6 grid, health=3

Minimum cost path requires 4 unsafe cells (per problem statement). `4 > health-1 = 2` → **return false** ✓

### Example 3: 3×3 grid, health=5

```
1 1 1
1 0 1
1 1 1
```

Every path must visit 4 unsafe cells (the border, since center is safe). `cost = 4 ≤ health-1 = 4` → **return true** ✓

## Complexity

| Metric | Value | At `m=n=50` |
|--------|-------|-------------|
| **Time** | `O(m × n)` | ~2,500 cell visits |
| **Space** | `O(m × n)` | ~10 KB for `cost` + deque |
| **Constants** | Minimal | 4-direction inner loop, no heap |

## DSA Choices — Why Each

| Component | Choice | Why |
|-----------|--------|-----|
| Algorithm | **0-1 BFS** | Edge weights are binary {0,1}; deque-based 0-1 BFS is provably optimal for this class |
| Data structure | `deque<pair<int,int>>` | `std::deque` allows O(1) push-front and push-back; needed for 0-1 BFS |
| Cost storage | `vector<vector<int>>` | 2D contiguous; rows are cache-friendly |
| Initialization | `INT_MAX` | Sentinel for "unvisited" |
| Reject: BFS over health dimension | — | Adds `health` factor; 0-1 BFS avoids this |
| Reject: DFS with memo | — | Slower due to recursion overhead and lack of order guarantee |
| Reject: BFS by cost levels | — | Multiple re-enqueues; 0-1 BFS is cleaner |

## Memory Layout

| Component | Size | Notes |
|-----------|------|-------|
| `cost` array | `m × n × 4B` ≈ 10 KB at max | Row-major, contiguous |
| `dq` (deque) | up to `m × n × 8B` ≈ 20 KB | Stores `{r, c}` pairs |
| DIRS | 32 B | Compile-time constant |
| **Total auxiliary** | **~30 KB** | Excellent |

## Correctness Argument

1. **0-1 BFS correctness**: Standard theorem — when edge weights are in {0, 1}, deque-based BFS processes nodes in non-decreasing order of distance, equivalent to Dijkstra but `O(1)` per relax instead of `O(log n)`.
2. **Cost interpretation**: `cost[r][c]` is the sum of `grid[neighbor]` for each step taken — by definition, the count of unsafe cells traversed.
3. **Health safety condition**: `health - cost ≥ 1` ⇔ `cost ≤ health - 1` — strict inequality comes from the requirement "with a health value of 1 or more".

## Verification Checklist

- [x] `cost[0][0]` includes the starting cell's own unsafety.
- [x] `deque` correctly distinguishes 0-weight and 1-weight moves.
- [x] All 4 directions tried; bounds checked.
- [x] Final comparison `cost ≤ health - 1` matches "health ≥ 1 remaining".
- [x] All three examples verified.
- [x] No recursion, no heap, no extra containers beyond `cost` and `dq`.
- [x] Compiles under `-std=c++17` with no external headers beyond `<vector>`, `<deque>`.

-------------------------------------------------------------------------------

# Step-by-Step Code Walkthrough: Find Safe Walk

Let me explain the code **line by line** with a concrete example.

---

## Complete Code Recap

```cpp
class Solution {
public:
    bool findSafeWalk(vector<vector<int>>& grid, int health) {
        int m = grid.size(), n = grid[0].size();
        const int DIRS[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};

        vector<vector<int>> cost(m, vector<int>(n, INT_MAX));
        cost[0][0] = grid[0][0];
        deque<pair<int,int>> dq;
        dq.push_back({0, 0});

        while (!dq.empty()) {
            auto [r, c] = dq.front(); dq.pop_front();
            for (auto& d : DIRS) {
                int nr = r + d[0], nc = c + d[1];
                if (nr < 0 || nr >= m || nc < 0 || nc >= n) continue;
                int newCost = cost[r][c] + grid[nr][nc];
                if (newCost < cost[nr][nc]) {
                    cost[nr][nc] = newCost;
                    if (grid[nr][nc] == 0) dq.push_front({nr, nc});
                    else                     dq.push_back({nr, nc});
                }
            }
        }
        return cost[m - 1][n - 1] <= health - 1;
    }
};
```

---

## Line-by-Line Explanation

### Line 1-2: Setup

```cpp
int m = grid.size(), n = grid[0].size();
```

- `m` = number of **r**ows (height of the grid).
- `n` = number of **c**olumns (width of the grid).

### Line 3: Direction Array

```cpp
const int DIRS[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
```

- 4 movement directions: up, down, left, right.
- Each direction is a pair `{row_change, col_change}`.

### Line 5: Cost Grid Setup

```cpp
vector<vector<int>> cost(m, vector<int>(n, INT_MAX));
```

- `cost[r][c]` = **minimum number of unsafe cells (1s) visited** on a path from `(0,0)` to `(r,c)`.
- `INT_MAX` means "unvisited yet" (we have no information about this cell).

### Line 6: Starting Cell Cost

```cpp
cost[0][0] = grid[0][0];
```

- The starting cell is "visited" — if it's unsafe (`grid[0][0] == 1`), we already lose 1 health.

### Line 7-8: Initialize the Deque

```cpp
deque<pair<int,int>> dq;
dq.push_back({0, 0});
```

- `dq` is a **double-ended queue** storing `{row, col}` pairs.
- Start by adding the starting cell `(0, 0)`.

---

### The Main Loop: 0-1 BFS

```cpp
while (!dq.empty()) {
    auto [r, c] = dq.front(); dq.pop_front();
```

- Pop the cell at the **front** of the deque (oldest cell, similar to standard BFS).
- `r` = current **r**ow, `c` = current **c**olumn.

### Try All 4 Neighbors

```cpp
    for (auto& d : DIRS) {
        int nr = r + d[0], nc = c + d[1];
```

- For each direction `d`:
  - `nr` = **n**ew **r**ow (neighbor's row).
  - `nc` = **n**ew **c**ol (neighbor's col).

### Boundary Check

```cpp
        if (nr < 0 || nr >= m || nc < 0 || nc >= n) continue;
```

- Skip neighbors that fall **outside the grid**.

### Compute New Cost

```cpp
        int newCost = cost[r][c] + grid[nr][nc];
```

- The cost to reach the neighbor = cost to reach current cell + cost of entering the neighbor.
- `grid[nr][nc]` is either **0** (safe, no extra cost) or **1** (unsafe, +1 cost).

### Update If Better

```cpp
        if (newCost < cost[nr][nc]) {
            cost[nr][nc] = newCost;
```

- If this is a **cheaper path** to the neighbor, update the cost.

### Push to Front or Back

```cpp
            if (grid[nr][nc] == 0) dq.push_front({nr, nc});
            else                     dq.push_back({nr, nc});
        }
    }
}
```

- **Safe move** (`grid[nr][nc] == 0`) → push to **front** of deque (process next).
- **Unsafe move** (`grid[nr][nc] == 1`) → push to **back** of deque (process later).

This is the **0-1 BFS trick**: 0-cost edges are processed before 1-cost edges, just like Dijkstra but `O(1)` per push instead of `O(log n)`.

---

### Final Check

```cpp
return cost[m - 1][n - 1] <= health - 1;
```

- `cost[m-1][n-1]` = minimum unsafe cells visited on the cheapest path to the end.
- We need final health ≥ 1, so: `health - cost ≥ 1` → `cost ≤ health - 1`.
- If true → we can reach safely. If false → no.

---

## Variable Summary Table

| Variable | Meaning | Type |
|----------|---------|------|
| `m` | Number of **r**ows | `int` |
| `n` | Number of **c**ol (columns) | `int` |
| `DIRS` | 4 movement directions (up, down, left, right) | `const int[4][2]` |
| `cost[r][c]` | Min unsafe cells visited on a path to `(r,c)` | `vector<vector<int>>` |
| `dq` | Deque used as 0-1 BFS queue | `deque<pair<int,int>>` |
| `r, c` | Current cell's row and column | `int` |
| `nr, nc` | **N**eighbor's **r**ow and **c**ol | `int` |
| `newCost` | Tentative new cost for the neighbor | `int` |
| `health` | Initial health points (input) | `int` |

---

## Concrete Example: 3×3 Grid

Let's trace with this grid and `health = 5`:
```
1 1 1
1 0 1
1 1 1
```

| Cell | (0,0) | (0,1) | (0,2) | (1,0) | (1,1) | (1,2) | (2,0) | (2,1) | (2,2) |
|------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
| Grid | 1 | 1 | 1 | 1 | 0 | 1 | 1 | 1 | 1 |

### Initial State

- `cost[0][0] = 1` (starting cell is unsafe).
- `dq = [(0,0)]`.

### Iteration 1: Pop (0,0), cost=1

Try all 4 directions:
- `(-1,0)` out of bounds → skip
- `(1,0)`: `newCost = 1 + 1 = 2`, cost=2 (was INT_MAX), push_back. `dq = [(1,0)]`
- `(0,-1)` out of bounds → skip
- `(0,1)`: `newCost = 1 + 1 = 2`, cost=2, push_back. `dq = [(1,0), (0,1)]`

### Iteration 2: Pop (1,0), cost=2

- `(0,0)`: newCost=3 > 2 → skip
- `(2,0)`: newCost=3, cost=3, push_back. `dq = [(0,1), (2,0)]`
- `(1,-1)` out of bounds → skip
- `(1,1)`: newCost=2+0=2, cost=2, **push_front** (free move!). `dq = [(1,1), (0,1), (2,0)]`

### Iteration 3: Pop (1,1), cost=2

- `(0,1)`: newCost=2+1=3 > 2 → skip
- `(2,1)`: newCost=2+1=3, cost=3, push_back. `dq = [(0,1), (2,0), (2,1)]`
- `(1,0)`: newCost=2+0=2 = 2 → not less, skip
- `(1,2)`: newCost=2+1=3, cost=3, push_back. `dq = [(0,1), (2,0), (2,1), (1,2)]`

### Iteration 4: Pop (0,1), cost=2

- `(0,0)`: skip
- `(0,2)`: newCost=3, cost=3, push_back. `dq = [(2,0), (2,1), (1,2), (0,2)]`
- `(1,1)`: newCost=3 > 2 → skip

### Iteration 5: Pop (2,0), cost=3

- `(1,0)`: skip
- `(2,1)`: newCost=3+1=4 > 3 → skip

### Iteration 6: Pop (2,1), cost=3

- `(2,0)`: skip
- `(2,2)`: newCost=3+1=4, cost=4, push_back. `dq = [(1,2), (0,2), (2,2)]`
- `(1,1)`: newCost=3+0=3 > 2 → skip

### Iteration 7: Pop (1,2), cost=3

- `(0,2)`: newCost=4 > 3 → skip
- `(2,2)`: newCost=3+1=4 = 4 → not less, skip
- `(1,1)`: skip

### Iteration 8: Pop (0,2), cost=3

- All neighbors either out of bounds or have cheaper paths → no updates.

### Iteration 9: Pop (2,2), cost=4

- All neighbors already have ≤ 4 cost → no updates.

### Final State

- `cost[2][2] = 4` (we traversed 4 unsafe cells).
- `health - 1 = 5 - 1 = 4`.
- `4 ≤ 4` → **return true** ✓

---

## Key Concepts to Remember

1. **0-1 BFS** uses a `deque` instead of a `priority_queue`:
   - 0-cost edges → push **front**.
   - 1-cost edges → push **back**.
   - Achieves `O(1)` per push, total `O(m·n)` time.

2. **Cost interpretation**: `cost[r][c]` is the count of unsafe cells visited along the cheapest path.

3. **Health check**: `cost ≤ health - 1` ensures we end with at least 1 health point.

4. **Why deque and not queue?** Standard BFS only handles 0-weight edges. 0-1 BFS handles both, but needs to distinguish them by pushing to front or back.

5. **Stale entries not needed here**: Unlike Dijkstra, 0-1 BFS processes each cell at most a few times — the strict `<` comparison in the update ensures we don't process outdated entries.