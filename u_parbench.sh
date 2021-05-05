#!/usr/bin/env zsh

CNTRUNS=40
declare -a SIZES
SIZES=( 200 )
FILENAMES=(
  'CubeFull' 'SphereFull'
  'Helix_D1' 'Helix_D2' 'Helix_D3' 'Helix_D4' 'Helix_D5' 
)
CNTSEEDINGS=(
  5 5
  5 5 5 5 5
)
CNTCMP1=$#FILENAMES; CNTCMP2=$#CNTSEEDINGS
if [ "$CNTCMP1" -ne "$CNTCMP2" ]; then
  echo "wrong count of consecutive seedingpoints"; exit 0
fi

BUILDDIR="../build/"
DATADIR="../data/"
TOOLDIR="../tools/"
SIZE=200
cd build
#./$BUILDDIR/"createExamples" "$SIZE" "1" "1" "5"      #Helix creation
#./$BUILDDIR/"createExamples" "$SIZE"                   #Cube and Sphere creation
OUTPUTWRITE="y"
for (( j=1;j<=${#FILENAMES[@]};++j )); do
  FILENAME="${FILENAMES[$j]}"
  CNTSEEDING="${CNTSEEDINGS[$j]}"
  MHD_IN="$DATADIR""$FILENAME"".mhd"
  echo "generate valid comparison solution.."
  ./$BUILDDIR/simpleSRG "$MHD_IN" "1" "$CNTSEEDING" "$OUTPUTWRITE" "1"
  #mhd_cnt="$DATADIR""$FILENAME""_""simpleSRG""1"".mhd"
  #echo "count number of written voxels.."
  #./$BUILDDIR/countChangedVoxels "$mhd_cnt"
done
cd "$TOOLDIR"
OUTPUTWRITE="n"
#---------- measure time ----------#
for k in {8..8}; do                #scale count of cores
  NUMTHREADS="$k"
  source umultiThread.sh            #multicore
done
NUMTHREADS="1"
source usingleThread.sh             #single core

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
