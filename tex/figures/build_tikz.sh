#!/bin/bash
tikzfiles=""
files=(*.tikz)
delim=""
for item in "${files[@]}"; do
  #tikzfiles
  word=$(printf "%s" "$delim$item")
  tikzfiles="${tikzfiles}${word}"
  delim=","
done
echo "$tikzfiles"

lualatex --shell-escape '\def\zzz{'"$tikzfiles"'} \input myscript.tex'
