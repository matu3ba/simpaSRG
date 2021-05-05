//FILE main.cpp for all input and output

#include "../../src/mhd.h"
#include <chrono>                   //time measurement
#include <iostream>                 //io streaming
#include <fstream>                  //file streaming

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "argc == 2, argc = " << argc << "\n"
      << "argv[0] =  executable name\n"
      << "argv[1] == mhdpath\n";
    return -1;
  }
  //INPUT
  const std::filesystem::path programname = argv[0]; //simpleSRG/simpaSRG_RAY/_ERC/_NAIV
  const std::filesystem::path mhdpath = argv[1];
  ST_Mhd Mhd = ST_Mhd();
  int res = 0;

  Int3 sizes = Int3(0,0,0);
  res = readMhdFile(mhdpath, Mhd, sizes);
  if (res != 0)
    return 0;
  int64_t total = static_cast<int64_t>(sizes.x)*sizes.y*sizes.z;
  if ( total > INT32_MAX || total < 0) {
    std::cout << "file size < " << INT32_MAX << ", file size = total\n";
    return 0;
  }
  std::vector<unsigned char> Cube_data = std::vector<unsigned char>(total);
  res = readRawFile(mhdpath.parent_path(), Cube_data, Mhd);
  if (res != 0)
    return 0;
  res = checkValidity(Cube_data, Mhd);
  if (res != 0)
    return 0;
  uint64_t cntMarkedVox = 0;
  for (auto &vox : Cube_data) {
    if (vox == 255)
      ++cntMarkedVox;
  }
  //programname, mhdpath.filename, datasize[MB], 
  std::ofstream timefile;
  timefile.open("markedVoxels.txt", std::ios::out | std::ios::app);
  timefile << sizes.string()
          << ", " << mhdpath.stem().string()
          << ", " << std::to_string(total)
          << ", " << std::to_string(cntMarkedVox)
          << "\n";
  timefile.close();
  return 0;
}
