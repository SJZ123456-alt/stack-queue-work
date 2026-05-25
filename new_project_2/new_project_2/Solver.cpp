#include "Solver.h"
#include <unordered_map>
#include <algorithm>

Solver::Solver() : idaLimit(80) {}

SolveResult Solver::solve(const Board& start) {
    if (!start.isSolvable()) {
        SolveResult res; res.solvable = false; return res;
    }
    if (start.size() == 3) return solveByBfs(start);
    return solveByIdaStar(start);
}

SolveResult Solver::solveByBfs(const Board& start) {
    SolveResult result;
    result.solvable = true;
    if (start.isGoal()) { result.solved = true; return result; }

    struct PrevInfo {
        std::string parent;
        MoveDirection move;
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

            if (visited.find(nextCode) != visited.end()) continue;

            visited[nextCode] = { currentCode, dir };
            if (nextCode == goalCode) {
                // »ØËÝÂ·¾¶
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

int Solver::idaSearch(Board& board, int g, int bound, MoveDirection previous) {
    int f = g + board.manhattan();
    if (f > bound) return f;
    if (board.isGoal()) return -1;

    int minNextBound = 999999;
    SeqList<MoveDirection> moves = board.legalMoves();

    for (int i = 0; i < moves.size(); ++i) {
        MoveDirection dir = moves[i];
        if (oppositeMove(dir) == previous) continue;

        board.move(dir);
        idaPath.push(dir);

        int value = idaSearch(board, g + 1, bound, dir);
        if (value == -1) return -1;
        if (value < minNextBound) minNextBound = value;

        idaPath.pop();
        board.move(oppositeMove(dir));
    }
    return minNextBound;
}

SolveResult Solver::solveByIdaStar(const Board& start) {
    SolveResult result;
    result.solvable = true;

    Board board = start;
    int bound = board.manhattan();
    idaPath.clear();

    while (bound <= idaLimit) {
        int value = idaSearch(board, 0, bound, MOVE_NONE);
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