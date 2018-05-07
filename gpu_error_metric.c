#include <stdio.h>

#define ELECTRODES 105
#define SIZE 15765

__global__
void error_metric(double *FM_EEG_105, double *GridLoc, double *Sources, double* FN25, double* FN50, double* FN75)
{
	int l = blockIdx.x;
	int l = threadIdx.x;

	int pss = Sources[l * 6 + i];
	saa[pss] = 1;
	sourceLocalization(FM_EEG_105, saa, xhat, y);
	for (k = 0; k < ELECTRODES; k++) {
		xhat[k] = xhat[k] > 0.0 ? xhat[k] : -xhat[k];
	}
	double s = max(xhat);
	for (k = 0; k < ELECTRODES; k++) {
		A[k] = xhat[k] > 0.1 * s;
	}

	memcpy(finalR + (i * SIZE), A, SIZE * sizeof(int));

	int cc = c[i];
	for (n = 0; n < SIZE; n++) {
		double xdist = GridLoc[cc * 3] - GridLoc[n * 3];
		double ydist = GridLoc[cc * 3 + 1] - GridLoc[n * 3 + 1];
		double zdist = GridLoc[cc * 3 + 2] - GridLoc[n * 3 + 2];
		
		double distancemat = sqrt(xdist * xdist + ydist * ydist + zdist * zdist);
		dd[i * SIZE + n] = distancemat;
	}

	multiply(finalR, dd, Multi, i);

	memcpy(M, Multi + (i * SIZE), SIZE * sizeof(double));
	sort(M);
	int idx = firstZero(M);
	double val50 = M[idx / 2];
	double val75 = M[idx / 4];
	double val25 = M[3 * idx / 4];

	FN50[l * 6 + i] = val50;
	FN75[l * 6 + i] = val75;
	FN25[l * 6 + i] = val25;

}

int main(void)
{
	double **FM_EEG_105_2d = init_double_matrix(SIZE, ELECTRODES);
	fill_double_matrix(SIZE, ELECTRODES, FM_EEG_105_2d, "FM_EEG_105.txt");
	double **GridLoc_2d = init_double_matrix(SIZE, 3);
	fill_double_matrix(SIZE, 3, GridLoc_2d, "GridLoc.txt");

	int Sources_2d[6][8] = 
		{
		{51, 1, 301, 351, 151, 102, 251, 201},
		{52, 2, 302, 352, 152, 102, 252, 202},
		{53, 3, 303, 353, 153, 103, 253, 203},
		{54, 4, 304, 354, 154, 104, 254, 204},
		{55, 5, 305, 355, 155, 105, 255, 205},
		{56, 6, 306, 356, 156, 106, 256, 206},
		};

	//declare 3 arrays for gpu and 3 arrays for host
	double* FM_EEG_105;
	double* GridLoc;
	int* Sources;
	double* DEVICE_FM_EEG_105;
	double* DEVICE_GridLoc;
	int* DEVICE_Sources;
	// arrays on host to fill from error_metric-CUDA routine
	double* FN25;
	double* FN50;
	double* FN75;
	double* DEVICE_FN25;
	double* DEVICE_FN50;
	double* DEVICE_FN75;

	FM_EEG_105 = (double*)malloc(sizeof(double) * ELECTRODES * SIZE);
	GridLoc = (double*)malloc(sizeof(double) * SIZE * 3);
	Sources = (int*)malloc(sizeof(int) * 6 * 8);

	FN25 = (double*)malloc(sizeof(double) * 6 * 8);
	FN50 = (double*)malloc(sizeof(double) * 6 * 8);
	FN75 = (double*)malloc(sizeof(double) * 6 * 8);

	// malloc 3 arrays on gpu
	cudaMalloc(&DEVICE_FM_EEG_105, sizeof(double) * ELECTRODES * SIZE); 
	cudaMalloc(&DEVICE_GridLoc, sizeof(double) * SIZE * 3); 
	cudaMalloc(&DEVICE_Sources, sizeof(int) * 6 * 8);
	cudaMalloc(&DEVICE_FN25, sizeof(int) * 6 * 8);
	cudaMalloc(&DEVICE_FN50, sizeof(int) * 6 * 8);
	cudaMalloc(&DEVICE_FN75, sizeof(int) * 6 * 8);

	// initialize 3 host arrays
	int a, b;
	for (a = 0; a < SIZE; a++) {
		for (b = 0; b < ELECTRODES; b++) {
			FM_EEG_105[b * SIZE + a] = FM_EEG_105_2d[a][b];
		}
	}

	for (a = 0; a < SIZE; a++) {
		for (b = 0; b < 3; b++) {
			GridLoc[a * 3 + b] = GridLoc_2d[a][b];
		}
	}

	for (a = 0; a < 8; a++) {
		for (b = 0; b < 6; b++) {
			Sources[a * 6 + b] = Sources_2d[b][a];
		}
	}

	cudaMemcpy(DEVICE_FM_EEG_105, FM_EEG_105, sizeof(double) * ELECTRODES * SIZE, cudaMemcpyHostToDevice);
	cudaMemcpy(DEVICE_GridLoc, GridLoc, sizeof(double) * SIZE * 3, cudaMemcpyHostToDevice);
	cudaMemcpy(DEVICE_Sources, Sources, sizeof(int) * 6 * 8, cudaMemcpyHostToDevice);

	error_metric<<<8, 6>>>(DEVICE_FM_EEG_105, DEVICE_GridLoc, DEVICE_Sources, DEVICE_FN25, DEVICE_FN50, DEVICE_FN75);

	cudaMemcpy(FN25, DEVICE_FN25, malloc(sizeof(double) * 6 * 8), cudaMemcpyDeviceToHost);
	cudaMemcpy(FN50, DEVICE_FN50, malloc(sizeof(double) * 6 * 8), cudaMemcpyDeviceToHost);
	cudaMemcpy(FN75, DEVICE_FN75, malloc(sizeof(double) * 6 * 8), cudaMemcpyDeviceToHost);

	int x;
	printf("FN25 : \n");
	for (x = 0; x < 48; x++)
	{
		printf("%lf ", FN25[x]);
	}
	printf("\n");
	printf("FN50 : \n");
	for (x = 0; x < 48; x++)
	{
		printf("%lf ", FN50[x]);
	}
	printf("\n");
	printf("FN75 : \n");
	for (x = 0; x < 48; x++)
	{
		printf("%lf ", FN75[x]);
	}
	printf("\n");


	cudaFree(DEVICE_FM_EEG_105);
	cudaFree(DEVICE_GridLoc);
	cudaFree(DEVICE_Sources);
	cudaFree(DEVICE_FN25);
	cudaFree(DEVICE_FN50);
	cudaFree(DEVICE_FN75);
	free(FM_EEG_105);
	free(GridLoc);
	free(Sources);
	free(FN25);
	free(FN50);
	free(FN75);
}