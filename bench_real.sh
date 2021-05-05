#!/usr/local_rwth/bin/zsh
### previous benchmarks ###
# Intel Core i5-8265U 1.6Ghz from 2017, Manjaro Linux[Arch derivate]
# i5 core 4 physical, 4 hypervised cores
### HPC benchmarks ###
# Intel Xeon Platinum 8160 Processor 2.1Ghz from 2017, SLURM ???
# 24 physical cores, disabled hypervised cores
###                     ###
module switch intel gcc/10 &> /dev/null
CNTRUNS=40
BUILDDIR="../build/"    ## start directory (from tools)
DATADIR="../data/"      ## relative to build (from tools)
OUTPUTWRITE="n"
#set -x #DEBUG

#echo "starting $0.." >> runtime.txt
#date >> runtime.txt
################## real examples ##################
FILENAMES=('BoneMaskHigh' 'Head_of_mouse' 'Maus_mit_Draht' 'Skeleton' 'Skull_of_mouse')
#FILENAMES=('Head_of_mouse' 'Maus_mit_Draht' 'Skull_of_mouse')
CNTSEEDINGS=( 
  5 5 10 5 5
)
CNTCMP1=${#FILENAMES[@]}; CNTCMP2=${#CNTSEEDINGS[@]}
if [ "$CNTCMP1" -ne "$CNTCMP2" ]; then
  echo "wrong count of consecutive seedingpoints"; exit 0
fi
OUTPUTWRITE="y"
NUMTHREADS="1"
declare -a DEPENDENCIES
cd tools
#zsh arrays always start at 1 instead of 0 in bash
for ((i = 1; i <= $#FILENAMES; ++i)); do
  FILENAME="${FILENAMES[$i]}"
  CNTSEEDING=${CNTSEEDINGS[$i]}
  MHD_IN="$DATADIR""$FILENAME"".mhd"
  #echo "generating valid comparison solutions.."
  # --- starting with 3 due to other scripts expecting
  jid=$(sbatch --parsable --job-name="crsol" --time=00:30:00 \
  --cpus-per-task=$NUMTHREADS --output="out_%j.log" \
  --export=BUILDDIR=$BUILDDIR,MHD_IN=$MHD_IN,CNTSEEDING=$CNTSEEDING,OUTPUTWRITE=$OUTPUTWRITE \
  jid3.sh)
  DEPENDENCIES+=$jid
  #./$BUILDDIR/simpleSRG "$MHD_IN" "1" "$CNTSEEDING" "$OUTPUTWRITE" "1")
  #mhd_cnt="$DATADIR""$FILENAME""_""simpleSRG""1"".mhd"
  #echo "count number of written voxels.."
  #./$BUILDDIR/countChangedVoxels "$mhd_cnt"
done
OUTPUTWRITE="n"
  #---------- measure time ----------#
NUMTHREADS="1"
source singleThread.sh               #single core
for i in {1..24}; do                  #scale count of cores
  NUMTHREADS="$i"
  source multiThread.sh              #multicore
done
  #-------- end measure time --------#
cd ..

#echo "stopped" >> runtime.txt
#date >> runtime.txt
