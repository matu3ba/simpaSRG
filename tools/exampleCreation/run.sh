#!/usr/bin/env bash
#testing simple SRG
DIR="../../build/"
EXEC="createExamples"
echo "$EXEC"
if [ -f "$DIR$EXEC" ]; then
  cd "$DIR"
  ./"$EXEC" "512" "1" "1" "24" #Helix creation
  ./"$EXEC" "512" #Cube and Sphere creation
else
  echo "FAIL no executable"
fi
