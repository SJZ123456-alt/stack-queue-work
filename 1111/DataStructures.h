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

// 循环队列
template <class T>
class CirQueue {
private:
    T* data;
    int capacity;
    int frontIndex; // 队头
    int rearIndex;  // 队尾
    int count;      // 队列元素个数

    // 队列扩容，容量翻倍
    void expand() 
    {
        int newCapacity = capacity * 2;     // 新容量为原容量2倍
        T* newData = new T[newCapacity];
        for (int i = 0; i < count; ++i) 
        {
            newData[i] = data[(frontIndex + i) % capacity];
        }
        delete[] data;
        data = newData;
        capacity = newCapacity;
        frontIndex = 0;
        rearIndex = count;
    }

public:
    // 构造函数，初始容量默认1024
    CirQueue(int initCapacity = 1024): capacity(initCapacity), frontIndex(0), rearIndex(0), count(0) 
    {
        if (capacity <= 1) capacity = 1024;
        data = new T[capacity];
    }

    // 析构
    ~CirQueue() 
    {
        delete[] data;
    }

    // 入队
    void push(const T& value) 
    {
        if (count + 1 >= capacity) expand(); // 队列即将满时扩容

        data[rearIndex] = value;
        rearIndex = (rearIndex + 1) % capacity;
        count++;
    }

    // 出队
    T pop() 
    {
        if (empty()) throw std::runtime_error("Queue is empty");
        T value = data[frontIndex];
        frontIndex = (frontIndex + 1) % capacity;
        count--;
        return value;
    }

    // 判断空
    bool empty() const 
    {
        return count == 0;
    }

    // 获取队列元素个数
    int size() const 
    {
        return count;
    }

    // 清空队列
    void clear() 
    {
        frontIndex = rearIndex = count = 0;
    }
};

