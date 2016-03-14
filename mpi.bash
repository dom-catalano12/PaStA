#BSUB -J Catalano-mpi
#BSUB -o mpi_output_file
#BSUB -e error_file
#BSUB -n 8
#BSUB -q ht-10g
#BSUB cwd /home/catalano.d/PaStA/MPI/
work=/home/catalano.d/PaStA/MPI/
cd $work
tempfile1=hostlistrun
tempfile2=hostlist-tcp
echo $LSB_MCPU_HOSTS > $tempfile1
declare -a hosts
read -a hosts < ${tempfile1}
for((i=0; i<${#hosts[@]}; i += 2)) ;
	do
		HOST=${hosts[$i]}
		CORE=${hosts[(($i+1))]}
		echo $HOST:$CORE >> $tempfile2
done

echo "MPI 2 workers"
mpirun -np 2 -prot -TCP -lsf ./MPIPaStA


echo "MPI 4 workers"
mpirun -np 4 -prot -TCP -lsf ./MPIPaStA

echo "MPI 8 workers"
mpirun -np 8 -prot -TCP -lsf ./MPIPaStA

echo "MPI 16 workers"
mpirun -np 16 -prot -TCP -lsf ./MPIPaStA

echo "MPI 32 workers"
mpirun -np 32 -prot -TCP -lsf ./MPIPaStA

