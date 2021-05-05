#include "../../src/mhd.h"
#include "../../src/Point3.h"
#include "../../src/commonSRG.h"
#include "example_creation.h"

  //Mhd.v_uiDimSize.second = { 1000, 1000, 1000, 1, 1 };  //1e9      -> 64e3ms/228e3ms
int main(int argc, char *argv[] ) {
  if (argc != 2 && argc != 5) {
    std::cout << "argc == 2, 5(Helix), argc = " << argc << "\n"
      << "argv[0] = program name\n"
      << "argv[1] = size\n"
      << "argv[2] = Helix: helixmin\n"
      << "argv[3] = Helix: helixstep\n"
      << "argv[4] = Helix: helixmax\n";
    return -1;
  }

  std::filesystem::path datafolder = "../data";
  ST_Mhd Mhd = ST_Mhd();
  int res = 0;
  
  Mhd.sObjectType.second = std::string("Image");
  Mhd.uchNDims.second = 5;
  uint64_t dimSize = (uint64_t)atoi(argv[1]);

  Mhd.v_uiDimSize.second = { dimSize, dimSize, dimSize, 1, 1 };     //216e6    -> 5.5s    DEBUG
  //Mhd.v_uiDimSize.second = { 600, 600, 600, 1, 1 };     //216e6    -> 5.5s
  //Mhd.v_uiDimSize.second = { 1000, 1000, 1000, 1, 1 };  //1e9      -> 64e3ms/228e3ms
  Mhd.v_dElementSize.second = { 0.14, 0.14, 0.14, 1, 0 };
  Mhd.sElementType.second = "MET_UCHAR";
  Mhd.v_uiOffset.second = { 0, 0, 0 };
  Mhd.v_uiRotation.second = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };           //visualization program needs this
  Mhd.sElementDataFile.second = "CubeFull.img";
  Int3 sizes = Int3(Mhd.v_uiDimSize.second[0],
                    Mhd.v_uiDimSize.second[1],
                    Mhd.v_uiDimSize.second[2]);
  std::vector<unsigned char> Cube_data(sizes.x * sizes.y * sizes.z);

  res = checkValidity(Cube_data, Mhd);
  if (res != 0)
    return 0;
  if (argc == 2) {
    //Full Cube
    res = writeCube(datafolder, Mhd, Cube_data);

    //Full Cube validity check
    ST_Mhd Mhdcheck = ST_Mhd();
    res = readMhdFile(datafolder / "CubeFull.mhd", Mhdcheck, sizes);
    if (res != 0)
      return 0;
    res = readRawFile(datafolder, Cube_data, Mhdcheck);
    if (res != 0)
      return 0;
    res = checkValidity(Cube_data, Mhd);
    if (res != 0)
      return 0;
    for ( auto &data: Cube_data) {
      if (data != 1) {
        std::cout << "main: invalid data of example CubeFull.mhd\n";
        return -1;
      }
    }
    std::cout << "success creating and checking example: CubeFull\n";
    //example2 Ball with radius r=d/2

    //Ball/Sphere
    res = writeSphere(datafolder, Mhd, Cube_data, sizes);
    std::cout << "success creating example: SphereFull\n";
  }
  if (argc == 5) {
    //Helix
    int64_t min  = atoll(argv[2]);
    int64_t step = atoll(argv[3]);
    int64_t max  = atoll(argv[4]);
    res = writeHelix(datafolder, Mhd, Cube_data, sizes, min, step, max);
    std::cout << "success creating example: Helix\n";
  }
  std::cout << "success creating examples\n";
}
