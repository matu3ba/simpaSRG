//FILE simpleSRG.h

#pragma once

#include "Point3.h"
#include "Return.h"
#include <vector>
//simpleSRG
Return SRG(uint8_t cnt_cores, const Int3 seedpoint, std::vector<unsigned char> &Cube_data, 
    Int3 &sizes);
