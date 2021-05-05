#!/usr/local_rwth/bin/zsh

cd "build"
if [ -f "timings.txt" ]; then
  today=`date +%Y%m%d_%H%M`
  mv timings.txt "$today""benchmark.csv"
fi
cd ..
