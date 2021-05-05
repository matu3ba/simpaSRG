//FILE main.cpp in test/seedFinder

#include "../../src/mhd.h"
#include "../../src/seedFinder.h"
#include <filesystem>

int main(int argc, char *argv[] ) {
  if (argc != 2) {
    std::cout << "argc == 2, argc = " << argc << "argv == mhdpath\n";
    return -1;
  }
  std::filesystem::path mhdpath = argv[1];
  ST_Mhd Mhd = ST_Mhd();
  int res = 0;
  Int3 sizes = Int3(0,0,0);
  res = readMhdFile(mhdpath, Mhd, sizes);
  if (res != 0)
    return 0;
  std::vector<unsigned char> Cube_data = std::vector<unsigned char>(sizes.x * sizes.y * sizes.z);
  res = readRawFile(mhdpath.parent_path(), Cube_data, Mhd);
  if (res != 0)
    return 0;
  res = checkValidity(Cube_data, Mhd);
  if (res != 0)
    return 0;
  std::vector<Int3> sp = {};
  const uint8_t cnt_consecutivepoints = 5;
  res = findSeedPoints(Cube_data, sizes, cnt_consecutivepoints, sp);
  //seedpoints were found and streamed to stdout, so we finish
  if (res != 0)
    return 0;
  return 0;
}
