#!/usr/bin/env bash
# -fsanitize=address
# -Werror
CPPFLAGS="-Wall -Wextra -Weffc++ -Wstrict-aliasing -Werror"
g++ $CPPFLAGS -g -O2 -o test main.cpp
