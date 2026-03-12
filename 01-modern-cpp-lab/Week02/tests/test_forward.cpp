#include <iostream>
#include <utility> // for std::forward, std::move
#include "../include/verbose.h"

using namespace infra;

// 中介函数 (问题版本：吃掉右值属性)
template <typename T>
void BadWrapper(T&& arg) {
    std::cout << "BadWrapper 正在传递..." << std::endl;
    // arg 有了名字，变成了左值
    Verbose v(arg); 
}

// 中介函数 (完美的版本：保留原有属性)
template <typename T>
void GoodWrapper(T&& arg) {
    std::cout << "GoodWrapper 正在传递..." << std::endl;
    Verbose v( std::forward<T>(arg) ); 
}

int main() {
    Verbose v1("hello");

    std::cout << "\n=== 测试 BadWrapper ===" << std::endl;
    BadWrapper(std::move(v1)); // 明明传了右值，结果会怎样？

    std::cout << "\n---\n";

    Verbose v2("world");
    std::cout << "\n=== 测试 GoodWrapper ===" << std::endl;
    GoodWrapper(std::move(v2)); // 用 forward 之后，结果会怎样？

    return 0;
}