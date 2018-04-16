#!/bin/bash

./timing.sh ./sssp_blocking ../inputs/graphs/simplegr.txt ../inputs/reference-outputs/simplegr-0.txt > simplegr_blocking.res
python get.py simplegr_blocking.res
./timing.sh ./sssp_lockfree ../inputs/graphs/simplegr.txt ../inputs/reference-outputs/simplegr-0.txt > simplegr_lockfree.res
python get.py simplegr_lockfree.res

./timing.sh ./sssp_blocking ../inputs/graphs/rmat16.txt ../inputs/reference-outputs/rmat16-0.txt > rmat16_blocking.res
python get.py rmat16_blocking.res
./timing.sh ./sssp_lockfree ../inputs/graphs/rmat16.txt ../inputs/reference-outputs/rmat16-0.txt > rmat16_lockfree.res
python get.py rmat16_lockfree.res

./timing.sh ./sssp_blocking ../inputs/graphs/NY.txt ../inputs/reference-outputs/NY-0.txt > NY_blocking.res
python get.py NY_blocking.res
./timing.sh ./sssp_lockfree ../inputs/graphs/NY.txt ../inputs/reference-outputs/NY-0.txt > NY_lockfree.res
python get.py NY_lockfree.res

./timing.sh ./sssp_blocking ../inputs/graphs/rmat22.txt ../inputs/reference-outputs/sssp-rmat22-0.txt > rmat22_blocking.res
python get.py rmat22_blocking.res
./timing.sh ./sssp_lockfree ../inputs/graphs/rmat22.txt ../inputs/reference-outputs/sssp-rmat22-0.txt > rmat22_lockfree.res
python get.py rmat22_lockfree.res

rm -f *.res

