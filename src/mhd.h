// FILE mhd.h

#pragma once

#include <iostream>     /* std::cout */
#include <fstream>      /* std::ifstream  */
#include <string>
#include <vector>
#include <cstdint>      /* defined int types */
#include <stdlib.h>     /* atof */
#include <filesystem>   /* c++17 filesystem */

#include <cstdio>       /* C-like file handling for proper byte writing */

#include "Return.h"     /* Return types */
#include "Point3.h"     /* 3D-point and vector operations for Voxels */

//.mhd FILE operations
//This implements only the subset of the specificitation as can be seen in the struct ST_Mhd.

struct ST_Mhd {
  ST_Mhd() :  sObjectType { std::make_pair("ObjectType","INVAL") },
                 uchNDims { std::make_pair("NDims",0) },
              v_uiDimSize { std::make_pair("DimSize",std::vector<uint64_t>()) },
           v_dElementSize { std::make_pair("ElementSize",std::vector<double>()) },
             sElementType { std::make_pair("ElementType","INVAL") },
               v_uiOffset { std::make_pair("Offset",std::vector<uint64_t>()) },
             v_uiRotation { std::make_pair("Rotation",std::vector<uint64_t>()) }, //OPTIONAL
         sElementDataFile { std::make_pair("ElementDataFile","INVAL") } {}
  std::pair <std::string,std::string> sObjectType;
  std::pair <std::string,uint16_t> uchNDims;
  std::pair <std::string,std::vector<uint64_t>> v_uiDimSize;
  std::pair <std::string,std::vector<double>> v_dElementSize;
  std::pair <std::string,std::string> sElementType;
  std::pair <std::string,std::vector<uint64_t>> v_uiOffset;
  std::pair <std::string,std::vector<uint64_t>> v_uiRotation; //OPTIONAL
  std::pair <std::string,std::string> sElementDataFile;
};

std::ostream &operator << (std::ostream &os, const ST_Mhd &mhd);
Return readMhdFile(const std::filesystem::path &path, ST_Mhd &Mhd, Int3 &sizes);
Return writeMhdFile(const std::filesystem::path &path, ST_Mhd &Mhd);

Return checkValidity(std::vector<unsigned char> &Cube_data, ST_Mhd &Mhd);

Return readRawFile(const std::filesystem::path &datafolder,
    std::vector<unsigned char> &Cube_data, ST_Mhd &Mhd);
Return writeRawFile(const std::filesystem::path &datafolder,
    std::vector<unsigned char> &Cube_data, ST_Mhd &Mhd);
