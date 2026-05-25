#pragma once
#include "Board.h"
#include "Structure.h"
#include <string>

struct SolveResult {
    bool solvable;  //有没有解
	bool solved;    //计算机是否成功找到
    int steps;      //最少步数
    SeqList<MoveDirection> moves;//具体的步骤
    SolveResult() : solvable(false), solved(false), steps(0) {}
};

class Solver {
private:
    int idaLimit; //最大搜索深度限制
    SeqStack<MoveDirection> idaPath;

    int idaSearch(Board& board, int g, int bound, MoveDirection previous);

public:
    Solver();
    SolveResult solve(const Board& start);
    SolveResult solveByBfs(const Board& start);
    SolveResult solveByIdaStar(const Board& start);
};
