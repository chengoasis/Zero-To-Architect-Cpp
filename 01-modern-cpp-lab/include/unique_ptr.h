#ifndef TINY_UNIQUE_PTR_H_
#define TINY_UNIQUE_PTR_H_

#include <utility> // for std::exchange

// 命名空间管理，防止污染全局
namespace infra {

// 1. 类定义
template<typename T>
class UniquePtr {
public:
  // 1. 默认构造与裸指针构造合并
  // explicit: 防止隐式转换 (e.g., UniquePtr<int> p = 10; -> Error)
  // noexcept: 构造函数不应抛出异常
  explicit UniquePtr(T* ptr = nullptr) noexcept : ptr_(ptr) {}

  // 2. 析构函数
  // RAII 核心：出作用域自动释放
  ~UniquePtr() {
    if (ptr_) delete ptr_;
  }

  // 3. 禁止拷贝 (Rule of 5)
  // 独占所有权，绝对禁止复制
  UniquePtr(const UniquePtr& other) = delete;
  UniquePtr& operator=(const UniquePtr& other) = delete;

  // 移动构造（Move Constructor）
  // 从临时对象中"窃取"资源
  // 注意：参数不能是 const，否则无法置空源指针
  UniquePtr(UniquePtr&& other) noexcept {
    // 惯用手法：接管 ptr，并将 other.ptr 置为 nullptr
    // ptr_ = other.ptr_;
    // other.ptr_ = nullptr;
    ptr_ = std::exchange(other.ptr_, nullptr);
  }

  // 5. 移动赋值 (Move Assignment)
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

  // 6. 观察器 (Observers)
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