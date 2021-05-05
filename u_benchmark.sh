#!/usr/bin/env zsh
### previous benchmarks ###
# Intel Core i5-8265U 1.6Ghz from 2017, Manjaro Linux[Arch derivate]
# i5 core 4 physical, 4 hypervised cores
### HPC benchmarks ###
# Intel Xeon Platinum 8160 Processor 2.1Ghz from 2017, SLURM ???
# i5 core 24 physical, disabled hypervised cores
###                     ###

CNTRUNS=40
BUILDDIR="../build/"    ## start directory (from tools)
DATADIR="../data/"      ## relative to build (from tools)
OUTPUTWRITE="n"
#set -x #DEBUG

echo "starting.." >> runtime.txt
date >> runtime.txt
################## real examples ##################
FILENAMES=('BoneMaskHigh' 'Head_of_mouse' 'Maus_mit_Draht' 'Skeleton' 'Skull_of_mouse')
CNTSEEDINGS=( 5 5 10 5 5 )
CNTCMP1=${#FILENAMES[@]}; CNTCMP2=${#CNTSEEDINGS[@]}
if [ "$CNTCMP1" -ne "$CNTCMP2" ]; then
  echo "wrong count of consecutive seedingpoints"; exit 0
fi
OUTPUTWRITE="y"
cd tools
for ((i=1;i<${#FILENAMES[@]};++i)); do
  PWD=$(pwd)
  cd $BUILDDIR
  FILENAME="${FILENAMES[$i]}"
  CNTSEEDING=${CNTSEEDINGS[$i]}
  MHD_IN="$DATADIR""$FILENAME"".mhd"
  echo "generating valid comparison solutions.."
  ./simpleSRG "$MHD_IN" "1" "$CNTSEEDING" "$OUTPUTWRITE" "1" &
  cd $PWD
  #echo ./$BUILDDIR/simpleSRG "$MHD_IN" "1" "$CNTSEEDING" "$OUTPUTWRITE" "1"
  #mhd_cnt="$DATADIR""$FILENAME""_""simpleSRG""1"".mhd"
  #echo "count number of written voxels.."
  #./$BUILDDIR/countChangedVoxels "$mhd_cnt"
done
OUTPUTWRITE="n"
wait
  #---------- measure time ----------#
#NUMTHREADS="1"
source usingleThread.sh               #single core
for i in {1..8}; do                  #scale count of cores
  NUMTHREADS="$i"
  source umultiThread.sh              #multicore
done
  #-------- end measure time --------#
cd ..
#SIZES=(
#  200 250 300 350 400 
#  450 500 550 600 650 
#  700 750 800
#)
################## artificial examples ##################
SIZES=( 200 )
FILENAMES=(
  'CubeFull' 'SphereFull'
  'Helix_D1' 'Helix_D2' 'Helix_D3' 'Helix_D4' 'Helix_D5' 
)
CNTSEEDINGS=(
  5 5
  5 5 5 5 5
)
CNTCMP1=${#FILENAMES[@]}; CNTCMP2=${#CNTSEEDINGS[@]}
if [ "$CNTCMP1" -ne "$CNTCMP2" ]; then
  echo "wrong count of consecutive seedingpoints"; exit 0
fi
cd tools
for ((i=1;i<${#SIZES[*]};++i)); do
  SIZE="${SIZES[$i]}"
  ./$BUILDDIR/"createExamples" "$SIZE" "1" "1" "5"      #Helix creation
  ./$BUILDDIR/"createExamples" "$SIZE"                   #Cube and Sphere creation
  OUTPUTWRITE="y"
  for (( j=1;j<${#FILENAMES[@]};++j )); do
    FILENAME="${FILENAMES[$j]}"
    CNTSEEDING="${CNTSEEDINGS[$j]}"
    MHD_IN="$DATADIR""$FILENAME"".mhd"
    echo "generate valid comparison solution.."
    ./$BUILDDIR/simpleSRG "$MHD_IN" "1" "$CNTSEEDING" "$OUTPUTWRITE" "1"
    #mhd_cnt="$DATADIR""$FILENAME""_""simpleSRG""1"".mhd"
    #echo "count number of written voxels.."
    #./$BUILDDIR/countChangedVoxels "$mhd_cnt"
  done
  OUTPUTWRITE="n"
  #---------- measure time ----------#
  for k in {4..8}; do                #scale count of cores
    NUMTHREADS="$k"
    source umultiThread.sh            #multicore
  done
  NUMTHREADS="1"
  source usingleThread.sh             #single core
done
cd ..
echo "stopped" >> runtime.txt
date >> runtime.txt
PWD_START="$(pwd)"
cd "build"
if [ -f "timings.txt" ]; then
  today=`date +%Y%m%d_%H%M`
  mv timings.txt "$today""benchmark.csv"
fi
cd ..
