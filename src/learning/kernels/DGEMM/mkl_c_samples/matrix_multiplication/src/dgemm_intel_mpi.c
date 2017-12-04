/* C source code is found in dgemm_example.c */

#define min(x,y) (((x) < (y)) ? (x) : (y))

#include <stdio.h>
#include <stdlib.h>
#include "mkl.h"
#include <mpi.h>

double stoptime(void) {
  struct timeval t;
  gettimeofday(&t,0);
  return (double) t.tv_sec + t.tv_usec/1000000.0;
}

#define LOOP_COUNT 10000

int main(int argc,char * argv[] )
{
  double *A, *B, *C;
  int m, n, k, i, j;
  double alpha, beta;

  double t;
  int size, rank;
  int matrix_size=0,r;


  if (argc==2)
    matrix_size=atoi(argv[1]);
  else
    matrix_size=10000;
  

  MPI_Init(&argc, &argv);  
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); /*-- entre 0 et size-1 --*/
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  mkl_set_num_threads(28/size);
  if (rank==0)
    printf ("\n This example computes real matrix C=alpha*A*B+beta*C using \n"
            " Intel® MKL function dgemm, where A, B, and  C are matrices and \n"
	    " alpha and beta are double precision scalars\n\nUsing %d threads\n",28/size);


  m = matrix_size, k = matrix_size, n = matrix_size;
  if(rank==0)
    printf (" Initializing data for matrix multiplication C=A*B for matrix \n"
	  " A(%ix%i) and matrix B(%ix%i)\n\n", m, k, k, n);
  alpha = 1.0; beta = 0.0;

  if (rank==0)
    printf (" Allocating memory for matrices aligned on 64-byte boundary for better \n"
	  " performance \n\n");
  A = (double *)mkl_malloc( m*k*sizeof( double ), 64 );
  B = (double *)mkl_malloc( k*n*sizeof( double ), 64 );
  C = (double *)mkl_malloc( m*n*sizeof( double ), 64 );

  if (A == NULL || B == NULL || C == NULL) {
    printf( "\n ERROR: Can't allocate memory for matrices. Aborting... \n\n");
    mkl_free(A);
    mkl_free(B);
    mkl_free(C);
    return 1;
  }

  if (rank==0)
    printf (" Intializing matrix data \n\n");
  for (i = 0; i < (m*k); i++) {
    A[i] = (double)(i+1);
  }

  for (i = 0; i < (k*n); i++) {
    B[i] = (double)(-i-1);
  }

  for (i = 0; i < (m*n); i++) {
    C[i] = 0.0;
  }

  if (rank==0)
    printf (" Computing matrix product using Intel® MKL dgemm function via CBLAS interface \n\n");
  
  t=stoptime();

  cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 
	      m, n, k, alpha, A, k, B, n, beta, C, n);
  for (r=0;r<LOOP_COUNT;r++){
  cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 
	      m, n, k, alpha, A, k, B, n, beta, C, n);
  }
  t=stoptime()-t;
  MPI_Barrier(MPI_COMM_WORLD);

  if (rank==0){
  printf("calculation time : %f\n",t);
  printf("gflops/s         : %f\n",((2.0*m*n*k)*1E-9)/t); 
  }


  mkl_free(A);
  mkl_free(B);
  mkl_free(C);

  MPI_Finalize();

  return 0;
}
