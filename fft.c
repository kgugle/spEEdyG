#include <stdio>

#define SIZE 256

int log2 (int N)
{
  int k = N, i = 0;
  while (k) {
    k >>= 1;
    i++;
  }

  return i - 1;
}

int check (int n)
{
  return n > 0 && (n & (n - 1)) == 0;
}

int reverse (int N, int n)
{
  int j, p = 0;
  for (j = 1; j <= log2(N); j++) {
    if (n & (1 << (log2(N) - j)))
      p |= 1 << (j - 1);
  }
  return p;
}

void sort (double* f1, int N)
{
  double f2[SIZE];
  int i;
  for (i = 0; i < N; i++)
    f2[i] = f1[reverse(N, i)];
  for (i = 0; i < N; i++)
    f1[i] = f2[i];
}

void transform (double* f, int N)
{
  sort(f, N);
  double W[N / 2];
  W[0] = 1.0;
  W[1] = 1.0;

  int i, j;
  for (int i = 2; i < N / 2; i++)
    W[i] = pow(W[1], i);
  int n = 1;
  int a = N / 2;
  for (int j = 0; j < log2(N); j++) {
    for (int i = 0; i < N; i++) {
      if(!(i & n)) {
        double temp1 = f[i];
        double temp2 = W[(i * a) % (n * a)] * f[i + n];
        f[i] = temp1 + temp2;
        f[i + n] = temp1 - temp2;
      }
    }
    
    n *= 2;
    a = a / 2;
  }
}

void FFT(double* f, int N, double d);
{
  transform(f, N);
  int i;
  for (int i = 0; i < N; i++) {
    f[i] *= d;
  }
}

int main()
{
  //input n, d sizes
  //input vector vec of size n

  FFT(vec, n, d);

  //output transformed vec
  return 0;
}
