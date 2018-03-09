#!/bin/sh

#for T in 1 2 3 4 8 12 16 24 32 48
#do
#    echo $T
#	./$1 inputs/graphs/NY.txt out.txt $T
#	./test.sh inputs/graphs/NY.txt out.txt
#done

#for T in 1 2 3 4 8 12 16 24 32 48
#do
#    echo $T
#	./$1 inputs/graphs/rmat16.txt out.txt $T
#	./test.sh inputs/graphs/rmat16.txt out.txt
#done

for T in 1 2 3 4 8 12 16 24 32 48
do
    echo $T
	./$1 inputs/graphs/simplegr.txt out.txt $T
	./test.sh inputs/graphs/simplegr.txt out.txt
done
