#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <chrono>

// 引入全组的结晶
#include "Board.h"
#include "Solver.h"
#include "DataStructures.h"

using namespace std;

// EasyX UI 界面动态参数
const int START_X = 60;
const int START_Y = 100; 
int TILE_SIZE = 80;      

// 右侧控制按钮区域坐标
const int BTN_X = 420;
const int BTN_WIDTH = 140;
const int BTN_HEIGHT = 40;

// 游戏全局大脑对象
int currentN = 4;        // 默认是 4x4
Board b = Board::randomBoard(currentN);
Solver solver;
SeqStack<MoveDirection> undoStack;
SeqStack<MoveDirection> redoStack;

// 函数声明
void drawUI();
void handleMouseClick(int mx, int my);
void tryHumanMove(int row, int col);
void runAISolve();
void changeMode(int n);

int main() {
    // 初始化 600x450 图形窗口
    initgraph(600, 450);
    setbkcolor(RGB(89, 60, 41));
    cleardevice();

    // 开启批量绘图防闪烁
    BeginBatchDraw();

    while (true) {
        drawUI(); // 实时绘制木质界面
        FlushBatchDraw();

        // 检查胜利状态
        if (b.isGoal()) {
            EndBatchDraw(); // 弹窗前先刷新最后一帧
            MessageBox(GetHWnd(), _T("Congratulations! You Win!"), _T("Victory"), MB_OK);
            b = Board::randomBoard(currentN);
            undoStack.clear();
            redoStack.clear();
            BeginBatchDraw();
        }

        // 鼠标交互捕获
        if (MouseHit()) {
            MOUSEMSG msg = GetMouseMsg();
            if (msg.uMsg == WM_LBUTTONDOWN) {
                handleMouseClick(msg.x, msg.y);
            }
        }

        // 额外的键盘快捷键捕获：按键盘 3 切换3x3，按 4 切换4x4
        if (_kbhit()) {
            char key = _getch();
            if (key == '3') changeMode(3);
            if (key == '4') changeMode(4);
        }

        Sleep(15);
    }

    EndBatchDraw();
    closegraph();
    return 0;
}

// 【切换棋盘规格核心逻辑】
void changeMode(int n) {
    currentN = n;
    // 自适应调整格子大小，让3x3和4x4在界面上看起来都一样规整舒服
    if (currentN == 3) {
        TILE_SIZE = 106; // 3x3 格子稍微大一点
    }
    else {
        TILE_SIZE = 80;  // 4x4 维持原样
    }
    b = Board::randomBoard(currentN); // 重新生成对应规格的随机可解棋盘
    undoStack.clear();                // 清空旧的悔步记录
    redoStack.clear();
}

// 【鼠标点击总控台】
void handleMouseClick(int mx, int my) {
    // 0. 点到了上方的规格切换按钮
    if (my >= 20 && my <= 50) {
        if (mx >= 60 && mx <= 140) changeMode(3);  // 点击 3x3 标签
        if (mx >= 160 && mx <= 240) changeMode(4); // 点击 4x4 标签
    }

    // 1. 点到了左边棋盘区域：尝试人类移动
    if ((mx >= START_X) && (mx < (START_X + currentN * TILE_SIZE))) {
        if ((my >= START_Y) && (my < (START_Y + currentN * TILE_SIZE))) {
            int col = (mx - START_X) / TILE_SIZE;
            int row = (my - START_Y) / TILE_SIZE;
            tryHumanMove(row, col);
        }
    }

    // 2. 点到了右侧 【Shuffle】 按钮
    if ((mx >= BTN_X) && (mx <= (BTN_X + BTN_WIDTH)) && (my >= 80) && (my <= (80 + BTN_HEIGHT))) {
        b = Board::randomBoard(currentN);
        undoStack.clear();
        redoStack.clear();
    }

    // 3. 点到了右侧 【Undo】 按钮
    if ((mx >= BTN_X) && (mx <= (BTN_X + BTN_WIDTH)) && (my >= 140) && (my <= (140 + BTN_HEIGHT))) {
        if (!undoStack.empty()) {
            MoveDirection lastMove = undoStack.pop();
            MoveDirection opp = oppositeMove(lastMove);
            if (b.canMove(opp)) {
                b.move(opp);
                redoStack.push(lastMove);
            }
        }
    }

    // 4. 点到了右侧 【Redo】 按钮
    if ((mx >= BTN_X) && (mx <= (BTN_X + BTN_WIDTH)) && (my >= 200) && (my <= (200 + BTN_HEIGHT))) {
        if (!redoStack.empty()) {
            MoveDirection nextMove = redoStack.pop();
            if (b.canMove(nextMove)) {
                b.move(nextMove);
                undoStack.push(nextMove);
            }
        }
    }

    // 5. 点到了右侧 【AI Solve】 按钮
    if ((mx >= BTN_X) && (mx <= (BTN_X + BTN_WIDTH)) && (my >= 260) && (my <= (260 + BTN_HEIGHT))) {
        runAISolve();
    }
}

// 【人类玩家点击格子移动】
void tryHumanMove(int row, int col) {
    int zeroRow = b.blankRow();
    int zeroCol = b.blankCol();

    MoveDirection dir = MOVE_NONE;
    if (row == zeroRow - 1 && col == zeroCol) dir = MOVE_DOWN;
    if (row == zeroRow + 1 && col == zeroCol) dir = MOVE_UP;
    if (row == zeroRow && col == zeroCol - 1) dir = MOVE_RIGHT;
    if (row == zeroRow && col == zeroCol + 1) dir = MOVE_LEFT;

    if (dir != MOVE_NONE) {
        if (b.canMove(dir)) {
            b.move(dir);
            undoStack.push(dir);
            redoStack.clear();
        }
    }
}

// 【AI 自动求解及动画演示】
void runAISolve() {
    EndBatchDraw();
    MessageBox(GetHWnd(), _T("AI is thinking... Click OK to start."), _T("AI Solve"), MB_OK);
    BeginBatchDraw();

    // 💡 自动匹配：队友的 solver.solve 内部会自动判断 3x3 用 BFS，4x4 用 IDA*
    SolveResult res = solver.solve(b);

    if (res.solved) {
        SeqStack<MoveDirection> demoStack = Solver::buildMoveStack(res.moves);
        while (!demoStack.empty()) {
            MoveDirection dir = demoStack.pop();
            if (b.canMove(dir)) {
                b.move(dir);

                drawUI();
                FlushBatchDraw();

                Sleep(300); // 3x3 步数少，可以稍微快一点走
            }
        }
        EndBatchDraw();
        MessageBox(GetHWnd(), _T("AI Solve Completed!"), _T("Done"), MB_OK);
        BeginBatchDraw();
    }
    else {
        EndBatchDraw();
        MessageBox(GetHWnd(), _T("Failed to solve!"), _T("Error"), MB_OK);
        BeginBatchDraw();
    }
}

// 【高颜值纯代码木质风 UI 绘制】
void drawUI() {
    cleardevice();

    // 1. 顶部规格选择标签绘制
    settextstyle(14, 0, _T("Arial"));
    setbkmode(TRANSPARENT);

    // 3x3 标签
    if (currentN == 3) setfillcolor(RGB(180, 135, 90)); else setfillcolor(RGB(65, 43, 28));
    setlinecolor(RGB(115, 83, 58));
    fillroundrect(60, 20, 140, 50, 4, 4);
    if (currentN == 3) settextcolor(RGB(50, 30, 10)); else settextcolor(RGB(180, 140, 110));
    outtextxy(82, 28, _T("3 x 3"));

    // 4x4 标签
    if (currentN == 4) setfillcolor(RGB(180, 135, 90)); else setfillcolor(RGB(65, 43, 28));
    fillroundrect(160, 20, 240, 50, 4, 4);
    if (currentN == 4) settextcolor(RGB(50, 30, 10)); else settextcolor(RGB(180, 140, 110));
    outtextxy(182, 28, _T("4 x 4"));

    // 2. 棋盘底座框
    setfillcolor(RGB(65, 43, 28));
    setlinecolor(RGB(115, 83, 58));
    fillroundrect(START_X - 10, START_Y - 10, START_X + currentN * TILE_SIZE + 10, START_Y + currentN * TILE_SIZE + 10, 10, 10);

    // 3. 循环画数字滑块
    for (int i = 0; i < currentN; i++) {
        for (int j = 0; j < currentN; j++) {
            int x1 = START_X + j * TILE_SIZE;
            int y1 = START_Y + i * TILE_SIZE;
            int x2 = x1 + TILE_SIZE;
            int y2 = y1 + TILE_SIZE;

            int val = b.at(i, j);

            if (val != 0) {
                // 木质方块底
                setfillcolor(RGB(226, 192, 141));
                setlinecolor(RGB(139, 105, 75));
                fillroundrect(x1 + 3, y1 + 3, x2 - 3, y2 - 3, 8, 8);

                // 巧克力色数字
                settextstyle(32, 0, _T("Arial"));
                settextcolor(RGB(90, 55, 30));

                TCHAR str[5];
                _stprintf_s(str, _T("%d"), val);
                // 3x3 字体居中微调
                int offsetX = (TILE_SIZE / 2) - 10;
                if (val >= 10) offsetX -= 8;
                outtextxy(x1 + offsetX, y1 + (TILE_SIZE / 2) - 16, str);
            }
            else {
                // 空格
                setfillcolor(RGB(45, 30, 20));
                setlinecolor(RGB(55, 40, 30));
                fillroundrect(x1 + 3, y1 + 3, x2 - 3, y2 - 3, 8, 8);
            }
        }
    }

    // 4. 绘制右侧控制按钮群
    settextstyle(16, 0, _T("Arial"));
    setfillcolor(RGB(180, 135, 90));
    setlinecolor(RGB(110, 80, 50));
    settextcolor(RGB(50, 30, 10));

    // Shuffle 打乱按钮
    fillroundrect(BTN_X, 80, BTN_X + BTN_WIDTH, 80 + BTN_HEIGHT, 6, 6);
    outtextxy(BTN_X + 44, 91, _T("Shuffle"));

    // Undo 按钮
    fillroundrect(BTN_X, 140, BTN_X + BTN_WIDTH, 140 + BTN_HEIGHT, 6, 6);
    outtextxy(BTN_X + 50, 151, _T("Undo"));

    // Redo 按钮
    fillroundrect(BTN_X, 200, BTN_X + BTN_WIDTH, 200 + BTN_HEIGHT, 6, 6);
    outtextxy(BTN_X + 51, 211, _T("Redo"));

    // AI Solve 按钮
    fillroundrect(BTN_X, 260, BTN_X + BTN_WIDTH, 260 + BTN_HEIGHT, 6, 6);
    outtextxy(BTN_X + 41, 271, _T("AI Solve"));
}
