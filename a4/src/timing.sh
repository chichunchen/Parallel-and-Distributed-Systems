#!/bin/bash

if [ $# -ne 3 ]; then
    echo Usage: $0 ./sssp_XXX
    exit 1;
fi;

for N in 1 2 4 8 72 
do
  echo "nthread$N"
  for i in {1..20} 
  do
    ./testpara.sh $1 $2 $3 $N 
  done
done

