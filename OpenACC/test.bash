#BSUB -J Catalano-test
#BSUB -q par-gpu
#BSUB -o test_output
#BSUB -e error_file
#BSUB -n 1
#BSUB -R span[ptile=32]
work=/home/catalano.d/PaStA/OpenACC
cd $work

rm test_output
rm error_file

./OpenACCPaStA.x


