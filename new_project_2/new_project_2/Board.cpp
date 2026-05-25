#include "Board.h"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <sstream>

MoveDirection oppositeMove(MoveDirection dir) {
    switch (dir) {
    case MOVE_UP: return MOVE_DOWN;
    case MOVE_DOWN: return MOVE_UP;
    case MOVE_LEFT: return MOVE_RIGHT;
    case MOVE_RIGHT: return MOVE_LEFT;
    default: return MOVE_NONE;
    }
}

Board::Board(int size) : n(size) {
    *this = Board::goal(size);
}

Board::Board(int size, const SeqList<int>& values) : n(size), cells(values) {}

int Board::size() const { return n; }
int Board::total() const { return n * n; }
const SeqList<int>& Board::values() const { return cells; }

int Board::at(int row, int col) const {
    return cells[row * n + col];
}

int Board::blankIndex() const {
    for (int i = 0; i < total(); ++i) {
        if (cells[i] == 0) return i;
    }
    return -1;
}

int Board::blankRow() const { return blankIndex() / n; }
int Board::blankCol() const { return blankIndex() % n; }

bool Board::canMove(MoveDirection dir) const {
    int r = blankRow();
    int c = blankCol();
    if (dir == MOVE_UP) return r < n - 1;
    if (dir == MOVE_DOWN) return r > 0;
    if (dir == MOVE_LEFT) return c < n - 1;
    if (dir == MOVE_RIGHT) return c > 0;
    return false;
}

bool Board::move(MoveDirection dir) {
    if (!canMove(dir)) return false;

    int blank = blankIndex();
    int target = blank;
    if (dir == MOVE_UP) target = blank + n;
    if (dir == MOVE_DOWN) target = blank - n;
    if (dir == MOVE_LEFT) target = blank + 1;
    if (dir == MOVE_RIGHT) target = blank - 1;

    // 交换
    int temp = cells[blank];
    cells[blank] = cells[target];
    cells[target] = temp;
    return true;
}

Board Board::moved(MoveDirection dir) const {
    Board next = *this;
    next.move(dir);
    return next;
}

SeqList<MoveDirection> Board::legalMoves() const {
    SeqList<MoveDirection> moves(4);
    MoveDirection dirs[4] = { MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT };
    for (int i = 0; i < 4; ++i) {
        if (canMove(dirs[i])) moves.push_back(dirs[i]);
    }
    return moves;
}

bool Board::isGoal() const {
    for (int i = 0; i < total() - 1; ++i) {
        if (cells[i] != i + 1) return false;
    }
    return cells[total() - 1] == 0;
}

bool Board::isSolvable() const {
    int inversions = 0;
    for (int i = 0; i < total(); ++i) {
        if (cells[i] == 0) continue;
        for (int j = i + 1; j < total(); ++j) {
            if (cells[j] != 0 && cells[i] > cells[j]) ++inversions;
        }
    }
    if (n % 2 == 1) return inversions % 2 == 0;
    int blankRowFromBottom = n - blankRow();
    return (inversions + blankRowFromBottom) % 2 == 1;
}

int Board::manhattan() const {
    int sum = 0;
    for (int i = 0; i < total(); ++i) {
        int value = cells[i];
        if (value == 0) continue;
        int currentRow = i / n, currentCol = i % n;
        int goalRow = (value - 1) / n, goalCol = (value - 1) % n;
        sum += abs(currentRow - goalRow) + abs(currentCol - goalCol);
    }
    return sum;
}

std::string Board::encode() const {
    std::string code = "";
    for (int i = 0; i < total(); ++i) {
        code += (char)(cells[i] + '0'); // 转成字符编码，方便哈希表存取
    }
    return code;
}

Board Board::goal(int size) {
    SeqList<int> values(size * size);
    for (int i = 0; i < size * size - 1; ++i) values.push_back(i + 1);
    values.push_back(0);
    return Board(size, values);
}

// 【核心修改】真正的随机打乱，空格固定在右下角，符合你的要求 5
Board Board::randomBoard(int size) {
    SeqList<int> values(size * size);
    for (int i = 1; i < size * size; ++i) values.push_back(i); // 1 到 N*N-1

    // 大一友好型洗牌算法
    for (int i = values.size() - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        int temp = values[i];
        values[i] = values[j];
        values[j] = temp;
    }
    values.push_back(0); // 空格放最后

    return Board(size, values);
}