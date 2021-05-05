#!/usr/bin/env bash
# -fsanitize=address
# -Werror
CPPFLAGS="-Wall -Wextra -Weffc++ -Wstrict-aliasing"
g++ $CPPFLAGS -g -O2 -o ../../build/failatomicdebug main.cpp -pthread
#dependencies before main.cpp
