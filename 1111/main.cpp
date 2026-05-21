#include "game_engine.h"
#include <cstdlib>
int main() {
    system("chcp 65001 > nul");
    int choice;
    cout << "数字华容道游戏系统\n";
    cout << "1. 人类游玩 (3x3)\n";
    cout << "2. 演示计算机求解动画 (需提供测试路径)\n";
    cout << "3. 跑性能测试报告 (N=3跑100次)\n";
    cout << "请选择: ";
    cin >> choice;

    if (choice == 1) {
        State game;
        GenerateRandomState(game, 3); // 随机生成，保证有解
        HumanPlay(game);
    }
    else if (choice == 2) {
        State game;
        InitBoard(game, 3);
        // 假设先打乱一下（往上和往左移了几步）
        Move(game, 'w'); Move(game, 'a'); Move(game, 'a');

        // 假装这是A同学算出的最优解回退路径
        string computer_path = "dds";
        PlayAutoPath(game, computer_path); // B同学的动画调度器
    }
    else if (choice == 3) {
        RunBatchTests(3, 100); // 跑数据给D同学用
    }

    return 0;
}