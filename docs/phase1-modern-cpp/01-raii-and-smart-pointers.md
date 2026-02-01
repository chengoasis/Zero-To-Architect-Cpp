# Week 01: RAII 与智能指针本质

> **Date**: 2025-02-01
> **Tags**: `C++20`, `RAII`, `Memory Management`, `Smart Pointers`
> **Status**: Completed

## 1. 核心思维：RAII (资源获取即初始化)

### 1.1 痛点：麻烦的C 语言
在 Old C++ 或 C 中，资源管理依赖人工：
```cpp
void OldWay() {
    Resource* res = new Resource();
    if (invalid_condition) {
        return; // ❌ 内存泄漏！忘记 delete
    }
    DoSomething(); // ❌ 如果这里抛出异常，delete 永远不会执行
    delete res;
}
```

### 1.2 解药：RAII

**RAII (Resource Acquisition Is Initialization)** 是 C++ 最重要的编程惯用法。

- **核心**: 将资源的生命周期绑定到**栈对象**的生命周期。
- **构造**: 获取资源（如 new, open file, lock）。
- **析构**: 释放资源（如 delete, close file, unlock）。
- **优势**: 无论函数是如何返回的（正常返回 or 抛出异常），**栈对象的析构函数一定会被调用，从而保证资源释放。**

------



## 2. 核心实现：手写智能指针

### 2.1 UniquePtr (独占所有权)

- **特点**: 同一时刻只能有一个指针拥有该对象。
- **关键**: **禁用拷贝** (delete copy ctor), **启用移动** (move ctor)。

### 2.2 SharedPtr (共享所有权)

- **特点**: 多个指针共享同一个对象，最后一个离开的人负责关灯（释放内存）。
- **原理**: 引入 **引用计数 (Reference Count)**。每次拷贝构造，计数 +1。每次析构，计数 -1。计数为 0 时，delete 对象。
- **内存布局**:SharedPtr 内部有两个指针：一个指向对象 T，一个指向堆上的 ControlBlock（存放引用计数）。

------



## 3.Q&A)

### Q1: unique_ptr 相比裸指针有性能损耗吗？

- **Answer**: **几乎没有**。编译器会将 `unique_ptr` 的析构和解引用操作内联 (inline)，生成的汇编代码通常与手写 delete 的裸指针完全一致。

### Q2: shared_ptr 是线程安全的吗？

- **Answer**: 需要考虑两个层面。**引用计数本身 (Control Block)**：是**线程安全**的。标准库通常使用 `std::atomic` 来操作计数，保证多线程拷贝/析构 `shared_ptr` 不会乱。**管理的对象 (Managed Object)**：**不是线程安全**的。如果多个线程同时读写同一个 `shared_ptr` 指向的对象，必须手动加锁 (std::mutex)。

### Q3: 为什么要用 std::make_shared？

- **Answer**: 性能与内存碎片优化。**普通 的new:**  `std::shared_ptr<T> p(new T);` 会触发**两次**堆内存分配（一次分配 T，一次分配 ControlBlock）。**make_shared： `auto p = std::make_shared<T>();` 只触发一次分配，它会分配一块足够大的连续内存同时存放 T 和 ControlBlock。这不仅减少了 malloc 开销，还提高了 CPU 缓存命中率。**

### Q4: 什么是循环引用 ？如何解决？

- **Answer**: 当 A 指向 B，B 也指向 A，且都用 `shared_ptr` 时，引用计数永远减不到 0，导致内存泄漏。
- **Solution**: 将其中一方（通常是“下级”指回“上级”的指针）改为 `std::weak_ptr`。`weak_ptr` 不增加引用计数。