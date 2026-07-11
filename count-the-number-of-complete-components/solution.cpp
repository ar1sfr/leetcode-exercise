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
