#!/bin/bash

if [ $# -ne 2 ] && [ $# -ne 3 ]; then
    echo Usage: $0 graph.txt reference-output.txt [binary]
    exit 1;
fi;
BINARY=${3:-./sssp_serial}
t=`mktemp`
echo Temporary output is $t
$BINARY $1 $t && diff -qs $t $2
rm -f $t
