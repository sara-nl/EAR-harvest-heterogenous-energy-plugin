//#define EAR_SHORT
//#define EAR_INT
//#define EAR_PAPI
#define SILENCE
#define SILENCE_LOOP
#define ELAPSED
//#define EXTENDED

#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <sys/time.h>
#include <common/output/verbose.h>
#include <common/string_enhanced.h>
#include <metrics/papi/stalls.h>
#include <metrics/papi/instructions.h>
#include <nmmintrin.h>

#ifdef EAR_SHORT
#include <library/dynais/dynais.h>
#else
#ifdef EAR_INT
#include <library/dynais/dynais_int.h>
#else
#include <library/dynais/dynais_old.h>
#endif
#endif

unsigned long mask = 0x000000000000FFFF;
unsigned long value[2];
unsigned long t_orig;
unsigned long t_star;
unsigned long c;

double loop_iter_one;
double loop_iter_mean;
double loop_duration;
double loop_elapsed;
unsigned int  loop_iterations;
unsigned long loop_c;

#ifdef EAR_SHORT
unsigned short sample;
unsigned short level;
unsigned short size;
  signed short result;

unsigned short loop_sample;
  signed short loop_result;
  signed short loop_level;
  signed short loop_size;
#else
#ifdef EAR_INT
unsigned int sample;
unsigned int level; 
unsigned int size;
  signed int result;

unsigned int loop_sample;
  signed int loop_result;
  signed int loop_level;
  signed int loop_size;
#else
unsigned long sample;
unsigned int level;
unsigned int size;
  signed int result;

unsigned long loop_sample;
   signed int loop_result;
   signed int loop_level;
   signed int loop_size;
#endif
#endif


void dynais_read_file(char *path, unsigned int panic)
{
	FILE *input;
	int result;
	int i = 0;
	int j = 0;

	if ((input = fopen(path, "r")) < 0) {
		perror("File read failed.");
		return;
	}

#ifndef SILENCE
#ifdef EXTENDED
	tprintf_init(stderr, STR_MODE_DEF, "11 11 9 9 16 9 6 6 6 6");
	tprintf("call beg.|||call end|||elapsed|||duration|||t. iter 1/mean|||sample|||level|||res.|||size|||iterations");
	tprintf("---------|||--------|||-------|||--------|||--------------|||------|||-----|||----|||----|||----------");
#else
	tprintf_init(stderr, STR_MODE_DEF, "12 12 25 6 6 6 6");
#endif
#endif

	//
	unsigned int *p_i = (unsigned int *) value;

	// Reading
	while (fread(&value, 2, sizeof(unsigned long), input) > 0 && c <= panic)
	{
		if (c == 0) {
			t_orig = value[1];
		}

		//
		t_star = value[1];
		p_i[0] = _mm_crc32_u32(p_i[0], p_i[1]);
		value[0] = value[0] & 0x000000000000FFFF;

		#ifdef EAR_SHORT
		sample = (unsigned short) value[0];
		#else
		#ifdef EAR_INT
		sample = (unsigned int) value[0];
		#else
		sample = value[0];
		#endif
		#endif

		result = dynais(sample, &size, &level);
		
	if (result == END_NEW_LOOP || result == END_LOOP) // && loop.level >= 1)
		{
			loop_duration = ((double) t_star - loop_duration) / 1000000.0;
			loop_iter_mean = (loop_duration / (double) loop_iterations);
			
			if (loop_iterations == 1) {
				loop_iter_one = 0.0;
			}
#ifndef SILENCE
			#ifdef EAR_SHORT
			#ifdef EXTENDED
			tprintf("%lu|||%lu|||%0.3lf|||%0.3lf|||%0.3lf %0.3lf|||%hu|||%hd|||%+hd|||%hd|||%d",
				loop_c, c, loop_elapsed, loop_duration, loop_iter_one, loop_iter_mean,
				loop_sample, loop_level, loop_result, loop_size, loop_iterations);
			#else
			tprintf("%lu|||%lu|||%hu|||%hd|||%+hd|||%hd|||%d",
				loop_c, c, loop_sample, loop_level, loop_result, loop_size, loop_iterations);
			#endif
			#else
			#ifdef EAR_INT
			tprintf("%lu|||%lu|||%u|||%hd|||%+hd|||%hd|||%d",
				loop_c, c, loop_sample, loop_level, loop_result, loop_size, loop_iterations);
			#else
			tprintf("%lu|||%lu|||%lu|||%d|||%+d|||%d|||%d",
				loop_c, c, loop_sample, loop_level, loop_result, loop_size, loop_iterations);
			#endif
			#endif
#endif
		}
		if (result >= NEW_LOOP)
		{
			loop_iterations = 0;
			loop_sample = sample;
			loop_result = result;
			loop_level  = level;
			loop_size   = size;
			loop_duration  = (double) t_star; 
			loop_elapsed   = ((double) t_star - (double) t_orig) / 1000000.0;
			loop_iter_one  = (double) t_star;
			loop_iter_mean = (double) t_star;
			loop_c      = c;

#ifndef SILENCE_LOOP
            #ifdef EAR_SHORT
            tprintf("%lu|||%lu|||%hu|||%hd|||%+hd|||%hd|||%d",
                loop_c, c, loop_sample, loop_level, loop_result, loop_size, loop_iterations);
            #else
            #ifdef EAR_INT
            tprintf("%lu|||%lu|||%u|||%hd|||%+hd|||%hd|||%d",
                loop_c, c, loop_sample, loop_level, loop_result, loop_size, loop_iterations);
            #else
            tprintf("%lu|||%lu|||%lu|||%d|||%+d|||%d|||%d",
                loop_c, c, loop_sample, loop_level, loop_result, loop_size, loop_iterations);
            #endif
            #endif
#endif
		}
		if (result >= NEW_ITERATION)
		{
			loop_iterations += 1;
			
			if (loop_iterations == 2 && (double) t_star > ((double) loop_iter_one + 0.0001)) { 
				loop_iter_one = (((double) t_star) - (double) loop_iter_one) / 1000000.0;
			}
		}
		++c;
		++i;
	}
	
	if (result != NO_LOOP)
	{
#ifndef SILENCE
		#ifdef EAR_SHORT
		#ifdef EXTENDED
	    tprintf("%lu|||%lu|||%0.3lf|||%0.3lf|||%0.3lf %0.3lf|||%hu|||%hd|||%+hd|||%hd|||%d",
                loop_c, c, loop_elapsed, loop_duration, loop_iter_one, loop_iter_mean,
                loop_sample, loop_level, loop_result, loop_size, loop_iterations);
		#else
		tprintf("%lu|||%lu|||%hu|||%hd|||%+hd|||%hd|||%d",
				loop_c, c, loop_sample, loop_level, loop_result, loop_size, loop_iterations);
		#endif
		#else
		#ifdef EAR_INT
		tprintf("%lu|||%lu|||%u|||%hd|||%+hd|||%hd|||%d",
				loop_c, c, loop_sample, loop_level, loop_result, loop_size, loop_iterations);
		#else
		tprintf("%lu|||%lu|||%lu|||%d|||%+d|||%d|||%d",
				loop_c, c, loop_sample, loop_level, loop_result, loop_size, loop_iterations);
		#endif
		#endif
#endif
	}
	loop_elapsed = ((double) t_star - (double) t_orig) / 1000000.0;
	
#ifndef SILENCE
	#ifdef EXTENDED
	tprintf("---------|||--------|||-------|||--------|||--------------|||------|||-----|||----|||----|||----------");
	verb(0, "Finished after %hu MPI calls and %0.3lf seconds of elapsed time", c, loop_elapsed);
	#endif
#endif
	fclose(input);
}

void dynais_validation(char *path, unsigned int panic)
{
	FILE *input;

	if ((input = fopen(path, "r")) < 0) {
		perror("File read failed.");
		return;
	}

	unsigned int *p_i = (unsigned int *) value;

	// Reading
	while (fread(&value, 2, sizeof(unsigned long), input) > 0 && c <= panic)
	{
		p_i[0] = _mm_crc32_u32(p_i[0], p_i[1]);
		value[0] = value[0] & 0x000000000000FFFF;

		#ifdef EAR_SHORT
		loop_sample = (unsigned short) value[0];
		#else
		#ifdef EAR_INT
		loop_sample = (unsigned int) value[0];
		#else
		loop_sample = value[0];
		#endif
		#endif

		result = dynais(loop_sample, &size, &level);

		#ifdef EAR_SHORT
		printf("%lu %hu %hd %hd %hd\n", c, loop_sample, result, level, size);
		#else
		#ifdef EAR_INT
		printf("%lu %u %hd %hd %hd\n", c, loop_sample, result, level, size);
		#else
		printf("%lu %lu %d %d %d\n", c, loop_sample, result, level, size);
		#endif
		#endif

		c++;
	}

	fclose(input);
}

int main(int argc, char **argv)
{
	struct timeval start, stop;
	long long insts, cycles;
	long long stalls;
	unsigned int max;
	double s = 0;

	verb_channel = STDOUT_FILENO;
	verb_channel = STDERR_FILENO;
	VERB_SET_FD(verb_channel);

	dynais_init(atoi(argv[1]), atoi(argv[2]));
	verb(0, "DynAIS type: %d", dynais_build_type());
	verb(0, "--------------");

	#ifdef EAR_PAPI
	init_stall_metrics();
	init_basic_metrics();

	reset_stall_metrics();
	reset_basic_metrics();

	start_basic_metrics();
	start_stall_metrics();
	#endif

	max = UINT_MAX;
	if (argc > 4) {
		max = (unsigned int) atoi(argv[4]);
	}

	gettimeofday(&start, NULL);

	dynais_read_file(argv[3], max);
	//dynais_validation(argv[3], max);
	//dynais_loop_2();

	gettimeofday(&stop, NULL);
	s = ((double) (stop.tv_usec - start.tv_usec) / 1000000) +
	     (double) (stop.tv_sec - start.tv_sec);
	s = s - 40.0; // READ & PRINTS (SUSPENSE)
#if 1
	verb(0, "--------------");
	verb(0, "DynAIS time %f secs (+read+prints)", s);
#endif

	#ifdef EAR_PAPI
	stop_basic_metrics(&cycles, &insts);
	stop_stall_metrics(&stalls);

	verb(0, "PAPI Instructions %llu", insts);
	verb(0, "PAPI Cycles %llu", cycles);
	verb(0, "PAPI Stalls %llu", stalls);
	#endif

	dynais_dispose();

	return 0;
}
