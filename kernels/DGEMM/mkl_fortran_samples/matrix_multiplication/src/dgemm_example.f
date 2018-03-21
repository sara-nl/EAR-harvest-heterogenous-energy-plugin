!==============================================================
!
! SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
! http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
!
! Copyright 2016-2017 Intel Corporation
!
! THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
! NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
! PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
!
! =============================================================
!*******************************************************************************
!   This example computes real matrix C=alpha*A*B+beta*C using Intel(R) MKL 
!   subroutine DGEMM, where A,B, and C are matrices and alpha and beta are 
!   double precision scalars. 
!
!   In this simple example, practices such as memory management, data alignment, 
!   and I/O that are necessary for good programming style and high MKL 
!   performance are omitted to improve readability.
!*******************************************************************************

      PROGRAM   MAIN

      IMPLICIT NONE
      include 'mpif.h'

      DOUBLE PRECISION ALPHA, BETA
      INTEGER          M, P, N, I, J
      PARAMETER        (M=18432, P=18432, N=18432)
      DOUBLE PRECISION A(M,P), B(P,N), C(M,N)
      
      INTEGER error,rank,size
      INTEGER NUM_THREADS
      INTEGER MKL_GET_MAX_THREADS

      PRINT *, "This example computes real matrix C=alpha*A*B+beta*C"
      PRINT *, "using Intel(R) MKL function dgemm, where A, B, and C"
      PRINT *, "are matrices and alpha and beta are double precision "
      PRINT *, "scalars"
      PRINT *, ""

      PRINT *, "Initializing data for matrix multiplication C=A*B for "
      PRINT 10, " matrix A(",M," x",P, ") and matrix B(", P," x", N, ")"
10    FORMAT(a,I5,a,I5,a,I5,a,I5,a)
      PRINT *, ""
      call mpi_init(error)
      call mpi_comm_size(MPI_COMM_WORLD, size, error)
      call mpi_comm_rank(MPI_COMM_WORLD, rank, error)

      NUM_THREADS = MKL_GET_MAX_THREADS()

      PRINT 15," Running Intel(R) MKL with ",NUM_THREADS," threads"
 15   FORMAT(A,I2,A)
      PRINT *, ""

      ALPHA = 1.0 
      BETA = 0.0
      if (rank .eq. 0) then 
      PRINT *, "Intializing matrix data"
      PRINT *, ""
      endif
      DO I = 1, M
          DO J = 1, P
              A(I,J) = (I-1) * P + J
          END DO
      END DO

      DO I = 1, P
          DO J = 1, N
              B(I,J) = -((I-1) * N + J)
          END DO
      END DO

      DO I = 1, M
          DO J = 1, N
              C(I,J) = 0.0
          END DO
      END DO
      if (rank .eq. 0) then

      PRINT *, "Computing matrix product using Intel(R) MKL DGEMM "
      PRINT *, "subroutine"
      endif
      CALL DGEMM('N','N',M,N,P,ALPHA,A,M,B,P,BETA,C,M)
      DO I=1, 9
      CALL DGEMM('N','N',M,N,P,ALPHA,A,M,B,P,BETA,C,M)
      END DO
      if (rank .eq. 0) then
      PRINT *, "Computations completed."
      PRINT *, ""

      PRINT *, "Top left corner of matrix A:"
      PRINT 20, ((A(I,J), J = 1,MIN(P,6)), I = 1,MIN(M,6))
      PRINT *, ""

      PRINT *, "Top left corner of matrix B:"
      PRINT 20, ((B(I,J),J = 1,MIN(N,6)), I = 1,MIN(P,6))
      PRINT *, ""

 20   FORMAT(6(F12.0,1x))

      PRINT *, "Top left corner of matrix C:"
      PRINT 30, ((C(I,J), J = 1,MIN(N,6)), I = 1,MIN(M,6))
      PRINT *, ""

 30   FORMAT(6(ES12.4,1x))

      PRINT *, "Example completed."
      endif
      call mpi_barrier(MPI_COMM_WORLD, error)
      call mpi_finalize(error)
!      STOP 

      END
