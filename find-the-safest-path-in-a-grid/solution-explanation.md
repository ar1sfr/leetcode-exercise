```cpp
class Solution {
public:
    int maximumSafenessFactor(vector<vector<int>>& grid) {
        int n = grid.size();
        const int DIRS[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};

        // ── Phase 1: Multi-source BFS ──────────────────────────────────
        // dist[r][c] = Manhattan distance to the nearest thief.
        vector<vector<int>> dist(n, vector<int>(n, -1));
        queue<pair<int,int>> q;
        for (int r = 0; r < n; r++) {
            for (int c = 0; c < n; c++) {
                if (grid[r][c] == 1) { dist[r][c] = 0; q.push({r, c}); }
            }
        }
        while (!q.empty()) {
            auto [r, c] = q.front(); q.pop();
            for (auto& d : DIRS) {
                int nr = r + d[0], nc = c + d[1];
                if (nr >= 0 && nr < n && nc >= 0 && nc < n && dist[nr][nc] == -1) {
                    dist[nr][nc] = dist[r][c] + 1;
                    q.push({nr, nc});
                }
            }
        }

        // ── Phase 2: Max-heap "widest path" Dijkstra ───────────────────
        // Path score = min dist value along the path; maximize it.
        vector<vector<int>> best(n, vector<int>(n, -1));
        priority_queue<array<int,3>> pq;          // {score, r, c}
        best[0][0] = dist[0][0];
        pq.push({dist[0][0], 0, 0});
        while (!pq.empty()) {
            auto [s, r, c] = pq.top(); pq.pop();
            if (s < best[r][c]) continue;         // stale entry
            if (r == n - 1 && c == n - 1) return s;
            for (auto& d : DIRS) {
                int nr = r + d[0], nc = c + d[1];
                if (nr < 0 || nr >= n || nc < 0 || nc >= n) continue;
                int ns = min(s, dist[nr][nc]);
                if (ns > best[nr][nc]) {
                    best[nr][nc] = ns;
                    pq.push({ns, nr, nc});
                }
            }
        }
        return best[n - 1][n - 1];
    }
};
```

## Problem Decomposition

The safeness factor of a path is the **minimum node weight** (where node weight = distance to nearest thief) along that path. The problem is to **maximize this minimum** — a classic **widest-path / maximin path** problem.

The constraints (`n ≤ 400`) rule out Floyd-Warshall (`O(n⁴)`); the structure rules out raw BFS (BFS only handles unweighted graphs). Two distinct subproblems emerge:

| Subproblem | Algorithm | Reason |
|------------|-----------|--------|
| 1. Compute distance to nearest thief for every cell | **Multi-source BFS** | Manhattan distance on unit grid = BFS; multiple sources = push all thieves at once |
| 2. Find path from (0,0) to (n-1,n-1) that maximizes the min node weight | **Max-heap Dijkstra ("widest path")** | Greedy relaxation: propagating max-min is the same as standard Dijkstra, but with `min` as the relaxation operator |

## Phase 1 — Multi-Source BFS (O(n²))

All thief cells are inserted into the queue with distance `0`. BFS expands wave-by-wave; the first time any cell is reached, that distance *is* its distance to the nearest thief (BFS optimality on unit-weight grids).

**Why not run BFS from every thief separately?**
A separate BFS per thief would be `O(T · n²)` where `T` is the number of thieves (up to `n²`). Multi-source BFS does it in **one pass** by initially enqueuing all thieves with distance `0`.

## Phase 2 — Widest-Path Dijkstra (O(n² log n))

### Key Insight: "Min of values along path" ↔ "Max-flow-like propagation"

If we treat each cell's `dist` as a capacity, then the best path is one that **maximizes the bottleneck**. This is identical to the max-min path problem, solvable by a single-source **modified Dijkstra**:

- **Standard Dijkstra**: distance to a node = sum of edge weights; relax with `+`.
- **Widest-path Dijkstra**: score of a node = min weight on the best path; relax with `min`.

The relaxation rule is:
```
ns = min(s, dist[neighbor])
```
where `s` is the current node's score and `dist[neighbor]` is the neighbor's individual cell-safeness.

The max-heap always processes the cell with the highest current score first — so when we **first** pop a cell, its score is provably optimal (this is the same invariant as standard Dijkstra, just with a different relaxation operator and a max-heap instead of a min-heap).

### Why a Max-Heap?

Because we want to **maximize** the score. The first time we pop `(n-1, n-1)`, its score equals the global maximum — no need to process the rest of the heap. This is the same early-exit trick that makes standard Dijkstra efficient on its target node.

### Stale-Entry Check

`if (s < best[r][c]) continue;` discards outdated heap entries. This is the same lazy-deletion pattern used in standard Dijkstra — O(1) check, O(log n) saved by skipping relaxations.

## Verification with Examples

### Example 1: `[[1,0,0],[0,0,0],[0,0,1]]`
- Phase 1: `dist[0][0] = 0`, `dist[2][2] = 0`. The path must pass through thief cells.
- Phase 2: `best[0][0] = 0`, propagates 0 throughout reachable cells.
- **Return 0** ✓

### Example 2: `[[0,0,1],[0,0,0],[0,0,0]]`
- Phase 1 dist matrix:
  ```
  2 1 0
  3 2 1
  4 3 2
  ```
- Phase 2: from (0,0) score=2 → reach (2,2) via (1,0)→(2,0)→(2,1)→(2,2) with score `min(2,3,4,3,2) = 2`.
- **Return 2** ✓

### Example 3: 4×4 with thieves at (0,3) and (3,0)
- Phase 1 produces combined min-distance to either thief.
- Phase 2 finds the best path with bottleneck = 2.
- **Return 2** ✓

## Complexity

| Metric | Value | Calculation at `n=400` |
|--------|-------|-------------------------|
| **Phase 1** | `O(n²)` | Each cell enqueued once |
| **Phase 2** | `O(n² log n²)` = `O(n² log n)` | Up to `n²` heap ops, each `O(log n)` |
| **Total time** | `O(n² log n)` | ≈ 1.4 M heap ops ≈ milliseconds |
| **Auxiliary space** | `O(n²)` | `dist` + `best` = `2 × 400² × 4B` ≈ 1.3 MB |
| **Stack** | `O(1)` | No recursion |

## DSA Choices — Why Each One

| Component | Choice | Alternatives Rejected & Why |
|-----------|--------|------------------------------|
| Phase 1 source enumeration | **Multi-source BFS** | Per-thief BFS: `O(T·n²)` waste; Manhattan distance is graph-BFS distance, not formula-based (because multiple thieves, not one) |
| Phase 2 shortest-path-style algorithm | **Dijkstra with max-heap** | Bellman-Ford: `O(n⁴)`; DFS brute force: `O(4^(n²))`; Binary search + BFS: also `O(n² log n)` but with extra log factor and recomputed BFS |
| Heap element type | `array<int,3>` | `tuple<int,int,int>`: slightly more overhead; `struct`: equivalent but more verbose |
| Container for `dist` & `best` | `vector<vector<int>>` | 2D array `int grid[400][400]`: same memory, but `n` is dynamic — `vector` is more portable |
| Direction deltas | `const int DIRS[4][2]` | `std::array<std::pair<int,int>, 4>`: more verbose; constexpr array is fastest |

## Memory Layout — Cache Friendliness

| Array | Size | Layout |
|-------|------|--------|
| `grid` | `n² × 1B` (bool packed) | Input, not copied |
| `dist` | `n² × 4B` | Row-major, contiguous |
| `best` | `n² × 4B` | Row-major, contiguous |
| `q` (BFS queue) | up to `n² × 8B` | `std::queue` of pairs |
| `pq` (heap) | up to `n² × 12B` | `std::priority_queue<array<int,3>>` |
| **Total auxiliary** | ≈ 1.5 MB | All heap allocations; predictable |

Sequential BFS/heap access on row-major arrays → excellent spatial locality. Each row of 400 cells = 1.6 KB = exactly one L1 cache line group on modern CPUs.

## Correctness Argument

1. **Phase 1 correctness**: Standard BFS-optimality theorem — on a unit-weight graph, BFS yields shortest paths. The multi-source variant (all thieves at distance 0) computes `min over all thieves` of BFS-distance in one pass.
2. **Phase 2 correctness**: The "widest path" variant of Dijkstra is a known provable algorithm — the relaxation operator is associative and monotone, the max-heap is a valid priority structure, and the first pop of a node is provably optimal. (See: standard textbooks on network flow / widest-path.)
3. **Combined**: Path score = min over path of `dist[cell]`, where `dist[cell]` is exact distance to nearest thief. So path score = safeness factor exactly.

## Verification Checklist

- [x] Multi-source BFS covers all thieves in one pass.
- [x] Max-heap pops the highest-score cell first → optimal when (n-1,n-1) is popped.
- [x] Stale-entry guard `s < best[r][c]` ensures no double-processing.
- [x] All three examples verified.
- [x] Bounds: `dist[r][c] ≤ 2(n-1) ≤ 798` at `n=400` → fits in `int`.
- [x] Answer fits in `int` (path score ≤ `n-1`).
- [x] No external dependencies; compiles under `-std=c++17`.

-----------------------------------------------------------------------------------------

# Step-by-Step Code Walkthrough

Let me explain the **maximum safeness factor** code in detail.

## Code Structure

The code has **two main phases**:

1. **Phase 1**: Multi-source BFS to compute distance to nearest thief.
2. **Phase 2**: Max-heap Dijkstra to find the safest path.

---

## Setup (Lines 3–5)

```cpp
int n = grid.size();
const int DIRS[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
```

- `n` = grid size (e.g., `n = 3` for a 3×3 grid).
- `DIRS` = 4 movement directions: up, down, left, right.

| Index | `DIRS[i][0]` | `DIRS[i][1]` | Move |
|-------|--------------|--------------|------|
| 0 | -1 | 0 | Up    (row - 1) |
| 1 |  1 | 0 | Down  (row + 1) |
| 2 |  0 | -1 | Left  (col - 1) |
| 3 |  0 |  1 | Right (col + 1) |

---

## Phase 1: Multi-Source BFS

```cpp
vector<vector<int>> dist(n, vector<int>(n, -1));
queue<pair<int,int>> q;
```

- `dist[r][c]` = Manhattan distance from cell `(r,c)` to the **nearest thief**.
- `-1` = "not visited yet".
- `q` = BFS queue storing `{row, col}` pairs.

### Step 1a: Push all thieves into queue

```cpp
for (int r = 0; r < n; r++) {
    for (int c = 0; c < n; c++) {
        if (grid[r][c] == 1) { dist[r][c] = 0; q.push({r, c}); }
    }
}
```

- Loop through every cell.
- If `grid[r][c] == 1` (thief), set its distance to **0** and add it to the queue.
- All thieves start at distance 0, BFS will expand from all of them at once.

### Step 1b: BFS expansion

```cpp
while (!q.empty()) {
    auto [r, c] = q.front(); q.pop();
```

- Pop the next cell from the queue.
- `r` = current row, `c` = current column.

```cpp
    for (auto& d : DIRS) {
        int nr = r + d[0], nc = c + d[1];
```

- For each direction `d`:
  - `nr` = **n**ew **r**ow (neighbor's row).
  - `nc` = **n**ew **c**ol (neighbor's col).

```cpp
        if (nr >= 0 && nr < n && nc >= 0 && nc < n && dist[nr][nc] == -1) {
            dist[nr][nc] = dist[r][c] + 1;
            q.push({nr, nc});
        }
    }
}
```

- Check the neighbor is:
  - **In bounds** (within `[0, n)`).
  - **Not yet visited** (`dist == -1`).
- If both true:
  - Set its distance to `dist[r][c] + 1` (one more step).
  - Add it to the queue.

**Result**: After BFS, `dist[r][c]` = distance to nearest thief.

---

## Phase 2: Max-Heap Dijkstra ("Widest Path")

```cpp
vector<vector<int>> best(n, vector<int>(n, -1));
priority_queue<array<int,3>> pq;
```

- `best[r][c]` = best (maximum) "min-distance-along-path" score we've found to reach `(r,c)`.
- `-1` = unreachable so far.
- `pq` = **max-heap** storing `{score, r, c}`.
  - `std::priority_queue` is a max-heap by default → pops the **largest** score first.

### Step 2a: Seed the heap at start position (0, 0)

```cpp
best[0][0] = dist[0][0];
pq.push({dist[0][0], 0, 0});
```

- The starting cell's best score is its own `dist` value (we start there).
- Push `{score, 0, 0}` into the heap.

### Step 2b: Main Dijkstra loop

```cpp
while (!pq.empty()) {
    auto [s, r, c] = pq.top(); pq.pop();
```

- Pop the cell with the **highest score** (max-heap behavior).
- `s` = **s**core of the path taken to reach `(r, c)`. This is the *minimum* dist value along the best path to `(r,c)` so far.
- `r` = current row, `c` = current column.

```cpp
    if (s < best[r][c]) continue;
```

- **Stale-entry check**: If the popped score is smaller than `best[r][c]`, this entry is outdated (we already found a better path). Skip it.

```cpp
    if (r == n - 1 && c == n - 1) return s;
```

- If we reached the destination `(n-1, n-1)`, this is **the optimal answer**.
- Why? The max-heap guarantees the first time we pop any cell, its score is optimal. So we can return immediately!

```cpp
    for (auto& d : DIRS) {
        int nr = r + d[0], nc = c + d[1];
```

- Try all 4 directions from `(r, c)`.
- `nr`, `nc` = neighbor's row and column.

```cpp
        if (nr < 0 || nr >= n || nc < 0 || nc >= n) continue;
```

- Skip neighbors outside the grid.

```cpp
        int ns = min(s, dist[nr][nc]);
```

- `ns` = **n**ew **s**core if we move to `(nr, nc)`.
- The path's bottleneck becomes the **minimum** of:
  - `s` (best score to reach current cell), and
  - `dist[nr][nc]` (this new cell's distance to nearest thief).

```cpp
        if (ns > best[nr][nc]) {
            best[nr][nc] = ns;
            pq.push({ns, nr, nc});
        }
    }
}
```

- If this new score is better than what we've seen for this neighbor:
  - Update `best[nr][nc]`.
  - Push the new entry into the heap.

### Final return

```cpp
return best[n - 1][n - 1];
```

- If the loop ends without early return, the destination is unreachable from start (shouldn't happen here per problem constraints).

---

## Variable Summary Table

| Variable | Meaning | Type | Scope |
|----------|---------|------|-------|
| `n` | Grid size (e.g., 3 for 3×3) | `int` | Function |
| `DIRS` | 4 movement directions | `const int[4][2]` | Function |
| `dist[r][c]` | Manhattan distance from `(r,c)` to nearest thief | `vector<vector<int>>` | Phase 1 |
| `q` | BFS queue of `{r, c}` pairs | `queue<pair<int,int>>` | Phase 1 |
| `r, c` | Current cell's row and column | `int` | BFS / Dijkstra loops |
| `nr, nc` | **N**eighbor cell's **r**ow and **c**ol | `int` | Direction iteration |
| `best[r][c]` | Best (max min) score found to reach `(r,c)` | `vector<vector<int>>` | Phase 2 |
| `pq` | Max-heap of `{score, r, c}` | `priority_queue<array<int,3>>` | Phase 2 |
| `s` | **S**core of the current cell's path (min dist along it) | `int` | Popped from heap |
| `ns` | **N**ew **s**core if we move to the neighbor | `int` | Relaxation |

---

## Concrete Example: 3×3 grid `[[0,0,1],[0,0,0],[0,0,0]]`

After Phase 1, `dist` looks like:
```
2 1 0
3 2 1
4 3 2
```

### Dijkstra trace:

| Step | Pop | `s` | Action |
|------|-----|-----|--------|
| 1 | `(0,0)` | 2 | Update neighbors: `(0,1)→1`, `(1,0)→2` |
| 2 | `(1,0)` | 2 | Update: `(2,0)→2`, `(1,1)→2` |
| 3 | `(2,0)` | 2 | Update: `(2,1)→2` |
| 4 | `(1,1)` | 2 | Update: `(2,1)` already 2, skip; `(1,2)→1` |
| 5 | `(2,1)` | 2 | Update: `(2,2)→2` |
| 6 | `(2,2)` | 2 | **Return 2** ✓ |

The path `(0,0) → (1,0) → (2,0) → (2,1) → (2,2)` has scores `2, 2, 2, 2, 2` — min = **2** = answer.

---

## Key Concepts to Remember

1. **Multi-source BFS**: All thieves are sources → computes "distance to nearest" in one pass.
2. **Max-heap Dijkstra**: The "widest path" problem is solved with a max-heap and `min` relaxation.
3. **Early exit**: The first time `(n-1, n-1)` pops from the max-heap, that's the answer.
4. **Stale entries**: The `s < best[r][c]` check skips outdated heap entries — this is standard lazy-deletion in Dijkstra.