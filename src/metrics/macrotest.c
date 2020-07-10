#include <stdio.h>
#include <unistd.h>
#include <immintrin.h>
#include <metrics/cpi/cpi.h>
#include <metrics/flops/flops.h>
#include <metrics/frequency/cpu.h>
#include <metrics/bandwidth/bandwidth.h>

// /hpc/base/ctt/packages/compiler/gnu/9.1.0/bin/gcc -I ../ -g -o macrotest macrotest.c libmetrics.a ../common/libcommon.a -lpthread -ldl

#define ret(fun) \
	i = fun; \
	printf(#fun " returned %d\n", i);

void flops_a_saco()
{
    //__m512d d512_0;
    //__m512d d512_1;
    //__m512d d512_2;
    //__m512  s512_0;
    //__m512  s512_1;
    //__m512  s512_2;
    __m256d d256_0;
    __m256d d256_1;
    __m256d d256_2;
    __m256  s256_0;
    __m256  s256_1;
    __m256  s256_2;
    __m128d d128_0;
    __m128d d128_1;
    __m128d d128_2;
    __m128  s128_0;
    __m128  s128_1;
    __m128  s128_2;
    double d;
    float f;
    int i;

    for (i = 0, f = 0.0, d = 0.0; i < 10000000; ++i) // 10 000 000
    {
        //s512_2 = _mm512_add_ps(s512_0, s512_1);
        //d512_2 = _mm512_add_pd(d512_0, d512_1);
        s256_2 = _mm256_add_ps(s256_0, s256_1);
        d256_2 = _mm256_add_pd(d256_0, d256_1);
        d128_2 = _mm_add_pd(d128_0, d128_1);
        s128_2 = _mm_add_ps(s128_0, s128_1);
		f += 2.0f;
		d += 2.0;
	}
}

int main(int argc, char *argv[])
{
	// Bandwidth
	ullong cas[100];
	double dgbs;
	ullong gbs;
	// Flops
	llong ops[100];
	double dgfs;
	ullong gfs;
	// Frequency
	freq_cpu_t ef1;
	freq_cpu_t ef2;
	ulong ghz;
	// Cpi	
	llong instructions;
	llong cycles;
	// Indexes
	topology_t topo;
	state_t s;
	int igbs;
	int igfs;
	int ighz;
	int i;
		
	ret(topology_init(&topo));

	ret(init_uncores(0));
	ret(init_flops_metrics());
	ret(init_basic_metrics());
	ret(freq_cpu_init(&topo));

	ret(count_uncores());
	igbs = i;
	ret(get_number_fops_events());
	igfs = i;
	ret(freq_cpu_data_alloc(&ef1, empty, empty));
	ret(freq_cpu_data_alloc(&ef2, empty, empty));

	ret(reset_uncores());
	ret(start_uncores());

	reset_flops_metrics();
	start_flops_metrics();

	reset_basic_metrics();
	start_basic_metrics();

	ret(freq_cpu_read(&ef1));

	sleep(2);
	flops_a_saco();
	
	ret(stop_uncores(cas));

	stop_flops_metrics(&gfs, ops);
	stop_basic_metrics(&cycles, &instructions);

	ret(freq_cpu_read(&ef2));
	ret(freq_cpu_read_diff(&ef2, &ef1, empty, &ghz));

	fprintf(stderr, "BANDWIDTH: ");
	for (i = 0, gbs = 0; i < igbs; ++i) {
		fprintf(stderr, "%llu ", cas[i]);
		gbs += cas[i];
	}
	gbs = (gbs*64LLU) / (1024LLU*1024LLU*1024LLU*4LLU);
	fprintf(stderr, "\nbandwidth %0.4lf GB/s\n", (double) gbs);

	fprintf(stderr, "FLOPS: ");
	for (i = 0; i < igfs; ++i) {
		fprintf(stderr, "%llu ", ops[i]);
	}
	dgfs = (double) gfs;
	dgfs = dgfs / (1024.0*1024.0*1024.0);
	fprintf(stderr, "\nflops %0.4lf GF/s\n", dgfs);
	//
	fprintf(stderr, "INSTRUCTIONS/CYCLES %llu/%llu\n", cycles, instructions);
	//
	fprintf(stderr, "FREQUENCY %lu\n", ghz);

	return 0;
}
