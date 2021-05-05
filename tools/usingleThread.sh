#!/usr/bin/env zsh

function runSingleThread()
{
  FILENAME="$1"
  CNTSEEDING="$2"
  CNTRUNS=$3
  SIZE=$4
  NUMTHREADS=$5
  DEPENDENCIES=$6
  source upathCreation.sh
  RUNNABLE=0
  [ -f "$BUILDDIR""$PROGN_SIMPLE" ] \
  && [ -f "$BUILDDIR""$PROGN_SIMPLE_CIRCBUF" ] \
  && RUNNABLE=1
  if [ "$RUNNABLE" -eq 1 ]; then
    if [ -f "$BUILDDIR""$MHD_IN" ]; then
      #echo "$NUMTHREADS thread(s) $FILENAME: $PROGN_SIMPLE cntseeding pts $CNTSEEDING"
      #echo "$PROGN_SIMPLE.."
      PWD=$(pwd)
      cd ../build
      ./"$PROGN_SIMPLE" "$MHD_IN" "1" "$CNTSEEDING" "$OUTPUTWRITE"  \
      "$CNTRUNS" "$MHD_OUT_SIMPLE")
      #echo "$PROGN_SIMPLE_CIRCBUF.."
      if [[ "$OUTPUTWRITE" == "n" ]]; then
        ./"$PROGN_SIMPLE_CIRCBUF" "$MHD_IN" "1" "$CNTSEEDING" "$OUTPUTWRITE"  \
        "$CNTRUNS" "$MHD_OUT_SIMPLE")
      fi
      cd $PWD
    fi
  else
    echo "FAIL executable ""$BUILDDIR""$PROGN_SIMPLE"
    echo " OR ""$BUILDDIR""$PROGN_SIMPLE_CIRCBUF"" missing"
  fi
}
source upathCreation.sh #getting program names
if [ ! -f "$BUILDDIR""$PROGN_SIMPLE" ]; then
  cd "$PROGN_SIMPLE"; ./build.sh; cd ..
fi

for ((l = 1; l <= $#FILENAMES; ++l)); do
	runSingleThread "${FILENAMES[$l]}" "${CNTSEEDINGS[$l]}" "$CNTRUNS" "$SIZE" "$NUMTHREADS" "$DEPENDENCIES"
done
