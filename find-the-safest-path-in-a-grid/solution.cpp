class Solution {
public:
    int maximumSafenessFactor(vector<vector<int>>& grid) {
        int n = grid.size();
        const int DIRS[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};

        // step 1: using multi-source BFS
        vector<vector<int>> dist(n, vector<int>(n ,-1));
        queue<pair<int, int>> q;
        for (int r = 0; r < n; r++) {
            for (int c = 0; c < n; c++) {
                if (grid[r][c] == 1) {
                    dist[r][c] = 0; q.push({r, c});
                }
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

        // step 2: using max-heap (Djikstra model)
        vector<vector<int>> best(n, vector<int>(n , -1));
        priority_queue<array<int, 3>> pq;
        best[0][0] = dist[0][0];
        pq.push({dist[0][0], 0, 0});

        while (!pq.empty()) {
            auto [s, r, c] = pq.top(); pq.pop();
            if (s < best[r][c]) continue;
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