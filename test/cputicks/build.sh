#!/usr/bin/env bash
CPPFLAGS="-Wall -Wextra -Weffc++ -Wstrict-aliasing -Werror"
g++ $CPPFLAGS -O3 main.cpp -o ../../build/cputicks
