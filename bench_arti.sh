#!/usr/local_rwth/bin/zsh
### previous benchmarks ###
# Intel Core i5-8265U 1.6Ghz from 2017, Manjaro Linux[Arch derivate]
# i5 core 4 physical, 4 hypervised cores
### HPC benchmarks ###
# Intel Xeon Platinum 8160 Processor 2.1Ghz from 2017, SLURM ???
# i5 core 24 physical, disabled hypervised cores
###                     ###
module switch intel gcc/10 &> /dev/null
CNTRUNS=40
BUILDDIR="../build/"    ## start directory (from tools)
DATADIR="../data/"      ## relative to build (from tools)
TOOLDIR="../tools/"
OUTPUTWRITE="n"
#set -x #DEBUG

SIZE=200

#echo "starting $0.." >> runtime.txt
#date >> runtime.txt

################## artificial examples ##################
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
declare -a DEPENDENCIES
declare -a DEPENDARTI
cd tools

#SIZE is given per script as
#sbatch --export=SIZE=200 test.sbatch

NUMTHREADS="1"
#Cube and Sphere creation
jid1=$(sbatch --parsable --job-name="crCuSp$SIZE" --time=04:00:00 \
  --cpus-per-task=$NUMTHREADS --output="out_%j.log"  \
  --export=BUILDDIR=$BUILDDIR,SIZE=$SIZE \
  crCuSp.sh)
DEPENDARTI+=$jid1
#Helix creation
jid2=$(sbatch --parsable --job-name="crHe$SIZE" --time=04:00:00 \
  --cpus-per-task=$NUMTHREADS --output="out_%j.log"  \
  --export=BUILDDIR=$BUILDDIR,SIZE=$SIZE \
  crHe.sh)
DEPENDARTI+=$jid2
depsarti=$( for i in $DEPENDARTI; do echo -n ":$i"; done )
OUTPUTWRITE="y"
for ((j = 1; j <= $#FILENAMES; ++j)); do
  FILENAME="${FILENAMES[$j]}"
  CNTSEEDING="${CNTSEEDINGS[$j]}"
  MHD_IN="$DATADIR""$FILENAME"".mhd"
  #echo "generate valid comparison solution.."
  jid=$(sbatch --parsable --job-name=sols$SIZE --time=04:00:00 \
  --cpus-per-task=$NUMTHREADS --output="out_%j.log"  \
  --dependency=afterok$depsarti \
  --export=BUILDDIR=$BUILDDIR,MHD_IN=$MHD_IN,CNTSEEDING=$CNTSEEDING,OUTPUTWRITE=$OUTPUTWRITE \
  jid3.sh)
  DEPENDENCIES+=$jid
  #mhd_cnt="$DATADIR""$FILENAME""_""simpleSRG""1"".mhd"
  #echo "count number of written voxels.."
  #jid4=$(sbatch --account=thes0766 --job-name=create_changedvoxels$SIZE \
  #  --mail-type=BEGIN,END,FAIL \
  #--mail-user=jan.hafer@rwth-aachen.de --nodes=1 --ntasks=1 \
  #--cpus-per-task=$NUMTHREADS --mem=16gb --partition=c18m \
  #--time=04:00:00 --output=single_%j.log --error=error_%j.log \
  #--dependency=afterok:$jid3 \
  #./$BUILDDIR/countChangedVoxels "$mhd_cnt"
done
OUTPUTWRITE="n"
#---------- measure time ----------#
pwd #tools
source singleThread.sh             #single core
for k in {1..24}; do                #scale count of cores
  NUMTHREADS="$k"
  source multiThread.sh            #multicore
done
cd ..
#echo "stopped" >> runtime.txt
#date >> runtime.txt
