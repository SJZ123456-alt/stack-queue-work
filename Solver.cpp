#include "Solver.h"

#include <algorithm>
#include <climits>
#include <unordered_map>

SolveResult::SolveResult()
    // 初始状态表示“还没有产生解法”。
    : solvable(false), solved(false), optimal(false),
      steps(0), expandedNodes(0), message("") {
}

Solver::Solver() : idaLimit(80), expanded(0) {
    // idaLimit 用于避免 4x4 极深搜索无限运行。
}

SolveResult Solver::solve(const Board& start) {
    // 先用数学规则判断是否有解，无解棋盘不需要搜索。
    if (!start.isSolvable()) {
        SolveResult result;
        result.solvable = false;
        result.solved = false;
        result.message = "该棋盘无解。";
        return result;
    }

    // 3x3 状态空间较小，用自定义队列 BFS 可以得到最短路。
    if (start.size() == 3) return solveByBfs(start);

    // 4x4 使用曼哈顿距离启发的 IDA*，也能得到最短路。
    return solveByIdaStar(start);
}

SolveResult Solver::solveByBfs(const Board& start) {
    SolveResult result;        // 最终返回给调用者的求解结果。
    result.solvable = start.isSolvable();
    if (!result.solvable) {
        result.message = "该棋盘无解。";
        return result;
    }

    if (start.isGoal()) {
        result.solved = true;
        result.optimal = true;
        result.message = "已经是目标状态。";
        return result;
    }

    struct PrevInfo {
        std::string parent;    // 前一个棋盘状态的编码。
        MoveDirection move;    // 从父状态移动到当前状态所用的方向。
    };

    CirQueue<Board> queue;                         // BFS 搜索队列，使用自己实现的循环队列。
    std::unordered_map<std::string, PrevInfo> visited; // 记录访问过的状态和路径来源。
    std::string startCode = start.encode();        // 初始棋盘的哈希键。
    std::string goalCode = Board::goal(start.size()).encode(); // 目标棋盘的哈希键。

    queue.push(start);
    visited[startCode] = { "", MOVE_NONE };

    while (!queue.empty()) {
        Board current = queue.pop();               // 取出队头状态，体现 BFS 先进先出特性。
        ++result.expandedNodes;
        std::string currentCode = current.encode(); // 当前状态的编码。

        std::vector<MoveDirection> moves = current.legalMoves(); // 当前状态可以扩展出的相邻状态。
        for (MoveDirection dir : moves) {
            Board next = current.moved(dir);        // 移动一步后的新棋盘。
            std::string nextCode = next.encode();   // 新棋盘状态编码。
            if (visited.find(nextCode) != visited.end()) continue;

            visited[nextCode] = { currentCode, dir };
            if (nextCode == goalCode) {
                std::vector<MoveDirection> reverseMoves; // 从目标状态向初始状态回溯得到的反向路径。
                std::string p = nextCode;                // 回溯过程中当前状态编码。
                while (visited[p].move != MOVE_NONE) {
                    reverseMoves.push_back(visited[p].move);
                    p = visited[p].parent;
                }
                std::reverse(reverseMoves.begin(), reverseMoves.end());

                result.moves = reverseMoves;
                result.steps = static_cast<int>(result.moves.size());
                result.solved = true;
                result.optimal = true;
                result.message = "BFS 已找到最短移动方案。";
                return result;
            }

            queue.push(next);
        }
    }

    result.message = "搜索结束但未找到解。";
    return result;
}

int Solver::idaSearch(Board& board, int g, int bound, MoveDirection previous) {
    int f = g + board.manhattan(); // IDA* 估价函数：实际深度 g + 启发值 h。
    if (f > bound) return f;
    if (board.isGoal()) return -1;

    ++expanded;
    int minNextBound = INT_MAX;                  // 超过当前 bound 的最小 f 值，用作下一轮 bound。
    std::vector<MoveDirection> moves = board.legalMoves(); // 当前节点的候选分支。

    // 优先尝试启发值更小的方向，通常可以减少搜索量。
    std::sort(moves.begin(), moves.end(), [&](MoveDirection a, MoveDirection b) {
        Board ba = board.moved(a);
        Board bb = board.moved(b);
        return ba.manhattan() < bb.manhattan();
    });

    for (MoveDirection dir : moves) {
        // 避免刚走一步又立刻反向走回来，减少无意义循环。
        if (oppositeMove(dir) == previous) continue;

        // 尝试当前分支。
        board.move(dir);
        idaPath.push_back(dir);
        int value = idaSearch(board, g + 1, bound, dir);
        if (value == -1) return -1;
        if (value < minNextBound) minNextBound = value;

        // 尝试其他分支前，恢复棋盘和路径。
        idaPath.pop_back();
        board.move(oppositeMove(dir));
    }

    return minNextBound;
}

SolveResult Solver::solveByIdaStar(const Board& start) {
    SolveResult result;      // 最终返回给调用者的求解结果。
    result.solvable = start.isSolvable();
    if (!result.solvable) {
        result.message = "该棋盘无解。";
        return result;
    }

    Board board = start;             // 搜索中会被递归临时修改的棋盘。
    int bound = board.manhattan();   // IDA* 第一轮 bound 为初始启发值。
    expanded = 0;
    idaPath.clear();

    while (bound <= idaLimit) {
        int value = idaSearch(board, 0, bound, MOVE_NONE); // 返回 -1 表示找到目标。
        if (value == -1) {
            result.moves = idaPath;
            result.steps = static_cast<int>(result.moves.size());
            result.expandedNodes = expanded;
            result.solved = true;
            result.optimal = true;
            result.message = "IDA* 已找到最短移动方案。";
            return result;
        }
        if (value == INT_MAX) break;
        bound = value;
    }

    result.expandedNodes = expanded;
    result.solved = false;
    result.optimal = true;
    result.message = "该棋盘有解，但当前深度上限内未找到完整方案。";
    return result;
}

SeqStack<MoveDirection> Solver::buildMoveStack(const std::vector<MoveDirection>& moves) {
    SeqStack<MoveDirection> stack(static_cast<int>(moves.size()) + 8); // 自动演示用的栈。
    // 倒序入栈，这样 pop() 时能按第一步、第二步的顺序弹出。
    for (int i = static_cast<int>(moves.size()) - 1; i >= 0; --i) {
        stack.push(moves[i]);
    }
    return stack;
}


