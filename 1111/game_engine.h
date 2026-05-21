#pragma once
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>

using namespace std;

const int MAX_N = 4;

// 棋盘状态结构体
struct State {
    int board[MAX_N][MAX_N]; // 棋盘矩阵，0 表示空格
    int N;                   // 当前规格，比如 3 或 4
    int blank_x;             // 空格所在的行坐标 (0 到 N-1)
    int blank_y;             // 空格所在的列坐标 (0 到 N-1)
    int step;                // 当前移动的步数
    string path;             // 记录移动路径，如 "WASD"
};

// ========================== 基础逻辑 ==========================

inline void InitBoard(State& s, int n) {
    s.N = n;
    s.step = 0;
    s.path = "";
    int count = 1;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            s.board[i][j] = count++;
        }
    }
    s.board[n - 1][n - 1] = 0;
    s.blank_x = n - 1;
    s.blank_y = n - 1;
}

inline bool IsWin(const State& s) {
    int count = 1;
    for (int i = 0; i < s.N; i++) {
        for (int j = 0; j < s.N; j++) {
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

inline bool Move(State& s, char direction) {
    int new_x = s.blank_x;
    int new_y = s.blank_y;

    if (direction >= 'A' && direction <= 'Z') direction += 32;

    if (direction == 'w') new_x++; // 空格往下，数字往上
    else if (direction == 's') new_x--;
    else if (direction == 'a') new_y++;
    else if (direction == 'd') new_y--;
    else return false;

    if (new_x < 0 || new_x >= s.N || new_y < 0 || new_y >= s.N) return false;

    swap(s.board[s.blank_x][s.blank_y], s.board[new_x][new_y]);
    s.blank_x = new_x;
    s.blank_y = new_y;
    s.step++;
    s.path += direction; // 记录路径
    return true;
}

inline void PrintBoard(const State& s) {
    cout << "\n======================\n";
    for (int i = 0; i < s.N; i++) {
        for (int j = 0; j < s.N; j++) {
            if (s.board[i][j] == 0) cout << setw(4) << " ";
            else cout << setw(4) << s.board[i][j];
        }
        cout << "\n\n";
    }
    cout << "当前步数: " << s.step << "\n";
    cout << "======================\n";
}

// ================== 成员B的新增核心功能 ==================

// 1. 获取逆序数 (用于判断是否可解)
inline int GetInversionCount(const vector<int>& arr) {
    int inv_count = 0;
    for (size_t i = 0; i < arr.size() - 1; i++) {
        for (size_t j = i + 1; j < arr.size(); j++) {
            if (arr[i] > arr[j]) inv_count++;
        }
    }
    return inv_count;
}

// 2. 随机生成初始状态（保证100%有解，且空格在右下角）
inline void GenerateRandomState(State& s, int n) {
    s.N = n;
    s.step = 0;
    s.path = "";
    s.blank_x = n - 1;
    s.blank_y = n - 1;

    int size = n * n - 1;
    vector<int> nums(size);
    for (int i = 0; i < size; ++i) nums[i] = i + 1;

    // 随机打乱
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    shuffle(nums.begin(), nums.end(), default_random_engine(seed));

    // 【重要数学逻辑】因为空格固定在最后一行最后一位：
    // N=3或N=4时，只要非0数字的“逆序数对”为偶数，就绝对有解。如果是奇数，则无解。
    // 如果无解，我们强行交换前两个数字的位置，改变奇偶性，使其变成有解！
    if (GetInversionCount(nums) % 2 != 0) {
        swap(nums[0], nums[1]);
    }

    // 填充回棋盘
    int idx = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == n - 1 && j == n - 1) s.board[i][j] = 0;
            else s.board[i][j] = nums[idx++];
        }
    }
}

// 3. 动画演示计算机求解过程（接收A同学算出的字符串，比如"wawds"）
inline void PlayAutoPath(State s, const string& path) {
    cout << "\n🤖 计算机开始演示求解过程，总共需要 " << path.length() << " 步...\n";
    this_thread::sleep_for(chrono::milliseconds(1000));

    for (char dir : path) {
        // ANSI 控制码清屏，产生动画效果（如果在VS终端运行有效）
        cout << "\033[2J\033[1;1H";
        cout << "🤖 电脑执行移动: " << dir << "\n";
        Move(s, dir);
        PrintBoard(s);
        this_thread::sleep_for(chrono::milliseconds(400)); // 0.4秒播放一帧
    }
    cout << "🎉 计算机演示完毕！\n";
}

// 4. 批量自动化测试（跑数据给成员D写报告）
// 注意：这个函数里需要调用成员A的算法
inline void RunBatchTests(int n, int test_count) {
    cout << "\n📊 开始进行 " << test_count << " 次自动化测试 (规模 N=" << n << ")...\n";
    int total_steps = 0;
    int success_count = 0;
    auto start_time = chrono::high_resolution_clock::now();

    for (int i = 0; i < test_count; i++) {
        State s;
        GenerateRandomState(s, n); // 生成必然有解的棋盘

        // ============================================
        // 这里需要调用 A同学 写的核心求解函数！
        // 假设 A同学的函数叫 BFS_Solve(State s) 并返回 string
        // string path = BFS_Solve(s);  
        // total_steps += path.length();
        // success_count++;
        // ============================================
    }

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> diff = end_time - start_time;

    cout << "✅ 测试完成！\n";
    cout << "总耗时: " << diff.count() << " 秒\n";
    // cout << "平均求解步数: " << (double)total_steps / success_count << " 步\n";
    cout << "(请取消注释代码对接A同学的算法后查看真实数据)\n";
}

// 5. 人类玩家循环
inline void HumanPlay(State s) {
    char dir;
    cout << "\n🎮 游戏开始！使用 W/A/S/D 移动棋子，输入 'q' 退出。\n";

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

    PrintBoard(s);
    cout << "🎉 恭喜你！成功还原了华容道！\n";
    cout << "总共使用步数: " << s.step << "\n";
}