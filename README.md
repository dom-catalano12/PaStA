# PaStA
This is the code for Dominic Catalano's High Performance Computing project, the Parallel Statistic Analyzer (PaStA).

To run an execution model, simply submit the bash script for the model that you would like (i.e. serial.bash)

Compilation instructions:
Enter the folder desired and run the following command depending on the folder

For CUDA implementations:
CUDA1.0:  nvcc -arch=sm_35 -o CUDA1.0PaStA CUDA1.0PaStA.cu
CUDA1.1:  nvcc -arch=sm_35 -o CUDA1.1PaStA CUDA1.1PaStA.cu
CUDA2.0:  nvcc -arch=sm_35 -o CUDA2.0PaStA CUDA2.0PaStA.cu
CUDA2.1:  nvcc -arch=sm_35 -o CUDA2.1PaStA CUDA2.1PaStA.cu

For MPI implemenation:
MPI:  mpicc -o MPIPaStA MPIPaStA.c rngs.o rvgs.o -lm

For OpenMP Implementation:
OpenMP:  gcc -fopenmp -o OpenMPPaStA OpenMPPaStA.c rngs.o rvgs.o -lm

For Serial Implementation:
Serial:  gcc -o SerialPaStA SerialPaStA.c rngs.o rvgs.o -lm

Running Instructions:
Enter the bash for the execution type desired:
CUDA:   cuda_all.bash       -> cuda_all_output_file
MPI:    mpi.bash            -> mpi_output_file
OpenMP: openmp.bash         -> openmp_output_file
Serial: serial.bash         -> serial_output_file
