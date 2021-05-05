#include "../../src/mhd.h"
#include "../../src/Point3.h"
#include "../../src/commonSRG.h"
#include "example_creation.h"

#include <cmath>
//#include <vector>

//example creation with simplifications of given size for Cube_data
//1. Full Cube
//2. Full Sphere
//3. Helix

//check, if voxel with radius^2 inside sphere
inline uint64_t distance(Int3 ce, Int3 pt) 
{ 
    uint64_t x1 = std::pow((pt.x - ce.x), 2); 
    uint64_t y1 = std::pow((pt.y - ce.y), 2); 
    uint64_t z1 = std::pow((pt.z - ce.z), 2); 
  
    return (x1 + y1 + z1);  
}
int writeFiles(std::filesystem::path &datafolder, ST_Mhd &Mhd, 
        std::vector<unsigned char> &Cube_data, std::string &filename) {
  int res = 0;
  Mhd.sElementDataFile.second = filename + ".img";
  res = writeRawFile(datafolder, Cube_data, Mhd);
  if (res != 0)
    return 0;
  std::filesystem::path file = filename + ".mhd";
  res = writeMhdFile(datafolder / file, Mhd);
  return 0;
}
int writeCube(std::filesystem::path &datafolder, ST_Mhd &Mhd, 
              std::vector<unsigned char> &Cube_data) {
  std::string filename = "CubeFull";
  int res = 0;
  for ( auto &data: Cube_data) {
    data = 1; //mark all data as valid
  }
  res = writeFiles(datafolder, Mhd, Cube_data, filename);
  return res;
}
int writeSphere(std::filesystem::path &datafolder, ST_Mhd &Mhd, 
                std::vector<unsigned char> &Cube_data, Int3 &sizes) {
  std::string filename = "SphereFull";
  int res = 0;
  for ( auto &data: Cube_data) {
    data = 0; //mark all data as invalid
  }
  Int3 center = sizes/2;
  uint64_t dist = std::pow((sizes.x / 2), 2);
  Int3 pt = Int3(0,0,0);
  for (pt.x = 0; pt.x<sizes.x; pt.x++) {
    for (pt.y = 0; pt.y<sizes.y; pt.y++) {
      for (pt.z = 0; pt.z<sizes.z; pt.z++) {
        if (distance(center, pt) < dist) //inside sphere ?
          vecwrite(Cube_data, 1, vecposition(pt, sizes));
      }
    }
  }
  res = writeFiles(datafolder, Mhd, Cube_data, filename);
  return res;
}
void create_dilation_mask(int64_t dilatation, std::vector<Int3> &xzcirc) {
  //Dilatation definition as rectangle
  std::cout << "Dilatationdefinitions " << dilatation << "..\n";
  xzcirc = {};
  for (int64_t x=-dilatation; x<dilatation; ++x) {
    for (int64_t y=-dilatation; y<dilatation; ++y) {
      for (int64_t z=-dilatation; z<dilatation; ++z) {
        xzcirc.push_back( Int3( x, y, z ) );
      }
    }
  }
}
int writeHelix(std::filesystem::path &datafolder, ST_Mhd &Mhd, 
                std::vector<unsigned char> &Cube_data, Int3 &sizes,
                int64_t min, int64_t step, int64_t max ) { //__attribute__((unused))
  assert(max < 17);
  //INVARIANT size.x = size.y = size.z
  //INVARIANT enough space for dilatation size (around x,y,z)
  const int64_t dilatationmin = min;
  const int64_t dilatationstep = step;
  const int64_t dilatationmax = max;
  if (sizes.x != sizes.y && sizes.z != sizes.x) {
    std::cout << "sizes.x == sizes.y"
              << ", sizes.x = " << sizes.x 
              << ", sizes.y = " << sizes.y 
              << ", sizes.z = " << sizes.z << "\n";
    return 0;
  }
  if (sizes.x / 4 + dilatationmax >= sizes.x) {
    std::cout << "sizes.x / 4 + diltatemax >= sizes.x not fullfilled\n"
              << "change dilatation maximum or Cube sizes\n";
    return 0;
  }
  int res = 0;
  std::string filename = "Helix_D";
  for ( auto &data: Cube_data) {
    data = 0; //mark all data as invalid
  }

  //Helix arguments
  //i = 2pi*factor
  //dz > i/zfactor = 2pi*factor/size.x^2 rounded
  //double factor = 25 * sizes.x * sizes.x / 7; //ceil(pi) = 7
  double factor = sizes.x * sizes.x * sizes.x / 56; //ceil(pi) = 7, 200=>25, 8*7
  //double factor = 5 * sizes.x * sizes.x / 7; //ceil(pi) = 7
  double radius = sizes.x / 4;
  Int3 center  = Int3 ( sizes.x / 2, sizes.y / 2, sizes.z / 4 );
  int64_t helix_zsize = sizes.z / 2;
  int64_t len = sizes.x*sizes.y*helix_zsize;
  int64_t zfactor = len/helix_zsize;
  std::vector<Int3> helixTrack = std::vector<Int3>(len);

  //Helix simple track
  std::cout << "Helixsimpletrack..\n";
  for (int64_t i=0; i<len; ++i) {
    double angle = i / factor; //anglespeed DEBUG
    Int3 posVox = Int3( static_cast<int64_t>( radius*std::cos(angle) ) + center.x,
                        static_cast<int64_t>( radius*std::sin(angle) ) + center.y,
                                                i / zfactor + center.z);
    helixTrack[i] = posVox;
  }
  Int3 dirVec = Int3();
  Int3 oldVox = helixTrack[0];
  //Int3 oldb4Vox = helixTrack[0];

  //helixOrientation and continuity check
  for(uint64_t i=1; i<helixTrack.size(); ++i) {
    dirVec = helixTrack[i] - oldVox;
    if (dirVec.y > 1) {
      std::cerr << "i: " << i << "y jump, slow down helix anglespeed" << dirVec.string()
                << "oldVec: " << oldVox.string()
                << "curVec: " << helixTrack[i].string();
      return 0;
    }
    if (dirVec.x > 1) {
      std::cerr << "i: " << i << "x jump, slow down helix anglespeed" << dirVec.string()
                << "oldVec: " << oldVox.string()
                << "curVec: " << helixTrack[i].string();
      return 0;
    }
    oldVox = helixTrack[i];
  }
  std::cout << "max val posVox.z = " << helixTrack.back().z << "\n";
  std::cout << "queue size = " << helixTrack.size() << "\n";
  std::vector<Int3> xzcirc = {}; //circ coordinates for up to down right circ part
  int64_t dilatation;
  //uint64_t imax = 7*sizes.x/32;
  for (dilatation=dilatationmin; dilatation <= dilatationmax; dilatation += dilatationstep) {
    std::string filenameD = filename + std::to_string(dilatation);
    create_dilation_mask(dilatation, xzcirc);
    
    for(uint64_t i=1; i<helixTrack.size(); ++i) {
    //for(uint64_t i=1; i<imax; ++i) {
      for ( auto &cVox : xzcirc ) {
        for (int64_t z=-dilatation; z<dilatation; ++z) {
          Int3 shiftedVox = cVox+helixTrack[i];;
          vecwrite(Cube_data, 1, vecposition(shiftedVox, sizes));  //writing from right to left
          //xzcirc.push_back( Int3( x, y, z ) );
        }
      }
    }
    res = writeFiles(datafolder, Mhd, Cube_data, filenameD);
    if (res != 0)
      return 0;
    for ( auto &data: Cube_data) {
      data = 0; //mark all data as invalid for next dilatation
    }
  }
  return 0;
}
