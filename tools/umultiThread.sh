#!/usr/bin/env zsh

function runMultiThread()
{
  FILENAME="$1"
  CNTSEEDING="$2"
  CNTRUNS=$3
  SIZE=$4
  NUMTHREADS=$5
  DEPENDENCIES=$6
  pwd
  echo runmulti
  source "$TOOLDIR""upathCreation.sh"
  RUNNABLE=0
  [ -f "$BUILDDIR""$PROGN_SIMPA_NAIVE" ] \
  && [ -f "$BUILDDIR""$PROGN_SIMPA_NAIVE_CIRCBUF" ] \
  && RUNNABLE=1
  if [ "$RUNNABLE" -eq 1 ]; then
    if [ -f "$BUILDDIR""$MHD_IN" ]; then
      echo "$NUMTHREADS thread(s) $FILENAME: $PROGN_SIMPA_NAIVE $PROGN_SIMPA_NAIVE_CIRCBUF cntseeding pts $CNTSEEDING"
      ./"$PROGN_SIMPA_NAIVE" "$MHD_IN" "$NUMTHREADS" "$CNTSEEDING" "$OUTPUTWRITE"  \
        "$CNTRUNS" "$MHD_OUT_SIMPLE"
      ./"$PROGN_SIMPA_NAIVE_CIRCBUF" "$MHD_IN" "$NUMTHREADS" "$CNTSEEDING" "$OUTPUTWRITE"  \
        "$CNTRUNS" "$MHD_OUT_SIMPLE"
      cd $PWD
    fi
  else
    echo "FAIL executable ""$BUILDDIR""$PROGN_SIMPA_NAIVE"
    echo "OR ""$BUILDDIR""$PROGN_SIMPA_NAIVE""$PROGN_SIMPA_NAIVE_CIRCBUF"
  fi
}
cd "$BUILDDIR"
source "$TOOLDIR""upathCreation.sh" #getting program names
if [ ! -f "$BUILDDIR""$PROGN_SIMPA_NAIVE" ]; then
  cd "$FOLDER_SIMPA"; ./build.sh; cd ..
fi

for ((l = 1; l <= $#FILENAMES; ++l)); do
	runMultiThread "${FILENAMES[$l]}" "${CNTSEEDINGS[$l]}" "$CNTRUNS" "$SIZE" "$NUMTHREADS" "$DEPENDENCIES"
done
cd "$TOOLDIR"
