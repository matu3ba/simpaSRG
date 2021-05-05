//FILE main.cpp

//CODING GUIDELINE
//queue -> qu_
//array -> ar_
//variable -> {s,d,ui,uch}Var for string, double, uint32_t,uchar
//types always before variable and combinations left scoped
//CL_Class, structs ST_Struct and in use Class,Struct

//#include <cstdint>
#include <chrono>
#include "mhd.h"
#include "Point3.h"
#include "simpleSRG.h"
#include "parallelSRG.h"

#include "seedFinder.h"  //get some initial seed values

int main(int argc, char *argv[]) {
  uint8_t cnt_cores = 0; //default auto
  if (argc == 2)
    cnt_cores = (uint8_t)atoi(argv[1]);

  ST_Mhd Mhd = ST_Mhd();
  int res = 0;
  std::string datafolder = "../data/";
  std::string mhdfile = "Skeleton.mhd";
  //Int3 seedpoint = Int3(0,0,0);

  //Skeleton.img arbitrary chosen seepoint
  Int3 seedpoint = Int3(69,137,424);

  res = readMhdFile(datafolder + mhdfile, Mhd);
  if (res != 0)
    return 0;
  //res = validity(Mhd);
  //if (res != 0)
  //  return 0;

  //new RawFile (computation depends on Mhd and files in datafolder)
  std::string rawFileNew = "SkeletonNEW.img";

  auto start_alloc = std::chrono::steady_clock::now();
  Int3 sizes = Int3(Mhd.v_uiDimSize.second[0],
                    Mhd.v_uiDimSize.second[1],
                    Mhd.v_uiDimSize.second[2]);
  std::vector<unsigned char> Cube_data = std::vector<unsigned char>(sizes.x
                                                                  * sizes.y
                                                                  * sizes.z);
  auto end_alloc = std::chrono::steady_clock::now();

  /***********  READING RAW FILE  ***********/
  auto start_readRAW = std::chrono::steady_clock::now();
  res = readRawFile(datafolder, Cube_data, Mhd);
  auto end_readRAW = std::chrono::steady_clock::now();
  if (res != 0)
    return 0;
  std::cout << "size of data " << Cube_data.size()*sizeof(unsigned char)/1024/1024 << "MB\n";
  //std::cout << "sizeof(unsigend char): " << sizeof(unsigned char) << "\n";
  res = checkValidity(Cube_data, Mhd);
  if (res != 0)
    return 0;
  Mhd.sElementDataFile.second = rawFileNew;

  /***********  ALGORITHM  ***********/
  std::vector<Int3> sp = {};
  auto start_SRG = std::chrono::steady_clock::now();
  //res = findSeedPoints(sp, Cube_data, sizes);
  //res = simpleSRG(seedpoint, Cube_data, sizes);
  res = parallelSRG(cnt_cores, seedpoint, Cube_data, sizes);
  auto end_SRG = std::chrono::steady_clock::now();
  if (res != 0)
    return 0;


  /***********  WRITING RAW FILE  ***********/
  auto start_writeRAW = std::chrono::steady_clock::now();
  res = writeRawFile(datafolder, Cube_data, Mhd);
  auto end_writeRAW = std::chrono::steady_clock::now();
  if (res != 0)
    return 0;
  
  res = writeMhdFile(datafolder + "SkeletonNEW.mhd", Mhd);
  if (res != 0)
    return 0;

  /***********  EVALUATION  ***********/
  //int datasize = Mhd.v_uiDimSize.second[0] * Mhd.v_uiDimSize.second[1] * Mhd.v_uiDimSize.second[2];
  std::cout << "Elapsed time allocating: " << std::chrono::duration_cast<std::chrono::milliseconds>
    (end_alloc - start_alloc).count() << " ms\n";
  std::cout << "Elapsed time allocating: " << std::chrono::duration_cast<std::chrono::milliseconds>
    (end_readRAW - start_readRAW).count() << " ms\n";
  std::cout << "Elapsed time algorithm: " << std::chrono::duration_cast<std::chrono::milliseconds>
    (end_SRG - start_SRG).count() << " ms\n";
  std::cout << "Elapsed time writeRAW: " << std::chrono::duration_cast<std::chrono::milliseconds>
    (end_writeRAW - start_writeRAW).count() << " ms\n";

  return res;
}
