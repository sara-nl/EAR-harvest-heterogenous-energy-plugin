#define _GNU_SOURCE
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sched.h>
#include <pthread.h>
#include <emmintrin.h> // -msse2
#include <immintrin.h> // -mavx -mfma
#include <ear_rapl_metrics.h>
#include <config.h>

int EAR_VERBOSE_LEVEL = 0;
static double A[8] = { M_PI, M_E, M_LN2, M_LN10, M_SQRT2, M_SQRT1_2, M_LOG2E, M_LOG10E };
static double B[8] = { M_SQRT2, M_SQRT1_2, M_LOG2E, M_LOG10E, M_PI, M_E, M_LN2, M_LN10 };
static double C[8] = { M_LN2, M_LN10, M_PI, M_E, M_LOG2E, M_LOG10E, M_SQRT2, M_SQRT1_2 };

static int iterations;
static int spinning = 1;
int test = 0;

#define PACK_OFFSET 1
//#define AVX_512 1

static void sse2_dp_add128(int iterations)
{
    static double D[2];
    __m128d *d;
    __m128d a;
    __m128d b;
    int i;

    a = _mm_load_pd(A);
    b = _mm_load_pd(B);
    d = (__m128d *) D;

    for (i = 0; i < iterations; i++) {
        *d = _mm_add_pd(a, b);
    }

    //printf("Result: %lf %lf\n", D[0], D[1]);
}

static void sse2_dp_mul128(int iterations)
{
    static double D[2];
    __m128d *d;
    __m128d a;
    __m128d b;
    int i;

    a = _mm_load_pd(A);
    b = _mm_load_pd(B);
    d = (__m128d *) D;

    for (i = 0; i < iterations; i++) {
        *d = _mm_mul_pd(a, b);
    }
}

static void fma_dp_fmadd128(int iterations)
{
    static double D[2];
    __m128d *d;
    __m128d a;
    __m128d b;
    __m128d c;
    int i;

    a = _mm_load_pd(A);
    b = _mm_load_pd(B);
    c = _mm_load_pd(C);
    d = (__m128d *) D;

    for (i = 0; i < iterations; i++) {
        *d = _mm_fmadd_pd(a, b, c);
    }
}

static void avx_dp_add256(int iterations)
{
    static double D[4];
    __m256d *d;
    __m256d a;
    __m256d b;
    int i;

    a = _mm256_load_pd(A);
    b = _mm256_load_pd(B);
    d = (__m256d *) D;

    for (i = 0; i < iterations; i++) {
        *d = _mm256_add_pd(a, b);
    }

    //printf("Result: %lf %lf %lf %lf\n", D[0], D[1], D[2], D[3]);
}

static void avx_dp_mul256(int iterations)
{
    static double D[4];
    __m256d *d;
    __m256d a;
    __m256d b;
    int i;

    a = _mm256_load_pd(A);
    b = _mm256_load_pd(B);
    d = (__m256d *) D;

    for (i = 0; i < iterations; i++) {
        *d = _mm256_mul_pd(a, b);
    }
}

static void fma_dp_fmadd256(int iterations)
{
    static double D[4];
    __m256d *d;
    __m256d a;
    __m256d b;
    __m256d c;
    int i;

    a = _mm256_load_pd(A);
    b = _mm256_load_pd(B);
    c = _mm256_load_pd(C);
    d = (__m256d *) D;

    for (i = 0; i < iterations; i++) {
        *d = _mm256_fmadd_pd(a, b, c);
    }
}

#if AVX_512
static void avx512_dp_add512(int iterations)
{
    static double D[8];
    __m512d *d;
    __m512d a;
    __m512d b;
    int i;

    a = _mm512_load_pd(A);
    b = _mm512_load_pd(B);
    d = (__m512d *) D;

    for (i = 0; i < iterations; i++) {
        *d = _mm512_add_pd(a, b);
    }

    //printf("Result: %lf %lf %lf %lf %lf %lf %lf %lf\n",
    //       D[0], D[1], D[2], D[3], D[4], D[5], D[6], D[7]);
}

static void avx512_dp_mul512(int iterations)
{
    static double D[8];
    __m512d *d;
    __m512d a;
    __m512d b;
    int i;

    a = _mm512_load_pd(A);
    b = _mm512_load_pd(B);
    d = (__m512d *) D;

    for (i = 0; i < iterations; i++) {
        *d = _mm512_mul_pd(a, b);
    }
}

static void avx512_dp_fmadd512(int iterations)
{
    static double D[8];
    __m512d *d;
    __m512d a;
    __m512d b;
    __m512d c;
    int i;

    a = _mm512_load_pd(A);
    b = _mm512_load_pd(B);
    c = _mm512_load_pd(C);
    d = (__m512d *) D;

    for (i = 0; i < iterations; i++) {
        *d = _mm512_fmadd_pd(a, b, c);
    }
}
#endif

static void set_affinity(int core)
{
    cpu_set_t set;
    CPU_SET(core, &set);
    sched_setaffinity(0, sizeof(cpu_set_t), &set);
}

static void run_test(int index)
{
    switch(test) {
        case 0: sse2_dp_add128(iterations); break;
        case 1: sse2_dp_mul128(iterations); break;
        case 2: fma_dp_fmadd128(iterations); break;
        case 3: avx_dp_add256(iterations); break;
        case 4: avx_dp_mul256(iterations); break;
        case 5: fma_dp_fmadd256(iterations); break;
        #if AVX_512
        case 6: avx512_dp_add512(iterations); break;
        case 7: avx512_dp_mul512(iterations); break;
        case 8: avx512_dp_fmadd512(iterations); break;
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
        case 6: return "AVS-512 DP ADD512";
        case 7: return "AVS-512 DP MUL512";
        case 8: return "AVS-512 DP FMADD512";
        #endif
    }
}

int get_test_ops(int index)
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

static void *threaded_run_test(void *arg)
{
    int core = *((int *) arg);
    set_affinity(core);

    while (spinning == 1);
    run_test(test);

    pthread_exit(0);
}

int main (int argc, char *argv[])
{
    int *tbinds;
    pthread_t *tids;
    ull metrics[EAR_RAPL_EVENTS];
    long long start_time, exec_time;
    double time_s, flops_m, flops_x_watt;
    double power_ins, power_w, power_raw;
    double energy_nj, energy_j, energy_raw;
    int n_tests, n_threads, result;
    int i_socket, i_test, i;
    int num_ops;

    if (argc >= 2) {
        n_threads = atoi(argv[1]);
    } else {
        n_threads = get_nprocs();
    }

    if (argc == 3) {
        iterations = atoi(argv[2]);
    } else {
        iterations = 10000000;
    }

    // Options
    set_affinity(0);

    n_tests = 6;
    #if AVX_512
    n_tests = 9;
    #endif

    // Reserving memory
    tbinds = malloc(sizeof(pid_t) * n_threads);
    tids = malloc(sizeof(pthread_t) * n_threads);

    printf("CPU Model: \t\t%i\n", 85);
    printf("Running in core: \t%d/%d\n", sched_getcpu(), n_threads);
    printf("-------------------------- Working\n");
    printf("-------------------------- Summary\n");
    init_rapl_metrics();

    // Creating the threads
    for (i_test = 0; i_test < 6; ++i_test)
    {
        test = i_test;
        spinning = 1;
        energy_raw = 0;

        for (i = 1; i < n_threads; ++i)
        {
            tbinds[i] = i;
            result = pthread_create(&tids[i], NULL, threaded_run_test, (void *) &tbinds[i]);

            if (result != 0) {
                printf("Error %s\n", strerror(errno));
                exit(result);
            }
        }

        reset_rapl_metrics();
        start_rapl_metrics();
        start_time = PAPI_get_real_usec();

        spinning = 0;
        run_test(test);

        // Execution time in micro seconds
        exec_time = (PAPI_get_real_usec() - start_time);

        for (i = 1; i < n_threads; i++) {
            pthread_join(tids[i], NULL);
        }

        stop_rapl_metrics(metrics);

        for (i_socket = 0; i_socket < 2; ++i_socket) {
            // Energy per socket in nano juls
            energy_raw += (double) metrics[1 * 2 + i_socket];
        }

        // Energy (nJ -> J) = energy / 1 000 000 000
        // Time   (us -> s) = time   / 1 000 000

        // Energy per core (nJ)
        energy_nj = energy_raw / ((double) n_threads);
        // Energy per core (J)
        energy_j = energy_nj / 1000000000.0;
        // Exec time in seconds
        time_s = ((double) exec_time) / 1000000;
        // Power (W) = energy (J) / time (s)
        power_w = energy_j / time_s;
        // Power divided per iteration
        power_ins = power_w / ((double) iterations / time_s);
        // Power to uW
        power_ins = power_ins * 1000000.0;

        // Total number of operations
        num_ops = get_test_ops(test) * iterations;
        // Floating point operations per second
        flops_m = ((double) num_ops) / time_s;
        // Flops to Mflops
        flops_m = flops_m / 1000000.0;
        // Mflops x watt
        flops_x_watt = flops_m / power_w;

        printf("%s, ", get_test_name(test));
        printf("%0.3lf uW/call ", power_ins);
        printf("(%0.2lf J in %0.2lf s per core), ", energy_j, time_s);
        printf("%0.3lf Mflops, %0.3lf Mflops/Watt \n", flops_m, flops_x_watt);
    }

    return 0;
}