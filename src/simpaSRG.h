//FILE simpaSRG.h

#pragma once

#include "Point3.h"
#include "Return.h"
#include <vector>
//simpaSRG
Return SRG(uint8_t cnt_cores, const Int3 seedpoint, std::vector<unsigned char> &Cube_data, 
    Int3 &sizes);
void threadSRG(uint8_t id, std::vector<unsigned char> &Cube_data);
#ifndef NAIVE
void raycastSRG(uint8_t id, std::vector<unsigned char> &Cube_data);
#endif
