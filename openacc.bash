#BSUB -J Catalano-openacc
#BSUB -q par-gpu
#BSUB -o openacc_output_file
#BSUB -e error_file
#BSUB -n 1
#BSUB -R span[ptile=32]
work=/home/catalano.d/PaStA/OpenACC
cd $work

./OpenACCPaStA.x


