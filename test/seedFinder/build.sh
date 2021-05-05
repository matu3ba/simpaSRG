#!/usr/bin/env bash

CPPFLAGS="-std=c++17 -Wall -Wextra -Weffc++ -Wstrict-aliasing -Werror"
g++ $CPPFLAGS -O3 -o ../../build/seedFinder ../../src/Return.cpp ../../src/mhd.cpp ../../src/seedFinder.cpp main.cpp
