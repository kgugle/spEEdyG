#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ELECTRODES 105
#define SIZE 15675

int compare_function(const void* a, const void* b) {
  double* x = (double*) a;
  double* y = (double*) b;

  if (*x > *y) return -1;
  else return 1;
}

void sourceLocalization(double* FM_EEG_105, double* saa, double* xhat, double* y) {
  return;
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
  double* FM_EEG_105 = (double*)malloc(sizeof(double) * ELECTRODES * SIZE);
  double* GridLoc = (double*)malloc(sizeof(double) * SIZE * 3);
  int* Sources = (int*)malloc(sizeof(int) * 6 * 8);

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
}
