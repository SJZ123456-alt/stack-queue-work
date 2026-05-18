#pragma once
#include <string>
#include "game_engine.h"
#include <iostream>
#include <iomanip>
using namespace std;

const int MAX_N = 4; 

// 棋盘状态结构体
struct State {
    int board[MAX_N][MAX_N]; // 棋盘矩阵，0 表示空格
    int N;                   // 当前规格，比如 3 或 4
    int blank_x;             // 空格所在的行坐标 (0 到 N-1)
    int blank_y;             // 空格所在的列坐标 (0 到 N-1)
    int step;                // 当前移动的步数
    std::string path;        // 记录移动路径（给电脑求解用的，同学A暂时用不到但要留着）
};


// 1. 初始化为胜利（目标）状态
void InitBoard(State& s, int n);

// 2. 判断当前是否胜利
bool IsWin(State s);

// 3. 移动棋子 (支持输入 W/A/S/D 或 U/D/L/R)
// 移动成功返回 true，如果是撞墙（越界）返回 false
bool Move(State& s, char direction);

// 4. 打印当前棋盘（控制台文字版）
void PrintBoard(State s);

// 5. 人类玩家主循环
void HumanPlay(State s);

// 初始化棋盘为目标状态 (1, 2, 3 ... 0)
void InitBoard(State& s, int n) {
    s.N = n;
    s.step = 0;
    s.path = "";

    int count = 1;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            s.board[i][j] = count++;
        }
    }
    // 最后一个位置设为空格（用 0 表示）
    s.board[n - 1][n - 1] = 0;
    s.blank_x = n - 1;
    s.blank_y = n - 1;
}

// 判断是否胜利
bool IsWin(State s) {
    int count = 1;
    int target = s.N * s.N;

    for (int i = 0; i < s.N; i++) {
        for (int j = 0; j < s.N; j++) {
            // 最后一个格子应该是 0
            if (i == s.N - 1 && j == s.N - 1) {
                if (s.board[i][j] != 0) return false;
            }
            else {
                if (s.board[i][j] != count) return false;
            }
            count++;
        }
    }
    return true;
}

// 移动棋子（难点：方向的映射）
// 我们假设人类玩家按 W(上), 意思是想把空格下方的数字推上去
// 这等价于：数字向上移动 = 空格向下移动
bool Move(State& s, char direction) {
    int new_x = s.blank_x;
    int new_y = s.blank_y;

    // 处理大小写
    if (direction >= 'A' && direction <= 'Z') {
        direction += 32;
    }

    // 根据按键计算空格的新位置
    if (direction == 'w') {
        new_x++; // 玩家想推数字上去，空格就要往下走
    }
    else if (direction == 's') {
        new_x--; // 推数字下去，空格往上走
    }
    else if (direction == 'a') {
        new_y++; // 推数字向左，空格往右走
    }
    else if (direction == 'd') {
        new_y--; // 推数字向右，空格往左走
    }
    else {
        return false; // 无效按键
    }

    // 判断是否越界
    if (new_x < 0 || new_x >= s.N || new_y < 0 || new_y >= s.N) {
        return false;
    }

    // 交换空格和目标数字
    int temp = s.board[s.blank_x][s.blank_y];
    s.board[s.blank_x][s.blank_y] = s.board[new_x][new_y];
    s.board[new_x][new_y] = temp;

    // 更新空格坐标和步数
    s.blank_x = new_x;
    s.blank_y = new_y;
    s.step++;

    return true;
}

// 打印棋盘
void PrintBoard(State s) {
    cout << "\n======================\n";
    for (int i = 0; i < s.N; i++) {
        for (int j = 0; j < s.N; j++) {
            if (s.board[i][j] == 0) {
                cout << setw(4) << " "; // 空格用空白表示，更加直观
            }
            else {
                cout << setw(4) << s.board[i][j];
            }
        }
        cout << "\n\n";
    }
    cout << "当前步数: " << s.step << "\n";
    cout << "======================\n";
}

// 人类玩家控制循环
void HumanPlay(State s) {
    char dir;
    cout << "\n游戏开始！使用 W/A/S/D 移动棋子，输入 'q' 退出。\n";

    while (!IsWin(s)) {
        PrintBoard(s);
        cout << "请输入移动方向 (w/a/s/d): ";
        cin >> dir;

        if (dir == 'q' || dir == 'Q') {
            cout << "游戏已退出。\n";
            return;
        }

        if (!Move(s, dir)) {
            cout << "【提示】无效移动或撞墙了，请重新输入！\n";
        }
    }

    // 胜利提示
    PrintBoard(s);
    cout << "🎉 恭喜你！成功还原了华容道！\n";
    cout << "总共使用步数: " << s.step << "\n";
}