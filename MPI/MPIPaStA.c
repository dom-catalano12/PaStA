//mpicc -o MPIPaStA MPIPaStA.c rngs.o rvgs.o -lm


#include "rngs.h"
#include "rvgs.h"
#include "util.h"
#include <float.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

double runTest(double *v, int setSize, double min, double max, MPI_Status status, int rank, int NP, int suppressOutput);

int main(int argc, char *argv[])
{
  MPI_Status status;
  MPI_Init(&argc, &argv);
  int rank, NP;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &NP);	
	
	size_t setSize = 1000000000;
	PutSeed(10);
	double v[setSize];

  double min = 100000000000;
  double max = -1*min;



  int i;
  
  if(rank == 0) {
  
  	for (i = 0; i < setSize; i++)
  	{
  		v[i] = Geometric(0.999);
  		if(min > v[i]) min = v[i];
  		if(max < v[i]) max = v[i];
  	}
  }
 
  double time;

  int m, run;
  for (m = 6; m < 10; m++ ) {
    for (run = 0; run < 5; run++ ){
      
      time = runTest(v, pow(10, m), min, max, status, rank, NP, 1);
      if(rank == 0) {
        printf("OpenMP,%d,%f\n",m, time);
      }
    }
  }
  
  MPI_Finalize();
}


double runTest(double v[], int setSize, double min, double max, MPI_Status status, int rank, int NP, int suppressOutput) {
  
  int i;
  int n = 0;
  double sum = 0;
  double pSum = 0;
  double mean = 0;
  
  double variance;
  double std;
  
  double pDelta = 0;
  double delta = 0;
  
  int bins;
  
  double histogram[100] = { 0 };
  
  int offset;
  double start, end;
  double cpu_time_used;
  
  int  numWorkers,  elementsToSend, remaining, sendOffset, countToSend, worker;
  
 	numWorkers = NP - 1;
	MPI_Barrier(MPI_COMM_WORLD);
	start = MPI_Wtime();
 
  MPI_Bcast(&min, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(&max, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  
  
  if(rank == 0) {
    elementsToSend = setSize / numWorkers;
    remaining = setSize % numWorkers;
    sendOffset = 0;
    for(worker = 1; worker <= numWorkers; worker++) {
			countToSend = (worker <= remaining) ? elementsToSend + 1 : elementsToSend;
			MPI_Send(&countToSend, 1, MPI_INT, worker, 1, MPI_COMM_WORLD);			
//printf("%d gets counttosend %d\n", worker, countToSend);
			MPI_Send(&v[sendOffset], countToSend, MPI_DOUBLE, worker, 2, MPI_COMM_WORLD);
//printf("%d gets first num %f\n", worker, v[sendOffset]);
			sendOffset = sendOffset + countToSend;
		}
  } else {
		MPI_Recv(&countToSend, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
//printf("%d has counttosend %d\n", rank, countToSend);
		MPI_Recv(v, countToSend, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, &status);
//printf("%d has first num %f\n", rank, v[0]);
   
		pSum = 0;

		for(i=0; i < countToSend; i++) {
			pSum = pSum + v[i];
		}
//printf("%d has pSum %f\n", rank, pSum);
	}

  MPI_Allreduce(&pSum,&sum, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

	mean = sum / setSize;
//printf("%d has mean %f\n", rank, mean);

  if(rank > 0) {
    for(i=0; i < countToSend; i++) {
			pDelta += (v[i] - mean)*(v[i] - mean);
		}
  
  }

  MPI_Allreduce(&pDelta,&delta, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

	variance = delta / ((float)setSize - 1.0);
//printf("%d has variance %f\n", rank, variance);
	std = sqrt(variance);

//  printf("%d has std %f\n", rank, std);
  
	bins = ceil((max - min) / std );
	offset = floor((mean - min) / std);

//  printf("%d has %d bins with offset %d\n",rank, bins, offset);
	double pHistogram[100] = {0};

  if(rank > 0) {
  	for (i = 0; i < countToSend; i++) {
  		if (v[i] < mean)
  		{
  			pHistogram[offset - (int)floor((mean - v[i]) / std)]++;
  		} 
  		else
  		{
  			pHistogram[(int)ceil((v[i]-mean) / std) + offset]++;
  		}		
  	}
//   printf("%d has %f\n", rank, pHistogram[0]);
	}



	MPI_Reduce(&pHistogram, &histogram, 100, MPI_DOUBLE,MPI_SUM, 0, MPI_COMM_WORLD);
	

  if(rank == 0 && suppressOutput == 0) {
  	printf("Here are the results:\n");
  	printf("Mean: %f\nStandard Deviation: %f\n", mean, std);
  
  	for (i = 0; i <= bins; i++) {
  		if (i <= offset) {
  			printf("%d stds: %f\n", (i - offset - 1),  histogram[i] / setSize);
  		}
  		else
  		{
  			printf("%d stds: %f\n", i - offset, histogram[i] / setSize);
  		}
  	}
  }
	MPI_Barrier(MPI_COMM_WORLD);
	end = MPI_Wtime();
 
	cpu_time_used = end - start;
	return cpu_time_used;

}
