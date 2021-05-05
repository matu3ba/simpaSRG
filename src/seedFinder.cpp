//FILE seedFinder.cpp

#include "seedFinder.h"
#include "commonSRG.h"
#include <iostream>

int findSeedPoint(std::vector<unsigned char> &Cube_data, const Int3 &sizes, 
    const uint8_t consecutivepoints, Int3 &seedpoint) {
  int cnt = 0; //count of consecutive points with value 1 (active and unmarked) in z-direction
  Int3 v = Int3(0,0,0);
  Int3 v_before = Int3(0,0,0);
  for(v.z=0; v.z<sizes.x; v.z++) {
    for(v.y=0; v.y<sizes.y; v.y++) {
      for(v.x=0; v.x<sizes.x; v.x++) {
        if(vecread(Cube_data, vecposition(v, sizes)) == 1) {
          if (v_before.z == v.z && v_before.y == v.y) { //x,y same as last step?
            ++cnt;
            if ( cnt == consecutivepoints ) { //5 consecutive points in x-direction reached
              seedpoint = v;
              goto foundpoint;
            }
          } else {
            cnt = 0;
          }
        } else {
          cnt = 0;
        }
        v_before = v;
      }
    }
  }
  std::cerr << "could not find " << consecutivepoints << " consecutive points\n";
  return -1;
  //, use next findable point\n";
  //  std::cerr << "could not find any seeding point\n";
foundpoint:
  std::cout << "seedpoint: "
    << seedpoint.x << " " << seedpoint.y << " " << seedpoint.z << "\n";
  return 0;
}
