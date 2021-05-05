#!/usr/bin/env bash
# -fsanitize=address
# -Werror
CPPFLAGS="-Wall -Wextra -Weffc++ -Wstrict-aliasing -Werror"
g++ $CPPFLAGS -O3 -o ../../build/failatomic main.cpp -pthread
#dependencies before main.cpp
