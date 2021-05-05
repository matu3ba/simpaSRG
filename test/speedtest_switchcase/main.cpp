#include <iostream>
#include <chrono>
#include <queue>

int main () {
  int test = 0;
  unsigned long long int factor = 8;
  unsigned long long int cycles = 1e08;
  std::queue<unsigned char> queue;

  for (int lv=0; lv<10; lv++) {
    for(unsigned long long int i=0; i<cycles; i++) {
      queue.push(1);
      queue.push(3);
      queue.push(5);
      queue.push(2);
      queue.push(6);
      queue.push(7);
      queue.push(4);
      queue.push(0);
    }
    auto r1start = std::chrono::steady_clock::now();
    for(unsigned long long int i=0; i<factor*cycles; i++) {
      unsigned char val = queue.front();
      queue.pop();
      switch(val) {
        case 0:
          test += 1;
          break;
        case 1:
          test %= 2+1;
          break;
        case 2:
          test %= 2+1;
          break;
        case 3:
          test %= 3*2;
          break;
        case 4:
          test %= 4*2;
          break;
        case 5:
          test %= 5*2;
          break;
        case 6:
          test %= 12;
          break;
        case 7:
          test %= 14;
          break;
        default:
          break;
      }
    }
    auto r1end = std::chrono::steady_clock::now();
    std::cout << "switch case(" << cycles*factor << "): " << test << "\n";
    std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::nanoseconds>
      (r1end - r1start).count() << " ns\n";
    if(queue.empty() == false) {
      std::cout << "queue not empty\n";
      return 0;
    }
    test = 0;

    for(unsigned long long int i=0; i<cycles; i++) {
      queue.push(1);
      queue.push(3);
      queue.push(5);
      queue.push(2);
      queue.push(6);
      queue.push(7);
      queue.push(4);
      queue.push(1); //here not 0
      //1 modulo 0 is undefined !!!
    }
    auto r2start = std::chrono::steady_clock::now();
    for(unsigned long long int i=0; i<factor*cycles; i++) {
      unsigned char val = queue.front();
      queue.pop();
      test += 1;
      test = test%5;
    }
    auto r2end = std::chrono::steady_clock::now();
    std::cout << "no switch case(" << cycles*factor << "): " << test << "\n";
    std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::nanoseconds>
      (r2end - r2start).count() << " ns\n";
  }
  return 0;
}
