//FILE commonSIMPA.h

//shared data structure and funcationality of simpa algorithms
#pragma once

#include "simpaSRG.h"
#include "commonSRG.h"
#include <queue>
#include <thread>
#include <atomic>
#include <vector>
#include <chrono>     //support for thread sleeping
#include <mutex>
#include <iostream>

//#include <cassert>  //DEBUG
//#include <cstdio>   //fast multithreading logging to file

/*
 * The algorithm simple parallel Seeded Region Growing(simpaSRG) is a parallelized
 * version of Seeded Region Growing(SRG).
 * SRG is fundamentally defined by the growing speed of the surface.
 * The idea is thus "to get away as fast as possible from the current running thread"
 * with the new thread.
 * Therefore every RAY_AFTER_COUNT a new thread is started (who activates itself,
 * when finished with its Region Growing).
 *
 * After benchmarking we settled for the naive parallelization with and without circular buffer.
 *
 * The Raycast versions remain for future interest in combination of GPUs
 */

//invariants constexpr > 0 !!!
constexpr uint16_t RAY_AFTER_COUNT{ 1000 };    //raycast: in each thread after COUNT limited
//enum Axis { x, y, z, };
enum State {
  active,
  deactive,
};
struct Threaddata {
  State state { State::deactive };            //state of each thread
  std::thread thread {};                      //thread ptr
#ifdef CIRCBUFFER
  CircBuffer<Int3> qu_work {};                //circular buffer as queue
#else
  std::queue<Int3> qu_work {};                //work queue for SRG
#endif
};
struct GlobalData {
  uint8_t cores {0};                          //used number of cores
  std::vector<Threaddata> threads {};
  std::atomic<uint8_t> freethreads_cnt {0};   //highest prio
  uint16_t raycast_after_cnt {0};             //raycast in all direciotns: after steps
  Int3 data_size {};                          //Voxel cube xyz size (fast access)
  std::mutex mut_threadstate {};              //mutex for threads.state and freethreads_cnt write
  //FILE *logfile {NULL};                       //logfile for all threads
};
GlobalData global;

[[gnu::always_inline]]
inline void deactivateThread(uint8_t id) {
  global.mut_threadstate.lock();
  global.threads[id].state = State::deactive;
  ++global.freethreads_cnt;                   //access order does not matter -> no guard needed
  global.mut_threadstate.unlock();
}
[[gnu::always_inline]]
inline void activateThread(uint8_t id) {
  global.mut_threadstate.lock();
  --global.freethreads_cnt;                   //only call from main thread -> no guard needed
  global.threads[id].state = State::active;
  global.mut_threadstate.unlock();
}

[[gnu::always_inline]]
inline State trystart_thread(Int3 &vox_pos, std::vector<unsigned char> &Cube_data) {
  uint8_t t = 0;
  global.mut_threadstate.lock();              //guard for exclusive check and write (never count <0)
  if (global.freethreads_cnt > 0) {
    --global.freethreads_cnt;
    for (t=0; t<global.threads.size(); ++t) {
      if (global.threads[t].state == State::deactive) {
        global.threads[t].state = State::active;
        break;
      }
    }
    global.mut_threadstate.unlock();
    global.threads[t].qu_work.push(vox_pos);
#ifdef NAIVE
    global.threads[t].thread = std::thread{ threadSRG, t, std::ref(Cube_data) };
#else
    global.threads[t].thread = std::thread{ raycastSRG, t, std::ref(Cube_data) };
#endif
    global.threads[t].thread.detach();
    return State::active;
  } else {
    global.mut_threadstate.unlock();
    return State::deactive;
  }
}

Return SRG(uint8_t cnt_cores, const Int3 seedpoint, std::vector<unsigned char> &Cube_data, 
    Int3 &sizes) {
  if (cnt_cores == 0)
    global.freethreads_cnt = std::thread::hardware_concurrency(); //incl hyperthreading threads
  else
    global.freethreads_cnt = cnt_cores;
  global.cores = global.freethreads_cnt;
  global.threads.resize(global.freethreads_cnt);
  global.raycast_after_cnt = RAY_AFTER_COUNT;
  global.data_size = sizes;

#ifdef CIRCBUFFER
  uint32_t min = sizes.minEl();
  uint32_t med = sizes.medEl();
  min += 2;
  med += 2;
  uint32_t circBufsize = min*med;         //TODO justify boundary
  for (uint8_t i=0; i<cnt_cores; ++i) {
    global.threads[i].qu_work.init(circBufsize);
  }
#endif

  //init first worker thread who calls other free threads(dependend on ALU cores)
  global.threads[0].qu_work.push(seedpoint);
  activateThread(0);
  global.threads[0].thread = 
        std::thread{ threadSRG, 0, std::ref(Cube_data) };
  global.threads[0].thread.detach();

  //wait in master thread for worker threads to finish and deactivate itself
  while (global.freethreads_cnt < global.cores)
    std::this_thread::sleep_for(std::chrono::microseconds(100));

  //spin for 500ms, so the other thread finishes and SLURM does not break things
  //auto t1 = std::chrono::system_clock::now().time_since_epoch();
  //while ( (std::chrono::duration_cast<std::chrono::milliseconds>
  //            (std::chrono::high_resolution_clock::now().time_since_epoch() - t1) )
  //            < std::chrono::milliseconds(500)
  //     ) {
  //  std::this_thread::yield();
  //}

  //return to main() for writing data
  return Return::ok;
}

#ifdef NAIVE
#ifdef CHECKWRITEVOX
  #error "simpaSRG_NAIVE wrong includes"
#endif
#define CHECKWRITEVOX                                                 \
vecpos = vecposition(vpos, global.data_size);                         \
if (vecread(Cube_data, vecpos) == 1) {                                \
  if (raycast_countdown % 64 == 0) {                                  \
    vox_pos_old -= vox_pos;                                           \
    uint16_t sumdist = std::abs(vox_pos_old.x)                        \
                      +std::abs(vox_pos_old.y)                        \
                      +std::abs(vox_pos_old.z);                       \
    if (sumdist <= 16)                                                \
      distsingle += 1;                                                \
  }                                                                   \
  if (raycast_countdown == 0) {                                       \
    raycast_countdown = global.raycast_after_cnt;                     \
    if (distsingle < 16) {                                            \
      if ( trystart_thread(vox_pos, Cube_data) == State::deactive ) { \
        vecwrite(Cube_data, 255, vecpos);                             \
        global.threads[id].qu_work.push(vpos);                        \
        --raycast_countdown;                                          \
      }                                                               \
    } else {                                                          \
      vecwrite(Cube_data, 255, vecpos);                               \
      global.threads[id].qu_work.push(vpos);                          \
      --raycast_countdown;                                            \
    }                                                                 \
    distsingle = 0;                                                   \
  } else {                                                            \
    vecwrite(Cube_data, 255, vecpos);                                 \
    global.threads[id].qu_work.push(vpos);                            \
    --raycast_countdown;                                              \
  }                                                                   \
}
#else                     //cases RAY and ERC (no logical or available)
#ifndef SIMPLE            //rule out SIMPLE
#ifdef CHECKWRITEVOX
  #error "simpaSRG_ERC wrong includes"
#endif
#define CHECKWRITEVOX                                               \
vecpos = vecposition(vpos, global.data_size);                       \
if (vecread(Cube_data, vecpos) == 1) {                              \
  if (raycast_countdown == 0) {                                     \
    raycast_countdown = global.raycast_after_cnt;                   \
    trystart_thread(vox_pos, Cube_data);                            \
  }                                                                 \
  vecwrite(Cube_data, 255, vecpos);                                 \
  global.threads[id].qu_work.push(vpos);                            \
  --raycast_countdown;                                              \
}
#endif
#endif

void threadSRG(uint8_t id, std::vector<unsigned char> &Cube_data) {
  uint16_t raycast_countdown = global.raycast_after_cnt;
  Int3 vox_pos = global.threads[id].qu_work.front();
  Int3 vox_pos_old = vox_pos;
  uint16_t distsingle = 0;
  int32_t vecpos = vecposition(vox_pos, global.data_size);
  vecwrite(Cube_data, 255, vecpos);                  //writing initial element
  while (global.threads[id].qu_work.empty() == false) {
    vox_pos = global.threads[id].qu_work.front();
    global.threads[id].qu_work.pop();
    Int3 vpos;

    if (vox_pos.x+1 < global.data_size.x) {
      vpos = Int3(vox_pos.x+1,vox_pos.y,vox_pos.z);
      CHECKWRITEVOX
    }
    if (vox_pos.x-1 >= 0) {
      vpos = Int3(vox_pos.x-1,vox_pos.y,vox_pos.z);
      CHECKWRITEVOX
    }
    if (vox_pos.y+1 < global.data_size.y) {
      vpos = Int3(vox_pos.x,vox_pos.y+1,vox_pos.z);
      CHECKWRITEVOX
    }
    if (vox_pos.y-1 >= 0) {
      vpos = Int3(vox_pos.x,vox_pos.y-1,vox_pos.z);
      CHECKWRITEVOX
    }
    if (vox_pos.z+1 < global.data_size.z) {
      vpos = Int3(vox_pos.x,vox_pos.y,vox_pos.z+1);
      CHECKWRITEVOX
    }
    if (vox_pos.z-1 >= 0) {
      vpos = Int3(vox_pos.x,vox_pos.y,vox_pos.z-1);
      CHECKWRITEVOX
    }
    vox_pos_old = vox_pos;
  }
  deactivateThread(id);
}
