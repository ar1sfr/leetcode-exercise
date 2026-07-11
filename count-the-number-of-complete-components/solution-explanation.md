```cpp
class Solution {
    vector<int> parent, sz;               // DSU: representative + component size

    // find with path halving — iterative, no recursion, no std::function overhead
    int find(int x) {
        while (parent[x] != x) {
            parent[x] = parent[parent[x]]; // halve the path on the way up
            x = parent[x];
        }
        return x;
    }

public:
    int countCompleteComponents(int n, vector<vector<int>>& edges) {
        parent.resize(n);
        sz.assign(n, 1);
        iota(parent.begin(), parent.end(), 0); // every vertex starts as its own root

        // step 1: union endpoints of every edge (union by size keeps trees shallow)
        for (auto& e : edges) {
            int a = find(e[0]), b = find(e[1]);
            if (a != b) {
                if (sz[a] < sz[b]) swap(a, b); // attach smaller tree under larger
                parent[b] = a;
                sz[a] += sz[b];
            }
        }

        // step 2: tally how many edges land in each component (keyed by root)
        vector<int> ecount(n, 0);
        for (auto& e : edges) ecount[find(e[0])]++;

        // step 3: a component of k vertices is complete iff it has k*(k-1)/2 edges
        int ans = 0;
        for (int i = 0; i < n; i++)
            if (find(i) == i && ecount[i] == sz[i] * (sz[i] - 1) / 2)
                ++ans;
        return ans;
    }
};
```

## Problem Reformulation

We must count connected components that are **complete** (every pair of vertices in the component is directly connected by an edge).

The whole problem collapses to a single counting identity. A complete graph on `k` vertices — the clique `Kₖ` — has exactly:

$$\binom{k}{2} = \frac{k(k-1)}{2} \text{ edges.}$$

So a component is complete **if and only if** its edge count equals `k·(k-1)/2`, where `k` is its vertex count. There is nothing to check pair-by-pair — we only need two numbers per component:

| Per component | How we get it |
|---------------|---------------|
| `k` = number of vertices | component **size** tracked during union |
| `m` = number of edges | each edge tallied to its component's root |

The problem therefore reduces to: **group vertices into components, then for each component compare `m` against `k·(k-1)/2`.**

| Component shape | `k` | `k·(k-1)/2` | Complete? |
|-----------------|-----|-------------|-----------|
| Isolated vertex | 1 | 0 | ✅ (vacuously) |
| Single edge | 2 | 1 | ✅ |
| Triangle | 3 | 3 | ✅ |
| Path of 3 (`3-4-5`) | 3 | 3, but has 2 | ❌ |

## Algorithm Choice — Union-Find (DSU)

Grouping vertices into components is the textbook job of **Disjoint Set Union**. It builds all components in near-linear time and — crucially — lets us maintain each component's **size** for free during the union step, which is exactly the `k` we need.

Two standard optimizations make each operation effectively O(1):

- **Union by size** — always attach the smaller tree under the larger root, bounding tree height.
- **Path halving** in `find` — every lookup shortcuts nodes to their grandparent, flattening the tree over time.

Together they give an amortized cost of `O(α(n))` per operation, where `α` is the inverse-Ackermann function (`≤ 4` for any input that fits in the universe).

| Alternative | Time | Space | Verdict |
|-------------|------|-------|---------|
| Build adjacency list + BFS/DFS, then per-vertex degree check | `O(n + E)` | `O(n + E)` | ✅ Correct, but materializes the whole graph → more memory |
| Brute force: for each component test all pairs | `O(n²)` per component | `O(n + E)` | ❌ Wasteful; redundant given the clique-edge identity |
| **DSU (size + edge tally)** | `O((n + E)·α(n))` | `O(n)` | ✅ Optimal complexity, **flat O(n) memory** (no graph built) |

The DSU route never constructs an adjacency structure — it scans the raw `edges` array twice — so its auxiliary memory stays at a flat `O(n)` instead of `O(n + E)`. That is the decisive edge over BFS/DFS, which share the same big-O but pay for storing the graph.

## Step-by-Step Walkthrough

**Step 1 — Initialize the DSU.**

```cpp
parent.resize(n);
sz.assign(n, 1);
iota(parent.begin(), parent.end(), 0);
```

Every vertex is its own root (`parent[i] = i`) and every component has size 1.

**Step 2 — Union each edge's endpoints.**

```cpp
for (auto& e : edges) {
    int a = find(e[0]), b = find(e[1]);
    if (a != b) {
        if (sz[a] < sz[b]) swap(a, b);
        parent[b] = a;
        sz[a] += sz[b];
    }
}
```

After this loop, two vertices share a root **iff** they are in the same connected component, and `sz[root]` holds that component's vertex count `k`.

**Step 3 — Tally edges per component.**

```cpp
vector<int> ecount(n, 0);
for (auto& e : edges) ecount[find(e[0])]++;
```

Both endpoints of an edge share the same root, so it suffices to look up the root of one endpoint. `ecount[root]` accumulates the edge count `m` of each component.

**Step 4 — Apply the clique identity.**

```cpp
int ans = 0;
for (int i = 0; i < n; i++)
    if (find(i) == i && ecount[i] == sz[i] * (sz[i] - 1) / 2)
        ++ans;
return ans;
```

We visit only **roots** (`find(i) == i`), so each component is counted once, and mark it complete exactly when `m == k·(k-1)/2`.

## Variable Definitions

| Variable | Meaning | Type |
|----------|---------|------|
| `n` | Number of vertices | `int` |
| `edges` | Input edge list `[a, b]` | `vector<vector<int>>` |
| `parent` | DSU parent pointer per vertex | `vector<int>` |
| `sz` | Component size, valid at roots | `vector<int>` |
| `ecount` | Edge count per component, keyed by root | `vector<int>` |
| `a, b` | Roots of the two endpoints being unioned | `int` |
| `k` (`sz[i]`) | Vertices in component `i` | `int` |
| `ans` | Count of complete components | `int` |

## Verification with Examples

### Example 1: `n = 6`, `edges = [[0,1],[0,2],[1,2],[3,4]]` → **3**

| Component | Vertices (`k`) | Edges (`m`) | `k·(k-1)/2` | Complete? |
|-----------|----------------|-------------|-------------|-----------|
| `{0,1,2}` | 3 | 3 | 3 | ✅ |
| `{3,4}` | 2 | 1 | 1 | ✅ |
| `{5}` | 1 | 0 | 0 | ✅ |

Count = **3** ✓

### Example 2: `n = 6`, `edges = [[0,1],[0,2],[1,2],[3,4],[3,5]]` → **1**

| Component | Vertices (`k`) | Edges (`m`) | `k·(k-1)/2` | Complete? |
|-----------|----------------|-------------|-------------|-----------|
| `{0,1,2}` | 3 | 3 | 3 | ✅ |
| `{3,4,5}` | 3 | 2 | 3 | ❌ (missing edge `4-5`) |

Count = **1** ✓

### Edge case: `n = 1`, `edges = []` → **1**

Single isolated vertex: `k = 1`, `m = 0`, `1·0/2 = 0`. Complete vacuously → **1** ✓

## Complexity Analysis

| Metric | Value | At `n = 50` |
|--------|-------|-------------|
| **Time** | `O((n + E)·α(n))` ≈ `O(n + E)` | ≤ 50 vertices + ≤ 1225 edges, trivial |
| **Space** | `O(n)` | 3 int arrays of size ≤ 50 |
| **Constants** | Minimal | iterative `find`, no heap, no adjacency list |

`α(n)` is the inverse-Ackermann function, bounded by 4 for all practical `n`, so time is effectively linear in the input size.

## DSA Choices — Why Each

| Component | Choice | Why |
|-----------|--------|-----|
| Grouping | **Union-Find** | Builds components in near-linear time and yields component **size** for free — exactly the `k` we need |
| `find` strategy | **Path halving** (iterative) | O(1) amortized lookups without recursion frames or `std::function` indirection |
| `union` strategy | **Union by size** | Keeps trees shallow so `find` stays near O(1) |
| Edge count | `vector<int>` keyed by root | Direct index array; no hash map needed since roots are in `[0, n)` |
| Completeness test | **`k·(k-1)/2` identity** | Reduces "every pair connected?" to one integer comparison |

## Memory Layout

| Component | Size | Notes |
|-----------|------|-------|
| `parent` array | `n × 4B` ≈ 200 B at max | Row of ints |
| `sz` array | `n × 4B` ≈ 200 B at max | Component sizes |
| `ecount` array | `n × 4B` ≈ 200 B at max | Per-root edge tally |
| **Total auxiliary** | **≈ 600 B** at `n = 50` | Flat `O(n)`, no graph materialized |

The input `edges` is scanned in place — never copied into an adjacency list — which is why memory stays at three small arrays.

## Correctness Argument

1. **Component identity**: After the union loop, `find(u) == find(v)` iff `u` and `v` are connected — the standard DSU invariant. Hence `sz[root]` equals the true vertex count `k` of each component.
2. **Edge attribution**: Each edge's endpoints share a root, so incrementing `ecount[find(e[0])]` counts every edge in exactly one component; `ecount[root] = m`.
3. **Clique characterization**: A simple graph on `k` vertices is complete iff it contains all `k·(k-1)/2` distinct vertex pairs as edges. Since the input has **no repeated edges**, `m == k·(k-1)/2` is both necessary and sufficient for completeness.
4. **No double counting**: The final loop counts a component only at its unique root (`find(i) == i`), so each component contributes at most once.

## Verification Checklist

- [x] `iota` seeds each vertex as its own root.
- [x] Union by size attaches the smaller tree under the larger.
- [x] `find` uses path halving — iterative, no recursion.
- [x] Edge tally keyed by root; both endpoints share it.
- [x] Only roots (`find(i) == i`) are counted, so no double counting.
- [x] Completeness uses the `k·(k-1)/2` clique identity.
- [x] `k·(k-1)/2` fits in `int` (max `50·49/2 = 1225`) — no overflow.
- [x] Isolated vertices (`k = 1`) correctly counted as complete.
- [x] Both LeetCode examples and the `n = 1` edge case verified.
- [x] Compiles under `-std=c++17`; relies on `<vector>`, `<numeric>` (`iota`), `<algorithm>` (`swap`).

-------------------------------------------------------------------------------

## Line-by-Line Code Walkthrough

Let's trace the code on **Example 2** (`n = 6`, `edges = [[0,1],[0,2],[1,2],[3,4],[3,5]]`).

### Initialization

```cpp
parent.resize(n);              // parent = [_, _, _, _, _, _]
sz.assign(n, 1);               // sz     = [1, 1, 1, 1, 1, 1]
iota(parent.begin(), parent.end(), 0);  // parent = [0, 1, 2, 3, 4, 5]
```

Every vertex is its own root; every component has size 1.

### Union Loop

Each edge merges two components. Roots are found via `find`, and the smaller tree is attached under the larger.

| Edge | `find(a), find(b)` | Action | Resulting `parent` / `sz` |
|------|--------------------|--------|---------------------------|
| `[0,1]` | 0, 1 | equal size → attach 1 under 0 | `parent[1]=0`, `sz[0]=2` |
| `[0,2]` | 0, 2 | `sz[0]=2 > sz[2]=1` → attach 2 under 0 | `parent[2]=0`, `sz[0]=3` |
| `[1,2]` | 0, 0 | same root → **skip** (no merge) | unchanged |
| `[3,4]` | 3, 4 | equal size → attach 4 under 3 | `parent[4]=3`, `sz[3]=2` |
| `[3,5]` | 3, 5 | `sz[3]=2 > sz[5]=1` → attach 5 under 3 | `parent[5]=3`, `sz[3]=3` |

After the loop:
- Component rooted at **0** = `{0,1,2}`, `sz[0] = 3`.
- Component rooted at **3** = `{3,4,5}`, `sz[3] = 3`.

### Edge Tally

```cpp
for (auto& e : edges) ecount[find(e[0])]++;
```

| Edge | `find(e[0])` | `ecount` after |
|------|--------------|----------------|
| `[0,1]` | 0 | `ecount[0]=1` |
| `[0,2]` | 0 | `ecount[0]=2` |
| `[1,2]` | 0 | `ecount[0]=3` |
| `[3,4]` | 3 | `ecount[3]=1` |
| `[3,5]` | 3 | `ecount[3]=2` |

So `ecount[0] = 3`, `ecount[3] = 2`.

### Final Count

```cpp
for (int i = 0; i < n; i++)
    if (find(i) == i && ecount[i] == sz[i] * (sz[i] - 1) / 2)
        ++ans;
```

Only roots are examined:

| Root `i` | `sz[i]` (`k`) | `k·(k-1)/2` | `ecount[i]` (`m`) | Complete? |
|----------|---------------|-------------|-------------------|-----------|
| 0 | 3 | 3 | 3 | ✅ `ans → 1` |
| 3 | 3 | 3 | 2 | ❌ (missing edge `4-5`) |

Non-root vertices (1, 2, 4, 5) fail `find(i) == i` and are skipped.

**Return `ans = 1`** ✓ — matching the expected output.

### Why the `k·(k-1)/2` Check Is Airtight

Component `{3,4,5}` *is* connected (a path `4-3-5`), so a naive "is it one piece?" test would wrongly accept it. The edge-count identity catches the missing `4-5` edge: a triangle needs 3 edges but this component has only 2. Because the input guarantees **no repeated edges**, the count `m` is an exact measure of distinct pairs connected — making the single comparison `m == k·(k-1)/2` a complete and correct clique test.
