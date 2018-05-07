#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "cycletimer.h"

#define ELECTRODES 522
#define SIZE 15765


double **init_double_matrix(int dim_x, int dim_y)
{
		double **p = malloc(sizeof(double) * dim_x);
		int i;
		for (i = 0; i < dim_x; i++) {
			p[i] = malloc(sizeof(double) * dim_y);
		}
		return p;
}

int **init_int_matrix(int dim_x, int dim_y)
{
		int **p = malloc(sizeof(int) * dim_x);
		int i;
		for (i = 0; i < dim_x; i++) {
			p[i] = malloc(sizeof(int) * dim_y);
		}
		return p;
}

void fill_double_matrix(int dim_x, int dim_y, double** matrix,  char *filename)
{
	int i;
	int j;

	FILE *file;
	file = fopen(filename, "r");
	if (file == NULL)
	{
		printf("file could not be opened");
		exit(1);
	}

	for (i = 0; i < dim_x; i++) {
		for (j = 0; j < dim_y; j++) {
			double number;
			fscanf(file, "%lf", &number);
			matrix[i][j] = number;

		}
	}   
}

void fill_int_matrix(int dim_x, int dim_y, int** matrix, char *filename)
{
	int i;
	int j;

	FILE *file;
	file = fopen(filename, "r");
	if (file == NULL)
	{
		printf("file could not be opened");
		exit(1);
	}

	for (i = 0; i < dim_x; i++) {
		for (j = 0; j < dim_y; j++) {
			int number;
			fscanf(file, "%d", &number);
			matrix[i][j] = number;

		}
	}   
}

int compare_function(const void* a, const void* b) {
	double* x = (double*) a;
	double* y = (double*) b;

	if (*x > *y) return -1;
	else return 1;
}

void sourceLocalization(double* FM_EEG_105, double* saa, double* xhat, double* y) {
	int i, j;

	for (i = 0; i < ELECTRODES; i++) {
		double value = 0.0;
		for (j = 0; j < SIZE; j++) {
			value += saa[j] * FM_EEG_105[i * SIZE + j];
		}
		y[i] = value;
	}
	
	for (i = 0; i < SIZE; i++) {
		double value = 0.0;
		for (j = 0; j < ELECTRODES; j++) {
			value += y[j] * FM_EEG_105[j * SIZE + i];
		}
		xhat[i] = value;
	}
}

double max(double* xhat) {
	double max = 0.0;
	int i;
	
	for (i = 0; i < SIZE; i++) {
		double val = xhat[i];
		max = val > max ? val : max;
	}

	return max;
}

void sort(double* M) {
	qsort(M, SIZE, sizeof(double), compare_function);
}

int firstZero(double* M) {
	int i;

	for (i = 0; i < SIZE; i++) {
		if (M[i] == 0.0) {
			return i;
		}
	}
	
	return i;
}

void multiply(int* finalR, double* dd, double* Multi) {
	int i, j;

	for (i = 0; i < 6; i++) {
		for (j = 0; j < SIZE; j++) {
			Multi[i * SIZE + j] = finalR[i * SIZE + j] * dd[i * SIZE + j];
		}
	}
}

int main() {
	double **FM_EEG_105_2d = init_double_matrix(SIZE, ELECTRODES);
	fill_double_matrix(SIZE, ELECTRODES, FM_EEG_105_2d, "FM_EEG_1378.txt");
	printf("Reached 1\n");
	double **GridLoc_2d = init_double_matrix(SIZE, 3);
	fill_double_matrix(SIZE, 3, GridLoc_2d, "GridLoc.txt");
	printf("Reached 2\n");

	int Sources_2d[6][8] = {
												{51, 1, 301, 351, 151, 102, 251, 201},
												{52, 2, 302, 352, 152, 102, 252, 202},
												{53, 3, 303, 353, 153, 103, 253, 203},
												{54, 4, 304, 354, 154, 104, 254, 204},
												{55, 5, 305, 355, 155, 105, 255, 205},
												{56, 6, 306, 356, 156, 106, 256, 206},
											};

	printf("Reached 3\n");
	double* FM_EEG_105 = (double*)malloc(sizeof(double) * ELECTRODES * SIZE);
	double* GridLoc = (double*)malloc(sizeof(double) * SIZE * 3);
	int* Sources = (int*)malloc(sizeof(int) * 6 * 8);
	printf("Reached 4\n");
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

	double* saa = (double*)calloc(sizeof(double), SIZE);
	double* xhat = (double*)calloc(sizeof(double), SIZE);
	double* y = (double*)calloc(sizeof(double), ELECTRODES);
	int* finalR = (int*)malloc(sizeof(int) * SIZE * 6);
	int* A = (int*)calloc(sizeof(int), SIZE);
	double* dd = (double*)malloc(sizeof(double) * SIZE * 6);
	int* c = Sources + (7 * 6);

	double* Multi = (double*)malloc(sizeof(double) * SIZE * 6);
	double* M = (double*)malloc(sizeof(double) * SIZE);

	double* FN50 = (double*)malloc(sizeof(double) * 6 * 8);
	double* FN25 = (double*)malloc(sizeof(double) * 6 * 8);
	double* FN75 = (double*)malloc(sizeof(double) * 6 * 8);
 
        double startTime = currentSeconds();

	int i, l, k, m, n;

	for (l = 0; l < 8; l++) {
		for (i = 0; i < 6; i++) {
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
		}
		
		for (m = 0; m < 6; m++) {
			int cc = c[m];
			for (n = 0; n < SIZE; n++) {
				double xdist = GridLoc[cc * 3] - GridLoc[n * 3];
				double ydist = GridLoc[cc * 3 + 1] - GridLoc[n * 3 + 1];
				double zdist = GridLoc[cc * 3 + 2] - GridLoc[n * 3 + 2];
				
				double distancemat = sqrt(xdist * xdist + ydist * ydist + zdist * zdist);
				dd[m * SIZE + n] = distancemat;
			}
		}

		multiply(finalR, dd, Multi);

		for (i = 0; i < 6; i++) {
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
	}

        double endTime = currentSeconds();

	double timeTaken = endTime - startTime;

	printf("%lf\n", timeTaken);

	int x;
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
	printf("FN25 : \n");
	for (x = 0; x < 48; x++)
	{
		printf("%lf ", FN25[x]);
	}
	printf("\n");

	free(FM_EEG_105);
	free(GridLoc);
	free(Sources);
	free(saa);
	free(xhat);
	free(y);
	free(finalR);
	free(A);
	free(dd);
	free(Multi);
	free(M);
	free(FN50);
	free(FN25);
	free(FN75);

	return 0;
}
