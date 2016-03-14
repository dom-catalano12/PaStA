

#include "rngs.h"
#include "rvgs.h"
#include "util.h"
#include <float.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <omp.h>

double runTest(double *v, int setSize, double min, double max, int suppressOutput);

int main(int argc, char *argv[])
{

	
	size_t setSize = 1000000000;
	PutSeed(10);
	double v[setSize];

  double min = 100000000000;
  double max = -1*min;



  int i;
	for (i = 0; i < setSize; i++)
	{
		v[i] = Geometric(0.999);
		if(min > v[i]) min = v[i];
		if(max < v[i]) max = v[i];
	}
 
   double time;

  int m, run;
  for (m = 6; m < 10; m++ ) {
    for (run = 0; run < 5; run++ ){
      time = runTest(v, pow(10, m), min, max, 1);
      printf("OpenMP,%d,%f\n",m, time);
    }
  }
}


double runTest(double *v, int setSize, double min, double max, int suppressOutput) {
  int i;
  int n = 0;
  double sum = 0;
  double pSum = 0;
  double mean = 0;
  
  double variance;
  double std;
  
  double delta = 0;
  
  int bins;
  
  double histogram[100] = { 0 };
  
  int offset;
  double start, end;
  double cpu_time_used;
  
  
  
  
  start = omp_get_wtime();

#pragma omp parallel shared(sum, delta)
{


#pragma omp for reduction(+:sum)
	for (i = 0; i < setSize; i++) {
		sum += v[i];	
	}
	mean = sum / setSize;


#pragma omp for reduction(+:delta)
	for (i = 0; i < setSize; i++) {
		delta += (v[i] - mean)*(v[i] - mean);
	}

	variance = delta / ((float)setSize - 1.0);

	std = sqrt(variance);
	bins = ceil((max - min) / std );
	offset = floor((mean - min) / std);


	double pHistogram[100] = {0};
#pragma omp for
	for (i = 0; i < setSize; i++) {
		if (v[i] < mean)
		{
			pHistogram[offset - (int)floor((mean - v[i]) / std)]++;
		} 
		else
		{
			pHistogram[(int)ceil((v[i]-mean) / std) + offset]++;
		}		
	}
	

	for(i = 0; i <= bins; i++){
#pragma omp atomic
 		histogram[i] += pHistogram[i];
	}
}	

if(suppressOutput == 0) {
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
	end = omp_get_wtime();
	cpu_time_used = end - start;
	return cpu_time_used;

}
