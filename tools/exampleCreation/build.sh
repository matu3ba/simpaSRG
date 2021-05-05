#!/usr/local_rwth/bin/zsh
#compiling on cluster compiling with module `module load gcc/10`
#!/usr/bin/env bash
#local crosscompiling

# -fsanitize=address
# own laptop skylake
#TARGET="-m64 -march=skylake-avx512"
CPPFLAGS="-std=c++17 -Wall -Wextra -Weffc++ -Wstrict-aliasing -Werror"
g++ $TARGET $CPPFLAGS -g -O3 -o ../../build/createExamples ../../src/Return.cpp ../../src/mhd.cpp example_creation.cpp main.cpp
