#!/usr/local_rwth/bin/zsh
#compiling on cluster compiling with module `module load gcc/10`
#!/usr/bin/env bash
#local crosscompiling

# -fsanitize=address
# own laptop skylake
#TARGET="-m64 -march=skylake-avx512"
CPPFLAGS="-std=c++17 -Wall -Wextra -Weffc++ -Wstrict-aliasing -Werror"
CNT=8
CC=g++
#CC=clang++
#echo "simpaSRG_ERC(1/$CNT).."
#$CC $CPPFLAGS -O3 -o ../build/simpaSRG_ERC -DSIMPA -DERC ../src/Return.cpp ../src/mhd.cpp ../src/seedFinder.cpp ../src/simpaSRG_ERC.cpp main.cpp -pthread
#echo "simpaSRG_RAY(2/$CNT).."
#$CC $CPPFLAGS -O3 -o ../build/simpaSRG_RAY -DSIMPA -DRAY ../src/Return.cpp ../src/mhd.cpp ../src/seedFinder.cpp ../src/simpaSRG_RAY.cpp main.cpp -pthread
#echo "simpaSRG_NAIVE(3/$CNT).."
#$CC $CPPFLAGS -O3 -o ../build/simpaSRG_NAIVE -DSIMPA -DNAIVE ../src/Return.cpp ../src/mhd.cpp ../src/seedFinder.cpp ../src/simpaSRG_NAIVE.cpp main.cpp -pthread
#echo "simpleSRG(4/$CNT).."
#$CC $CPPFLAGS -O3 -o ../build/simpleSRG -DSIMPLE ../src/Return.cpp ../src/mhd.cpp ../src/seedFinder.cpp ../src/simpleSRG.cpp main.cpp
#
#echo "simpaSRG_ERCcib(5/$CNT).."
#$CC $CPPFLAGS -O3 -o ../build/simpaSRG_ERCcib -DCIRCBUFFER -DSIMPA -DERC ../src/Return.cpp ../src/mhd.cpp ../src/seedFinder.cpp ../src/simpaSRG_ERC.cpp main.cpp -pthread
#echo "simpaSRG_RAYcib(6/$CNT).."
#$CC $CPPFLAGS -O3 -o ../build/simpaSRG_RAYcib -DCIRCBUFFER -DSIMPA -DRAY ../src/Return.cpp ../src/mhd.cpp ../src/seedFinder.cpp ../src/simpaSRG_RAY.cpp main.cpp -pthread
#echo "simpaSRG_NAIVEcib(7/$CNT).."
#$CC $CPPFLAGS -O3 -o ../build/simpaSRG_NAIVEcib -DCIRCBUFFER -DSIMPA -DNAIVE ../src/Return.cpp ../src/mhd.cpp ../src/seedFinder.cpp ../src/simpaSRG_NAIVE.cpp main.cpp -pthread
#echo "simpleSRGcib(8/$CNT).."
#$CC $CPPFLAGS -O3 -o ../build/simpleSRGcib -DCIRCBUFFER -DSIMPLE ../src/Return.cpp ../src/mhd.cpp ../src/seedFinder.cpp ../src/simpleSRG.cpp main.cpp

echo "simpaSRG_NAIVE(1/$CNT).."
$CC $TARGET $CPPFLAGS -O3 -o ../build/simpaSRG_NAIVE -DSIMPA -DNAIVE ../src/Return.cpp ../src/mhd.cpp ../src/seedFinder.cpp ../src/simpaSRG_NAIVE.cpp main.cpp -pthread
echo "simpleSRG(2/$CNT).."
$CC $TARGET $CPPFLAGS -O3 -o ../build/simpleSRG -DSIMPLE ../src/Return.cpp ../src/mhd.cpp ../src/seedFinder.cpp ../src/simpleSRG.cpp main.cpp
echo "simpaSRG_NAIVEcib(3/$CNT).."
$CC $TARGET $CPPFLAGS -O3 -o ../build/simpaSRG_NAIVEcib -DCIRCBUFFER -DSIMPA -DNAIVE ../src/Return.cpp ../src/mhd.cpp ../src/seedFinder.cpp ../src/simpaSRG_NAIVE.cpp main.cpp -pthread
echo "simpleSRGcib(4/$CNT).."
$CC $TARGET $CPPFLAGS -O3 -o ../build/simpleSRGcib -DCIRCBUFFER -DSIMPLE ../src/Return.cpp ../src/mhd.cpp ../src/seedFinder.cpp ../src/simpleSRG.cpp main.cpp
