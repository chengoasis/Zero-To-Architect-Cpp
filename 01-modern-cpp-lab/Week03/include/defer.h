#ifndef INFRA_DEFER_H_
#define INFRA_DEFER_H_

#include <utility>

namespace infra {

// 类模板声明：Func 是一个由编译器推导出来的泛型类型
template <typename Func>
class Defer {
 public:
  // 1. 构造函数：接收传入的函数对象，并通过完美转发初始化给成员变量
  explicit Defer(Func&& f) : func_(std::forward<Func>(f)) {}

  // 2. 析构函数,在离开作用域时自动调用
   ~Defer() {
    func_(); 
   }

  // 3. Rule of Five 防御
  Defer(const Defer&) = delete;
  Defer& operator=(const Defer&) = delete;
  Defer(Defer&&) = delete;
  Defer& operator=(Defer&&) = delete;

 private:
  Func func_; // 直接存储泛型类型的对象，没有任何指针和动态分配！
};

} // namespace infra

#endif // INFRA_DEFER_H_