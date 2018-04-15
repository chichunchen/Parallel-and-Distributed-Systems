#!/bin/bash

if [ $# -ne 4 ]; then
    echo Usage: $0 [binary] graph.txt reference-output.txt [n_threads]
    exit 1;
fi;
t=`mktemp`
echo Temporary output is $t
$1 $2 $t $4 && diff -qs $t $3
rm -f $t

