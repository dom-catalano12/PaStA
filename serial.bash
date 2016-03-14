#BSUB -J Catalano-serial
#BSUB -o serial_output_file
#BSUB -e error_file
#BSUB -n 1
#BSUB -q ht-10g
#BSUB cwd /home/catalano.d/PaStA/Serial/
work=/home/catalano.d/PaStA/Serial/
cd $work

echo "Serial"
./SerialPaStA 

