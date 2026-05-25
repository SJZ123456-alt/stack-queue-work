#pragma once
#include <stdexcept>
#include <utility>
#include <vector>

template <class T>
class SeqStack {
private:
    T* data;
    int capacity;   // 当前最大容量，满时自动扩容
    int topIndex;

    void expand() 
    {
        int newCapacity = capacity * 2;     // 新容量设置为原容量的2倍
        T* newData = new T[newCapacity];
        for (int i = 0; i <= topIndex; ++i) 
        {
            newData[i] = data[i];
        }

        delete[] data;
        data = newData;
        capacity = newCapacity;
    }

public:
    // 栈初始容量，默认64
    SeqStack(int initCapacity = 64): capacity(initCapacity), topIndex(-1) 
    {
        if (capacity <= 0) capacity = 64;
        {
            data = new T[capacity];
        }
    }

    SeqStack(const SeqStack& other): capacity(other.capacity), topIndex(other.topIndex) 
    {
        data = new T[capacity];  
        for (int i = 0; i <= topIndex; ++i)
        {
            data[i] = other.data[i];
        }
    }

    //这个是因为拷贝构造没法被成员函数直接调用，所以重载重新写一遍
    SeqStack& operator=(const SeqStack& other) 
    {
        if (this == &other) return *this;   // 自赋值判断，防止错误操作
        delete[] data;
        capacity = other.capacity;
        topIndex = other.topIndex;
        data = new T[capacity];
        for (int i = 0; i <= topIndex; ++i)
        {
            data[i] = other.data[i];
        }
        return *this;
    }

    // 析构函数
    ~SeqStack() 
    {
        delete[] data;
    }

    // 入栈
    void push(const T& value) 
    {
        if (topIndex + 1 >= capacity) expand(); // 栈满则扩容
        data[++topIndex] = value;
    }

    // 出栈
    T pop() {
        if (empty()) throw std::runtime_error("Stack is empty");
        // 返回当前栈顶元素，然后将 topIndex 下移一位
        return data[topIndex--];
    }

    // 获取栈顶,可修改
    T& top() {
        if (empty()) throw std::runtime_error("Stack is empty");
        return data[topIndex];
    }

	// 获取栈顶,不可修改
    const T& top() const {
        if (empty()) throw std::runtime_error("Stack is empty");
        return data[topIndex];
    }

    // 判断栈是否为空
    bool empty() const {
        return topIndex < 0;
    }

    // 获取栈中元素个数
    int size() const {
        return topIndex + 1;
    }

    // 清空栈
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

