#BSUB -J Catalano-CUDA
#BSUB -q par-gpu
#BSUB -o cuda_all_output_file
#BSUB -e error_file
#BSUB -n 1
#BSUB -R span[ptile=32]
work=/home/catalano.d/PaStA
cd $work

echo 'CUDA 1.0-Strided without GPU Hist Creation'
CUDA1.0/CUDA1.0PaStA

echo 'CUDA 1.1-Strided with GPU Hist Creation'
CUDA1.1/CUDA1.1PaStA

echo 'CUDA 1.0-Unrolled without GPU Hist Creation'
CUDA2.0/CUDA2.0PaStA

echo 'CUDA 1.0-Unrolled without GPU Hist Creation'
CUDA2.1/CUDA2.1PaStA

