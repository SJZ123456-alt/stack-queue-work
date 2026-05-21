#pragma once

#include "Board.h"
#include "DataStructures.h"

#include <string>
#include <vector>

// SolveResult 保存计算机玩家求解后的全部结果。
struct SolveResult {
    bool solvable;                    // 初始棋盘在数学上是否有解。
    bool solved;                      // 求解器是否已经找到完整路径。
    bool optimal;                     // 当前路径是否保证为最短路径。
    int steps;                        // 解法包含的移动步数。
    long long expandedNodes;          // BFS 或 IDA* 扩展过的搜索节点数。
    std::string message;              // 显示在界面上的状态提示。
    std::vector<MoveDirection> moves; // 从初始状态到目标状态的完整移动序列。

    // 将求解结果初始化为空结果。
    SolveResult();
};

// Solver 是计算机玩家。
// 3x3 使用 BFS，4x4 使用 IDA*。
class Solver {
private:
    int idaLimit;                         // IDA* 搜索允许的最大深度。
    long long expanded;                   // IDA* 搜索过程中扩展的节点计数。
    std::vector<MoveDirection> idaPath;   // IDA* 当前递归路径。

    // IDA* 的递归深度优先搜索部分。
    // board 会在搜索中临时改变，每个分支结束后再恢复。
    // g 表示当前深度，bound 表示当前 f=g+h 上限，previous 用于避免立即走回头路。
    int idaSearch(Board& board, int g, int bound, MoveDirection previous);

public:
    // 构造求解器，并设置默认 IDA* 深度上限。
    Solver();

    // 根据棋盘规模自动选择合适算法。
    SolveResult solve(const Board& start);

    // 使用自定义循环队列实现广度优先搜索，适合 3x3。
    SolveResult solveByBfs(const Board& start);

    // 使用 IDA* 迭代加深搜索，适合 4x4 最短路。
    SolveResult solveByIdaStar(const Board& start);

    // 根据解法数组构造栈，使自动演示时可以按正确顺序 pop 出移动方向。
    static SeqStack<MoveDirection> buildMoveStack(const std::vector<MoveDirection>& moves);
};


