#!/bin/sh
#
# Your job name
#$ -N mp_job
#
# Use current working directory
#$ -cwd
#
# pe (Parallel environment) request. Set your number of processors here.
#$ -pe smp 1
#
# Run job through bash shell
#$ -S /bin/bash
 
# If modules are needed, source modules environment:
. /etc/profile.d/modules.sh
# Add any modules you might require:
module load gcc/4.9.4

# The following output will show in the output file
echo ''Got $NSLOTS processors.''

BODY=64
ITERATION=1000
THREADS=12
INPUT=input_$BODY.txt
ATTEMPTS=1
SYMMETRIC=1
PERSIST=1
 
mkdir output
mkdir output/mp

# Run your application 
./nbody_mp_optimised $ITERATION $INPUT $THREADS $ATTEMPTS $SYMMETRIC $PERSIST ./"$BODY"_mp >> ./output/mp/"$ITERATION"_"$BODY"_"$THREADS".out
tar -zcvf "$BODY"_mp.tar.gz ./"$BODY"_mp
rm ./"$BODY"_mp/*.txt
