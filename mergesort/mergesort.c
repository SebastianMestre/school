#include <omp.h>
#include <stdlib.h>
#include "timing.h"
#include "mergesort.h"
#include <stdio.h>
#include <unistd.h>

int arr[SZ];

static void merge(int A[], int m, int N) {
  // esta funcion asume que los primeros m elementos estan
  // ordenados y los ultimos N-m elementos estan ordenados
  if (N < 2)
    return;
  int lc = 0;
  int rc = m;
  int i;
  for (i = 0; i < N; i++) {
    if (rc == N || A[lc] < A[rc]) {
      A[i] = A[lc];
      lc++;
    } else {
      A[i] = A[rc];
      rc++;
    }
  }
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

static int validar(){
  int ordenado = 1, i;
  for (i = 0; i < SZ-1; i++) {
    ordenado &= (arr[i] <= arr[i+1]);
  }
  return ordenado;
}

int main() {
  int i;

  for (i = 0; i < SZ; i++) {
    arr[i] = random();
  }
  TIME_void(sec_mergesort(arr, SZ), NULL);
  printf("Ordenado: %d\n", validar());

  for (i = 0; i < SZ; i++) {
    arr[i] = random();
  }
  TIME_void(par_mergesort(arr, SZ), NULL);
  printf("Ordenado: %d\n", validar());

  for (i = 0; i < SZ; i++) {
    arr[i] = random();
  }
  TIME_void(for_mergesort(arr, SZ), NULL);
  printf("Ordenado: %d\n", validar());

  for (i = 0; i < SZ; i++) {
    arr[i] = random();
  }
  TIME_void(par_for_mergesort(arr, SZ), NULL);
  printf("Ordenado: %d\n", validar());
}
