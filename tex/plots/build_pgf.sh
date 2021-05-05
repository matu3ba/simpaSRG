#!/bin/bash
for f in *.pgf
do 
  jn=$(basename $f .pgf)
  pdflatex $f
  rm $jn.aux $jn.log
done
