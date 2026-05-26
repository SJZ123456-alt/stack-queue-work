#include "Solver.h"
#include <unordered_map>
#include <algorithm>
#include <time.h>

Solver::Solver() : idaLimit(80) {}//因为4*4最大数为80

SolveResult Solver::solve(const Board& start) {
    if (!start.isSolvable()) {
        SolveResult res;
        res.solvable = false;
        return res;
    }
    if (start.size() == 3) return solveByBfs(start);
    return solveOptimal(start);
}

SolveResult Solver::solveByBfs(const Board& start) {
    SolveResult result;
    result.solvable = true;
    if (start.isGoal()) 
    { 
        result.solved = true;
        return result;
    }

    struct PrevInfo {
		std::string parent;//上一个状态的编码
		MoveDirection move;//从上一个状态到当前状态的移动方向
    };

    CirQueue<Board> queue(100000);
    std::unordered_map<std::string, PrevInfo> visited;

    std::string startCode = start.encode();
    std::string goalCode = Board::goal(start.size()).encode();

    queue.push(start);
    visited[startCode] = { "", MOVE_NONE };

    while (!queue.empty()) {
        Board current = queue.pop();
        std::string currentCode = current.encode();

        SeqList<MoveDirection> moves = current.legalMoves();
        for (int i = 0; i < moves.size(); ++i) {
            MoveDirection dir = moves[i];
            Board next = current.moved(dir);
            std::string nextCode = next.encode();

            if (visited.find(nextCode) != visited.end()) continue;//剪枝

            visited[nextCode] = { currentCode, dir };
            if (nextCode == goalCode) // 回溯路径
            {
                SeqStack<MoveDirection> reverseStack;
                std::string p = nextCode;
                while (visited[p].move != MOVE_NONE) {
                    reverseStack.push(visited[p].move);
                    p = visited[p].parent;
                }
                while (!reverseStack.empty()) {
                    result.moves.push_back(reverseStack.pop());
                }
                result.steps = result.moves.size();
                result.solved = true;
                return result;
            }
            queue.push(next);
        }
    }
    return result;
}

int Solver::idaSearch(Board& board, int g, int bound, MoveDirection previous, int weight) {
    if (++nodeCount % 1000000 == 0) {
        if ((clock() - startTime) / CLOCKS_PER_SEC >= 2) {
            return -2;
        }
    }

    int f = g + board.manhattan() * weight;
    if (f > bound) return f;
    if (board.isGoal()) return -1;

    int minNextBound = 999999;
    MoveDirection dirs[4] = { MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT };

    for (int i = 0; i < 4; ++i) {
        MoveDirection dir = dirs[i];  
        if (oppositeMove(dir) == previous) continue;//剪枝
        if (!board.move(dir)) continue;
        idaPath.push(dir);

        int value = idaSearch(board, g + 1, bound, dir, weight);
        if (value == -2) return -2;//超时
        if (value == -1) return -1;//成功
        if (value < minNextBound) minNextBound = value;

        idaPath.pop();
        board.move(oppositeMove(dir));
    }
    return minNextBound;
}

/*SolveResult Solver::solveByIdaStar(const Board& start) {
    SolveResult result;
    result.solvable = true;
    result.solved = false; // 默认没算完

    Board board = start;
    int bound = board.manhattan();
    idaPath.clear();

    nodeCount = 0;
    startTime = clock();

    while (bound <= idaLimit) {
        int value = idaSearch(board, 0, bound, MOVE_NONE);
        if (value == -2) {
            result.solved = false;
            return result;
        }

        if (value == -1) {
            SeqStack<MoveDirection> tempPath = idaPath;
            SeqStack<MoveDirection> reverseStack;
            while (!tempPath.empty()) reverseStack.push(tempPath.pop());
            while (!reverseStack.empty()) result.moves.push_back(reverseStack.pop());

            result.steps = result.moves.size();
            result.solved = true;
            return result;
        }
        bound = value;
    }
    return result;
}*/

SolveResult Solver::solveFast(const Board& start) {
    if (start.size() == 3) return solveByBfs(start); // 3x3 极其简单，直接给最优解就行

    SolveResult result;
    result.solvable = true; result.solved = false;

    Board board = start;
    int bound = board.manhattan() * 4; // 初始阈值也要带上权重
    idaPath.clear();
    nodeCount = 0;
    startTime = clock();

    // 贪婪算法的步数可能会走到 100 多步，所以这里的限制放宽到 9999
    while (bound <= 9999) {
        int value = idaSearch(board, 0, bound, MOVE_NONE, 4); // 传入权重 4
        if (value == -2) return result; // 超时退出
        if (value == -1) {
            SeqStack<MoveDirection> tempPath = idaPath;
            SeqStack<MoveDirection> reverseStack;
            while (!tempPath.empty()) reverseStack.push(tempPath.pop());
            while (!reverseStack.empty()) result.moves.push_back(reverseStack.pop());
            result.steps = result.moves.size();
            result.solved = true;
            return result;
        }
        bound = value;
    }
    return result;
}

// 【核心修改 3】：实现极限最优解（权重 = 1）
SolveResult Solver::solveOptimal(const Board& start) {
    if (start.size() == 3) return solveByBfs(start);

    SolveResult result;
    result.solvable = true; result.solved = false;

    Board board = start;
    int bound = board.manhattan();
    idaPath.clear();
    nodeCount = 0;
    startTime = clock();

    while (bound <= idaLimit) {
        int value = idaSearch(board, 0, bound, MOVE_NONE, 1); // 传入权重 1，纯正血统
        if (value == -2) return result; // 触发 2 秒超时
        if (value == -1) {
            SeqStack<MoveDirection> tempPath = idaPath;
            SeqStack<MoveDirection> reverseStack;
            while (!tempPath.empty()) reverseStack.push(tempPath.pop());
            while (!reverseStack.empty()) result.moves.push_back(reverseStack.pop());
            result.steps = result.moves.size();
            result.solved = true;
            return result;
        }
        bound = value;
    }
    return result;
}