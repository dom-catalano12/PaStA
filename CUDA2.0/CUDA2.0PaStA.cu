//compile nvcc -arch=sm_35 -o CUDA2PaStA CUDA2PaStA.cu

#include <stdio.h>
#include <algorithm>    

#include <limits.h>
//#include "util.h"


// cuda error function
#define checkCuda(err) (CheckCuda(err, __FILE__, __LINE__))
static void CheckCuda(cudaError_t result, const char *file, int line)
{

	if (result != cudaSuccess) {
		printf("%s in %s at line %d\n", cudaGetErrorString(result),
			file, line);
		exit(EXIT_FAILURE);
	}
}

// CUDA kernel. Each thread takes care of one element of c
// This function is based off of the NVIDIA code shown in class but it changed
// to reflect a min reduction
__global__ void reduceSum(float * g_idata, float * g_odata, int n, int vectLen)
{
	extern __shared__ float sdata[];

	//global index
	int i = threadIdx.x + blockIdx.x*blockDim.x;
	int j = blockIdx.y*blockDim.y + threadIdx.y;

	//thread index
	int tid_x = threadIdx.x;
	int tid_y = threadIdx.y;
  int blockSize = blockDim.x*blockDim.y;

	//int blockId = blockIdx.x + blockIdx.y * gridDim.x;
	int globalId = i*n + j;
	int sharedIdx = tid_y*blockDim.x + tid_x;

	sdata[sharedIdx] = ((globalId < vectLen) && (i < n) && (j < n)) ? g_idata[globalId] : 0;
	__syncthreads();
/*
	int s;
	for (s = (blockDim.x*blockDim.y) / 2; s > 0; s >>= 1) {
		if (sharedIdx < s) {
			sdata[sharedIdx] += sdata[sharedIdx + s];
		}
		__syncthreads();

	}*/
 if(blockSize >= 1024) {
   if(sharedIdx < 512) {sdata[sharedIdx] += sdata[sharedIdx + 512];} __syncthreads();
 }
 if(blockSize >= 512) {
   if(sharedIdx < 256) {sdata[sharedIdx] += sdata[sharedIdx + 256];} __syncthreads();
 }
 if(blockSize >= 256) {
   if(sharedIdx < 128) {sdata[sharedIdx] += sdata[sharedIdx + 128];} __syncthreads();
 }
 if(blockSize >= 128) {
   if(sharedIdx < 64) {sdata[sharedIdx] += sdata[sharedIdx + 64];} __syncthreads();
 }
 if(sharedIdx < 32){
   if(blockSize >= 64) sdata[sharedIdx] += sdata[sharedIdx + 32];
   if(blockSize >= 32) sdata[sharedIdx] += sdata[sharedIdx + 16];
   if(blockSize >= 16) sdata[sharedIdx] += sdata[sharedIdx + 8];
   if(blockSize >= 8) sdata[sharedIdx] += sdata[sharedIdx + 4];
   if(blockSize >= 4) sdata[sharedIdx] += sdata[sharedIdx + 2];
   if(blockSize >= 2) sdata[sharedIdx] += sdata[sharedIdx + 1];
 }
	float sum;
	if (sharedIdx == 0) {
		sum = sdata[sharedIdx];
		atomicAdd(g_odata, sum);

	}
}

__global__ void deltaReduceSum(float * g_idata, float * g_odata, int n, int vectLen, float *mean)
{
	extern __shared__ float sdata[];

	//global index
	int i = threadIdx.x + blockIdx.x*blockDim.x;
	int j = blockIdx.y*blockDim.y + threadIdx.y;

	//thread index
	int tid_x = threadIdx.x;
	int tid_y = threadIdx.y;

	//int blockId = blockIdx.x + blockIdx.y * gridDim.x;
	int globalId = i*n + j;
	int sharedIdx = tid_y*blockDim.x + tid_x;
  int blockSize = blockDim.x*blockDim.y;
  
	sdata[sharedIdx] = ((globalId < vectLen) && (i < n) && (j < n))
		? (g_idata[globalId] - mean[0])*(g_idata[globalId] - mean[0]) : 0;
	__syncthreads();

	/*
	int s;
	for (s = (blockDim.x*blockDim.y) / 2; s > 0; s >>= 1) {
		if (sharedIdx < s) {
			sdata[sharedIdx] += sdata[sharedIdx + s];
		}
		__syncthreads();

	}*/
 if(blockSize >= 1024) {
   if(sharedIdx < 512) {sdata[sharedIdx] += sdata[sharedIdx + 512];} __syncthreads();
 }
 if(blockSize >= 512) {
   if(sharedIdx < 256) {sdata[sharedIdx] += sdata[sharedIdx + 256];} __syncthreads();
 }
 if(blockSize >= 256) {
   if(sharedIdx < 128) {sdata[sharedIdx] += sdata[sharedIdx + 128];} __syncthreads();
 }
 if(blockSize >= 128) {
   if(sharedIdx < 64) {sdata[sharedIdx] += sdata[sharedIdx + 64];} __syncthreads();
 }
 if(sharedIdx < 32){
   if(blockSize >= 64) sdata[sharedIdx] += sdata[sharedIdx + 32];
   if(blockSize >= 32) sdata[sharedIdx] += sdata[sharedIdx + 16];
   if(blockSize >= 16) sdata[sharedIdx] += sdata[sharedIdx + 8];
   if(blockSize >= 8) sdata[sharedIdx] += sdata[sharedIdx + 4];
   if(blockSize >= 4) sdata[sharedIdx] += sdata[sharedIdx + 2];
   if(blockSize >= 2) sdata[sharedIdx] += sdata[sharedIdx + 1];
 }
	float sum;
	if (sharedIdx == 0) {
		sum = sdata[sharedIdx];
		atomicAdd(g_odata, sum);

	}
}

double runTest(float *v, int setSize, float min, float max, int suppressOutput);

int main()
{

	size_t setSize = 1000000000;
	//PutSeed(10);
	float * v;
	v = (float*)malloc(setSize*sizeof(float));
	float min = 1000000000000000;
	float max = -1000000000000000;

	int i;
	for (i = 0; i < setSize; i++)
	{
		v[i] = rand() / (float)RAND_MAX + rand() / (float)RAND_MAX + rand() / (float)RAND_MAX + 1;
   
		if (v[i] < min) min = v[i];
		if (v[i] > max) max = v[i];
	}

	double time;
	//time = runTest(v, 10000, min, max, 0);
	
	int m, run;
	for (m = 6; m < 10; m++) {
	for (run = 0; run < 11; run++){
	time = runTest(v, pow(10,m), min, max, 1);
 
  if(run >= 5)	printf("CUDA,%d,%f\n", m, time);
	}
	}
}
double runTest(float *h_v, int setSize, float min, float max, int suppressOutput) {

	cudaEvent_t start = 0;
	cudaEvent_t stop = 0;
	float time = 0;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);

	float *h_sum;

	//device vectors
	float *d_v;
	float *d_sum;

	size_t bytes = setSize*sizeof(float);
	int gridSide = ceil(sqrt(setSize) / 32.0);

	size_t outbytes = sizeof(float);

	// Number of threads in each thread block
	h_sum = (float*)malloc(outbytes);
	h_sum[0] = 0;
	int i;


	checkCuda(cudaMalloc((void**)&d_v, bytes));
	checkCuda(cudaMalloc((void**)&d_sum, outbytes));


	checkCuda(cudaMemcpy(d_v, h_v, bytes, cudaMemcpyHostToDevice));


	int blockSide = 32;
	cudaEventRecord(start, 0);
	dim3 block(blockSide, blockSide);
	dim3 grid(gridSide, gridSide);

	reduceSum << <grid, block, blockSide * blockSide * sizeof(float) >> >(d_v, d_sum, ceil(sqrt(setSize)), setSize);


	checkCuda(cudaMemcpy(h_sum, d_sum, outbytes, cudaMemcpyDeviceToHost));

	



	float * h_mean;
	float * h_delta;

	h_mean = (float*)malloc(outbytes);
	h_delta = (float*)malloc(outbytes);

	float * d_mean;
	float * d_delta;

	checkCuda(cudaMalloc((void**)&d_mean, outbytes));
	checkCuda(cudaMalloc((void**)&d_delta, outbytes));

	h_mean[0] = h_sum[0] / setSize;

	checkCuda(cudaMemcpy(d_mean, h_mean, outbytes, cudaMemcpyHostToDevice));

	deltaReduceSum << <grid, block, blockSide * blockSide * sizeof(float) >> >(d_v, d_delta, ceil(sqrt(setSize)), setSize, d_mean);

	checkCuda(cudaMemcpy(h_delta, d_delta, outbytes, cudaMemcpyDeviceToHost));

	float * h_std;
	h_std = (float*)malloc(outbytes);
	h_std[0] = sqrt(h_delta[0] / ((float)setSize - 1.0));

	

	size_t bins = ceil((max - min) / h_std[0]);

	int histogram[100] = { 0 };

	int offset = floor((h_mean[0] - min) / h_std[0]);
	for (i = 0; i < setSize; i++) {
		if (h_v[i] < h_mean[0])
		{
			histogram[offset - (int)floor((h_mean[0] - h_v[i]) / h_std[0])]++;
		}
		else
		{
			histogram[(int)ceil((h_v[i] - h_mean[0]) / h_std[0]) + offset]++;
		}
	}

	if (suppressOutput == 0) {
		printf("(sum %f)\n", h_sum[0]);
		printf("Here are the results:\n");
		printf("Mean: %f\nStandard Deviation: %f\n", h_mean[0], h_std[0]);

		for (i = 0; i <= bins; i++) {
			if (i <= offset) {
				printf("%d stds: %f\n", (i - offset - 1), histogram[i] / (float)setSize);
			}
			else
			{
				printf("%d stds: %f\n", i - offset, histogram[i] / (float)setSize);
			}
		}
	}





	cudaEventRecord(stop, 0);
	cudaEventSynchronize(stop);

	// Release device memory
	cudaFree(d_v);
	cudaFree(d_sum);
	cudaFree(d_mean);
	cudaFree(d_delta);
	cudaEventElapsedTime(&time, start, stop);

	//	printf("Real min: %.20f, myMin: %.20f\n", realMin, myMin);
	if(suppressOutput == 0) printf("Time for the kernel: %f\n", time);

	return time/1000.0;
}