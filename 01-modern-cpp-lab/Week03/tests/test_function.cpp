#include <iostream>
#include <vector>
#include <functional>

int main() {
  // void() 表示这个函数不接收参数，也没有返回值
  std::vector<std::function<void()>> task_queue;

  int total_users = 100;

  task_queue.push_back([](){
    std::cout << "Task 1: Initialize Database..." << std::endl;
  });

  task_queue.push_back([total_users](){
    std::cout << "Task 2: Current users:" << total_users << std::endl;
  });

  task_queue.push_back([](){
    std::cout << "Task 3: Send email to admin..." << std::endl;
  });

  std::cout << "=== 开始执行任务队列 ===" << std::endl;
  for (int i = 0; i < task_queue.size(); i++) {
    task_queue[i](); // 调用存储的函数
  }
  return 0;
}