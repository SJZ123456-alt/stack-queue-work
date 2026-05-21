#pragma once

#include <string>
#include <vector>

// MoveDirection 表示数字方块移动的方向。
// 例如：MOVE_UP 表示空格下方的方块向上移动到空格中。
enum MoveDirection {
    MOVE_UP = 0,       // 方块向上移动。
    MOVE_DOWN = 1,     // 方块向下移动。
    MOVE_LEFT = 2,     // 方块向左移动。
    MOVE_RIGHT = 3,    // 方块向右移动。
    MOVE_NONE = 4      // 无移动，用作标记。
};

// 将移动方向转换成可读文字，用于保存完整解法。
std::string moveToText(MoveDirection dir);

// 返回某个移动方向的反方向，用于 IDA* 回溯撤销一步。
MoveDirection oppositeMove(MoveDirection dir);

// Board 类表示一个 N x N 数字华容道棋盘状态。
// 棋盘中的空格用数字 0 表示。
class Board {
private:
    int n;                    // 棋盘边长，例如 3 或 4。
    std::vector<int> cells;   // 一维数组，按从左到右、从上到下存储所有格子。

public:
    // 创建指定规模的目标棋盘。
    Board(int size = 3);

    // 根据给定数组创建棋盘，values 必须包含 size * size 个数字。
    Board(int size, const std::vector<int>& values);

    // 返回棋盘边长 N。
    int size() const;

    // 返回格子总数，即 N * N。
    int total() const;

    // 返回棋盘数组的只读引用。
    const std::vector<int>& values() const;

    // 返回第 row 行第 col 列的数字，行列下标都从 0 开始。
    int at(int row, int col) const;

    // 返回空格 0 在一维数组中的下标。
    int blankIndex() const;

    // 返回空格所在行。
    int blankRow() const;

    // 返回空格所在列。
    int blankCol() const;

    // 判断是否可以按给定方向移动方块。
    bool canMove(MoveDirection dir) const;

    // 执行一次移动，成功返回 true，非法移动返回 false。
    bool move(MoveDirection dir);

    // 返回移动后的新棋盘，不改变当前棋盘。
    Board moved(MoveDirection dir) const;

    // 返回当前棋盘所有合法移动方向。
    std::vector<MoveDirection> legalMoves() const;

    // 判断棋盘是否已经达到目标状态：1,2,3,...,0。
    bool isGoal() const;

    // 使用逆序数判断当前状态是否有解。
    bool isSolvable() const;

    // 返回曼哈顿距离，用作 IDA* 的启发函数。
    int manhattan() const;

    // 将棋盘编码成字符串，便于 BFS 中用哈希表判重。
    std::string encode() const;

    // 将棋盘转换成可打印的多行字符串。
    std::string toString() const;

    // 创建目标状态棋盘。
    static Board goal(int size);

    // 创建随机可解棋盘，并保证最后空格位于右下角。
    static Board randomBoard(int size);

    // 根据 BFS 编码字符串还原棋盘。
    static Board fromCode(int size, const std::string& code);
};


