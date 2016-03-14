

#include "rngs.h"
#include "rvgs.h"
#include "util.h"
#include <float.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <omp.h>

int main(int argc, char *argv[])
{

	
	size_t setSize = parse_args(argc, argv);
	PutSeed(10);
	double v[setSize];

        double min = 0;
        double max = 204000;

        int n = 0;
        double sum = 0;
        double pSum = 0;
	double mean = 0;
        
        double variance;
        double std;


	int i;
	
	double delta = 0;

        size_t bins = ceil((max - min) / std);

        double histogram[100] = { 0 };
	double pHistogram[100] = {0};

        int offset;
        int bin;

#pragma omp parallel shared(sum,delta) private(pHistogram)


#pragma omp parallel for
	for (i = 0; i < setSize; i++)
	{
		v[i] = Geometric(0.9999);
	}

        double start, end;
        double cpu_time_used;
        start = omp_get_wtime();



#pragma omp parallel for reduction(+:sum)
	for (i = 0; i < setSize; i++) {
		sum += v[i];	
	}
	mean = sum / setSize;


#pragma omp parallel for reduction(+:delta)
	for (i = 0; i < setSize; i++) {
		delta += (v[i] - mean)*(v[i] - mean);
	}

	variance = delta / ((float)setSize - 1.0);

	std = sqrt(variance);


#pragma omp parallel for
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

	for(i = 0; i <= bins; i++) {
#pragma omp atomic
		histogram[i] += pHistogram[i];
	}

	offset = floor((mean - min) / std);

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
	end = omp_get_wtime();
	cpu_time_used = end - start;
	printf("Time used %f\n", cpu_time_used);
}
