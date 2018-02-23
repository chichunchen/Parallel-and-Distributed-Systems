#!/bin/sh

for N in 1 2 4 8 16
do
    echo $N
    ./offsched inputs/large.gr out.sched $N
    ./des.py -n $N inputs/large.gr out.sched
done

echo "---------------"
