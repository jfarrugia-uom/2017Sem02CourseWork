#!/bin/sh
#
# Your job name
#$ -N seq_job
#
# Use current working directory
#$ -cwd
#
#
# Run job through bash shell
#$ -S /bin/bash
 
# If modules are needed, source modules environment:
. /etc/profile.d/modules.sh
# Add any modules you might require:
module load gcc/4.9.4

# The following output will show in the output file
echo ''Got $NSLOTS processors.''

BODY=1024
ITERATION=100
THREADS=1
INPUT=input_$BODY.txt
ATTEMPTS=5
SYMMETRIC=0
PERSIST=0
 
mkdir output
mkdir output/seq

# Run your application 
./nbody_mp_optimised $ITERATION $INPUT $THREADS $ATTEMPTS $SYMMETRIC $PERSIST >> ./output/seq/"$ITERATION"_"$BODY"_"$THREADS".out
