//Compile gcc OpenACCPaStA.c -fopenacc -foffload=nvptx-none -foffload="-O3" -O3 -o OpenACCPaStA.x -lm

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#include <limits.h>

float runTest(float *v, int setSize, float min, float max, int suppressOutput);

int main(int argc, char *argv[])
{

	
	int setSize = 1000000000;
	float *v;
  v = (float*)malloc(setSize*sizeof(float));
 

  float min = 100000000000;
  float max = -1*min;



  int i;
	for (i = 0; i < setSize; i++)
	{
		v[i] = rand() / (float)RAND_MAX + rand() / (float)RAND_MAX + rand() / (float)RAND_MAX + 1;
		if(min > v[i]) min = v[i];
		if(max < v[i]) max = v[i];
	}
 
   float time;

//runTest(v, 100000, min,max, 0);

  int m, run;
  for (m = 6; m < 10; m++ ) {
    for (run = 0; run < 11; run++ ){
      time = runTest(v, pow(10, m), min, max, 1);
      if(run >= 5) printf("OpenACC,%d,%f\n",m, time);
    }
  }
}


float runTest(float *v, int setSize, float min, float max, int suppressOutput) {

	struct timeval starttime,endtime;
	double   runTime = 0.0;
 
    clock_t start, end;
  gettimeofday(&starttime,NULL);
  int i;
  int n = 0;
  float sum = 0;
  float pSum = 0;
  float mean = 0;
  
  float variance;
  float std;
  
  float delta = 0;
  
  int bins;
  
  int histogram[100] = { 0 };
  
  int offset;
  

#pragma acc data copyin(v[0:setSize])
#pragma acc region 
{
#pragma acc loop //reduction(+:sum)
	for (i = 0; i < setSize; i++) {
		sum += v[i];	
	}
	mean = sum / setSize;
 
 

#pragma acc loop // reduction(+:delta)
	for (i = 0; i < setSize; i++) {
		delta += (v[i] - mean)*(v[i] - mean);
	}

	variance = delta / ((float)setSize - 1.0);

	std = sqrt(variance);
	bins = ceil((max - min) / std );
	offset = floor((mean - min) / std);
  


#pragma acc loop
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
}


if(suppressOutput == 0) {
	printf("Here are the results:\n");
	printf("Mean: %f\nStandard Deviation: %f\n", mean, std);

	for (i = 0; i <= bins; i++) {
		if (i <= offset) {
			printf("%d stds: %f\n", (i - offset - 1),  histogram[i] / (float)setSize);
		}
		else
		{
			printf("%d stds: %f\n", i - offset, histogram[i] / (float)setSize);
		}
	}
}
	gettimeofday(&endtime,NULL);
	runTime=(1000000*(endtime.tv_sec-starttime.tv_sec)+endtime.tv_usec-starttime.tv_usec)/1000000.0;
   if(suppressOutput == 0) printf("time: %f\n",runTime);
  	return runTime;
}
