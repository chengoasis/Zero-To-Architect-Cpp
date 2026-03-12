#include <iostream>
#include <vector>
#include <algorithm>

int main() {
  std::vector<int> nums = {1, 5, 9, 2, 6, 8, 3, 7};
  int threshold = 5;

  int result = std::count_if(nums.begin(), nums.end(), [threshold](int n) {
    return n > threshold;
  });

  std::cout << "大于 " << threshold << " 的数字有 " << result << " 个。\n";
  return 0;
}