//FILE Point3.h

#pragma once

#include <cstdint>      /* defined int types */
#include <string>
#include <cmath>        /* std::abs etc */
#include <queue>
#include <algorithm>    /* std::sort */
#include <cassert>       /* medEl()  */
//3D Voxel position + multiplication and vector operations
//int32_t for file sizes < 4GB Byte of any size

enum Axis {              /* only use in simpaSRG and Point3, but also available for simpleSRG */
  x=0,
  y=1,
  z=2,                  // checking algorithm setdirection
  none=100,
};
//template <typename NumericType>
class Int3 {
public:
	int32_t x;
	int32_t y;
	int32_t z;

	//Default constructor
	Int3() : x(0), y(0), z(0) {}

	//Constructor with an element for initialization
	explicit Int3(int32_t init) :x(init), y(init), z(init) {}

	//Constructor that takes the elements as input.
	Int3(int32_t x, int32_t y, int32_t z) : x(x), y(y), z(z) {}

  //Division operator for two Int3
  Int3 operator / (const Int3 &in2)
      { return Int3(x/in2.x, y/in2.y, z/in2.z); }
  //Vector addition operator for two Int3
  Int3 operator + (const Int3 &in2)
      { return Int3(x+in2.x, y+in2.y, z+in2.z); }
  //Vector subtraction operator for two Int3
  Int3 operator - (const Int3 &in2)
      { return Int3(x-in2.x, y-in2.y, z-in2.z); }
  //Division operator for Int3 and uint8_t
  Int3 operator / (const uint8_t &in2)
      { return Int3(x/in2, y/in2, z/in2); }
  bool operator == (const Int3& in2) {
       return (x == in2.x)
       && (y == in2.y)
       && (z == in2.z);
  }
  bool operator != (const Int3& in2) {
    return !operator==(in2);
  }
  [[gnu::always_inline]]
  inline void maxabsAxis(const std::array<bool,3> &ar_axistodo, Axis &axis) {
    int32_t maxabsEl = 0; axis = Axis::none;
    if (ar_axistodo[0] == true) {
      maxabsEl = x; axis = Axis::x;
    }
    if (ar_axistodo[1] == true) {
      if ( std::abs(y) > std::abs(maxabsEl) )      { maxabsEl = y; axis = Axis::y; }
    }
    if (ar_axistodo[2] == true) {
      if ( std::abs(z) > std::abs(maxabsEl) )      { maxabsEl = z; axis = Axis::z; }
    }
  }
  int32_t maxEl() {
    int32_t max = x;
    if (y>max)
      max = y;
    if (z>max)
      max = z;
    return max;
  }
  int32_t minEl() {
    int32_t min = x;
    if (y<min)
      min = y;
    if (z<min)
      min = z;
    return min;
  }
  int32_t medEl() {
    int32_t min = minEl();
    //int32_t max = maxEl();
    if (x == min) {
      if (y < z)
        return y;
      else
        return z;
    } else if (y == min) {
      if (x < z)
        return x;
      else
        return z;
    } else {
      if (x < y)
        return x;
      else
        return y;
    }
  }

  //debug and example creation
  std::string string() {
    return std::to_string(x) 
        + ", " + std::to_string(y)
        + ", " + std::to_string(z);
  }
};
