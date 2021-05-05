//FILE simpaSRG.cpp

#include "commonSIMPA.h"

[[gnu::always_inline]]
inline int32_t raycastStepWrite(const int32_t steppl, const int32_t stepmi) {
  if (steppl > stepmi) { 
    return steppl;
  } else { 
    return -stepmi;
  }
}
// INVARIANT: global.threads[id].qu_work contains only 1 valid voxel index
#define STEP_VAR_PL steppl
#define STEP_VAR_MI stepmi
#define RAYCAST_STEP(STEP_VAR)                                          \
voxval = vecread(Cube_data, vecposition(vpos, global.data_size));       \
if (voxval == 1) {                                                      \
  ++STEP_VAR;                                                           \
} else {                                                                \
  if (voxval == 255)                                                    \
    STEP_VAR /= 2;                                                      \
  break;                                                                \
}
void raycastSRG(const uint8_t id, std::vector<unsigned char> &Cube_data) {
  //RAYCAST: read elements in maximum fac(timestep)*2 = 3*2*1*2 = 6*2 = 12 directions doing 3 timesteps
  //          every time the maximum (if feasible) is taken and a thread is started from the combinations of all
  std::array<bool,3> ar_axistodo = {true, true, true};
  uint8_t i;
  int32_t STEP_VAR_PL;
  int32_t STEP_VAR_MI;
  Int3 step = {0, 0, 0};
  const Int3 vox_pos = global.threads[id].qu_work.front();
  global.threads[id].qu_work.pop();                           //queue is now empty
  //assert (global.threads[id].qu_work.empty() == true);
  Int3 vposit = vox_pos;
  Int3 vpos;
  Axis ax;                                                    //ax is set in maxabsAxis()
  unsigned char voxval = 0;
  for (uint8_t freeDir = 3; freeDir > 0; --freeDir) {
    i = 0;
    if (ar_axistodo[i] == true) {
      steppl = 0; stepmi = 0;
      for (vpos = Int3(vposit.x+1,vposit.y,vposit.z); vpos.x < global.data_size.x; ++vpos.x) {
        RAYCAST_STEP(STEP_VAR_PL)
      }
      for (vpos = Int3(vposit.x-1,vposit.y,vposit.z); vpos.x >= 0; --vpos.x) {
        RAYCAST_STEP(STEP_VAR_MI)
      }
      step.x = raycastStepWrite(steppl, stepmi);
    }
    ++i;
    if (ar_axistodo[i] == true) {
      steppl = 0; stepmi = 0;
      for (vpos = Int3(vposit.x,vposit.y+1,vposit.z); vpos.y < global.data_size.y; ++vpos.y) {
        RAYCAST_STEP(STEP_VAR_PL)
      }
      for (vpos = Int3(vposit.x,vposit.y-1,vposit.z); vpos.y >= 0; --vpos.y) {
        RAYCAST_STEP(STEP_VAR_MI)
      }
      step.y = raycastStepWrite(steppl, stepmi);
    }
    ++i;
    if (ar_axistodo[i] == true) {
      steppl = 0; stepmi = 0;
      for (vpos = Int3(vposit.x,vposit.y,vposit.z+1); vpos.z < global.data_size.z; ++vpos.z) {
        RAYCAST_STEP(STEP_VAR_PL)
      }
      for (vpos = Int3(vposit.x,vposit.y,vposit.z-1); vpos.z >= 0; --vpos.z) {
        RAYCAST_STEP(STEP_VAR_MI)
      }
      step.z = raycastStepWrite(steppl, stepmi);
    }
    step.maxabsAxis(ar_axistodo, ax);                         //get maximum axis element from step
    switch (ax) {
      case Axis::x:                                          { vposit.x += step.x; break; }
      case Axis::y:                                          { vposit.y += step.y; break; }
      case Axis::z:                                          { vposit.z += step.z; break; }
      case Axis::none:                                       { break; }                                                      
    }
    if (ax != Axis::none) {
      ar_axistodo[ax] = false; ax = Axis::none;
    }
  }
  if ( vposit != vox_pos ) {
    global.threads[id].qu_work.push(vposit);
    threadSRG(id, Cube_data);
  } else {
    deactivateThread(id);
  }
}
