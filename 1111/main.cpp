#include <iostream>
#include <vector>
#include <windows.h>
#include <chrono>
#include "Board.h"
#include "Solver.h"

using namespace std;

// 辅助：获取人类输入
MoveDirection getHumanMove() {
    char ch;
    cout << "请输入移动 (w:上, s:下, a:左, d:右, q:退出): ";
    cin >> ch;
    switch (ch) {
    case 'w': return MOVE_UP;
    case 's': return MOVE_DOWN;
    case 'a': return MOVE_LEFT;
    case 'd': return MOVE_RIGHT;
    case 'q': return MOVE_NONE;
    default: return MOVE_NONE;
    }
}

int main() {
    SetConsoleOutputCP(65001);
    // 设置编码，防止控制台乱码
    system("chcp 65001 > nul"); 
    int n;
    cout << "=== 数字华容道系统 ===\n";
    cout << "请输入棋盘大小 (3 或 4): ";
    cin >> n;

    Board b = Board::randomBoard(n);
    Solver solver;

    while (true) {
        cout << "\n当前状态:\n" << b.toString() << endl;
        cout << "1. 人类操作\n";
        cout << "2. AI 自动求解\n";
        cout << "3. 退出\n";
        cout << "请选择: ";

        int choice;
        cin >> choice;

        if (choice == 1) {
            MoveDirection dir = getHumanMove();
            if (dir == MOVE_NONE) break;
            if (b.move(dir)) {
                if (b.isGoal()) {
                    cout << "恭喜你！完成还原！\n";
                    break;
                }
            }
            else {
                cout << "【警告】无效移动！\n";
            }
        }
        else if (choice == 2) {
            cout << "AI 正在思考中...\n";
            auto start_time = chrono::high_resolution_clock::now();
            SolveResult res = solver.solve(b);
            auto end_time = chrono::high_resolution_clock::now();
            chrono::duration<double> diff = end_time - start_time;

            if (res.solved) {
                cout << "求解成功！共 " << res.steps << " 步，耗时 " << diff.count() << " 秒。\n";
                cout << "路径: ";
                for (auto m : res.moves) cout << moveToText(m) << " ";
                cout << "\n是否演示？(y/n): ";
                char confirm; cin >> confirm;
                // 在 main.cpp 中，演示部分修改为：
                if (confirm == 'y') {
                    auto stack = Solver::buildMoveStack(res.moves);
                    while (!stack.empty()) {
                        MoveDirection dir = stack.pop();
                        b.move(dir);
                        system("cls"); // 清屏，让画面看起来是在“动”
                        cout << "演示中: " << moveToText(dir) << "\n" << b.toString() << endl;
                        // 加入延时，让动画慢一点
                        Sleep(500); // 500毫秒延迟
                    }
                    cout << "演示完毕！\n";
                }
            }
            else {
                cout << "失败: " << res.message << endl;
            }
        }
        else {
            break;
        }
    }

    return 0;
}