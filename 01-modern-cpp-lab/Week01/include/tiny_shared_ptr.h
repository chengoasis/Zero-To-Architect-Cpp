#ifndef TINY_SHARED_PTR_H_
#define TINY_SHARED_PTR_H_

namespace infra {

template <typename T> 
class SharedPtr {
 public:
/**
 * 构造函数
 * 
 * @param ptr 原始指针，默认为 nullptr
 * 
 * 工作原理：
 * 1. 保存传入的原始指针
 * 2. 在堆上创建引用计数器并初始化为 1
 * 3. 如果传入空指针，将引用计数器设为 0
 * 
 * explicit 关键字的作用：
 * - 防止隐式转换，例如：SharedPtr<int> p = 123; 这种写法会被禁止
 * - 要求必须显式调用构造函数：SharedPtr<int> p(new int(123));
 * 
 * 引用计数设计：
 * - 使用堆内存分配计数器，便于多个 SharedPtr 对象共享
 * - 初始为 1 表示当前对象独占资源
 * - 空指针特殊处理：计数器为 0，表示无资源可管理
 * 
 * 注意事项：
 * - 如果 ptr 为 nullptr，ref_count_ 仍然会分配内存（简单处理）
 * - 更好的设计可能是：ptr 为 nullptr 时，ref_count_ 也设为 nullptr
 */
  explicit SharedPtr(T* ptr = nullptr) : ptr_(ptr), ref_count_(new int(1)) { // 堆上分配引用计数器，初始为 1
    // 如果接管的是空指针，计数器可以直接删掉或者特殊处理
    if (ptr == nullptr) { 
      *ref_count_ = 0; // 简单处理：空指针计数为0
      // 注意：这种方式下，即使管理空指针，ref_count_ 仍然占用堆内存
    }
  }

  // 2. 析构函数
  ~SharedPtr() {
    release();
  }

/**
 * 拷贝构造函数(Copy Constructor)
 * 
 * @param other 被拷贝的源对象（左值引用）
 * 
 * 工作原理：
 * 1. 共享源对象的资源：复制 ptr_ 和 ref_count_ 指针
 * 2. 增加引用计数：表示新增一个所有者共享该资源
 * 3. 实现浅拷贝 + 引用计数，实现多个智能指针共享同一资源
 * 
 * 为什么使用 const 引用：
 * - 拷贝操作不应修改源对象
 * - 允许从 const 对象进行拷贝
 * 
 * 引用计数变化：
 * - 拷贝前：源对象的引用计数为 n
 * - 拷贝后：源对象和当前对象的引用计数都为 n+1
 * - 计数器本身的值被所有共享者共同维护
 * 
 * 空指针处理：
 * - 如果 other 管理空指针 (ref_count_ == nullptr)
 * - 则当前对象也管理空指针，无需增加计数
 * 
 * 内存语义：浅拷贝，共享资源所有权
 */
  SharedPtr(const SharedPtr& other) {
    ptr_ = other.ptr_;
    ref_count_ = other.ref_count_;
    if (ref_count_) {
      (*ref_count_)++;
    }
  }

/**
 * 拷贝赋值运算符(Copy Assignment)
 * 
 * @param other 被赋值的源对象（左值引用）
 * @return *this 的引用，支持链式赋值 (a = b = c)
 * 
 * 工作原理：
 * 1. 自检：检查是否是自赋值 (this == &other)
 * 2. 释放当前资源：调用 release() 减少原引用计数，必要时删除资源
 * 3. 共享新资源：复制指针，增加新资源的引用计数
 * 4. 返回自身引用
 * 
 * 为什么需要自检：
 * - 防止 self-assignment 导致的问题：obj = obj
 * - 如果不检查，会先释放自己的资源，然后再试图复制已释放的资源
 * 
 * 为什么需要先释放再增加：
 * - 正确管理资源生命周期
 * - 避免内存泄漏
 * - 保持引用计数的准确性
 * 
 * 内存语义：释放旧资源，共享新资源
 */
  SharedPtr& operator=(const SharedPtr& other) {
    // 自检
    if (this == &other) return *this;
    release();
    ptr_ = other.ptr_;
    ref_count_ = other.ref_count_;
    if (ref_count_) {
      (*ref_count_)++;
    }
    return *this;
  }

/**
 * 移动构造函数(Move Constructor)
 * 
 * @param other 右值引用，将被移动的对象
 * 
 * 工作原理：
 * 1. 直接接管 other 的资源（指针和引用计数）
 * 2. 将 other 置为空状态
 * 3. 引用计数保持不变（因为资源没变，只是所有者变了）
 * 
 * 为什么需要将 other 置空：
 * - 如果不置空，other 析构时会减少引用计数或释放资源
 * - 这会导致新对象持有的资源被错误释放
 * - 移动后的对象应该处于"有效但未指定"的状态，通常为空
 * 
 * noexcept 保证：
 * - 移动操作不应抛出异常
 * - 便于标准库在重新分配时优先使用移动而非拷贝
 */
  SharedPtr(SharedPtr&& other) noexcept {
    ptr_ = other.ptr_;
    ref_count_ = other.ref_count_;
    // 置空
    other.ptr_ = nullptr;
    other.ref_count_ = nullptr;
  }

/**
 * 移动赋值运算符
 * 
 * @param other 右值引用，将被移动的源对象
 * @return *this 的引用，支持链式赋值
 * 
 * 工作原理：
 * 1. 自检：检查是否是自赋值 (this == &other)
 * 2. 释放当前资源：调用 release() 减少原引用计数
 * 3. 接管资源：直接窃取 other 的资源（指针和引用计数）
 * 4. 置空源对象：将 other 置为空状态，完成所有权转移
 * 5. 返回自身引用
 * 
 * 为什么需要将 other 置空：
 * - 防止 other 析构时释放我们刚接管的资源
 * - 确保资源的唯一所有权完全转移到当前对象
 * - 保持移动语义：源对象应处于"空"状态
 * - 避免双重释放（double free）
 * 
 * noexcept 保证：
 * - 移动操作不应抛出异常
 * - 便于标准库容器在重新分配时优先使用移动而非拷贝
 * - 当前实现只涉及指针赋值和释放操作，不会抛出异常
 * 
 * 与拷贝赋值的区别：
 * - 拷贝赋值：增加引用计数，共享资源
 * - 移动赋值：不改变引用计数，转移所有权
 * 
 * 内存语义：转移所有权，源对象置空
 */
  SharedPtr& operator=(SharedPtr&& other) noexcept {
    if (this == &other) return *this;
    release();
    ptr_ = other.ptr_;
    ref_count_ = other.ref_count_;
    // 同理，other必须置空
    other.ptr_ = nullptr;
    other.ref_count_ = nullptr;
    return *this;
  }

  // 访问器
  T* get() const { return ptr_; }
  T& operator*() const { return *ptr_; }
  T* operator->() const { return ptr_; }
  operator bool() const {
    return ptr_ != nullptr;
  }

  // 查看引用计数
  int use_count() const {
    return ref_count_ ? *ref_count_ : 0;
  }

 private:
  // 为了避免代码重复，我们写一个 release() 函数。这是常见的去重手段。
  void release() {
    if (!ref_count_) return; // 空指针保护
    
    (*ref_count_)--; // 大家都看同一个记分牌，减1
    
    if (*ref_count_ == 0) {
      // 只有当记分牌归零时，才真正动手杀人
      delete ptr_;
      delete ref_count_; // 记得把记分牌也销毁！
    }
  }

  T* ptr_;          // 指向真实数据
  int* ref_count_;  // 指向堆上的计数器 (Scoreboard)
};

} // namespace infra

#endif // TINY_SHARED_PTR_H_