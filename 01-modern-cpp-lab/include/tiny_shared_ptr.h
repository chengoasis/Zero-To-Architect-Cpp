#ifndef TINY_SHARED_PTR_H_
#define TINY_SHARED_PTR_H_

namespace infra {

template <typename T> 
class SharedPtr {
 public:
  // 1. 构造函数
  // 刚出生：引用计数初始化为 1 （在堆heap上new出来）
  explicit SharedPtr(T* ptr = nullptr) : ptr_(ptr), ref_count_(new int(1)) {
    // 如果接管的是空指针，计数器可以直接删掉或者特殊处理
    if (ptr == nullptr) { 
      *ref_count_ = 0; // 简单处理：空指针计数为0
    }
  }

  // 2. 析构函数
  ~SharedPtr() {
    release();
  }

  // 3. 拷贝构造(Copy Constructor)
  // 共享同一个计数器，计数 +1
  SharedPtr(const SharedPtr& other) {
    ptr_ = other.ptr_;
    ref_count_ = other.ref_count_;
    if (ref_count_) {
      (*ref_count_)++;
    }
  }

  // 4. 拷贝赋值(Copy Assignment)
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

  // 5. 移动构造(Move Constructor)
  // 仅仅是交接所有权，引用计数数值不变
  SharedPtr(SharedPtr&& other) noexcept {
    ptr_ = other.ptr_;
    ref_count_ = other.ref_count_;
    // 此时other必须置空
    other.ptr_ = nullptr;
    other.ref_count_ = nullptr;
  }

  // 6. 移动赋值
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