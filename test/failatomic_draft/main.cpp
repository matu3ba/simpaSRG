//FILE main.cpp in test/simpleSRG

#include<atomic>
#include<queue>
#include<vector>
#include<thread>
#include<chrono>
#include<cstdint>

enum Test {
  a,
  unvisited,
};
struct Threaddata {
  State state { State::deactive };
  std::thread thread {};
  std::queue<uint8_t> qu_work {};
};
struct GlobalData {
  std::vector<Threaddata> threads {};
  std::atomic<uint8_t> cntfreethreads {8};
  std::vector<std::atomic<Test>> atomicaction{};
};
GlobalData globaldata;

void work(uint8_t id) {
  while (stack.empty() == false) {
    uint8_t val = globaldata.threads[id].front();
    globaldata.threads[id].pop();
    if (val == 1
        && globaldata.cnt_freethreads > 0
        && atomicaction[id] == Test::unvisited )
    {


    }
  }
}
int main(int argc, char *argv[]) {
  globaldata.threads[0].thread = 
        std::thread{ work, 0 }; //second item is id
  globaldata.threads[0].thread.detach();

  while (globaldata.cnt_freethreads < globaldata.cores)
    std::this_thread::sleep_for(std::chrono::microseconds(100));

  return 0;
}
