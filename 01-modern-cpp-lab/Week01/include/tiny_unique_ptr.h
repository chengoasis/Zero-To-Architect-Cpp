#ifndef TINY_UNIQUE_PTR_H_
#define TINY_UNIQUE_PTR_H_

#include <utility> // for std::exchange
#include <iostream>

// 命名空间管理，防止污染全局
namespace infra {

// 类定义
template<typename T>
class UniquePtr {
 public:
/**
 * 构造函数（默认构造 + 裸指针构造）
 * 
 * @param ptr 原始指针，默认为 nullptr
 * 
 * explicit 关键字的作用：
 * - 防止隐式转换：UniquePtr<int> p = new int(10);  ❌ 编译错误
 * - 必须显式调用：UniquePtr<int> p(new int(10));  ✅ 正确
 * - 避免意外的类型转换，增强代码安全性
 * 
 * noexcept 保证：
 * - 构造函数不应抛出异常
 * - 只涉及指针赋值，不会抛出异常
 * - 便于标准库优化（例如 vector 重新分配时优先使用移动）
 * 
 * 默认参数：
 * - ptr = nullptr 允许默认构造 UniquePtr<int> p;
 * - 这样空指针和有效指针使用同一个构造函数
 * 
 * 资源管理原则：
 * - 构造函数只保存指针，不分配资源（资源由外部传入）
 * - 遵循 RAII 原则：构造时获取资源所有权
 * 
 * 异常安全：noexcept 保证，绝不抛出异常
 */
  explicit UniquePtr(T* ptr = nullptr) noexcept : ptr_(ptr) {}

  // 2. 析构函数
  // RAII 核心：出作用域自动释放
  ~UniquePtr() {
    if (ptr_) delete ptr_;
  }

/**
 * 禁止拷贝构造 - 删除拷贝构造函数
 * 
 * 语法：= delete 告诉编译器删除这个函数
 * 效果：任何试图拷贝 UniquePtr 的操作都会导致编译错误
 * 
 * 为什么禁止拷贝：
 * 1. 独占所有权：unique_ptr 的核心语义是独占资源所有权
 * 2. 避免双重释放：如果两个指针指向同一资源，析构时都会 delete
 * 3. 语义明确：明确表达"只能移动，不能复制"的设计意图
 * 
 * 编译错误示例：
 * UniquePtr<int> p1(new int(42));
 * UniquePtr<int> p2(p1);         // ❌ 编译错误：使用了被删除的函数
 * UniquePtr<int> p3 = p1;        // ❌ 编译错误：同上
 * 
 * Rule of 5 的一部分：
 * - 自定义析构函数
 * - 删除拷贝构造
 * - 删除拷贝赋值
 * - 自定义移动构造
 * - 自定义移动赋值
 */
  UniquePtr(const UniquePtr& other) = delete;

/**
 * 禁止拷贝赋值运算符 - 删除拷贝赋值函数
 * 
 * 同样使用 = delete 删除拷贝赋值操作
 * 
 * 为什么也禁止拷贝赋值：
 * 1. 保持一致性：如果禁止拷贝构造，也应该禁止拷贝赋值
 * 2. 避免复杂语义：拷贝赋值涉及释放当前资源、复制新资源
 * 3. 资源管理清晰：独占所有权意味着不能共享
 * 
 * 编译错误示例：
 * UniquePtr<int> p1(new int(42));
 * UniquePtr<int> p2(new int(100));
 * p2 = p1;  // ❌ 编译错误：使用了被删除的函数
 * 
 * 注意：即使 p2 已经管理着资源，也不能通过赋值来转移所有权
 * 所有权转移必须通过移动语义显式表达：p2 = std::move(p1);
 */
  UniquePtr& operator=(const UniquePtr& other) = delete;

/**
 * 移动构造函数（Move Constructor）
 * 
 * @param other 右值引用，将被移动的源对象
 * 
 * 工作原理：
 * 1. 从 other 中"窃取"资源（接管原始指针）
 * 2. 将 other 置为空状态，完成所有权转移
 * 3. 原对象不再拥有资源，析构时安全
 * 
 * 为什么参数不能是 const：
 * - 需要修改 other 的内部状态（置空）
 * - const 对象不能被修改
 * 
 * noexcept 保证：
 * - 移动操作不应抛出异常
 * - 便于标准库优化（vector 重新分配时优先使用移动）
 * 
 * 资源管理原则：
 * - 转移所有权，不复制资源
 * - 被移动的对象处于"空"状态
 * - 符合 unique_ptr 的独占语义
 */
  UniquePtr(UniquePtr&& other) noexcept {
    // 惯用手法：接管 ptr，并将 other.ptr 置为 nullptr
    // ptr_ = other.ptr_;
    // other.ptr_ = nullptr;
    ptr_ = std::exchange(other.ptr_, nullptr);
  }

/**
 * 移动赋值运算符 (Move Assignment)
 * 
 * @param other 右值引用，将被移动的源对象
 * @return *this 的引用，支持链式赋值
 * 
 * 工作原理：
 * 1. 自检：检查是否是自赋值 (this != &other)
 * 2. 释放当前资源：delete 当前管理的指针
 * 3. 接管资源：从 other 窃取指针
 * 4. 置空源对象：将 other 置为空状态
 * 5. 返回自身引用
 * 
 * noexcept 保证：
 * - 移动操作不应抛出异常
 * - 便于标准库优化（vector 重新分配时优先使用移动）
 * 
 * 与拷贝赋值的区别：
 * - 拷贝赋值：被禁止（= delete）
 * - 移动赋值：允许，转移所有权
 */
  UniquePtr& operator=(UniquePtr&& other) noexcept {
    // 极其重要：防止自赋值 (e.g., p = std::move(p))
    if (this != &other) {
      if (ptr_) {
        delete ptr_;
      }
      // ptr_ = other.ptr_;
      // other.ptr_ = nullptr;
      ptr_ = std::exchange(other.ptr_, nullptr);
    }
    return *this;
  }

  // 访问器
  // const 修饰成员函数，表示该操作不会修改成员变量
  T& operator*() const {
    return *ptr_;
  }

  T* operator->() const {
    return ptr_;
  }

  // 显式布尔转换 (e.g., if (ptr) ...)
  // operator bool() 本身就声明了返回类型是 bool
  // 不需要像普通函数那样在前面单独写返回类型
  explicit operator bool() const {
    return ptr_ != nullptr;
  }

 private:
  T* ptr_ = nullptr;
};

} // namespace infra

#endif // TINY_UNIQUE_PTR_H_