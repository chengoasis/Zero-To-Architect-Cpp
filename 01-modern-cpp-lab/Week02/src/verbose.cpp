#include "../include/verbose.h"
#include <iostream>
#include <cstring> // strlen 和 strcpy

namespace infra {

// 构造函数
Verbose::Verbose(const char* str) {
  if (str) {
    int len = strlen(str);
    data_ = new char[len + 1];
    strcpy(data_, str);
    std::cout << "[Construct] " << data_ << "\n";
  }
}

// 1. 拷贝构造（深拷贝）
Verbose::Verbose(const Verbose& other) {
  if (other.data_) {
    int len = strlen(other.data_);
    data_ = new char[len + 1];
    strcpy(data_, other.data_);
    std::cout << "[Deep Copy] " << data_ << "\n";
  }
}

// 2. 移动构造
Verbose::Verbose(Verbose&& other) noexcept {
  data_ = other.data_;
  other.data_ = nullptr;
  if (data_) {
    std::cout << "[Move] " << data_ << "\n";
  }
}

// 3. 析构函数
Verbose::~Verbose() {
  if (data_) {
    std::cout << "[Destruct]  " << data_ << "\n";
    delete[] data_;
    data_ = nullptr;
  }
}

// 4. 拷贝赋值有运算
Verbose& Verbose::operator=(const Verbose& other) {
  if (this == &other) {
    return *this;
  }

  delete[] data_;
  data_ = nullptr;

  if (other.data_) {
    int len = strlen(other.data_);
    data_ = new char[len + 1];
    strcpy(data_, other.data_);
    std::cout << "[Copy Assign]" << data_ << "\n";
  }

  return *this;
}

// 5. 移动赋值运算
Verbose& Verbose::operator=(Verbose&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  
  delete[] data_;

  data_ = other.data_;
  other.data_ = nullptr;

  if (data_) {
    std::cout << "[Move Assign]" << data_ << "\n";
  }

  return *this;
}

} // namespace infra