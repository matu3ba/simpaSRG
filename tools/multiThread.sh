#!/usr/local_rwth/bin/zsh

function runMultiThread()
{
  FILENAME="$1"
  CNTSEEDING="$2"
  CNTRUNS=$3
  SIZE=$4
  NUMTHREADS=$5
  DEPENDENCIES=$6
  source pathCreation.sh
  RUNNABLE=0
  [ -f "$BUILDDIR""$PROGN_SIMPA_NAIVE" ] \
  && [ -f "$BUILDDIR""$PROGN_SIMPA_NAIVE_CIRCBUF" ] \
  && RUNNABLE=1
  if [ "$RUNNABLE" -eq 1 ]; then
    if [ -f "$BUILDDIR""$MHD_IN" ]; then
      #echo "$NUMTHREADS thread(s) $FILENAME: $PROGN_SIMPA_NAIVE $PROGN_SIMPA_NAIVE_CIRCBUF cntseeding pts $CNTSEEDING"
      mujobname="mu""$SIZE"
      mucibjobname="mucib""$SIZE"
      deps=$( for i in $DEPENDENCIES; do echo -n ":$i"; done )
      sbatch --parsable --job-name=$mujobname --cpus-per-task=$NUMTHREADS \
        --output=out_%j.log \
        --dependency=afterok$deps \
        --export=BUILDDIR=$BUILDDIR,PROGN_SIMPA_NAIVE=$PROGN_SIMPA_NAIVE,MHD_IN=$MHD_IN,NUMTHREADS=$NUMTHREADS,CNTSEEDING=$CNTSEEDING,OUTPUTWRITE=$OUTPUTWRITE,CNTRUNS=$CNTRUNS,MHD_OUT_SIMPLE=$MHD_OUT_SIMPLE  \
        mu.sh
      #./"$PROGN_SIMPA_NAIVE" "$MHD_IN" "$NUMTHREADS" "$CNTSEEDING" "$OUTPUTWRITE"  \
        #"$CNTRUNS" "$MHD_OUT_SIMPLE"
      #sbatch --parsable --job-name=$mucibjobname --cpus-per-task=$NUMTHREADS \
      #  --output=out_%j.log \
      #  --dependency=afterok$deps \
      #  --export=BUILDDIR=$BUILDDIR,PROGN_SIMPA_NAIVE_CIRCBUF=$PROGN_SIMPA_NAIVE_CIRCBUF,MHD_IN=$MHD_IN,NUMTHREADS=$NUMTHREADS,CNTSEEDING=$CNTSEEDING,OUTPUTWRITE=$OUTPUTWRITE,CNTRUNS=$CNTRUNS,MHD_OUT_SIMPLE=$MHD_OUT_SIMPLE \
      #  mucib.sh
      #./"$PROGN_SIMPA_NAIVE_CIRCBUF" "$MHD_IN" "$NUMTHREADS" "$CNTSEEDING" "$OUTPUTWRITE"  \
        #"$CNTRUNS" "$MHD_OUT_SIMPLE"
    fi
  else
    echo "FAIL executable ""$BUILDDIR""$PROGN_SIMPA_NAIVE"
    echo "OR ""$BUILDDIR""$PROGN_SIMPA_NAIVE""$PROGN_SIMPA_NAIVE_CIRCBUF"
  fi
}

source pathCreation.sh #getting program names
if [ ! -f "$BUILDDIR""$PROGN_SIMPA_NAIVE" ]; then
  cd "$FOLDER_SIMPA"; ./build.sh; cd ..
fi

for ((l = 1; l <= $#FILENAMES; ++l)); do
	runMultiThread "${FILENAMES[$l]}" "${CNTSEEDINGS[$l]}" "$CNTRUNS" "$SIZE" "$NUMTHREADS" "$DEPENDENCIES"
done
