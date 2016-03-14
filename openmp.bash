#BSUB -J Catalano-openmp
#BSUB -o openmp_output_file
#BSUB -e error_file
#BSUB -n 1
#BSUB -q ht-10g
#BSUB cwd /home/catalano.d/PaStA/OpenMP/
work=/home/catalano.d/PaStA/OpenMP/
cd $work

echo "OpenMP, 2 Threads"
export OMP_NUM_THREADS=2
./OpenMPPaStA

echo "OpenMP, 4 Threads"
export OMP_NUM_THREADS=4
./OpenMPPaStA

echo "OpenMP, 8 Threads"
export OMP_NUM_THREADS=8
./OpenMPPaStA

echo "OpenMP, 16 Threads"
export OMP_NUM_THREADS=16
./OpenMPPaStA

echo "OpenMP, 32 Threads"
export OMP_NUM_THREADS=32
./OpenMPPaStA