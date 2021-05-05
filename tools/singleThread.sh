#!/usr/bin/env bash

function runSingleThread()
{
  FILENAME="$1"
  CNTSEEDING="$2"
  CNTRUNS=$3
  SIZE=$4
  NUMTHREADS=$5
  DEPENDENCIES=$6
  source pathCreation.sh
  RUNNABLE=0
  [ -f "$BUILDDIR""$PROGN_SIMPLE" ] \
  && [ -f "$BUILDDIR""$PROGN_SIMPLE_CIRCBUF" ] \
  && RUNNABLE=1
  if [ "$RUNNABLE" -eq 1 ]; then
    if [ -f "$BUILDDIR""$MHD_IN" ]; then
      #echo "$NUMTHREADS thread(s) $FILENAME: $PROGN_SIMPLE cntseeding pts $CNTSEEDING"
      #echo "$PROGN_SIMPLE.."
      sijobname="si""$SIZE"
      sicibjobname="sicib""$SIZE"
      deps=$( for i in $DEPENDENCIES; do echo -n ":$i"; done )
      sbatch --parsable --job-name=$sijobname --cpus-per-task=$NUMTHREADS \
        --parsable --output=out_%j.log \
        --dependency=afterok$deps \
        --export=BUILDDIR=$BUILDDIR,PROGN_SIMPLE=$PROGN_SIMPLE,MHD_IN=$MHD_IN,CNTSEEDING=$CNTSEEDING,OUTPUTWRITE=$OUTPUTWRITE,CNTRUNS=$CNTRUNS,MHD_OUT_SIMPLE=$MHD_OUT_SIMPLE  \
        si.sh
      #./"$PROGN_SIMPLE" "$MHD_IN" "1" "$CNTSEEDING" "$OUTPUTWRITE"  \
      #"$CNTRUNS" "$MHD_OUT_SIMPLE")
      #echo "$PROGN_SIMPLE_CIRCBUF.."
      #if [[ "$OUTPUTWRITE" == "n" ]]; then
      #  sbatch --parsable --job-name=$sicibjobname --cpus-per-task=$NUMTHREADS \
      #    --parsable --output=out_%j.log \
      #    --dependency=afterok$deps \
      #    --export=BUILDDIR=$BUILDDIR,PROGN_SIMPLE_CIRCBUF=$PROGN_SIMPLE_CIRCBUF,MHD_IN=$MHD_IN,CNTSEEDING=$CNTSEEDING,OUTPUTWRITE=$OUTPUTWRITE,CNTRUNS=$CNTRUNS,MHD_OUT_SIMPLE=$MHD_OUT_SIMPLE  \
      #    sicib.sh
      #  #./"$PROGN_SIMPLE_CIRCBUF" "$MHD_IN" "1" "$CNTSEEDING" "$OUTPUTWRITE"  \
      #  #"$CNTRUNS" "$MHD_OUT_SIMPLE")
      #fi
    fi
  else
    echo "FAIL executable ""$BUILDDIR""$PROGN_SIMPLE"
    echo " OR ""$BUILDDIR""$PROGN_SIMPLE_CIRCBUF"" missing"
  fi
}
source pathCreation.sh #getting program names
if [ ! -f "$BUILDDIR""$PROGN_SIMPLE" ]; then
  cd "$PROGN_SIMPLE"; ./build.sh; cd ..
fi

for ((l = 1; l <= $#FILENAMES; ++l)); do
	runSingleThread "${FILENAMES[$l]}" "${CNTSEEDINGS[$l]}" "$CNTRUNS" "$SIZE" "$NUMTHREADS" "$DEPENDENCIES"
done
