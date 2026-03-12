#ifndef VERBOSE_H_
#define VERBOSE_H_

namespace infra {

class Verbose {
 public:
  // 构造函数
  explicit Verbose(const char* str);
  
  // 拷贝构造函数 (深拷贝)
  Verbose(const Verbose& other);
  
  // 移动构造函数 (窃取资源)
  Verbose(Verbose&& other) noexcept;
  
  // 析构函数 (释放资源)
  ~Verbose();

  // 严禁拷贝赋值和移动赋值 (为了简化本关卡)
  Verbose& operator=(const Verbose& other);
  Verbose& operator=(Verbose&& other) noexcept;

 private:
  char* data_ = nullptr; // 堆内存指针
};

} // namespace infra

#endif // VERBOSE_H_