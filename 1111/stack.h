#pragma once
#include <iostream>
#include <stdexcept>
#include <string>

//----------------基于数组的Stack-------------------//
template <typename T>
class ArrayStack {
private:
    T* _data;
    int  _top;
    int  _capacity;

    void resize() {
        _capacity *= 2;
        T* newData = new T[_capacity];
        for (int i = 0; i <= _top; ++i) { 
            newData[i] = _data[i];
        }
        delete[] _data;
        _data = newData;
    }

public:
    ArrayStack(int cap = 16) : _top(-1), _capacity(cap) { 
        _data = new T[_capacity];
    }
    ~ArrayStack() { delete[] _data; }
    ArrayStack(const ArrayStack&) = delete;
    ArrayStack& operator=(const ArrayStack&) = delete;

    void push(const T& val) {
        if (_top + 1 == _capacity) resize();
        _data[++_top] = val;
    }
    void pop() {
        if (empty()) throw std::underflow_error("Stack is empty");
        --_top;
    }
    T& top() {
        if (empty()) throw std::underflow_error("Stack is empty");
        return _data[_top];
    }
    bool empty() const { return _top == -1; }
    int  size()  const { return _top + 1; }
};

//------------------基于数组的 Queue---------------------//

template <typename T>
class ArrayQueue {
private:
    T* data_;
    int  _front;
    int  _back;      
    int  _size;
    int  _capacity;

    void resize() {
        int newCap = _capacity * 2;
        T* newData = new T[newCap];
        for (int i = 0; i < _size; ++i)
            newData[i] = data_[(_front + i) % _capacity];
        delete[] data_;
        data_ = newData;
        _front = 0;
        _back = _size;
        _capacity = newCap;
    }

public:
    explicit ArrayQueue(int cap = 16)
        : _front(0), _back(0), _size(0), _capacity(cap) {
        data_ = new T[_capacity];
    }

    ~ArrayQueue() { delete[] data_; }

    ArrayQueue(const ArrayQueue&) = delete;
    ArrayQueue& operator=(const ArrayQueue&) = delete;

    void enqueue(const T& val) {
        if (_size == _capacity) resize();
        data_[_back] = val;
        _back = (_back + 1) % _capacity;
        ++_size;
    }

    void dequeue() {
        if (empty()) throw std::underflow_error("Queue is empty");
        _front = (_front + 1) % _capacity;
        --_size;
    }

    T& front() {
        if (empty()) throw std::underflow_error("Queue is empty");
        return data_[_front];
    }

    T& back() {
        if (empty()) throw std::underflow_error("Queue is empty");
        return data_[(_back - 1 + _capacity) % _capacity];
    }

    bool empty() const { return _size == 0; }
    int  size()  const { return _size; }
};

