//FILE commonSRG.h
#pragma once
#ifdef CIRCBUFFER
#include "ringbuffer.hpp"
#else
#include <queue>
#endif

//common access functions and operation in every SRG algorithm

//get position of Voxel[Int3] in vector
[[gnu::always_inline]]
inline int32_t vecposition(const Int3 &pt, const Int3 &lim) {
  return pt.x + pt.y*lim.x + pt.z*lim.x*lim.y;
}
//unsafe read value of Voxel[Int3] (max performance)
[[gnu::always_inline]] //static_cast<int64_t> for huge files
inline unsigned char vecread(std::vector<unsigned char> &data, const int32_t vecposition) { 
  return data[vecposition]; 
}
//unsafe write value of Voxel[Int3] (max performance)
[[gnu::always_inline]] //static_cast<int64_t> for huge files
inline void vecwrite(std::vector<unsigned char> &data, const unsigned char val, 
    const int32_t vecposition) {
  data[vecposition] = val;
}
