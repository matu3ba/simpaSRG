//FILE seedFinder.h

#pragma once

#include "Point3.h"
#include <vector>

int findSeedPoint(std::vector<unsigned char> &Cube_data, const Int3 &sizes, 
    const uint8_t consecutivepoints, Int3 &seedpoint);
