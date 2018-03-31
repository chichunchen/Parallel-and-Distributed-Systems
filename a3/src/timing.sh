#!/bin/bash

if [ $# -ne 2 ]; then
    echo Usage: $0 ./histoXXX moon
    exit 1;
fi;

for N in 1 2 4 8
do
  echo "nthread$N"
  for i in {1..10}
  do
    ./test.sh ./$1 ../images/$2.ppm sample/$2.hist $N 
  done
done

