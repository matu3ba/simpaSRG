//FILE simpleSRG.cpp
#include "simpleSRG.h"
#include "commonSRG.h"
#include <queue>

#include <iostream>                     //DEBUG

#ifdef CHECKWRITEVOX
  #error "simpleSRG wrong includes"
#endif
#define CHECKWRITEVOX                                               \
vecpos = vecposition(vpos, sizes);                                  \
if (vecread(Cube_data, vecpos) == 1) {                              \
  vecwrite(Cube_data, 255, vecpos);                                 \
  qu_work.push(vpos);                                               \
}

//simpleSRG
Return SRG(uint8_t cnt_cores __attribute__((unused)), const Int3 seedpoint,
            std::vector<unsigned char> &Cube_data, Int3 &sizes) {
  //worklist as queue with surface as size
#ifdef CIRCBUFFER
  uint32_t min = sizes.minEl();
  uint32_t med = sizes.medEl();
  min += 2;
  med += 2;
  uint32_t circBufsize = min*med;
  CircBuffer<Int3> qu_work {};
  //std::cout << "sizes: " << sizes.string() << "\n";
  qu_work.init(circBufsize);
#else
  std::queue<Int3> qu_work;
#endif
  //uint32_t bufmaxSize = 0;                        //DEBUG
  qu_work.push(seedpoint);
  Int3 vox_pos;
  int32_t vecpos = vecposition(seedpoint, sizes);
  vecwrite(Cube_data, 255, vecpos);               //mark initial state as visited active
  while(qu_work.empty() == false) {
    //uint32_t tmpsize = qu_work.size();            //DEBUG
    //if (tmpsize > bufmaxSize)                     //DEBUG
    //  bufmaxSize = tmpsize;                       //DEBUG
    vox_pos = qu_work.front();
    qu_work.pop();
    Int3 vpos;

    if (vox_pos.x+1 < sizes.x) {
      vpos = Int3(vox_pos.x+1,vox_pos.y,vox_pos.z);
      CHECKWRITEVOX
    }
    if (vox_pos.x-1 >= 0) {
      vpos = Int3(vox_pos.x-1,vox_pos.y,vox_pos.z);
      CHECKWRITEVOX
    }
    if (vox_pos.y+1 < sizes.y) {
      vpos = Int3(vox_pos.x,vox_pos.y+1,vox_pos.z);
      CHECKWRITEVOX
    }
    if (vox_pos.y-1 >= 0) {
      vpos = Int3(vox_pos.x,vox_pos.y-1,vox_pos.z);
      CHECKWRITEVOX
    }
    if (vox_pos.z+1 < sizes.z) {
      vpos = Int3(vox_pos.x,vox_pos.y,vox_pos.z+1);
      CHECKWRITEVOX
    }
    if (vox_pos.z-1 >= 0) {
      vpos = Int3(vox_pos.x,vox_pos.y,vox_pos.z-1);
      CHECKWRITEVOX
    }
  }
  //std::cout << "buffmaxSize: " << bufmaxSize << "\n";       //DEBUG
  return Return::ok;
}
