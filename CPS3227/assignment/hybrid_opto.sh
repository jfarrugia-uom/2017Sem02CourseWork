#!/bin/sh
#
# Your job name
#$ -N hybrid_opto_job
#
# Use current working directory
#$ -cwd
#
# pe (Parallel environment) request. Set your number of processors here.
#$ -pe openmpi_12x1 48
#
# Run job through bash shell
#$ -S /bin/bash
 
# If modules are needed, source modules environment:
. /etc/profile.d/modules.sh
# Add any modules you might require:
module load gcc/4.9.4
module add shared openmpi/gcc/64/1.8.8

# The following output will show in the output file
echo ''Got $NSLOTS processors.''

BODY=64
ITERATION=1000
THREADS=12
INPUT=input_$BODY.txt
ATTEMPTS=1
PERSIST=0
 
mkdir output
mkdir output/hybrid

# Run your application 
mpirun -np 4 nbody_mpi_optimised $ITERATION $INPUT $THREADS $ATTEMPTS $PERSIST ./"$BODY"_hybrid >> ./output/hybrid/"$ITERATION"_"$BODY"_"$THREADS".out
#mpirun -np 4 --npernode 1 nbody_mpi 1000 input_1024.txt 1 2 0 >> ./output/mpi_nonopto/1000_1024_1.out
if [ "$PERSIST" -eq 1 ]; then
	tar -zcvf "$BODY"_hybrid.tar.gz ./"$BODY"_hybrid
	rm ./"$BODY"_hybrid/*.txt
fi
