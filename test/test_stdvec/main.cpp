#include <vector>
#include <iostream>

int main() {

  //std::vector<unsigned char> t1 (1e9); //OK
  //std::vector<unsigned char> t1 (2e9); //OK
  //std::vector<unsigned char> t1 (4e9); //OK
  uint32_t cntp = 2000;
  int64_t cnt = cntp*cntp*cntp;
  std::vector<unsigned char> t1 (cnt); //= 8e9
  //std::vector<unsigned char> t1 (2e9);
  t1[100] = 5;
  std::cout << "t1 capacity: " << t1.capacity() << "\n";
  std::cout << "cnt: " << cnt << "\n";

}
