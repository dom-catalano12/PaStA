//Compile gcc -o SerialPaStA SerialPaStA.c rngs.o rvgs.o -lm


#include "rngs.h"
#include "rvgs.h"
#include "util.h"
#include <float.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

double runTest(double *v, int setSize, double min, double max, int suppressOutput);

int main(int argc, char *argv[])
{

	size_t setSize = 1000000000;
	PutSeed(10);
	double v[1000000000];

	double min = 1000000000000000;
	double max = -1000000000000000;

	int i;
	for (i = 0; i < setSize; i++)
	{
		v[i] = pow(Geometric(0.9999), 1);
		if(v[i] < min) min = v[i];
		if(v[i] > max) max = v[i]; 
	}

  double time;

  int m, run;
  for (m = 6; m < 10; m++ ) {
    for (run = 0; run < 5; run++ ){
      time = runTest(v, pow(10, m), min, max, 1);
      printf("Serial,%d,%f\n",m, time);
    }
  }

}

double runTest(double *v, int setSize, double min, double max, int suppressOutput) {
  clock_t start, end;
  double cpu_time_used;
  start = clock();

  int i;
	int n = 0;
	double sum = 0;
	double mean = 0;
	double delta = 0;
	double variance;
	double std;


	for (i = 0; i < setSize; i++) {
		sum += v[i];
	}

	mean = sum / setSize;

	for (i = 0; i < setSize; i++) {
		delta += (v[i] - mean)*(v[i] - mean);
	}

	variance = delta / ((float)setSize - 1.0);

	std = sqrt(variance);

	size_t bins = ceil((max - min) / std);

	double histogram[100] = { 0 };

	int offset = floor((mean - min) / std);
	int bin;
	for (i = 0; i < setSize; i++) {
		if (v[i] < mean)
		{
			histogram[offset - (int)floor((mean - v[i]) / std)]++;
		} 
		else
		{
			histogram[(int)ceil((v[i]-mean) / std) + offset]++;
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
   
     
  	end = clock();
  	cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
  	return cpu_time_used;
}
