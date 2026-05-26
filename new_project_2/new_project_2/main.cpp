#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <time.h>
#include "Board.h"
#include "Solver.h"
#include "Structure.h"

// 游戏状态枚举
enum GameState { PLAYING, GAME_OVER, DEMOING };

const int START_X = 50;
const int START_Y = 80;
int TILE_SIZE = 90;
const int BTN_X = 430;
const int BTN_WIDTH = 140;
const int BTN_HEIGHT = 45;

int currentN = 3;
Board currentBoard;
Board initialBoard; // 记录开局状态
Solver solver;
GameState state = PLAYING;
int humanSteps = 0;

void initNewGame(int n);
void drawUI();
void handleMouseClick(int mx, int my);
void tryHumanMove(int row, int col);
void showSolution(bool isFast);
void checkSolvable();
void processVictory();

int main()
{
    srand((unsigned)time(NULL));
    initgraph(620, 480);
    setbkcolor(RGB(240, 235, 225));
    cleardevice();

    initNewGame(currentN);
    BeginBatchDraw();

    while (true)
    {
        drawUI();
        FlushBatchDraw();

        //PLAYING 状态才去检测胜利
        if (state == PLAYING && currentBoard.isGoal())
        {
            processVictory();
        }

        while (MouseHit())
        {
            MOUSEMSG msg = GetMouseMsg();
            if (msg.uMsg == WM_LBUTTONDOWN)
            {
                handleMouseClick(msg.x, msg.y);
            }
        }
        Sleep(15);
    }
    EndBatchDraw();
    closegraph();
    return 0;
}

void initNewGame(int n)
{
    currentN = n;
    TILE_SIZE = (currentN == 3) ? 110 : 85;
    currentBoard = Board::randomBoard(currentN);
    initialBoard = currentBoard;
    humanSteps = 0;
    state = PLAYING;
}

void processVictory()
{
    state = GAME_OVER;
    drawUI();
    FlushBatchDraw();

    settextcolor(RGB(100, 100, 100));
    outtextxy(BTN_X, 390, _T("正在进行最终结算..."));
    FlushBatchDraw();
    SolveResult res = solver.solve(initialBoard);

    TCHAR msg[256];
    if (res.solved)
    {
        // 在 2 秒内算出来了
        if (humanSteps == res.steps)
        {
            // 人类步数 == 最少步数
            _stprintf_s(msg, _T("太强了！你只用了 %d 步就完成了！\n这就是这局理论上的最少步数！ヾ(*´∀ ˋ*)ﾉ"), humanSteps);
        }
        else
        {
            //人类步数 > 最少步数
            _stprintf_s(msg, _T("恭喜通关！你用了 %d 步。\n提示：算出这局的理论最少步数是 %d 步。\n要不要点重新开始挑战一下最短路线？(ง๑•_•)ง"), humanSteps, res.steps);
        }
    }
    else
    {
        //超时没算出来
        _stprintf_s(msg, _T("恭喜通关！你用了 %d 步！\n这局开局极度复杂，连计算机在限时内都算不出极限最少步数。\n你能靠自己解开简直太厉害了！(๑•̀ㅂ•́)و✧"), humanSteps);
    }

    drawUI();
    FlushBatchDraw();
    MessageBox(GetHWnd(), msg, _T("胜利！"), MB_OK);
}

void handleMouseClick(int mx, int my) {
    // 规格切换
    if (my >= 20 && my <= 55) {
        if (mx >= 50 && mx <= 130) initNewGame(3);
        if (mx >= 150 && mx <= 230) initNewGame(4);
    }

    // 棋盘点击
    if (state == PLAYING) {
        if (mx >= START_X && mx < START_X + currentN * TILE_SIZE &&
            my >= START_Y && my < START_Y + currentN * TILE_SIZE) {
            int col = (mx - START_X) / TILE_SIZE;
            int row = (my - START_Y) / TILE_SIZE;
            tryHumanMove(row, col);
        }
    }

    // 右侧按钮交互
    if (mx >= BTN_X && mx <= BTN_X + BTN_WIDTH) 
    {

        //新的一局
        if (my >= 80 && my <= 80 + BTN_HEIGHT) 
        {
            initNewGame(currentN);
        }

        //重新开始本局
        if (my >= 140 && my <= 140 + BTN_HEIGHT) 
        {
            currentBoard = initialBoard;
            humanSteps = 0;
            state = PLAYING;
        }

        if (state == PLAYING) {
            //检查是否有解
            if (my >= 200 && my <= 200 + BTN_HEIGHT) 
            {
                checkSolvable();
            }
            // 查看最少步数解
            if (my >= 260 && my <= 260 + BTN_HEIGHT) showSolution(true); // 传入 true

            // 极限最优解
            if (my >= 320 && my <= 320 + BTN_HEIGHT) showSolution(false); // 传入 false
        }
    }
}

void tryHumanMove(int row, int col) {
    int zeroRow = currentBoard.blankRow();
    int zeroCol = currentBoard.blankCol();

    // 曼哈顿距离若为1，相邻
    if (abs(row - zeroRow) + abs(col - zeroCol) == 1) {
        MoveDirection dir = MOVE_NONE;
        if (row == zeroRow - 1) dir = MOVE_DOWN;
        if (row == zeroRow + 1) dir = MOVE_UP;
        if (col == zeroCol - 1) dir = MOVE_RIGHT;
        if (col == zeroCol + 1) dir = MOVE_LEFT;

        if (currentBoard.move(dir)) {
            humanSteps++;
        }
    }
}

void checkSolvable() {
    if (currentBoard.isSolvable()) {
        MessageBox(GetHWnd(), _T("放心玩，这个棋盘有解(`・ω・´)"), _T("检测结果"), MB_OK);
    }
    else {
        MessageBox(GetHWnd(), _T("此局无解，点击【新的一局】重新开始叭ㅍ_ㅍ"), _T("检测结果"), MB_OK);
    }
}

void showSolution(bool isFast) {
    if (!currentBoard.isSolvable()) {
        MessageBox(GetHWnd(), _T("此局无解，点击【新的一局】重新开始叭ㅍ_ㅍ"), _T("提示"), MB_OK);
        return;
    }

    outtextxy(BTN_X, 390, _T("正在计算最少步数..."));
    FlushBatchDraw();

    SolveResult res = isFast ? solver.solveFast(currentBoard) : solver.solveOptimal(currentBoard);

    if (res.solved) {
        state = PLAYING;
        drawUI();
        FlushBatchDraw();

        TCHAR msg[128];
        _stprintf_s(msg, _T("共需要 %d 步\n点击确认开始演示"), res.steps);
        MessageBox(GetHWnd(), msg, isFast ? _T("快速求解完成") : _T("最优解计算完成"), MB_OK);

        state = DEMOING;
        for (int i = 0; i < res.moves.size(); ++i) {
            currentBoard.move(res.moves[i]);
            drawUI();
            FlushBatchDraw();
            Sleep(250);
        }
        state = GAME_OVER;
        MessageBox(GetHWnd(), _T("演示结束"), _T("提示"), MB_OK);
    }
    else 
    {
        state = PLAYING;
        drawUI();
        FlushBatchDraw();
        MessageBox(GetHWnd(), _T("再算电脑就卡死了\n要不换个相对简单的再试吧"), _T("计算超时"), MB_OK | MB_ICONINFORMATION);
    }
}

void drawUI() {
    cleardevice();
    setbkmode(TRANSPARENT);

    // 1. 顶部标签
    settextstyle(20, 0, _T("微软雅黑"));
    setfillcolor(currentN == 3 ? RGB(100, 150, 200) : RGB(200, 200, 200));
    fillroundrect(50, 20, 130, 55, 10, 10);
    settextcolor(RGB(20, 20, 20));
    outtextxy(65, 28, _T("3 x 3"));

    setfillcolor(currentN == 4 ? RGB(100, 150, 200) : RGB(200, 200, 200));
    fillroundrect(150, 20, 230, 55, 10, 10);
    outtextxy(165, 28, _T("4 x 4"));

/*    TCHAR stepStr[64];//步数
    _stprintf_s(stepStr, _T("当前步数: %d"), humanSteps);
    settextcolor(RGB(50, 50, 50));
    outtextxy(260, 28, stepStr);*/

    // 2. 绘制棋盘
    setlinecolor(RGB(150, 150, 150));
    setfillcolor(RGB(180, 180, 180));
    fillroundrect(START_X - 5, START_Y - 5, START_X + currentN * TILE_SIZE + 5, START_Y + currentN * TILE_SIZE + 5, 8, 8);

    for (int i = 0; i < currentN; i++) {
        for (int j = 0; j < currentN; j++) {
            int x = START_X + j * TILE_SIZE;
            int y = START_Y + i * TILE_SIZE;
            int val = currentBoard.at(i, j);

            if (val != 0) {
                int correctVal = i * currentN + j + 1;
                if (val == correctVal)
                {
                    setfillcolor(RGB(120, 180, 120));
                }else
                {
                    setfillcolor(RGB(220, 220, 220));
                }
                    
                fillroundrect(x + 2, y + 2, x + TILE_SIZE - 2, y + TILE_SIZE - 2, 8, 8);

                settextstyle(36, 0, _T("Arial"));
                settextcolor(RGB(30, 30, 30));
                TCHAR str[5];
                _stprintf_s(str, _T("%d"), val);
                int offsetX = (TILE_SIZE - textwidth(str)) / 2;
                int offsetY = (TILE_SIZE - textheight(str)) / 2;
                outtextxy(x + offsetX, y + offsetY, str);
            }
        }
    }

    // 3. 绘制右侧按钮
    settextstyle(18, 0, _T("微软雅黑"));
    setlinecolor(BLACK);

    //新的一局
    setfillcolor(RGB(100, 180, 255));
    fillroundrect(BTN_X, 80, BTN_X + BTN_WIDTH, 80 + BTN_HEIGHT, 5, 5);
    outtextxy(BTN_X + 35, 93, _T("新的一局"));

    //重新开始本局
    setfillcolor(RGB(150, 200, 255));
    fillroundrect(BTN_X, 140, BTN_X + BTN_WIDTH, 140 + BTN_HEIGHT, 5, 5);
    outtextxy(BTN_X + 20, 153, _T("重新开始本局"));

    // 检查是否有解
    if (state == PLAYING)
    {
        setfillcolor(RGB(200, 200, 200));
    }
    else
    {
        setfillcolor(RGB(100, 100, 100));
    }
    fillroundrect(BTN_X, 200, BTN_X + BTN_WIDTH, 200 + BTN_HEIGHT, 5, 5);
    outtextxy(BTN_X + 20, 213, _T("检查是否有解"));

    // 快速求解
    if (state == PLAYING)
    {
        setfillcolor(RGB(200, 200, 200));
    }
    else
    {
        setfillcolor(RGB(100, 100, 100));
    }
    fillroundrect(BTN_X, 260, BTN_X + BTN_WIDTH, 260 + BTN_HEIGHT, 5, 5);
    outtextxy(BTN_X + 25, 273, _T("快速求解"));

    // 最优解
    if (state == PLAYING)
    {
        setfillcolor(RGB(200, 200, 200));
    }
    else
    {
        setfillcolor(RGB(100, 100, 100));
    }
    fillroundrect(BTN_X, 320, BTN_X + BTN_WIDTH, 320 + BTN_HEIGHT, 5, 5);
    outtextxy(BTN_X + 20, 333, _T("求最优解"));

    // 状态提示文字
    if (state == GAME_OVER) 
    {
        settextcolor(RGB(200, 50, 50));
        outtextxy(BTN_X, 390, _T("游戏结束"));
    }
}