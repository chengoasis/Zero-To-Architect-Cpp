#include <iostream>
#include <vector>
#include "../include/verbose.h"

using namespace infra;

int main() {
  std::cout << "--- 阶段 1: 准备探测器 ---\n";
  Verbose v1("hello");

  std::cout << "\n--- 阶段 2: 放入左值 (Lvalue) ---\n";
  std::vector<Verbose> vec;
  // vec 需要扩容时会移动元素
  vec.reserve(2); 
  
  // v1 是有名字的变量，是个左值。
  // 编译器认为你以后可能还要用 v1：深拷贝！
  vec.push_back(v1);

  std::cout << "\n--- 阶段 3: 放入右值 (Rvalue) ---\n";
  // Verbose("world") 是个临时对象，这行代码结束它就灰飞烟灭了。
  // 既然它马上要死，vector 毫不客气地触发 Move 语义，直接把内存抢过来！
  vec.push_back(Verbose("world"));


  std::cout << "\n--- 阶段 4: 强制移动 (std::move) ---\n";
  Verbose v2("massive_data");
  
  // 此时我想把 v2 塞进 vector，但我不想引发深拷贝
  vec.push_back(std::move(v2));

  std::cout << "v2 现在变成了空壳。\n";
  std::cout << "\n--- 阶段 5: 程序结束，开始销毁 ---\n";
  return 0;
}