#include <iostream>
#include "../include/defer.h"

using namespace infra;

int main() {
  std::cout << "=== 阶段 1: 基础 Defer 测试 ===" << std::endl;
  {
    std::cout << "打开文件..." << std::endl;
    
    // 在这个局部作用域里注册了一个 Defer
    Defer defer_close([]() {
      std::cout << "[Defer 触发] 自动关闭文件！" << std::endl;
    });
    
    std::cout << "写入数据..." << std::endl;
  } // <--- 注意这个右大括号，离开作用域的瞬间

  std::cout << "\n=== 阶段 2: 捕获外部变量测试 ===" << std::endl;
  int counter = 0;
  {
    std::cout << "准备执行累加任务..." << std::endl;
    
    // 使用 [&counter] 按引用捕获外部变量
    Defer defer_add([&counter]() {
      std::cout << "[Defer 触发] 自动增加计数器！" << std::endl;
      counter++;
    });
    
    std::cout << "正在执行业务逻辑，当前 counter = " << counter << std::endl;
  } // <--- 离开作用域，触发 counter++
  
  std::cout << "离开作用域后，最终 counter = " << counter << " (如果为1说明成功)" << std::endl;

  return 0;
}