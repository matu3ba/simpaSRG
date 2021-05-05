#!/usr/local_rwth/bin/zsh

#creation of correct Examples to compare against
#set -x #DEBUG
BUILDDIR="../build/"
DATADIR="../data/"
NUMTHREADS=1 #simple version is not parallelized
OUTPUTWRITE="y"
FILENAMES=('BoneMaskHigh' 'CubeFull' 'Head of mouse' 'Maus mit Draht'
  'Skeleton' 'Skull of mouse' 'SphereFull' 
#Helix
  'Helix_D1' 'Helix_D2' 'Helix_D3' 'Helix_D4' 'Helix_D5' 'Helix_D6' 
  'Helix_D7' 'Helix_D8' 'Helix_D9' 'Helix_D10' 'Helix_D11' 'Helix_D12'
  'Helix_D13' 'Helix_D14' 'Helix_D15' 'Helix_D16' 'Helix_D17' 'Helix_D18'
  'Helix_D19' 'Helix_D20' 'Helix_D21' 'Helix_D22' 'Helix_D23' 'Helix_D24'
)
CNTSEEDINGS=( 
  5 5 5 10
  5 5 5
#Helix
  5 5 5 5 5 5
  5 5 5 5 5 5
  5 5 5 5 5 5
  5 5 5 5 5 5
)
PROGN_SIMPLE="simpleSRG"
MHD_EXTENSION=".mhd"
IMG_EXTENSION=".img"

CNTCMP1=${#FILENAMES[@]}
CNTCMP2=${#CNTSEEDINGS[@]}
if [ "$CNTCMP1" -ne "$CNTCMP2" ]; then
  echo "no count of consecutive seedingpoints"
  exit 0
fi
PWD_START="$(pwd)"
RUNNABLE=0
[ -f "$BUILDDIR""$PROGN_SIMPLE" ] && RUNNABLE=1
if [ "$RUNNABLE" -eq 1 ]; then
  cd "$BUILDDIR"
  for ((i = 1; i <= $#FILENAMES; ++i)); do
    FILENAME="${FILENAMES[i]}" 
    CNTSEEDING="${CNTSEEDINGS[i]}"
    MHD_IN="$DATADIR""$FILENAME""$MHD_EXTENSION"
    IMG_IN="$DATADIR""$FILENAME""$IMG_EXTENSION"
    MHD_OUT_SIMPLE="$DATADIR""$FILENAME""_""$PROGN_SIMPLE""1""$MHD_EXTENSION"
    if [ -f "$MHD_IN" ]; then
        ./"$PROGN_SIMPLE" "$MHD_IN" "$NUMTHREADS" "$CNTSEEDING" "$OUTPUTWRITE" "1"
    else
      echo "could not find $MHD_IN"
    fi
  done
  if [ -f "timings.txt" ]; then
    rm "timings.txt"
  fi
  cd "$PWD_START"
else
  echo "FAIL executable ""$BUILDDIR""$PROGN_SIMPLE"" missing"
fi
