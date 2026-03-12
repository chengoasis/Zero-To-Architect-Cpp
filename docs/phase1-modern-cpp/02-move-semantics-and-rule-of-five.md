# Phase 1 - Week 02: 移动语义、Rule of Five 与完美转发

> **Date**: 2025-02-08
> **Tags**: `Modern C++`, `Move Semantics`, `Rule of Five`, `Perfect Forwarding`, `std::move/forward`
> **Status**: Completed

## 1. 核心概念与直觉 (Core Concepts)

### 1.1 左值 (Lvalue) vs 右值 (Rvalue)
* **左值 (Lvalue)**：有名字，在内存中有确定地址，下一行代码还能访问到它。
    * *策略*：因为以后还要用，传递时必须进行**拷贝 (Copy)**。
* **右值 (Rvalue)**：没名字（或是临时对象），当前语句执行完即刻销毁。
    * *策略*：因为它马上要消亡，可以直接**窃取 (Steal)** 它的内部资源，这被称为**移动 (Move)**。

### 1.2 std::move 的本质
**坑❗❗❗**：`std::move` 不移动任何东西！
它在底层毫无运行时代价，仅仅是一个**编译期类型转换器 (Cast)**。**它将一个左值强制转换为右值引用 (`T&&`)，以此“欺骗”编译器去调用移动构造函数，从而完成所有权的剥夺。**

---

## 2. The Rule of Five (五大准则)

只要你的类中包含底层资源**（如裸指针 `char* data_`、文件描述符、网络 Socket 等）**，并且在析构函数中手动释放了它，就**必须**完整审视以下五个特殊成员函数，否则必定引发 Double Free 或内存泄漏：

1.  **析构函数 (Destructor)**
2.  **拷贝构造 (Copy Constructor)**
3.  **移动构造 (Move Constructor)**
4.  **拷贝赋值 (Copy Assignment)**
5.  **移动赋值 (Move Assignment)**

### 赋值操作符的“四步曲”
构造函数操作的是“空壳”，而赋值操作符 (`operator=`) 面对的是“肚子里可能已经有数据的对象”。实现赋值时必须严格遵循以下步骤：

1.  **自检**：`if (this == &other) return *this;`
2.  **清理门户**：**极其重要！** 先释放自身的旧资源 (e.g., `delete[] data_;`)，否则会导致严重内存泄漏。
3.  **接管新资源**：深拷贝 (`new` + `strcpy`) 或 窃取指针 (`data_ = other.data_; other.data_ = nullptr;`)。
4.  **返回自身**：`return *this;` 支持 `a = b = c;` 的链式调用。

---

## 3. 极致优化：noexcept 与 std::vector 扩容

在实现**移动构造**和**移动赋值**时，必须加上 `noexcept` 关键字（除非确实可能抛出异常）。

**Why?**
当 `std::vector` 因为容量不足触发扩容时，它需要将旧元素搬家到新内存中。为了保证强异常安全性：

* 如果移动构造标记了 `noexcept`，`vector` 会极速地**Move**元素。
* 如果没有标记 `noexcept`，`vector` 害怕搬家半途抛异常导致数据损坏，会退化使用极其缓慢的**Deep Copy**。

---

## 4. 模板黑魔法：完美转发 (Perfect Forwarding)

当我们在编写模板中介函数（如容器的 `emplace_back` 或智能指针的工厂函数 `MakeUnique`）时，会面临**“万物皆左值”**的陷阱。

```cpp
template <typename T>
void Wrapper(T&& arg) {  
    // 无论外部传入左值还是右值，arg 作为参数拥有了名字，在这里统统退化为“左值”。
    // 这会导致底层代码硬生生触发深拷贝！
    Verbose v(arg); 
}
```

**解决方案：`std::forward`** `std::forward<T>(arg)` 能够完美保留参数传递进来时的原始值类别。如果是左值，它传递左值；如果是右值，它还原为右值，从而精准触发底层的移动语义。
