/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

/*
 * Usage:
 * Just call dynais() passing the sample and the size of
 * this sample. It will be returned one of these states:
 *      END_LOOP
 *      NO_LOOP
 *      IN_LOOP
 *      NEW_ITERATION
 *      NEW_LOOP
 *      END_NEW_LOOP
 *
 * Errors:
 * A returned dynais_init() value different than 0,
 * means that something went wrong while allocating
 * memory.
 *
 * You HAVE to call dynais_init() method before with
 * the window length and the number of levels and
 * dynais_dispose() at the end of the execution.
 *
 * You can also set the METRICS define to 1 in case you
 * want some metrics at the end of the execution. Moreover,
 * you can increase MAX_LEVELS in case you need more or
 * METRICS_WINDOW, used to store the information of the
 * different loops found (sorted by size) because could
 * be necessary if you want to test big single level
 * windows.
 */

#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <library/dynais/dynais.h>

#include <x86intrin.h>
#include <emmintrin.h> // -msse2
#include <immintrin.h> // -mavx -mfma


static clock_t chrono_start[100];
static clock_t chrono_total[100];

#if 0
#define CHRONO_START(region) \
    chrono_start[region] = clock();
#define CHRONO_END(region) \
    chrono_total[region] += (clock() - chrono_start[region]);
#else
#define CHRONO_START(region)
#define CHRONO_END(region)
#endif

// General indexes.
unsigned int _levels;
unsigned int _window;

// Per level data
unsigned long *sample_vec[MAX_LEVELS];
unsigned int  *zero_vec[MAX_LEVELS];
unsigned int  *size_vec[MAX_LEVELS];

signed int   level_result[MAX_LEVELS];
unsigned int level_previous_length[MAX_LEVELS];
unsigned int level_length[MAX_LEVELS];
unsigned int level_limit[MAX_LEVELS];
unsigned int level_index[MAX_LEVELS];

//
//
// Dynamic Application Iterative Structure Detection (DynAIS)
//
//

int dynais_init(unsigned int window, unsigned int levels)
{
    unsigned long *p_data;
    unsigned int *p_zero, *p_size, *p_crc;
    int mem_res1, mem_res2, mem_res3;
    int i;

    _window = (window < METRICS_WINDOW) ? window : METRICS_WINDOW;
    _levels = (levels < MAX_LEVELS) ? levels : MAX_LEVELS;
    mem_res1 = posix_memalign((void *) &p_data, sizeof(long), sizeof(long) * window * levels);
    mem_res2 = posix_memalign((void *) &p_zero, sizeof(long), sizeof(int) * window * levels);
    mem_res3 = posix_memalign((void *) &p_size, sizeof(long), sizeof(int) * window * levels);

    //EINVAL = 22, ENOMEM = 12
    if (mem_res1 != 0 || mem_res2 != 0 || mem_res3 != 0) {
        return -1;
    }

    for (i = 0; i < levels; ++i)
    {
        level_limit[i] = 0;
        level_index[i] = 0;
        level_result[i] = NO_LOOP;
        level_length[i] = 0;

        sample_vec[i] = &p_data[i * window];
        zero_vec[i] = &p_zero[i * window];
        size_vec[i] = &p_size[i * window];
    }

    return 0;
}

void dynais_dispose()
{
    free((void *) sample_vec[0]);
    free((void *) zero_vec[0]);
    free((void *) size_vec[0]);

    #define NUM_REGIONS 7
	double CLOCKS_PER_USEC = (double) CLOCKS_PER_SEC / 1000000.0;
	double total_0, total, percent;
    clock_t others = 0;
    int i = 0;

    printf("reg     \t\tus \t\t%%\n");

	total_0 = (double) chrono_total[0] / CLOCKS_PER_USEC;

    while(i != NUM_REGIONS)
    {
		total = (double) chrono_total[i] / CLOCKS_PER_USEC;
        percent = (total / total_0) * 100.0;
		printf("REGION %d:\t\t%0.lf \t\t%0.2lf\n", i, total, percent);
		++i;
    }

    others += chrono_total[4];
    others += chrono_total[5];
    others = chrono_total[0] - others;
	total = (double) others / CLOCKS_PER_USEC;
	percent = (total / total_0) * 100.0;
    printf("REGION o:\t\t%0.lf\t\t%0.2lf\n", total, percent);
}

// How it works?
//    It's based on a two ring buffers. The data buffer
//    to store new data sample passed to the function,
//    and the zero buffer, which stores the equality
//    of the arriving data sample with the other data
//    samples.
//
//    When new data sample arrives, this sample is
//    compared with the previous passed samples. If
//    both are equal, we store 1 in it's indexed
//    position in the array, plus the following position
//    in the zero buffer, and the following position
//    Is reset to 0.
//
//    Why? Because this way we can accumulate the
//    differences. For instance:
//
//    - 1 is passed to dynais. Nothing happens.
//    - 2 is passed to dynais. Nothing happens.
//    - 3 is passed to dynais. Nothing happens.
//    - 1 is passed to dynais. Now the zero buffer is:
//      0 0 0 1, which means that the current 1
//      matches with the first 1.
//    - 2 is passed to dynais. The zero buffer is:
//      0 0 0 2 0, which means that the current 2
//      matches with the first 2, and from the last
//      dynais call, we have two coincidences.
//    - And so on.
//
//    But when we consider a new loop is found?
//    It's easy. When two iterations have passed,
//    and the first value of the third arrives.
//
//    In the source, this is obtained when number
//    of accumulated coincides stored in some index in
//    the zero buffer is greater than the index itself.
//
//    Why? Because it means that te current N samples
//    are found from index (index = N + 1) to index + N,
//    an thus the N sequence is repeating.
//
//    But why wee need two completed iterations to find
//    a new loop? Why isn't one enough? Ok, let's see
//    with an example:
//
//    Passing these samples: 1, 2, 3, 1, 2, 3, 1, the
//    buffers are:
//        data:  1 3 2 1 3 2 1
//        zeros: 0 0 0 4 0 0 1
//
//     As you can see, at the index 3, is when we have
//     a value that is greater than the index, 4 > 3.
//     But this fact just happens when two iterations
//     of length N have passed.
//
static int dynais_basic(unsigned long sample, unsigned int size, unsigned int level)
{
    unsigned long *samples;
    unsigned int *zeros, *sizes, *crcs;
    unsigned int end_loop, in_loop, new_loop, new_iteration;
    unsigned int index, limit, previous_length, sample_size;
    unsigned int max_length = 0, max_zeros = 0, mask;
    unsigned int i, j, k, m;
    int result = 0;

    CHRONO_START(1);

    // Getting level data (index, limit...)
    index = level_index[level];
    limit = level_limit[level];
    previous_length = level_previous_length[level];

    samples = sample_vec[level];
    zeros = zero_vec[level];
    sizes = size_vec[level];

    CHRONO_END(1);
    CHRONO_START(2);

#if 1
	samples[index] = 0;
	sizes[index] = 0;

	// window 504 (63 blocks of 8)
	static unsigned char locations[63];

	__m512i sample_dynamic;
	__m512i sample_static;
	__m256i size_dynamic;
	__m256i size_static;
	__mmask8 mmask;

    __m256i shifts_static;
   	__m256i zeros_dynamic;
    __m256i ones_static;

	sample_static = _mm512_set1_epi64(sample);
	size_static   = _mm256_set1_epi32(size);

	static unsigned int shifts[8]    = {  1,  2,  3,  4,  5,  6,  7,  7  };
	shifts_static = _mm256_load_si256((__m256i *) &shifts);
    ones_static   = _mm256_set1_epi32(1);

	unsigned int zeros_0 = zeros[0];

	for (k = 0, i = 0; k < _window; k += 8, i += 1)
	{
		//printf("B%d ", i);
		//for (j = k; j < k+8; ++j) printf("%lu ", samples[j]);

		// Se cargan los datos de samples y los de sizes. Los samples son de 64 bits,
		// mientras que los sizes son de 32. Próximamente se puede probar en dividir
		// esto en dos vectores, un primer escaneo de 8 en 8, y un posterior cómputo
		// de 16 en 16.
		sample_dynamic = _mm512_load_epi64(&samples[k]);
		size_dynamic   = _mm256_load_si256((__m256i *) &sizes[k]);

		// Con estas comparación se busca descartar un bloque de 8 muestras.
		mmask = _mm512_cmp_epu64_mask(sample_static, sample_dynamic, _MM_CMPINT_EQ);
		mmask = _mm256_mask_cmp_epi32_mask(mmask, size_static, size_dynamic, _MM_CMPINT_EQ);

		zeros_dynamic = _mm256_load_si256((__m256i *) &zeros[k]);
     	zeros_dynamic = _mm256_permutevar8x32_epi32(zeros_dynamic, shifts_static);
     	zeros_dynamic = _mm256_maskz_add_epi32(mmask, zeros_dynamic, ones_static);
     	_mm256_store_si256 ((__m256i *) &zeros[k], zeros_dynamic);

		// 
		locations[i] = ((char) mmask != 0);

		if (mmask & 0x80 && (k + 7) < 503) {
			zeros[k + 7] = zeros[k + 8] + 1;
		} else {
			zeros[k + 7] = 0;
		}
	}
	if (mmask & 0x80) {
		zeros[503] = zeros_0 + 1;
	} else {
		zeros[503] = 0;
	}

	// Indexing
    i = index + 1;
    i = i & ((i == _window) - 1);
    m = i + 1;
    m = m & ((m == _window) - 1);

   	for (k = 1; k <= limit; ++k)
    {
		//printf("%u ", zeros[i]);
		
		if (zeros[i] > max_zeros && k < zeros[i])
        {
			max_length = k;
 			max_zeros = zeros[i];
        }

        i = i + 1;
        i = i & ((i == _window) - 1);
        m = m + 1;
        m = m & ((m == _window) - 1);
    }

    samples[index] = sample; 
    sizes[index] = size;
#else
	sample_size = size;
    sizes[index] = size;
	samples[index] = sample;

    // Indexing
    i = index + 1;
    i = i & ((i == _window) - 1);
    m = i + 1;
    m = m & ((m == _window) - 1);

	for (k = 1; k <= limit; ++k)
	{
		//printf("(%lu,%u,%u,%u) ", samples[i], sizes[i], zeros[i], zeros[m]);
		if (sample == samples[i] && sample_size == sizes[i])
		{
			zeros[i] = zeros[m] + 1;

			if (zeros[i] > max_zeros && k < zeros[i])
			{
				max_length = k;
				max_zeros = zeros[i];
			}
		}
		zeros[m] = 0;

		//printf("%u ", zeros[i]);
		
		i = i + 1;
		i = i & ((i == _window) - 1);
		m = m + 1;
		m = m & ((m == _window) - 1);
	}
	
	//printf("max_length %d, max_zeros %d\n", max_length, max_zeros);	
#endif

    CHRONO_END(2);

    CHRONO_START(3);

    // Mask is 0xFFFFFFFF (-1) when _index is greater than 0
    // or 0x00000000 when _index is equal to 0.
    mask = (index == 0) - 1;
    index = index + mask + (~mask & (_window - 1));

    // Mask is 0xFFFFFFFF (-1) when limit is inferior than
    // window -1, and 0x00000000 when is equal.
    mask = (limit == (_window - 1)) - 1;
    limit = ((limit + 1) & mask) + ((_window - 1) & ~mask);

    // STATUS is obtained. If max_length is greater than 0
    // and if max_zeros is greater than the max_length, which
    // means we are in a loop (equal is the last sample of the
    // second iteration).
    in_loop = max_length > 0 & (max_zeros > max_length);

    // We know that we are at the begining of a loop or an
    // iteration because the module between max_zeros and
    // max_length is 1. We also protect this operation
    // with the last boolean sample, because if max_length
    // were 0, the application would crash.
    new_iteration = in_loop && (max_length == 1 || (max_zeros % max_length) == 1);

    // If max_zeros is equal to max_length plus one, means
    // that we are exactly at the point that the loop starts
    // not the iteration.
    new_loop = new_iteration & (previous_length != max_length);

    // If the last loop size is different than 0, which means
    // that we come from another loop, and the detected size is
    // different than the last saved size, it means that this is
    // an end loop status.
    end_loop = (previous_length != max_length && previous_length != 0);

    result = 0;
    result -= !in_loop & end_loop; // -1 = end lopp
    result += in_loop;       // 1 = in loop
    result += new_iteration; // 2 = new iteration
    result += new_loop;      // 3 = new loop
    result += new_loop & end_loop; // 4 = end and new loop

    // Saving the loop length if we are in a loop.
    if (result >= 3) previous_length = max_length;
    else if (result <= 0) previous_length = 0;

    // Length of the loop is obtained.
    level_length[level] = max_length;
    level_previous_length[level] = previous_length;
    level_index[level] = index;
    level_limit[level] = limit;

    CHRONO_END(3);

    return result;
}

static unsigned int dynais_add_samples_size(unsigned int amount, unsigned int level)
{
    unsigned int *sizes, limit, index, total_size = 0;
    unsigned int i, k;

    // Getting content index and limit
    index = level_index[level];
    limit = level_limit[level];

    // Pointing to the selected level vector of sizes
    sizes = size_vec[level];

    // Indexing
    i = index + 2;
    i = (i & ((i >= _window) - 1)) + (i > _window);

    // Reading size vector
    for (k = 0; k < amount; ++k)
    {
        total_size += sizes[i];

        i = i + 1;
        i = i & ((i == _window) - 1);
    }

    return total_size;
}

// Returns the highest level.
static int dynais_hierarchical(unsigned long sample, unsigned int size, unsigned int level)
{
    unsigned int new_sample_size;

    if (level >= _levels) {
        return level - 1;
    }

    CHRONO_START(4);

    // DynAIS basic algorithm call.
    level_result[level] = dynais_basic(sample, size, level);

    CHRONO_END(4);

    // If new loop is detected, the sample and the size
    // is passed recursively to dynais_hierarchical.
    if (level_result[level] >= NEW_LOOP)
    {
        CHRONO_START(5);

        // Finding the correct size of the loop. If the level
        // is 0 then is not needed, because all level 0 inputs
        // size is 1, and then is equal to level size.
        if (level > 0) {
            new_sample_size = dynais_add_samples_size(level_length[level], level);
        } else {
            new_sample_size = level_length[level];
        }

        CHRONO_END(5);

        return dynais_hierarchical(sample, new_sample_size, level + 1);
    }

    // If is not a NEW_LOOP.
    return level;
}

int dynais(unsigned long sample, unsigned int *size, unsigned int *govern_level)
{
    int end_loop = 0;
    int in_loop = 0;
    int result;
    int reach;
    int l, ll;

    CHRONO_START(0);

    // Hierarchical algorithm call. The maximum level
    // reached is returned. All those values were updated
    // by the basic DynAIS algorithm call.
    reach = dynais_hierarchical(sample, 1, 0);

    // Cleans didn't reach levels. Cleaning means previous
    // loops with a state greater than IN_LOOP have to be
    // converted to IN_LOOP and also END_LOOP have to be
    // converted to NO_LOOP.
    for (l = _levels - 1; l > reach; --l)
    {
        result = level_result[l];
        if (result > IN_LOOP) level_result[l] = IN_LOOP;
        if (result < NO_LOOP) level_result[l] = NO_LOOP;
    }

    // After cleaning, the highest IN_LOOP or greater
    // level is returned with its state data. If an
    // END_LOOP is detected before a NEW_LOOP,
    // END_NEW_LOOP is returned.
    for (l = _levels - 1; l >= 0; --l)
    {
        end_loop = end_loop | (level_result[l] == END_LOOP);

        if (level_result[l] >= IN_LOOP)
        {
            *govern_level = l;
            *size = level_length[l];

            // END_LOOP is detected above, it means that in this and
            // below levels the status is NEW_LOOP or END_NEW_LOOP,
            // because the only way to break a loop is with the
            // detection of a new loop.
            if (end_loop) {
                CHRONO_END(0);
                return END_NEW_LOOP;
            }

            // If the status of this level is NEW_LOOP, it means
            // that the status in all below levels is NEW_LOOP or
            // END_NEW_LOOP. If there is at least one END_NEW_LOOP
            // the END part have to be propagated to this level.
            if (level_result[l] == NEW_LOOP)
            {
                for (ll = l - 1; ll >= 0; --ll)
                {
                    end_loop |= level_result[ll] == END_NEW_LOOP;

                    if (level_result[ll] < NEW_LOOP)
                    {
                        CHRONO_END(0);
                        return IN_LOOP;
                    }
                }
            }

            if (end_loop) {
                CHRONO_END(0);
                return END_NEW_LOOP;
            }

            CHRONO_END(0);
            return level_result[l];
        }
    }

    CHRONO_END(0);

    // In case no loop were found: NO_LOOP or END_LOOP
    // in level 0, size and government level are 0.
    *size = 0;
    *govern_level = 0;
    return -end_loop;
}
