#!/usr/bin/env bash
#compiling on cluster compiling with module `module load gcc/10`
#!/usr/bin/env bash
#local crosscompiling

# -fsanitize=address
# own laptop skylake
#TARGET="-m64 -march=skylake-avx512"
CPPFLAGS="-std=c++17 -Wall -Wextra -Weffc++ -Wstrict-aliasing -Werror"
CNT=8
CC=g++
OPT="-O1 -g"
#CC=clang++

echo "simpaSRG_NAIVE(1/$CNT).."
$CC $TARGET $CPPFLAGS $OPT -o ../build/simpaSRG_NAIVE -DSIMPA -DNAIVE ../src/Return.cpp ../src/mhd.cpp ../src/seedFinder.cpp ../src/simpaSRG_NAIVE.cpp main.cpp -pthread &
P1=$!
echo "simpleSRG(2/$CNT).."
$CC $TARGET $CPPFLAGS $OPT -o ../build/simpleSRG -DSIMPLE ../src/Return.cpp ../src/mhd.cpp ../src/seedFinder.cpp ../src/simpleSRG.cpp main.cpp &
P2=$!
echo "simpaSRG_NAIVEcib(3/$CNT).."
$CC $TARGET $CPPFLAGS $OPT -o ../build/simpaSRG_NAIVEcib -DCIRCBUFFER -DSIMPA -DNAIVE ../src/Return.cpp ../src/mhd.cpp ../src/seedFinder.cpp ../src/simpaSRG_NAIVE.cpp main.cpp -pthread &
P3=$!
echo "simpleSRGcib(4/$CNT).."
$CC $TARGET $CPPFLAGS $OPT -o ../build/simpleSRGcib -DCIRCBUFFER -DSIMPLE ../src/Return.cpp ../src/mhd.cpp ../src/seedFinder.cpp ../src/simpleSRG.cpp main.cpp &
P4=$!
wait $P1 $P2 $P3 $P4
