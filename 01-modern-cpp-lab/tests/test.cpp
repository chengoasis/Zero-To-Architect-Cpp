#include <iostream> 
#include <string>
#include "../include/tiny_unique_ptr.h" 
#include "../include/tiny_shared_ptr.h" 

class Tracker {
 public:
  Tracker(int id) : id_(id) {
    std::cout << "[Construct] ID: " << id_ << " @" << this << std::endl;
  }

  ~Tracker() {
    std::cout << "[Destruct]  ID: " << id_ << " @" << this << std::endl;
  }

  void Print() const {
    std::cout << "Running task " << id_ << std::endl;
  }

 private:
  int id_;
};

void test_unique_ptr() {
  std::cout << "\n========= UniquePtr 测试 =========" << std::endl;
    
  // 创建 unique_ptr
  infra::UniquePtr<Tracker> p1(new Tracker(1));
  p1->Print();
  
  // 测试移动构造
  infra::UniquePtr<Tracker> p2(std::move(p1));
  if (!p1) {
    std::cout << "p1 已被移动（此时 p1 为空）" << std::endl;
  }
  if (p2) {
    std::cout << "p2 此时接管了 p1" << std::endl;
    p2->Print();
  }
  
  // 测试移动赋值
  infra::UniquePtr<Tracker> p3;
  p3 = std::move(p2);
  if (!p2) {
    std::cout << "p2 已被移动（此时 p2 为空）" << std::endl;
  }
  if (p3) {
    std::cout << "p3 此时接管了 p2" << std::endl;
    p3->Print();
  }
  
  // 测试作用域
  std::cout << "---测试作用域---" << std::endl;
  {
      infra::UniquePtr<Tracker> p4(new Tracker(2));
      p4->Print();
  } // 离开作用域：p4 在这里析构，应打印 [Destruct] ID: 2
  std::cout << "离开内部作用域" << std::endl;
} // p3 在这里析构

void test_shared_ptr() {
  std::cout << "\n========= Test shared_ptr =========" << std::endl;

  // 创建 shared_ptr
  infra::SharedPtr<Tracker> p1(new Tracker(101));
  p1->Print();
  std::cout << "Count p1: " << p1.use_count() << std::endl; // 应为 1
  
  // 拷贝构造
  infra::SharedPtr<Tracker> p2(p1); // 拷贝构造
  p2->Print();
  std::cout << "Count p1: " << p1.use_count() << std::endl; // 应为 2
  std::cout << "Count p2: " << p2.use_count() << std::endl; // 应为 2

  // 拷贝赋值
  infra::SharedPtr<Tracker> p3;
  p3 = p2;
  std::cout << "赋值后 Count p3: " << p1.use_count() << std::endl; // 应为 3

  // 测试作用域
  {
    infra::SharedPtr<Tracker> p4 = p1;
    std::cout << "内部作用域 use_count: " << p1.use_count() << std::endl; // 应为 4
  }
  std::cout << "离开内部作用域 use_count: " << p1.use_count() << std::endl; // 应为 3

  // 移动语义测试
  infra::SharedPtr<Tracker> p5 = std::move(p1);
  if (!p1) {
    std::cout << "p1 已被移动(此时为空了)" << std::endl;
  }
  if (p5) {
    p5->Print();
    std::cout << "p5.use_count: " << p5.use_count() << std::endl; // 应为 3
  }
}

int main() {
  std::cout << "=========================== 开始智能指针测试 ===========================" << std::endl;
    
  test_unique_ptr();
  test_shared_ptr();
    
  std::cout << "\n=========================== 测试完成 ===========================" << std::endl;
  return 0;
}