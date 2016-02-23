#BSUB -J Catalano-serial
#BSUB -o serial_output_file
#BSUB -e error_file
#BSUB -n 1
#BSUB -q ht-10g
#BSUB cwd /home/catalano.d/PaStA/Serial/
work=/home/catalano.d/PaStA/Serial/
cd $work

echo "10000000"
./SerialPaStA 10000000
echo "100000000"
./SerialPaStA 100000000
echo "1000000000"
./SerialPaStA 1000000000
echo "10000000000"
./SerialPaStA 10000000000
