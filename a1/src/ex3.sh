#!/bin/sh

for N in 1 2 4 8 16
do
    echo $N
    ./offsched inputs/3x5x2.gr out.sched $N
    ./des.py -n $N inputs/3x5x2.gr out.sched
done

echo "---------------"

for N in 1 2 4 8 16
do
    echo $N
    ./offsched inputs/3x5x4.gr out.sched $N
    ./des.py -n $N inputs/3x5x4.gr out.sched
done

echo "---------------"

for N in 1 2 4 8 16
do
    echo $N
    ./offsched inputs/small-1.gr out.sched $N
    ./des.py -n $N inputs/small-1.gr out.sched
done


echo "---------------"

for N in 1 2 4 8 16
do
    echo $N
    ./offsched inputs/small-2.gr out.sched $N
    ./des.py -n $N inputs/small-2.gr out.sched
done

echo "---------------"

for N in 1 2 4 8 16
do
    echo $N
    ./offsched inputs/medium-1.gr out.sched $N
    ./des.py -n $N inputs/medium-1.gr out.sched
done
