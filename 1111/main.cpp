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
            // 【修改点】：进入人类专属的游戏交互循环，并声明两个自定义顺序栈
            SeqStack<MoveDirection> undoStack;
            SeqStack<MoveDirection> redoStack;

            while (true) {
                system("cls"); // 刷新屏幕，展示清晰的操作提示
                cout << "=== 人类操作模式 ===\n";
                cout << b.toString() << endl;
                cout << "【操作指南】\n";
                cout << "  w: 上 | s: 下 | a: 左 | d: 右\n";
                cout << "  u: 悔步 (Undo) | r: 撤销悔步 (Redo)\n";
                cout << "  q: 返回主菜单\n\n";
                cout << "请输入操作: ";

                char ch;
                cin >> ch;

                if (ch == 'q') {
                    break; // 退出当前模式，返回主菜单
                }
                else if (ch == 'u') {
                    // 悔步逻辑：弹出历史步骤，做反方向移动，并压入 redo 栈
                    if (!undoStack.empty()) {
                        MoveDirection lastMove = undoStack.pop();
                        b.move(oppositeMove(lastMove));
                        redoStack.push(lastMove);
                    }
                    else {
                        cout << "【提示】已处于初始状态，无法再悔步！\n";
                        Sleep(800);
                    }
                }
                else if (ch == 'r') {
                    // 撤销悔步逻辑：弹出重做步骤，做正方向移动，并重新压回 undo 栈
                    if (!redoStack.empty()) {
                        MoveDirection nextMove = redoStack.pop();
                        b.move(nextMove);
                        undoStack.push(nextMove);
                    }
                    else {
                        cout << "【提示】没有可以撤销悔步的记录！\n";
                        Sleep(800);
                    }
                }
                else {
                    // 普通移动方向转换
                    MoveDirection dir = MOVE_NONE;
                    if (ch == 'w') dir = MOVE_UP;
                    else if (ch == 's') dir = MOVE_DOWN;
                    else if (ch == 'a') dir = MOVE_LEFT;
                    else if (ch == 'd') dir = MOVE_RIGHT;

                    if (dir != MOVE_NONE) {
                        if (b.move(dir)) {
                            undoStack.push(dir); // 正常移动，压入悔步栈
                            redoStack.clear();   // 每次进行新移动，清空重做栈

                            if (b.isGoal()) {
                                system("cls");
                                cout << b.toString() << endl;
                                cout << "恭喜你！完成还原！\n";
                                Sleep(1500);
                                break;
                            }
                        }
                        else {
                            cout << "【警告】无效移动！\n";
                            Sleep(600);
                        }
                    }
                }
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