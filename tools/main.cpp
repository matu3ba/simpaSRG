//FILE main.cpp for all input and output

#ifdef SIMPA
#include "../src/simpaSRG.h"
#endif
#ifdef SIMPLE
#include "../src/simpleSRG.h"
#endif

#include "../src/mhd.h"
#include "../src/seedFinder.h"
#include <chrono>                   //time measurement
#include <iostream>                 //io streaming
#include <fstream>                  //file streaming

void errorInput(int argc) {
  std::cout << "argc == 5, argc = " << argc << "\n"
    << "argv[0] =  executable name\n"
    << "argv[1] == mhdpath\n"
    << "argv[2] == numberofcores\n"
    << "argv[3] == numberofconsecutive points\n"
    << "argv[4] == writing output[y/n]\n"
    << "argv[5] == count of repeatings\n"
    << "argv[6] == (optional) compare mhd for equality\n";
    //<< "invalid argv[3..5] = seepoint(x,y,z)\n";
}

int main(int argc, char *argv[]) {
  if (argc != 6 && argc != 7) {
    errorInput(argc);
    return -1;
  }
  std::string outputwrite = std::string(argv[4]);
  if (outputwrite != "y" && outputwrite != "n") {
    std::cout << "arg[4] == [y/n], arg[4] = " << outputwrite << "\n";
    errorInput(argc);
    return -1;
  }
  //INPUT
  const std::filesystem::path programname = argv[0]; //simpleSRG/simpaSRG_RAY/_ERC/_NAIV(cib)
  const std::filesystem::path mhdpath = argv[1];
  uint8_t cnt_cores = (uint8_t)atoi(argv[2]);                 //todo later overflow check
  uint8_t cnt_consecutivepts = (uint8_t)atoi(argv[3]);        //todo later overflow check
  uint8_t cnt_repeatings = (uint8_t)atoi(argv[5]);            //todo later overflow check
  ST_Mhd Mhd = ST_Mhd();
  int res = 0;

  Int3 sizes = Int3(0,0,0);
  //std::cout << mhdpath << "\n";
  res = readMhdFile(mhdpath, Mhd, sizes);
  if (res != 0)
    return 0;
  int64_t total = static_cast<int64_t>(sizes.x)*sizes.y*sizes.z;
  if ( total > INT32_MAX || total < 0) {
    std::cout << "file size < " << INT32_MAX << ", file size = total\n";
    return 0;
  }
  std::cout << "sizes: " << total << "\n";
  std::vector<unsigned char> Cube_dataIN = std::vector<unsigned char>(total);
  res = readRawFile(mhdpath.parent_path(), Cube_dataIN, Mhd);
  if (res != 0)
    return 0;
  res = checkValidity(Cube_dataIN, Mhd);
  if (res != 0)
    return 0;
  Int3 seedpoint = Int3();
  res = findSeedPoint(Cube_dataIN, sizes, cnt_consecutivepts, seedpoint);
  if (res != 0)
    return 0;

  //Time measure: programname, mhdpath.filename, datasize[MB], time[us]
  std::ofstream timefile;
  //timefile.open("timings.txt", std::ios::out | std::ios::app); //remove ios::app to overwrite
  timefile.open(programname.filename().string() 
          + "_" + mhdpath.stem().string()
          + "_" + std::to_string(total)
          + "_" + std::to_string(cnt_cores), std::ios::out | std::ios::app);

  std::cout << "repeat " << std::to_string(cnt_repeatings) << " times\n";
  //OUTPUTPATH
  const std::filesystem::path mhd_outfile = mhdpath.stem().string() + "_"
                                          + programname.filename().string() 
                                          + std::to_string(cnt_cores) + ".mhd";
  const std::filesystem::path img_outfile = mhdpath.stem().string() + "_"
                                          + programname.filename().string() 
                                          + std::to_string(cnt_cores) + ".img";
  Mhd.sElementDataFile.second = img_outfile.string();
  // Loading data for 7. Comparison with correct result
  std::filesystem::path mhdpathCMP;
  ST_Mhd MhdCMP = {};
  std::vector<unsigned char> Cube_dataCMP = {};
  Int3 sizesCMP = Int3(0,0,0);
  std::ofstream corrfile;
  if ( argc == 7 ) {
    mhdpathCMP = argv[6];
    MhdCMP = ST_Mhd();
    res = readMhdFile(mhdpathCMP, MhdCMP, sizesCMP);
    if (res != 0) {
      std::cout << "MhdCMP reading errornous\n";
      return 0;
    }

    int64_t totalCMP = static_cast<int64_t>(sizesCMP.x)*sizesCMP.y*sizesCMP.z;
    if ( totalCMP > INT32_MAX || totalCMP < 0) {
      std::cerr << "CMPfile size < " << INT32_MAX << ", CMPfile size = " 
                << std::to_string(totalCMP) << "\n";
      return 0;
    }
    Cube_dataCMP = std::vector<unsigned char>(totalCMP);
    res = readRawFile(mhdpathCMP.parent_path(), Cube_dataCMP, MhdCMP);
    if (res != 0) {
      return 0;
    }
    res = checkValidity(Cube_dataCMP, MhdCMP);
    if (res != 0) {
      return 0;
    }
    corrfile.open("correctness.txt", std::ios::out | std::ios::app);
  }
  std::chrono::steady_clock::time_point start_SRG;
  std::chrono::steady_clock::time_point end_SRG;
  std::vector<unsigned char> Cube_data = {};
  for (uint8_t i=0; i<cnt_repeatings; ++i) {
    //std::copy(Cube_dataIN.begin(), Cube_dataIN.end(), Cube_data);
    Cube_data = Cube_dataIN;

    start_SRG = std::chrono::steady_clock::now();
    res = SRG(cnt_cores, seedpoint, Cube_data, sizes);
    end_SRG = std::chrono::steady_clock::now();
    if (res != 0)
      return 0;
    std::cout << "Elapsed time algo: " << std::chrono::duration_cast<std::chrono::microseconds>
      (end_SRG - start_SRG).count() << " us\n";

    timefile << programname.filename().string()
             << "," << mhdpath.stem().string()
             << "," << std::to_string(total)
             << "," << std::to_string(cnt_cores)
             << "," << std::chrono::duration_cast<std::chrono::microseconds>
                       (end_SRG - start_SRG).count()
             << "\n";

    //COMPARISON to single core simple result
    if ( argc == 7 ) {
      if (Cube_dataCMP.size() == Cube_data.size()) {
        //SIMD instructions
        if (std::equal(Cube_data.begin(), Cube_data.end(), Cube_dataCMP.begin()) == true) {
          corrfile << mhd_outfile.string() << " " << programname.string()
                  << "and simpleSRG  EQUAL\n";
          std::cout << programname.string() << " solution and simpleSRG solution     equal\n";
            //<< mhdpath << " " << mhdpathCMP << " equal\n";
        } else {
          corrfile << "Data of" << mhdpath.stem().string() << "and" 
                                << mhd_outfile.string() << "UNEQUAL\n";
          std::cout << programname.string() 
                    << " solution and simpleSRG solution   --- UNEQUAL ---\n";
        }
      } else {
        corrfile << "Data of" << mhdpath.stem().string() << "and" 
                                << mhd_outfile.string() << "UNEQUAL in size\n";
        std::cout << programname.string() 
                    << " solution and simpleSRG solution   --- unequal in size ----\n";
      }
    }
  }
  // Closing file descriptor of 7. Comparison with correct result
  if ( argc == 7 ) {
    corrfile.close();
  }

  //OUTPUT
  if (outputwrite == "y") {
    res = writeRawFile(mhdpath.parent_path(), Cube_data, Mhd);
    if (res != 0)
      return 0;
    res = writeMhdFile(mhdpath.parent_path() / mhd_outfile, Mhd);
    if (res != 0)
      return 0;
  }
  //Time measure
  timefile.close();
  return 0;
}
