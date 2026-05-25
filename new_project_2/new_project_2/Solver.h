#pragma once
#include "Board.h"
#include "Structure.h"
#include <string>

struct SolveResult {
    bool solvable;
    bool solved;
    int steps;
    SeqList<MoveDirection> moves;
    SolveResult() : solvable(false), solved(false), steps(0) {}
};

class Solver {
private:
    int idaLimit;
    SeqStack<MoveDirection> idaPath;

    int idaSearch(Board& board, int g, int bound, MoveDirection previous);

public:
    Solver();
    SolveResult solve(const Board& start);
    SolveResult solveByBfs(const Board& start);
    SolveResult solveByIdaStar(const Board& start);
};
