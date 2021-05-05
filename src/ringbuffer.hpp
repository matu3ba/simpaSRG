//FILE ringbuffer.hpp
#pragma once
#include <iostream>
#include <cassert>
//ring buffer are fastest on single thread application
template <class T>
class CircBuffer { //idea https://www.snellman.net/blog/archive/2016-12-13-ring-buffers/
private:
  std::vector<T> buffer;
  uint32_t tail;
  uint32_t head;
  uint32_t bsize;
public: //efficient c++ does not allow logical consistency checks
        //in simple manners inside a template constructor
  CircBuffer() : buffer(0),
                 tail { 0 },
                 head { 0 },
                 bsize {0} {}
  //use power of two https://stackoverflow.com/a/24844826 for wrapping
  //due to maximum Cube size being of type int32_t we can use this safely
  void init(uint32_t size) { //use power of two for wrapping
    assert( size > 1 ); //invalid usage size<=1
    //std::cout << "example size: " << size << "\n";            //DEBUG
    uint32_t power = 2;
    size--;
    while (size >>= 1)  //infinity loop on wrapping to 0
      power <<= 1;
    buffer.resize(power);
    tail = 0;
    head = 0;
    bsize = power;
    //std::cout << "pow2 size: " << bsize << "\n";              //DEBUG
  }
  inline uint32_t mask(uint32_t val)  { return val & (bsize - 1); }
  void push(const T &val)  {
    //assert(!full());
    buffer[head] = val;
    ++head;
    head = mask(head);
  }
  inline void pop() {
    //assert(!empty());
    ++tail;
    tail = mask(tail);
  } //does shift
  T front() {
    return buffer[tail];
  }
  inline uint32_t size() {
    if (head >= tail)
      return head - tail;
    else
      return bsize + head - tail;
  }
  inline bool empty()              { return tail == head; }
};
