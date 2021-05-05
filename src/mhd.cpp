//FILE mhd.cpp
#include "mhd.h"

#include <boost/algorithm/string.hpp> 
#include <iterator> //for copy iterator

//ostream writes random ^M on carriage return, which invalidates up number conversion
//C FILE API was used to read/write files due to this behavior

std::ostream &operator << (std::ostream &os, const ST_Mhd &mhd) {
  os << mhd.sObjectType.first << "=" << mhd.sObjectType.second << "\n"
     << mhd.uchNDims.first << "=" << (mhd.uchNDims).second << "\n"
     << mhd.v_uiDimSize.first << "=";
  for (auto& i : mhd.v_uiDimSize.second)
    os << i << " ";
  os << "\n";
  os << mhd.v_dElementSize.first << "=";
  for (auto& i : mhd.v_dElementSize.second)
    os << i << " ";
  os << "\n";
  os << mhd.sElementType.first << "=" << mhd.sElementType.second << "\n"
     << mhd.v_uiOffset.first << "=";
  for (auto& i : mhd.v_uiOffset.second)
    os << i << " ";
  os << "\n"
     << mhd.sElementDataFile.first << "=" << mhd.sElementDataFile.second << "\n";
  return os;
}
Return checkPath(const std::filesystem::path &mhdpath, std::string extension) {
  if (!is_regular_file(mhdpath)) {
    return Return::invalidpath2;
  }
  if ( (mhdpath.has_stem() == false) || (mhdpath.has_extension() == false) ) {
    return Return::invalidpath3;
  }
  if ( mhdpath.extension() != extension ) {
    return Return::invalidpath4;
  }
  if ( mhdpath.parent_path().filename() != "data" ) {
    return Return::invalidpath5;
  }
  return Return::ok;
}

Return getFileContent(const std::filesystem::path &mhdpath, 
                          std::vector<std::string> & vec_mhd) {
  Return status = Return::ok;
  status = checkPath(mhdpath, ".mhd");
  if ( status != Return::ok )
    return status;
  std::ifstream in(mhdpath.c_str());
  if (!in) {
    std::cerr << "cannot open file: " << mhdpath.string() << "\n";
    return Return::mhd_inputfile;
  }
  std::string str;
  while(std::getline(in, str)) {
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
    vec_mhd.push_back(str);
  }
  in.close();
  return Return::ok;
}

// read MhdFile in path to struct Mhd and return sizes of 3D Cube
Return readMhdFile(const std::filesystem::path &path, ST_Mhd &Mhd, Int3 &sizes) {
  const size_t max_len = 100;
  std::vector<std::string> spl_line;
  std::string stmp;
  std::vector<std::string> vec_mhd;

  Return status = Return::ok;
  status = getFileContent(path, vec_mhd);
  if ( status != Return::ok )
    return returnPrint(status);

  int32_t i = 0;
  if( vec_mhd[i].compare(0, Mhd.sObjectType.first.length(), Mhd.sObjectType.first) != 0 ) {
    return returnPrint(Return::mhd_invalid_format);
  } else {
    //std::cout << vec_mhd[i].substr(Mhd.sObjectType.first.length()+1, max_len) << "\n";
    // we dont want the '=' in the string
    stmp = vec_mhd[i].substr(Mhd.sObjectType.first.length()+1, max_len); 
    Mhd.sObjectType.second = stmp;
  }
  ++i;
  if( vec_mhd[i].compare(0, Mhd.uchNDims.first.length(), Mhd.uchNDims.first) != 0 ) {
    return returnPrint(Return::mhd_invalid_mhdfile1);
  } else {
    stmp = vec_mhd[i].substr(Mhd.uchNDims.first.length()+1, max_len);
    //base means hex etc values
    Mhd.uchNDims.second = static_cast<uint16_t>(std::stoul(stmp,nullptr,0)); 
  }
  ++i;
  if( vec_mhd[i].compare(0, Mhd.v_uiDimSize.first.length(), Mhd.v_uiDimSize.first) != 0 ) {
    return returnPrint(Return::mhd_invalid_mhdfile2);
  } else {
    stmp = vec_mhd[i].substr(Mhd.v_uiDimSize.first.length()+1, max_len);
    boost::split(spl_line, stmp, boost::is_any_of(" "));
    for (auto& el: spl_line) {
      //std::cout << el << "\n";
      if (el.length() > 0 && el != " ") //x" "" " may result in " " or emptystring
        Mhd.v_uiDimSize.second.push_back( static_cast<uint64_t>(std::stoul(el,nullptr,0)) ); 
    }
    if ( Mhd.v_uiDimSize.second.size() < 3 ) //less than 3 dimensions
      return returnPrint(Return::mhd_dimensionlower3);
  }
  ++i;
  if( vec_mhd[i].compare(0, Mhd.v_dElementSize.first.length(), Mhd.v_dElementSize.first) != 0 ) {
    return returnPrint(Return::mhd_invalid_mhdfile3);
  } else {
    stmp = vec_mhd[i].substr(Mhd.v_dElementSize.first.length()+1, max_len);
    boost::split(spl_line, stmp, boost::is_any_of(" "));
    for (auto& el: spl_line) {
      //std::cout << el << "\n";
      if (el.length() > 0 && el != " ") //x" "" " may result in " " or emptystring
        Mhd.v_dElementSize.second.push_back( std::stod(el,nullptr) ); //base means hex etc values
    }
  }
  ++i;
  if( vec_mhd[i].compare(0, Mhd.sElementType.first.length(), Mhd.sElementType.first) != 0 ) {
    return returnPrint(Return::mhd_invalid_mhdfile4);
  } else {
    // we dont want the '=' in the string
    stmp = vec_mhd[i].substr(Mhd.sElementType.first.length()+1, max_len); 
    Mhd.sElementType.second = stmp;
  }
  ++i;
  if( vec_mhd[i].compare(0, Mhd.v_uiOffset.first.length(), Mhd.v_uiOffset.first) != 0 ) {
    return returnPrint(Return::mhd_invalid_mhdfile5);
  } else {
    stmp = vec_mhd[i].substr(Mhd.v_uiOffset.first.length()+1, max_len);
    boost::split(spl_line, stmp, boost::is_any_of(" "));
    for (auto& el: spl_line) {
      //std::cout << el << "\n";
      if (el.length() > 0 && el != " ") //x" "" " may result in " " or emptystring
        Mhd.v_uiOffset.second.push_back( static_cast<uint64_t>(std::stoul(el,nullptr,0)) ); 
    }
  }
  ++i;
  if( vec_mhd[i].compare(0, Mhd.v_uiRotation.first.length(), Mhd.v_uiRotation.first) != 0 ) {
    //FALLTHROUGH (no field Rotation)
  } else {
    // we dont want the '=' in the string
    stmp = vec_mhd[i].substr(Mhd.v_uiRotation.first.length()+1, max_len); 
    boost::split(spl_line, stmp, boost::is_any_of(" "));
    for (auto& el: spl_line) {
      //std::cout << el << "\n";
      if (el.length() > 0 && el != " ") //x" "" " may result in " " or emptystring
        Mhd.v_uiRotation.second.push_back( static_cast<uint64_t>(std::stoul(el,nullptr,0)) ); 
    }
    ++i;
  }
  if( vec_mhd[i].compare(0, Mhd.sElementDataFile.first.length(), Mhd.sElementDataFile.first) != 0 ) {
    return returnPrint(Return::mhd_invalid_mhdfile6);
  } else {
    // we dont want the '=' in the string
    stmp = vec_mhd[i].substr(Mhd.sElementDataFile.first.length()+1, max_len); 
    Mhd.sElementDataFile.second = stmp;
  }
  ++i;
  sizes.x = Mhd.v_uiDimSize.second[0];
  sizes.y = Mhd.v_uiDimSize.second[1];
  sizes.z = Mhd.v_uiDimSize.second[2];
  return Return::ok;
}
// write MhdFile to path from struct Mhd
// REMARK carriage return as ^M are appended on strings
Return writeMhdFile(const std::filesystem::path &path, ST_Mhd &Mhd) {
  std::FILE * mhdfile;
  mhdfile = std::fopen(path.c_str(), "wb");
  //binary mode would be .write()
  if (mhdfile == NULL) {
    std::cerr << "cannot write file: " << path << "\n";
    return returnPrint(Return::mhd_outputfile);
  }
  //std::cout << Mhd.sObjectType.first + "=" + Mhd.sObjectType.second << "\n"
  //  << Mhd.uchNDims.first + "=" << std::to_string((Mhd.uchNDims).second) + "\n";
  std::string s1 = Mhd.sObjectType.first + "=" + Mhd.sObjectType.second + "\n";
  s1.append ( Mhd.uchNDims.first + "=" );
  s1.append ( std::to_string((Mhd.uchNDims).second) + "\n" );
  s1.append ( Mhd.v_uiDimSize.first + "=" );
  for (uint32_t i=0; i<Mhd.v_uiDimSize.second.size(); ++i) {
    if ( i == Mhd.v_uiDimSize.second.size()-1 )
      s1.append ( std::to_string(Mhd.v_uiDimSize.second[i]) );
    else
      s1.append ( std::to_string(Mhd.v_uiDimSize.second[i]) + " " );
  }
  s1.append ( "\n" );
  s1.append ( Mhd.v_dElementSize.first + "=" );
  for (uint32_t i=0; i<Mhd.v_dElementSize.second.size(); ++i) {
    if ( i == Mhd.v_dElementSize.second.size()-1 )
      s1.append ( std::to_string(Mhd.v_dElementSize.second[i]) );
    else
      s1.append ( std::to_string(Mhd.v_dElementSize.second[i]) + " " );
  }
  s1.append ( "\n" );
  s1.append ( Mhd.sElementType.first + "=" + Mhd.sElementType.second + "\n" );
  s1.append ( Mhd.v_uiOffset.first + "=" );
  for (uint32_t i=0; i<Mhd.v_uiOffset.second.size(); ++i) {
    if ( i == Mhd.v_uiOffset.second.size()-1 )
      s1.append ( std::to_string(Mhd.v_uiOffset.second[i]) );
    else
      s1.append ( std::to_string(Mhd.v_uiOffset.second[i]) + " " );
  }
  s1.append ( "\n" );
  //OPTIONAL Rotation field exists
  if (Mhd.v_uiRotation.second.size() > 0) {
    s1.append ( Mhd.v_uiRotation.first + "=" );
    for (uint32_t i=0; i<Mhd.v_uiRotation.second.size(); ++i) {
      if ( i == Mhd.v_uiRotation.second.size()-1 )
        s1.append ( std::to_string(Mhd.v_uiRotation.second[i]) );
      else
        s1.append ( std::to_string(Mhd.v_uiRotation.second[i]) + " " );
    }
    s1.append ( "\n" );
  }
  s1.append( Mhd.sElementDataFile.first + "=" + Mhd.sElementDataFile.second + "\n" );
  //std::cout << s1 ;  
  //std::cout << s1.c_str();
  std::fwrite(s1.data(),sizeof(char),s1.length(),mhdfile);
  std::fclose(mhdfile);
  return Return::ok;
}
Return checkValidity(std::vector<unsigned char> &Cube_data, ST_Mhd &Mhd) {
  uint64_t size = Mhd.v_uiDimSize.second[0] 
                * Mhd.v_uiDimSize.second[1] 
                * Mhd.v_uiDimSize.second[2];
  if(Cube_data.size() != size) {
    std::cerr << "mhd size: " << size << " raw size: " 
      << Cube_data.size() << "\n";
    return returnPrint(Return::mhd_size_unequal);
  }
  return Return::ok;
}

// RawFile belongs to Mhd header and is defined by Mhd.sElementDataFile
// Cube_data contains the RawData as 3D Cube
Return readRawFile(const std::filesystem::path &datafolder, 
                      std::vector<unsigned char> &Cube_data, ST_Mhd &Mhd) {
  std::filesystem::path imgpath = datafolder;
  imgpath /= Mhd.sElementDataFile.second;
  Return status = Return::ok;
  status = checkPath(imgpath, ".img");
  if ( status != Return::ok ) {
    std::cerr << "invalid file path: " << imgpath << "\n";
    return returnPrint(Return::raw_inputfile);
  }
  //std::cout << "imgpath: " << imgpath << "\n";
  std::FILE * rawfile;
  rawfile = std::fopen(imgpath.c_str(), "rb");
  if (rawfile == NULL) {
    std::cerr << "cannot open file: " << Mhd.sElementDataFile.second << "\n";
    return returnPrint(Return::raw_inputfile);
  }
  std::size_t res = std::fread(Cube_data.data(),sizeof(unsigned char),Cube_data.size(),rawfile);
  if (res == 0)
    std::cerr << "nothing was read\n";
  //std::cout << res << " voxels(1B)    read\n";
  std::fclose(rawfile);
  return Return::ok;
}
// RawFile belongs to Mhd header and is defined by Mhd.sElementDataFile
// Cube_data contains the RawData as cube
Return writeRawFile(const std::filesystem::path &datafolder, 
          std::vector<unsigned char> &Cube_data, ST_Mhd &Mhd) {
  std::filesystem::path imgpath = datafolder;
  imgpath /= Mhd.sElementDataFile.second;
  std::FILE * rawfile;
  rawfile = std::fopen(imgpath.c_str(), "wb");
  if (rawfile == NULL) {
    std::cerr << "cannot write file: " << imgpath.string() << "\n";
    return returnPrint(Return::raw_outputfile);
  }
  std::size_t res = std::fwrite(Cube_data.data(),sizeof(unsigned char),Cube_data.size(),rawfile);
  if (res == 0)
    std::cerr << "nothing was written\n";
  //std::cout << res << " voxels(1B) written\n";
  std::fclose(rawfile);
  return Return::ok;
}
