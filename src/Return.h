//FILE Return.h

#pragma once

//Common return codes

enum Return {
  ok,
  invalidpath1,
  invalidpath2,
  invalidpath3,
  invalidpath4,
  invalidpath5,
  mhd_inputfile,
  mhd_invalid_format,
  mhd_invalid_mhdfile1,
  mhd_invalid_mhdfile2,
  mhd_dimensionlower3,
  mhd_invalid_mhdfile3,
  mhd_invalid_mhdfile4,
  mhd_invalid_mhdfile5,
  mhd_invalid_mhdfile6,
  raw_inputfile,
  mhd_size_unequal,
  wrong_initialization,
  raw_outputfile,
  mhd_outputfile,
  log_broken,
  dircomp_broken,
};
Return returnPrint(Return returnCode); //cout Return code and lookup range
