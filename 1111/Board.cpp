#include "Board.h"

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <random>
#include <sstream>

std::string moveToText(MoveDirection dir) {
    // 用于把完整移动方案写入文本文件。
    switch (dir) {
    case MOVE_UP: return "上";
    case MOVE_DOWN: return "下";
    case MOVE_LEFT: return "左";
    case MOVE_RIGHT: return "右";
    default: return "无";
    }
}

MoveDirection oppositeMove(MoveDirection dir) {
    // IDA* 回溯时需要撤销临时移动，因此要取得反方向。
    switch (dir) {
    case MOVE_UP: return MOVE_DOWN;
    case MOVE_DOWN: return MOVE_UP;
    case MOVE_LEFT: return MOVE_RIGHT;
    case MOVE_RIGHT: return MOVE_LEFT;
    default: return MOVE_NONE;
    }
}

Board::Board(int size) : n(size), cells(size * size, 0) {
    // 默认构造一个目标状态棋盘，保证棋盘合法。
    *this = Board::goal(size);
}

Board::Board(int size, const std::vector<int>& values) : n(size), cells(values) {
}

int Board::size() const {
    return n;
}

int Board::total() const {
    return n * n;
}

const std::vector<int>& Board::values() const {
    return cells;
}

int Board::at(int row, int col) const {
    return cells[row * n + col];
}

int Board::blankIndex() const {
    // 空格用数字 0 表示。
    for (int i = 0; i < total(); ++i) {
        if (cells[i] == 0) return i;
    }
    return -1;
}

int Board::blankRow() const {
    return blankIndex() / n;
}

int Board::blankCol() const {
    return blankIndex() % n;
}

bool Board::canMove(MoveDirection dir) const {
    int r = blankRow();    // 当前空格所在行。
    int c = blankCol();    // 当前空格所在列。
    if (dir == MOVE_UP) return r < n - 1;      // 空格下方的方块可以上移。
    if (dir == MOVE_DOWN) return r > 0;        // 空格上方的方块可以下移。
    if (dir == MOVE_LEFT) return c < n - 1;    // 空格右侧的方块可以左移。
    if (dir == MOVE_RIGHT) return c > 0;       // 空格左侧的方块可以右移。
    return false;
}

bool Board::move(MoveDirection dir) {
    if (!canMove(dir)) return false;

    int blank = blankIndex();   // 空格在一维数组中的下标。
    int target = blank;         // 要与空格交换的数字方块下标。
    if (dir == MOVE_UP) target = blank + n;
    if (dir == MOVE_DOWN) target = blank - n;
    if (dir == MOVE_LEFT) target = blank + 1;
    if (dir == MOVE_RIGHT) target = blank - 1;

    std::swap(cells[blank], cells[target]);
    return true;
}

Board Board::moved(MoveDirection dir) const {
    Board next = *this;         // 复制当前棋盘，在副本上移动。
    next.move(dir);
    return next;
}

std::vector<MoveDirection> Board::legalMoves() const {
    std::vector<MoveDirection> moves;   // 保存当前所有可用移动方向。
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
    int inversions = 0;  // 非零数字之间的逆序对数量。
    for (int i = 0; i < total(); ++i) {
        if (cells[i] == 0) continue;
        for (int j = i + 1; j < total(); ++j) {
            if (cells[j] != 0 && cells[i] > cells[j]) ++inversions;
        }
    }

    if (n % 2 == 1) return inversions % 2 == 0;

    // 偶数阶棋盘：逆序数和空格自底向上的行号共同决定是否有解。
    int blankRowFromBottom = n - blankRow();
    return (inversions + blankRowFromBottom) % 2 == 1;
}

int Board::manhattan() const {
    int sum = 0;  // 所有方块到目标位置的曼哈顿距离之和。
    for (int i = 0; i < total(); ++i) {
        int value = cells[i];       // 当前方块数字。
        if (value == 0) continue;
        int currentRow = i / n;           // 当前方块所在行。
        int currentCol = i % n;           // 当前方块所在列。
        int goalRow = (value - 1) / n;    // 该方块目标所在行。
        int goalCol = (value - 1) % n;    // 该方块目标所在列。
        sum += std::abs(currentRow - goalRow) + std::abs(currentCol - goalCol);
    }
    return sum;
}

std::string Board::encode() const {
    std::string code;  // BFS 判重时使用的紧凑字符串键。
    code.reserve(total());
    for (int value : cells) {
        code.push_back(static_cast<char>(value));
    }
    return code;
}

std::string Board::toString() const {
    std::ostringstream oss;  // 用于拼接可打印棋盘字符串。
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            int value = at(r, c);
            if (value == 0) oss << "  *";
            else {
                if (value < 10) oss << "  " << value;
                else oss << " " << value;
            }
        }
        oss << "\n";
    }
    return oss.str();
}

Board Board::goal(int size) {
    std::vector<int> values(size * size);  // 目标布局：1 到 N*N-1，最后是 0。
    for (int i = 0; i < size * size - 1; ++i) values[i] = i + 1;
    values[size * size - 1] = 0;
    return Board(size, values);
}

Board Board::randomBoard(int size) {
    static std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr))); // 随机数引擎。
    Board board = Board::goal(size);                                       // 从目标状态开始打乱。
    int shuffleSteps = (size == 3) ? 36 : 48;                              // 随机行走步数。
    MoveDirection previous = MOVE_NONE;                                    // 上一步移动方向。

    // 从目标状态随机移动得到初始状态，可以天然保证棋盘有解。
    for (int i = 0; i < shuffleSteps; ++i) {
        std::vector<MoveDirection> moves = board.legalMoves();       // 当前所有合法移动。
        std::vector<MoveDirection> candidates;                       // 排除立刻反向移动后的候选方向。
        for (MoveDirection dir : moves) {
            if (oppositeMove(dir) != previous) candidates.push_back(dir);
        }
        if (candidates.empty()) candidates = moves;
        std::uniform_int_distribution<int> dist(0, static_cast<int>(candidates.size()) - 1);
        MoveDirection chosen = candidates[dist(rng)];                // 随机选中的移动方向。
        board.move(chosen);
        previous = chosen;
    }

    while (board.blankRow() < size - 1) board.move(MOVE_UP);
    while (board.blankCol() < size - 1) board.move(MOVE_LEFT);

    if (board.isGoal()) return randomBoard(size);

    return board;
}

Board Board::fromCode(int size, const std::string& code) {
    std::vector<int> values(size * size);  // 解码后得到的棋盘数组。
    for (int i = 0; i < size * size; ++i) {
        values[i] = static_cast<unsigned char>(code[i]);
    }
    return Board(size, values);
}


