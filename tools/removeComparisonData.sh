#!/usr/local_rwth/bin/zsh

#creation of correct Examples to compare against
#set -x #DEBUG
BUILDDIR="../build/"
DATADIR="../data/"
NUMTHREADS=1 #simple version is not parallelized
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
PWD_START="$(pwd)"
cd "$BUILDDIR"
for ((i = 1; i <= $#FILENAMES; ++i)); do
  FILENAME="${FILENAMES[i]}" 
  MHD_OUT_SIMPLE="$DATADIR""$FILENAME""_""$PROGN_SIMPLE""1""$MHD_EXTENSION"
  IMG_OUT_SIMPLE="$DATADIR""$FILENAME""_""$PROGN_SIMPLE""1""$IMG_EXTENSION"
  if [ -f "$MHD_OUT_SIMPLE" ]; then
    rm "$MHD_OUT_SIMPLE"
  else
    echo "could not find $MHD_OUT_SIMPLE"
  fi
  if [ -f "$IMG_OUT_SIMPLE" ]; then
    rm "$IMG_OUT_SIMPLE"
  else
    echo "could not find $IMG_OUT_SIMPLE"
  fi
done
cd "$PWD_START"
