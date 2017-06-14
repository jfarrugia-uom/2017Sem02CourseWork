#!/bin/bash

TEST=64
ITERATION=1000
THREADS=2
INPUT=input_$TEST.txt
ATTEMPTS=1
PERSIST=0

DIR=test/{sd1,sd2}

echo $DIR
mkdir -p  $DIR

#./nbody_mp_optimised $ITERATION $INPUT $THREADS $ATTEMPTS $PERSIST > "$TEST"_"$THREADS".out
