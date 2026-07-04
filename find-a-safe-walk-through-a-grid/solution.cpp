class Solution {
public:
    bool findSafeWalk(vector<vector<int>>& grid, int health) {
        int m = grid.size(), n = grid[0].size();
        const int DIRS[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};

        // minimum unsafe cells visited on path from (0,0) to (r,c)
        vector<vector<int>> cost(m, vector<int>(n, INT_MAX));
        cost [0][0] = grid[0][0];
        deque<pair<int,int>> dq;
        dq.push_back({0, 0});

        // implement 0 - 1 BFS
        while (!dq.empty()) {
            auto [r, c] = dq.front(); dq.pop_front();
            for (auto& d : DIRS) {
                int nr = r + d[0], nc = c + d[1];
                if (nr < 0 || nr >= m || nc < 0 || nc >= n) continue;

                int newCost = cost[r][c] + grid[nr][nc];
                if (newCost < cost[nr][nc]) {
                    cost[nr][nc] = newCost;

                    if (grid[nr][nc] == 0) dq.push_front({nr, nc});
                    else dq.push_back({nr, nc});
                }
            }
        }

        return cost[m - 1][n - 1] <= health - 1;
    }
};