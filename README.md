# LeetCode Exercise Repository

Welcome to my LeetCode solutions repository! 🎯

This repository contains a collection of LeetCode problems I have solved, written in **C++** and accompanied by **detailed explanations** for every solution. The main goal of this repository is to serve as a **learning reference** for anyone who wants to understand how to solve LeetCode problems — not just the working code, but also **the reasoning behind every algorithm and data structure choice**.

---

## 📑 Table of Contents

- [Purpose of This Repository](#-purpose-of-this-repository)
- [Repository Structure](#-repository-structure)
- [Writing Conventions & Style](#-writing-conventions--style)
- [Problem Index](#-problem-index)
- [How to Learn from / Contribute to This Repo](#-how-to-learn-from--contribute-to-this-repo)
- [Roadmap & Targets](#-roadmap--targets)
- [Prerequisites](#-prerequisites)
- [License](#-license)

---

## 🎯 Purpose of This Repository

1. **Personal Documentation** — An archive of LeetCode solutions I have completed, with a strong focus on quality and clarity.
2. **Learning Reference** — A reference point for others who want to:
   - Understand *problem reformulation* (transforming a problem into a mathematical/algorithmic form).
   - See *trade-offs* between algorithms (e.g., BFS vs. Dijkstra vs. 0-1 BFS).
   - Study time and space complexity along with their justifications.
   - Follow a *line-by-line code walkthrough* with concrete examples.
3. **High Code Standards** — Every solution is documented with:
   - Algorithm choice based on the problem's constraints.
   - Big-O complexity analysis.
   - Memory layout and overhead.
   - A *verification checklist* to validate correctness.

---

## 📂 Repository Structure

```
leetcode-exercise/
├── README.md
├── <problem-slug>/
│   ├── solution.cpp              # C++ solution source
│   └── solution-explanation.md   # Detailed explanation + walkthrough
└── ...
```

### Folder Naming

Each problem folder uses its **LeetCode slug** (lowercase, separated by `-`).
Example: `find-the-safest-path-in-a-grid/`

### Contents of Each Problem Folder

| File | Description |
|------|-------------|
| `solution.cpp` | Solution implementation in C++ (typically only the `Solution` class, matching the LeetCode standard). |
| `solution-explanation.md` | Comprehensive write-up: problem reformulation, algorithm, complexity, line-by-line walkthrough, example-based verification. |

---

## ✍️ Writing Conventions & Style

Every `solution-explanation.md` follows a consistent 11-section structure:

1. **Problem Reformulation** — Re-stating the problem in a form that is easier to solve algorithmically.
2. **Algorithm Choice** — Discussion of why a particular algorithm is selected, including the alternatives considered.
3. **Step-by-Step Walkthrough** — Detailed explanation of each step with tables and pseudocode.
4. **Variable Definitions** — Table describing the meaning of every variable.
5. **Verification with Examples** — Validation using LeetCode sample inputs.
6. **Complexity Analysis** — Big-O time & space complexity, with justification.
7. **DSA Choices — Why Each** — Comparison table explaining why each data structure is used.
8. **Memory Layout** — Estimated byte footprint for every component.
9. **Correctness Argument** — A brief proof of algorithmic correctness.
10. **Verification Checklist** — List of properties that have been verified.
11. **Line-by-Line Code Walkthrough** — A detailed code explanation with simulation on a small input.

### Coding Principles

- **C++17** as the minimum standard.
- **STL only** — no external dependencies.
- **Minimum time & space complexity** — always pick the most efficient solution that remains easy to understand.
- **Consistent variable naming** — `nr`, `nc` for neighbor, `r`, `c` for current, `s`/`ns` for score, `dq` for deque, `pq` for priority queue, etc.
- **English content** for solutions and explanations (the universal technical standard).

---

## 📚 Problem Index

| # | Problem | LeetCode # | Difficulty | Topics | Algorithm | Folder |
|---|---------|------------|------------|--------|-----------|--------|
| 1 | Find the Safest Path in a Grid | 2812 | Medium | Grid BFS, Max-Heap, Widest Path | Multi-source BFS + Dijkstra (max-heap) | [find-the-safest-path-in-a-grid](./find-the-safest-path-in-a-grid/) |
| 2 | Find a Safe Walk Through a Grid | 3286 | Medium | Grid, Shortest Path, 0-1 Weights | 0-1 BFS with `deque` | [find-a-safe-walk-through-a-grid](./find-a-safe-walk-through-a-grid/) |
| 3 | Count the Number of Complete Components | 2685 | Medium | Graph, Union-Find, Connected Components | DSU (union by size + path halving) + clique-edge identity | [count-the-number-of-complete-components](./count-the-number-of-complete-components/) |

> 📌 This table will be updated as new problems are added.

---

## 🚀 How to Learn from / Contribute to This Repo

### For Readers (Learning Path)

1. Open the folder of the problem you want to study.
2. Read `solution-explanation.md` **before** opening `solution.cpp` to build intuition.
3. Once you understand the algorithm, open `solution.cpp` and match it with the explanation.
4. Try re-implementing the solution **without looking at the code** as practice.
5. Run the solution on LeetCode to verify correctness.

### For Contributors (Adding a New Problem)

If you would like to add a new problem, follow the existing structure:

```
leetcode-exercise/<problem-slug>/
├── solution.cpp
└── solution-explanation.md
```

Make sure the explanation covers at minimum:
- Problem reformulation
- Algorithm + justification
- Time & space complexity
- Verification with examples
- Line-by-line walkthrough

---

## 🗺️ Roadmap & Targets

- [ ] Add new problems on a regular basis
- [ ] Categorize problems by *difficulty tier* (Easy / Medium / Hard)
- [ ] Add **topic categories** (Array, Graph, DP, Greedy, etc.)
- [ ] Auto-generate a *problem index* by topic
- [ ] Multi-language versions (C++ + Python) for selected problems

---

## 🛠️ Prerequisites

- A C++ compiler with **C++17** support (e.g., `g++` ≥ 7, `clang++` ≥ 5, or MSVC 2017+).
- Standard headers used: `<vector>`, `<deque>`, `<queue>`, `<array>`, `<algorithm>`, `<climits>`, etc.

Example compilation:
```bash
g++ -std=c++17 -O2 -Wall -o solution solution.cpp
```

---

## 📄 License

This repository is released under the **MIT License** — free to use for learning, reference, or contribution, provided that attribution is preserved.

---

## 💬 Contact & Feedback

If you have feedback, corrections, or suggestions for improving any of the solution explanations, please feel free to open an **Issue** or **Pull Request** in this repository.

> *"The best way to learn is to teach."* — Happy coding! 🚀
