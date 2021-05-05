#include <iostream>
#include <thread>

int main() {
  unsigned int n = std::thread::hardware_concurrency();
  std::cout << n << " concurrent threads are supported.\n";
  //std::cout << "sizeof(std::thread):" << sizeof(std::thread) << "\n";
	return 0;
}
