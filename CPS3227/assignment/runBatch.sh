./nbody_mp_optimised 1000 input_64.txt 2 1 1 1 64_mp
#mpiexec -np 2 nbody_mpi_optimised 1000 input_64.txt 1 1 1 64_mpi
#mpiexec -np 2 nbody_mpi_optimised 1000 input_64.txt 2 1 1 64_hybrid
./nbody_mp_optimised 1000 input_1024.txt 2 1 1 1 1024_mp
#mpiexec -np 2 nbody_mpi_optimised 1000 input_1024.txt 1 1 1 1024_mpi
#mpiexec -np 2 nbody_mpi_optimised 1000 input_1024.txt 2 1 1 1024_hybrid
./nbody_mp_optimised 1000 input_4096.txt 2 1 1 1 4096_mp
#mpiexec -np 2 nbody_mpi_optimised 1000 input_4096.txt 1 1 1 4096_mpi
#mpiexec -np 2 nbody_mpi_optimised 1000 input_4096.txt 2 1 1 4096_hybrid

./nbody_mp_optimised 1000 input_16384.txt 2 1 1 1 16384_mp
#mpiexec -np 2 nbody_mpi_optimised 1000 input_16384.txt 1 1 1 16384_mpi
#mpiexec -np 2 nbody_mpi_optimised 1000 input_16384.txt 2 1 1 16384_hybrid

