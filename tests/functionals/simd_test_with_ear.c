#define _GNU_SOURCE
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <emmintrin.h> // -msse2
#include <immintrin.h> // -mavx -mfma
#include <mpi.h>

int EAR_VERBOSE_LEVEL = 4;
static  double __attribute__((aligned(64))) A[8] = { M_PI, M_E, M_LN2, M_LN10, M_SQRT2, M_SQRT1_2, M_LOG2E, M_LOG10E };
static  double __attribute__((aligned(64))) B[8] = { M_SQRT2, M_SQRT1_2, M_LOG2E, M_LOG10E, M_PI, M_E, M_LN2, M_LN10 };
static  double __attribute__((aligned(64))) C[8] = { M_LN2, M_LN10, M_PI, M_E, M_LOG2E, M_LOG10E, M_SQRT2, M_SQRT1_2 };
static  double *D;

int ear_papi_init = 0;
int* my_omp_get_max_threads = NULL;
static ulong n_iterations;
static uint spinning = 1;
uint test = 0;

#define PACK_OFFSET 1
#define AVX_512 1

static void sse2_dp_add128()
{
    static double D[2];
    __m128d d;
    __m128d a;
    __m128d b;
    __m128d c;
    ulong i;


    a = _mm_load_pd(A);
    b = _mm_load_pd(B);

    for (i = 0; i < n_iterations; i++) {
        d = _mm_add_pd(a, b);
    }
    
    _mm_store_pd(D, d);
}

static void sse2_dp_mul128()
{
    static double D[2];
    __m128d d;
    __m128d a;
    __m128d b;
    __m128d c;
    ulong i;

    a = _mm_load_pd(A);
    b = _mm_load_pd(B);

    for (i = 0; i < n_iterations/2; i++) {
        c = _mm_mul_pd(a, b);
        d = _mm_mul_pd(c, d);
    }

    _mm_store_pd(D, d);
}

static void fma_dp_fmadd128()
{
    static double D[2];
    __m128d d;
    __m128d a;
    __m128d b;
    __m128d c;
    ulong i;

    a = _mm_load_pd(A);
    b = _mm_load_pd(B);
    c = _mm_load_pd(C);

    for (i = 0; i < n_iterations; i++) {
        d = _mm_fmadd_pd(a, b, c);
    }
    
    _mm_store_pd(D, d);
}

static void avx_dp_add256()
{
    static double D[4];
    __m256d d;
    __m256d a;
    __m256d b;
    __m256d c;
    ulong i;

    a = _mm256_load_pd(A);
    b = _mm256_load_pd(B);

    for (i = 0; i < n_iterations/2; i++) {
        c = _mm256_add_pd(a, b);
        d = _mm256_add_pd(c, d);
    }

    _mm256_store_pd(D, d);
}

static void avx_dp_mul256()
{
    static double D[4];
    __m256d d;
    __m256d a;
    __m256d b;
    ulong i;

    a = _mm256_load_pd(A);
    b = _mm256_load_pd(B);

    for (i = 0; i < n_iterations; i++) {
        d = _mm256_mul_pd(a, b);
    }
    
    _mm256_store_pd(D, d);
}

static void fma_dp_fmadd256()
{
    static double D[4];
    __m256d d;
    __m256d a;
    __m256d b;
    __m256d c;
    ulong i;

    a = _mm256_load_pd(A);
    b = _mm256_load_pd(B);
    c = _mm256_load_pd(C);

    for (i = 0; i < n_iterations; i++) {
        d = _mm256_fmadd_pd(a, b, c);
    }
    
    _mm256_store_pd(D, d);
}

#if AVX_512
static void avx512_dp_add512()
{
    static double D[8];
    __m512d a,b,c,d;
    ulong i;

    a = _mm512_load_pd(A);
    b = _mm512_load_pd(B);

    for (i = 0; i < n_iterations; i++) {
        d = _mm512_add_round_pd(a, b,_MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC);
    }

    _mm512_store_pd(D, d);
    
}

static void avx512_dp_mul512()
{
    static double D[8];
    __m512d d;
    __m512d a;
    __m512d b;
    ulong i;

    a = _mm512_load_pd(A);
    b = _mm512_load_pd(B);

    for (i = 0; i < n_iterations; i++) {
        d = _mm512_mul_round_pd(a, b,_MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC);
    }
    
    _mm512_store_pd(D, d);
}

static void avx512_dp_fmadd512()
{
    static double D[8];
    __m512d d;
    __m512d a;
    __m512d b;
    __m512d c;
    ulong i;

    a = _mm512_load_pd(A);
    b = _mm512_load_pd(B);
    c = _mm512_load_pd(C);

    for (i = 0; i < n_iterations; i++) {
        d = _mm512_fmadd_round_pd(a, b, c,_MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC);
    }

    _mm512_store_pd(D, d);
}
#endif


static void run_test(int index)
{
    int i;
    switch(test) {
        case 0: sse2_dp_add128(); break;
        case 1: sse2_dp_mul128(); break;
        case 2: fma_dp_fmadd128(); break;
        case 3: avx_dp_add256(); break;
        case 4: avx_dp_mul256(); break;
        case 5: fma_dp_fmadd256(); break;
        #if AVX_512
        case 6: avx512_dp_add512(); break;
        case 7: avx512_dp_mul512(); break;
        case 8: avx512_dp_fmadd512(); break;
        #endif
    }
}

char *get_test_name(int index)
{
    switch(index) {
        case 0: return "SSE2 DP ADD128";
        case 1: return "SSE2 DP MUL128";
        case 2: return "FMA DP FMADD128";
        case 3: return "AVX DP ADD256";
        case 4: return "AVX DP MUL256";
        case 5: return "FMA DP FMADD256";
        #if AVX_512
        case 6: return "AVX-512F DP ADD512";
        case 7: return "AVX-512F DP MUL512";
        case 8: return "AVX-512F DP FMADD512";
        #endif
    }
}

uint get_test_ops(int index)
{
    switch(index) {
        case 0: return (128 / 64);
        case 1: return (128 / 64);
        case 2: return (128 / 64) * 2;
        case 3: return (256 / 64);
        case 4: return (256 / 64);
        case 5: return (256 / 64) * 2;
        #if AVX_512
        case 6: return (512 / 64);
        case 7: return (512 / 64);
        case 8: return (512 / 64) * 2;
        #endif
    }
}


void usage()
{
    printf("Usage: simd_power_metrics n_iterations test\n");
    exit(1);
}

int main (int argc, char *argv[])
{

    int rank;
    if (argc != 3) {
        usage();
    }
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    // Options
    n_iterations = strtoul(argv[1], NULL, 10);
    test=atoi(argv[2]);
    int i;


    if (rank==0) printf("running test %d with %llu iterations\n",test,1000000*n_iterations);
    for (i=0;i<1000000;i++) run_test(test);


    MPI_Finalize();

    return 0;
}
