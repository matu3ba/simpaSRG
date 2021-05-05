#!/usr/bin/env bash
DIR="../../build/"
EXEC="seedFinder"
ARGS="../data/Skeleton.mhd"
if [ -f "$DIR$EXEC" ]; then
  cd "$DIR"
  ./"$EXEC" "$ARGS"
else
  echo "FAIL no executable"
fi
