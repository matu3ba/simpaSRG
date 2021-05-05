#!/usr/bin/env bash
#counting amount of marked voxels in every example
#set -x #DEBUG
BUILDDIR="../../build/"
DATADIR="../data/"  #relative to BUILDDIR
FILENAMES=('BoneMaskHigh' 'CubeFull' 'Head of mouse' 'Maus mit Draht'
  'Skeleton' 'Skull of mouse' 'SphereFull' 
#Helix
  'Helix_D1' 'Helix_D2' 'Helix_D3' 'Helix_D4' 'Helix_D5' 'Helix_D6' 
  'Helix_D7' 'Helix_D8' 'Helix_D9' 'Helix_D10' 'Helix_D11' 'Helix_D12'
  'Helix_D13' 'Helix_D14' 'Helix_D15' 'Helix_D16' 'Helix_D17' 'Helix_D18'
  'Helix_D19' 'Helix_D20' 'Helix_D21' 'Helix_D22' 'Helix_D23' 'Helix_D24'
)
PROGN="countChangedVoxels"
PROGN_SIMPLE="simpleSRG"
MHD_EXTENSION=".mhd"
PWD_START="$(pwd)"
RUNNABLE=0
[ -f "$BUILDDIR""$PROGN" ] && RUNNABLE=1
if [ "$RUNNABLE" -eq 1 ]; then
  cd "$BUILDDIR"
  for ((i=0;i<${#FILENAMES[@]};++i)); do
    FILENAME="${FILENAMES[i]}"
    echo "$FILENAME"".."
    MHD_IN="$DATADIR""$FILENAME""_""$PROGN_SIMPLE""1""$MHD_EXTENSION"
    if [ -f "$MHD_IN" ]; then
        ./"$PROGN" "$MHD_IN"
    else
      echo "could not find $MHD_IN"
    fi
  done
  if [ -f "timings.txt" ]; then
    rm "timings.txt"
  fi
  cd "$PWD_START"
else
  echo "FAIL executable ""$BUILDDIR""$PROGN"" missing"
fi
