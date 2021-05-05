//FILE simpaSRG.cpp

#include "commonSIMPA.h"

// INVARIANT: global.threads[id].qu_work contains only 1 valid voxel index
#define RAYCAST_STEP                                                    \
voxval = vecread(Cube_data, vecposition(vpos, global.data_size));       \
if (voxval == 1) {                                                      \
  ++step;                                                               \
} else {                                                                \
  if (voxval == 255)                                                    \
    step /= 2;                                                          \
  break;                                                                \
}
[[gnu::always_inline]]
inline void raycastSRG(const uint8_t id, std::vector<unsigned char> &Cube_data) {
  //RAYCAST: read elements in 6 directions and start with maximum distance
  int32_t step;
  const Int3 vox_pos = global.threads[id].qu_work.front();
  global.threads[id].qu_work.pop();                           //queue is now empty
  //assert (global.threads[id].qu_work.empty() == true);
  Int3 vposit = vox_pos;
  Int3 vpos;
  int32_t stepmax = 0;
  Axis ax = Axis::none;
  step = 0;
  unsigned char voxval = 0;
  for (vpos = Int3(vposit.x+1,vposit.y,vposit.z); vpos.x < global.data_size.x; ++vpos.x) {
    RAYCAST_STEP
  }
  if (step > std::abs(stepmax)) { stepmax = step;  ax = Axis::x; }
  step = 0;
  for (vpos = Int3(vposit.x-1,vposit.y,vposit.z); vpos.x >= 0; --vpos.x) {
    RAYCAST_STEP
  }
  if (step > std::abs(stepmax)) { stepmax = -step; ax = Axis::x; }
  step = 0;
  for (vpos = Int3(vposit.x,vposit.y+1,vposit.z); vpos.y < global.data_size.y; ++vpos.y) {
    RAYCAST_STEP
  }
  if (step > std::abs(stepmax)) { stepmax = step;  ax = Axis::y; }
  step = 0;
  for (vpos = Int3(vposit.x,vposit.y-1,vposit.z); vpos.y >= 0; --vpos.y) {
    RAYCAST_STEP
  }
  if (step > std::abs(stepmax)) { stepmax = -step; ax = Axis::y; }
  step = 0;
  for (vpos = Int3(vposit.x,vposit.y,vposit.z+1); vpos.z < global.data_size.z; ++vpos.z) {
    RAYCAST_STEP
  }
  if (step > std::abs(stepmax)) { stepmax = step;  ax = Axis::z; }
  step = 0;
  for (vpos = Int3(vposit.x,vposit.y,vposit.z-1); vpos.z >= 0; --vpos.z) {
    RAYCAST_STEP
  }
  if (step > std::abs(stepmax)) { stepmax = -step; ax = Axis::z; }
  switch (ax) {
    case Axis::x:                                          { vposit.x += stepmax; break; }
    case Axis::y:                                          { vposit.y += stepmax; break; }
    case Axis::z:                                          { vposit.z += stepmax; break; }
    case Axis::none:                                       { break; }
  }
  if ( vposit != vox_pos ) {
    global.threads[id].qu_work.push(vposit);
    threadSRG(id, Cube_data);
  } else {
    deactivateThread(id);
  }
}
