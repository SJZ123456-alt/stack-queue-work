#pragma once
#include <string>
#include "Structure.h"

enum MoveDirection {
    MOVE_UP = 0,
    MOVE_DOWN = 1,
    MOVE_LEFT = 2,
    MOVE_RIGHT = 3,
    MOVE_NONE = 4
};

MoveDirection oppositeMove(MoveDirection dir);//获取相反方向，方便IDA*搜索时剪枝

class Board {
private:
    int n;
    SeqList<int> cells;

public:
    Board(int size = 3);
    Board(int size, const SeqList<int>& values);

    int size() const;
    int total() const;
    const SeqList<int>& values() const;

    int at(int row, int col) const;
    int blankIndex() const;
    int blankRow() const;
    int blankCol() const;

    bool canMove(MoveDirection dir) const;
    bool move(MoveDirection dir);
    Board moved(MoveDirection dir) const;

    SeqList<MoveDirection> legalMoves() const; // 返回值改为 SeqList

    bool isGoal() const;
    bool isSolvable() const; // 逆序数判断是否有解
    int manhattan() const;
    std::string encode() const;

    static Board goal(int size);
    // 纯随机生成：打乱前 N*N-1 个数字，空格固定在右下角
    static Board randomBoard(int size);
};
