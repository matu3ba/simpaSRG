#!/usr/bin/env bash
#testing failatomic
DIR="../../build/"
EXEC="failatomicdebug"
if [ -f "$DIR$EXEC" ]; then
  cd "$DIR"
  ./"$EXEC"
else
  echo "FAIL no executable"
fi
