# Phase 1 - Week 03: 函数式编程与泛型 

> **Date**: 2025-02-15
> **Tags**: `Modern C++`, `Lambda`, `std::function`, `Templates`, `Universal Reference`, `Perfect Forwarding`
> **Status**: Completed

## 1. 前言
在 C++98 时代，传递“行为（函数）”只能依赖极其繁琐的函数指针。Modern C++ 引入了 Lambda 表达式、`std::function` 以及更加强大的 Template 泛型推导系统，让我们能够以“零运行时开销 (Zero-Overhead)”的方式，将逻辑作为数据传递。

核心在于理解：**如何在兼顾极致性能的前提下，安全地管理异步回调与作用域守卫。**

---

## 2. 核心概念

### 2.1 Lambda 表达式
Lambda 允许我们“就地”编写逻辑，并通过**捕获列表 (Capture List)** 记录诞生时的外部环境变量，形成闭包 (Closure)。

* **`[ ]` (不捕获)**：只能使用内部局部变量或外部全局常量。最安全。
* **`[=]` / `[var]` (按值捕获)**：拷贝外部变量的副本。适用于基本类型（如 `int`），只读操作，无副作用。
* **`[&]` / `[&var]` (按引用捕获)**：直接操作外部变量的内存。

**坑❗❗❗**：**绝不能将带有 `[&]` 捕获的 Lambda 返回到外部作用域或跨线程传递！**一旦原作用域结束，外部变量被销毁，Lambda 内部将持有一个致命的**悬空引用 (Dangling Reference)**。

### 2.2 std::function (万能函数容器)
`std::function<Ret(Args...)>` 是一个多态的函数包装器。

* **优点**：可以装下任何匹配签名的可调用对象（函数指针、Lambda、仿函数等）。
* **代价**：它底层使用了**类型擦除 (Type Erasure)**。**如果 Lambda 捕获的数据体积过大，`std::function` 会偷偷在堆上分配内存 (Heap Allocation)，且由于间接调用，会阻碍编译器的内联优化 (Inline)。**在极致性能的底层基建中需谨慎使用。

### 2.3 C++ 模板 (Templates) 与万能引用 (Universal Reference)
**为了规避 `std::function` 的性能开销，我们采用泛型编程。**

* **万能引用 (`T&&`)**：在模板推导中，`T&&` 不再是纯粹的右值引用，而是**万能引用 (Forwarding Reference)**。
  * **传左值 -> 推导为左值引用。**
  * **传右值 -> 推导为普通类型（参数即右值引用）。**
* **完美转发 (`std::forward<T>`)**：因为参数一旦有了名字就变成了左值，我们必须用 `std::forward` 将其在初始化列表中“打回原形”，以此触发正确的拷贝或移动构造，实现真正的零开销转移。