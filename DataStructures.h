#pragma once

#include <stdexcept>
#include <utility>
#include <vector>

// 顺序栈：基于动态数组实现的栈结构
// 核心用途：保存和回放路径求解过程中的节点/状态，支持后进先出访问
// T：栈中存储的元素类型（泛型）

//注：因为华容道3乘3或者4乘4最多只要几十步，所以几乎不需要扩容，而且顺序栈比链栈访问快，便于画图。

template <class T>
class SeqStack {
private:
    T* data;        // 动态数组指针，用于实际存储栈内元素
    int capacity;   // 动态数组当前最大容量，满时自动扩容
    int topIndex;   // 栈顶元素下标，初始为-1表示空栈，指向最后一个有效元素

    // 栈扩容函数：当元素数量达到容量上限时自动扩容
    // 扩容策略：容量直接翻倍，保证均摊时间复杂度O(1)
    void expand() {
        int newCapacity = capacity * 2;     // 新容量设置为原容量的2倍
        T* newData = new T[newCapacity];    // 申请新的更大的动态数组
        // 将原数组所有元素逐个拷贝到新数组中
        for (int i = 0; i <= topIndex; ++i) {
            newData[i] = data[i];
        }
        delete[] data;                      // 释放原数组内存，避免内存泄漏
        data = newData;                     // 指向新数组
        capacity = newCapacity;            // 更新容量
    }

public:
    // 构造函数：初始化栈
    // initCapacity：栈初始容量，默认64，若传入非法值则自动修正为64
    explicit SeqStack(int initCapacity = 64)
        : capacity(initCapacity), topIndex(-1) {
        if (capacity <= 0) capacity = 64;   // 保证容量合法，最小为64
        data = new T[capacity];             // 分配初始动态数组
    }

    // 拷贝构造函数：深拷贝另一个栈的所有元素
    // 避免浅拷贝导致的指针重复释放问题
    SeqStack(const SeqStack& other)
        : capacity(other.capacity), topIndex(other.topIndex) {
        data = new T[capacity];  
        for (int i = 0; i <= topIndex; ++i) data[i] = other.data[i];
    }

    //这个是因为拷贝构造没法被成员函数直接调用，所以重载重新写一遍
    // 赋值运算符重载：深拷贝赋值，支持栈之间的赋值操作
    SeqStack& operator=(const SeqStack& other) {
        if (this == &other) return *this;   // 自赋值判断，防止错误操作
        delete[] data;                      // 释放当前对象原有内存
        capacity = other.capacity;
        topIndex = other.topIndex;
        data = new T[capacity];             // 重新分配内存并拷贝
        for (int i = 0; i <= topIndex; ++i) data[i] = other.data[i];
        return *this;
    }

    // 析构函数：释放动态数组内存
    ~SeqStack() {
        delete[] data;
    }

    // 入栈操作：将元素压入栈顶
    // 先判断是否需要扩容，再更新栈顶并赋值
    void push(const T& value) {
        if (topIndex + 1 >= capacity) expand(); // 栈满则扩容
        data[++topIndex] = value;               // 栈顶上移，存入新元素
    }

    // 出栈操作：删除并返回栈顶元素
    // 这里空栈时使用关键字throw,可以立即终止函数
    T pop() {
        if (empty()) throw std::runtime_error("Stack is empty");
        // 返回当前栈顶元素，然后将 topIndex 下移一位
        return data[topIndex--];
    }

    // 获取栈顶元素的引用（非const版本，可修改）
    T& top() {
        if (empty()) throw std::runtime_error("Stack is empty");
        return data[topIndex];
    }

    // const重载版本
    const T& top() const {
        if (empty()) throw std::runtime_error("Stack is empty");
        return data[topIndex];
    }

    // 判断栈是否为空
    bool empty() const {
        return topIndex < 0;
    }

    // 获取栈中当前元素个数
    int size() const {
        return topIndex + 1;
    }

    // 清空栈：仅重置栈顶指针，不释放内存，高效复用
    void clear() {
        topIndex = -1;
    }
};

// 循环队列：基于循环数组实现的队列结构
// 核心用途：BFS（广度优先搜索）求解最短路径，支持先进先出访问
//同理，这里也选择动态数组类型的
template <class T>
class CirQueue {
private:
    T* data;        // 循环动态数组指针，存储队列元素
    int capacity;   // 数组最大容量，满时自动扩容
    int frontIndex; // 队头下标：指向即将出队的元素位置
    int rearIndex;  // 队尾下标：指向即将入队的元素位置
    int count;      // 队列当前有效元素个数，简化判空/判满逻辑

    // 队列扩容函数：容量翻倍，重新整理循环数组为线性顺序
    void expand() {
        int newCapacity = capacity * 2;     // 新容量为原容量2倍
        T* newData = new T[newCapacity];    // 新建更大数组
        // 按队头到队尾的顺序，将元素拷贝到新数组（解开循环结构）
        for (int i = 0; i < count; ++i) {
            newData[i] = data[(frontIndex + i) % capacity];//这个是循环数组的赋值方式
        }
        delete[] data;                      // 释放原数组
        data = newData;
        capacity = newCapacity;
        frontIndex = 0;                     // 新数组队头重置为0
        rearIndex = count;                  // 新数组队尾指向元素末尾
    }

public:
    // 构造函数：初始化循环队列
    // initCapacity：初始容量，默认1024，非法值自动修正为1024
    explicit CirQueue(int initCapacity = 1024)
        : capacity(initCapacity), frontIndex(0), rearIndex(0), count(0) {
        if (capacity <= 1) capacity = 1024; // 保证容量合法
        data = new T[capacity];
    }

    // 析构函数：释放数组内存
    ~CirQueue() {
        delete[] data;
    }

    // 入队操作：将元素添加到队尾
    void push(const T& value) {
        if (count + 1 >= capacity) expand(); // 队列即将满时扩容
        data[rearIndex] = value;             // 在队尾位置存入元素
        rearIndex = (rearIndex + 1) % capacity; // 队尾指针循环后移
        ++count;                             // 元素数量+1
    }

    // 出队操作：删除并返回队头元素
    T pop() {
        if (empty()) throw std::runtime_error("Queue is empty");
        T value = data[frontIndex];         // 保存队头元素
        frontIndex = (frontIndex + 1) % capacity; // 队头指针循环后移
        --count;                           // 元素数量-1
        return value;
    }

    // 判断队列是否为空
    bool empty() const {
        return count == 0;
    }

    // 获取队列当前元素个数
    int size() const {
        return count;
    }

    // 清空队列：重置所有指针和计数，不释放内存
    void clear() {
        frontIndex = rearIndex = count = 0;
    }
};

// 二叉最小堆：手动实现的完全二叉堆结构
// 核心用途：优先队列，用于A*等路径搜索算法，保证每次取最小权值元素
// T：堆中元素类型；Compare：比较规则，满足最小堆语义
template <class T, class Compare>
class MinHeap {
private:
    std::vector<T> heap; // 用vector动态数组存储堆，模拟完全二叉树
    Compare cmp;         // 比较函数对象：cmp(a,b)为true表示a优先级更高（更小）

    // 向上调整（上浮）：新元素插入后，向上交换维持最小堆性质
    // index：新插入元素的起始下标
    void siftUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;   // 计算当前节点的父节点下标
            // 当前节点不小于父节点，堆结构合法，退出循环
            if (!cmp(heap[index], heap[parent])) break;
            // 交换当前节点与父节点，继续向上检查
            std::swap(heap[index], heap[parent]);
            index = parent;
        }
    }

    // 向下调整（下沉）：堆顶删除后，向下交换维持最小堆性质
    // index：待调整节点的起始下标
    void siftDown(int index) {
        int n = static_cast<int>(heap.size());
        while (true) {
            int left = index * 2 + 1;       // 左孩子节点下标
            int right = left + 1;           // 右孩子节点下标
            int best = index;               // 记录当前节点与子节点中的最小值下标
            // 左孩子更小，更新最小值下标
            if (left < n && cmp(heap[left], heap[best])) best = left;
            // 右孩子更小，更新最小值下标
            if (right < n && cmp(heap[right], heap[best])) best = right;
            // 最小值就是自身，堆结构合法，退出循环
            if (best == index) break;
            // 交换当前节点与最小值节点，继续向下检查
            std::swap(heap[index], heap[best]);
            index = best;
        }
    }

public:
    // 入堆操作：添加元素并上浮调整
    void push(const T& value) {
        heap.push_back(value);              // 插入到数组末尾
        siftUp(static_cast<int>(heap.size()) - 1); // 对最后一个元素上浮
    }

    // 出堆操作：取出堆顶最小值，并用末尾元素替换后下沉调整
    T pop() {
        if (empty()) throw std::runtime_error("Heap is empty");
        T answer = heap[0];                 // 保存堆顶最小值
        heap[0] = heap.back();              // 用最后一个元素覆盖堆顶
        heap.pop_back();                    // 删除最后一个元素
        if (!heap.empty()) siftDown(0);     // 下沉调整恢复堆结构
        return answer;
    }

    // 判断堆是否为空
    bool empty() const {
        return heap.empty();
    }

    // 获取堆中元素个数
    int size() const {
        return static_cast<int>(heap.size());
    }

    // 清空堆：释放vector内存
    void clear() {
        heap.clear();
    }
};