//FILE Return.cpp
#include "Return.h"

#include <iostream>

Return returnPrint(Return returnCode) {
  std::cerr << returnCode << "\n";
  std::cout << Return::ok << "\tok, all good\n";
  std::cout << Return::invalidpath1 << "\tmhd: no file path given\n";
  std::cout << Return::invalidpath2 << "mhd: no regular file\n";
  std::cout << Return::invalidpath3 << "mhd: path contains not of extension and filename\n";
  std::cout << Return::invalidpath4 << "mhd: extension != .mhd\n";
  std::cout << Return::invalidpath5 << "mhd: .mhd file is not in folder data/\n";
  std::cout << Return::mhd_inputfile << "\tmhd: could not open input mhd file\n";
  std::cout << Return::mhd_invalid_format << "\tmhd: invalid format\n";
  std::cout << Return::mhd_invalid_mhdfile1 <<"-" << mhd_invalid_mhdfile2 
            <<"\tinvalid mhd file format\n";
  std::cout << Return::mhd_dimensionlower3 << "\tmhd: dimension < 3\n";
  std::cout << Return::mhd_invalid_mhdfile3 <<"-" << mhd_invalid_mhdfile6 
            <<"\tinvalid mhd file format\n";
  std::cout << Return::raw_inputfile << "\tcould not open raw input file\n";
  std::cout << Return::mhd_size_unequal << "\tmhd and raw size do not match\n";
  std::cout << Return::wrong_initialization << "\terror in algorithm\n";
  std::cout << Return::raw_outputfile << "\tcould not open raw output file\n";
  std::cout << Return::mhd_outputfile << "\tmhd: could not open output file\n";
  //log broken
  return returnCode;
}
