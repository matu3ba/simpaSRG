#pragma once

#include "../../src/mhd.h"
#include "../../src/Point3.h"

int writeCube(std::filesystem::path &datafolder, ST_Mhd &Mhd, 
              std::vector<unsigned char> &Cube_data);
int writeSphere(std::filesystem::path &datafolder, ST_Mhd &Mhd, 
                std::vector<unsigned char> &Cube_data, Int3 &sizes);
int writeHelix(std::filesystem::path &datafolder, ST_Mhd &Mhd, 
                std::vector<unsigned char> &Cube_data, Int3 &sizes,
                int64_t min, int64_t step, int64_t max);
