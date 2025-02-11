#include "timing.h"
#include "mergesort.h"

#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int arr[SZ];

static void merge(int A[], int m, int N) {
  // esta funcion asume que los primeros m elementos estan
  // ordenados y los ultimos N-m elementos estan ordenados
  if (N < 2)
    return;

  int* temp = malloc(m * sizeof(*temp));
  memcpy(temp, A, m * sizeof(*temp));

  int lc = 0;
  int rc = m;
  for (int i = 0; i < N && lc < m; i++) {
    if (rc == N || temp[lc] <= A[rc]) {
      A[i] = temp[lc];
      lc++;
    } else {
      A[i] = A[rc];
      rc++;
    }
  }

  free(temp);
}

static void _par_mergesort(int A[], int N) {
  if (N < 2)
    return;

  int m = N/2;
  #pragma omp task if (N > 1000)
  _par_mergesort(A, m);
  _par_mergesort(A+m, N-m);

  #pragma omp taskwait
  merge(A, m, N);
}

static void par_mergesort(int A[], int N) {
  #pragma omp parallel num_threads(W)
  {
    #pragma omp single
    _par_mergesort(A, N);
  }
}

static void for_mergesort(int A[], int N) {
  int seg;
  int i;
  int num_seg;
  for (seg = 1; seg <= N/2; seg <<= 1) {
    // mergear dos segmentos para formar uno del doble de largo
    num_seg = N / seg;
    for (i = 0; i < num_seg; i += 2) {
      if (i == num_seg-1 && N % seg > 0) {
        merge(A+i*seg, seg, N - i*seg);
      } else if (i < num_seg-1) {
        merge(A+i*seg, seg, 2*seg);
      }
    }
  }
  merge(A, seg, N);
}

static void par_for_mergesort(int A[], int N) {
  int seg;
  int i;
  int num_seg;
  for (seg = 1; seg <= N/2; seg <<= 1) {
    // mergear dos segmentos para formar uno del doble de largo
    num_seg = N / seg;
    #pragma omp parallel for
    for (i = 0; i < num_seg; i += 2) {
      if (i == num_seg-1 && N % seg > 0) {
        merge(A+i*seg, seg, N - i*seg);
      } else if (i < num_seg-1) {
        merge(A+i*seg, seg, 2*seg);
      }
    }
  }
  merge(A, seg, N);
}

static void sec_mergesort(int A[], int N) {
  if (N < 2)
    return;

  int m = N/2;
  sec_mergesort(A, m);
  sec_mergesort(A+m, N-m);

  merge(A, m, N);
}

static long long calcular_suma() {
  long long suma = 0;
  for (int i = 0; i < SZ; i++)
    suma += arr[i];
  return suma;
}

static long long suma_;
static int validar() {
  for (int i = 0; i+1 < SZ; i++)
    if (arr[i] > arr[i+1])
      return 0;
  return suma_ == calcular_suma();
}

static void iniciar() {
  for (int i = 0; i < SZ; i++)
    arr[i] = random();
  suma_ = calcular_suma();
}

int main() {
  int i;

  iniciar();
  TIME_void(sec_mergesort(arr, SZ), NULL);
  printf("Ordenado: %d\n", validar());

  iniciar();
  TIME_void(par_mergesort(arr, SZ), NULL);
  printf("Ordenado: %d\n", validar());

  iniciar();
  TIME_void(for_mergesort(arr, SZ), NULL);
  printf("Ordenado: %d\n", validar());

  iniciar();
  TIME_void(par_for_mergesort(arr, SZ), NULL);
  printf("Ordenado: %d\n", validar());
}
