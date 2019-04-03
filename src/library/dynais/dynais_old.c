/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, transparent and ligth-weigth solution for
*	Energy management.
*
*    	It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017  
*	BSC Contact 	mailto:ear-support@bsc.es
*	Lenovo contact 	mailto:hpchelp@lenovo.com
*
*	EAR is free software; you can redistribute it and/or
*	modify it under the terms of the GNU Lesser General Public
*	License as published by the Free Software Foundation; either
*	version 2.1 of the License, or (at your option) any later version.
*	
*	EAR is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*	Lesser General Public License for more details.
*	
*	You should have received a copy of the GNU Lesser General Public
*	License along with EAR; if not, write to the Free Software
*	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*	The GNU LEsser General Public License is contained in the file COPYING	
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
#include <string.h>
#include <library/dynais/dynais_old.h>
#include <immintrin.h> // -mavx -mfma


// Local defines
#define AVX_512 _FEATURE_AVX512F
#define AVX_256 _FEATURE_AVX2
#define ALI64 __attribute__ ((aligned (64)))

// General indexes.
unsigned int _levels;
unsigned int _window;

// Per level data
unsigned long *samples_vec[MAX_LEVELS];
unsigned int  *zeros_vec[MAX_LEVELS];
unsigned int  *sizes_vec[MAX_LEVELS];
unsigned int  *indes_vec[MAX_LEVELS];

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

int dynais_old_init(unsigned short window_s, unsigned short levels_s)
{
	unsigned long *p_smpls;
	unsigned int *p_zeros, *p_sizes, *p_indes;
	int mem_res1, mem_res2, mem_res3, mem_res4;
	int i, k;

	unsigned int window = (unsigned int) window_s;
	unsigned int levels = (unsigned int) levels_s;

	unsigned int multiple = window / 16;
	window = 16 * (multiple + 1);

	#if AVX_512
	#define __dyn_size sizeof(__m512i)
	#elif AVX_256
	#define __dyn_size sizeof(__m256i)
	#else
	#define __dyn_size sizeof(long)
	#endif

	_window = (window < METRICS_WINDOW) ? window : METRICS_WINDOW;
	_levels = (levels < MAX_LEVELS) ? levels : MAX_LEVELS;

	// Allocating memory
	mem_res1 = posix_memalign((void *) &p_smpls, __dyn_size, sizeof(long) * _window * levels);
	mem_res3 = posix_memalign((void *) &p_sizes, __dyn_size, sizeof(int)  * _window * levels);
	mem_res2 = posix_memalign((void *) &p_zeros, __dyn_size, sizeof(int)  * (_window + 8) * levels);
	mem_res4 = posix_memalign((void *) &p_indes, __dyn_size, sizeof(int)  * (_window + 8) * levels);

	//EINVAL = 22, ENOMEM = 12
	if (mem_res1 != 0 || mem_res2 != 0 || mem_res3 != 0 || mem_res4 != 0) {
		return -1;
	}

	// Setting memory to 0
	memset((void *) p_smpls, 0, sizeof(long) * _window * levels);
	memset((void *) p_sizes, 0, sizeof(int)  * _window * levels);
	memset((void *) p_zeros, 0, sizeof(int)  * (_window + 8) * levels);
	memset((void *) p_indes, 0, sizeof(int)  * (_window + 8) * levels);

	// Storing levels references
	for (i = 0; i < levels; ++i)
	{
		level_limit[i] = 0;
		level_index[i] = 0;
		level_result[i] = NO_LOOP;
		level_length[i] = 0;

		// Normal window allocations
		samples_vec[i] = &p_smpls[i * _window];
		sizes_vec[i] = &p_sizes[i * _window];

		// Extended window allocations
		zeros_vec[i] = &p_zeros[i * (_window + 8)];
		indes_vec[i] = &p_indes[i * (_window + 8)];

		for (k = 0; k < _window; ++k) {
			indes_vec[i][k] = k-1;
		}
		indes_vec[i][0] = _window - 1;
	}

	return 0;
}

void dynais_old_dispose()
{
	free((void *) samples_vec[0]);
	free((void *) zeros_vec[0]);
	free((void *) sizes_vec[0]);
	free((void *) indes_vec[0]);
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
	unsigned int previous_length;
	unsigned int max_length = 0;
	unsigned int max_zeros = 0;
	  signed int result;
	unsigned int index;
	unsigned int limit;
	unsigned int mask;
	unsigned int i, k;

	unsigned int new_iteration;
	unsigned int end_loop;
	unsigned int new_loop;
	unsigned int in_loop;

	unsigned long *samples;
	unsigned int *zeros;
	unsigned int *indes;
	unsigned int *sizes;

	// Getting level data (index, limit...)
	index   		= level_index[level];
	limit   		= level_limit[level];
	previous_length = level_previous_length[level];

	samples 		= samples_vec[level];
	sizes   		= sizes_vec[level];
	zeros   		= zeros_vec[level];
	indes   		= indes_vec[level];

#if AVX_512 || AVX_256
	zeros[_window]  = zeros[0];
	indes[_window]  = indes[0];
	samples[index]  = 0;
	sizes[index]    = 0;
#endif

#if AVX_512
	static unsigned int shifts_array[16] ALI64 = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,15 };

	__m512i max_zeros_block;
	__m512i max_index_block;
	__m512i samples_block_1;
	__m512i samples_block_2;
	__m512i sizes_block;
	__m512i zeros_block;
	__m512i index_block;

	__m512i sample_replica;
	__m512i shift_replica;
	__m512i size_replica;
	__m512i one_replica;

	unsigned int mask_aux4;
	__mmask16 mask_block;
	__mmask16 mask_aux3;
	__mmask8 mask_aux1;
	__mmask8 mask_aux2;

	/*
	 * Phase 0, initialization
	 */

	sample_replica  = _mm512_set1_epi64(sample);
	size_replica    = _mm512_set1_epi32(size);
	shift_replica   = _mm512_load_si512((__m512i *) &shifts_array);
	one_replica     = _mm512_set1_epi32(1);
	max_zeros_block = _mm512_setzero_si512();
	max_index_block = _mm512_set1_epi32(0xFFFFFFFF);

	/*
	 * Phase 1, computation
	 */

	for (k = 0, i = 0; k < _window; k += 16, i += 1)
	{
		//
		samples_block_1 = _mm512_load_si512((__m512i *) &samples[k]);
		samples_block_2 = _mm512_load_si512((__m512i *) &samples[k + 8]);
		sizes_block     = _mm512_load_si512((__m512i *) &sizes[k]);

		//
		mask_aux1 = _mm512_cmp_epu64_mask(samples_block_1, sample_replica, _MM_CMPINT_EQ);
		mask_aux2 = _mm512_cmp_epu64_mask(samples_block_2, sample_replica, _MM_CMPINT_EQ);

		//
		mask_aux4 = ((int) mask_aux2 << 8) | ((int) mask_aux1);
		mask_block     = _mm512_int2mask(mask_aux4);

		//
		mask_block = _mm512_mask_cmp_epi32_mask(mask_block, sizes_block, size_replica, _MM_CMPINT_EQ);

		//
     	zeros_block = _mm512_load_si512((__m512i *) &zeros[k]);
		index_block = _mm512_load_si512((__m512i *) &indes[k]);

        // Round buffer
        zeros_block = _mm512_permutevar_epi32(shift_replica, zeros_block);
        index_block = _mm512_permutevar_epi32(shift_replica, index_block);

		zeros_block = _mm512_mask_set1_epi32(zeros_block, 0x8000, zeros[k + 16]);
		index_block = _mm512_mask_set1_epi32(index_block, 0x8000, indes[k + 16]);

		//
     	zeros_block = _mm512_maskz_add_epi32(mask_block, zeros_block, one_replica);

		_mm512_store_si512 ((__m512i *) &zeros[k], zeros_block);
        _mm512_store_si512 ((__m512i *) &indes[k], index_block);

		//
		mask_block = _mm512_cmp_epi32_mask(index_block, zeros_block, _MM_CMPINT_LT);

		if (mask_block == 0) continue;

		//
		zeros_block = _mm512_maskz_mov_epi32(mask_block, zeros_block);
		index_block = _mm512_maskz_mov_epi32(mask_block, index_block);

		// - max_zeros_provisional[i]  > max_zeros[i]: obligadamente reemplazable
		// - max_zeros_provisional[i] == max_zeros[i]: posiblemente reemplazable
		mask_block = _mm512_cmp_epu32_mask(max_zeros_block, zeros_block, _MM_CMPINT_LT);
		mask_aux3 = _mm512_cmp_epu32_mask(max_zeros_block, zeros_block, _MM_CMPINT_EQ);

		mask_aux3 = _mm512_mask_cmp_epu32_mask(mask_aux3, index_block, max_index_block, _MM_CMPINT_LT);
		mask_block = mask_block | mask_aux3;

		//
		max_zeros_block = _mm512_mask_mov_epi32(max_zeros_block, mask_block, zeros_block);
		max_index_block = _mm512_mask_mov_epi32(max_index_block, mask_block, index_block);
	}

	max_zeros      		= _mm512_reduce_max_epu32(max_zeros_block);
	zeros_block			= _mm512_set1_epi32(max_zeros);

	mask_block    		= _mm512_cmp_epu32_mask(max_zeros_block, zeros_block, _MM_CMPINT_EQ);
	max_length      	= _mm512_mask_reduce_min_epu32(mask_block, max_index_block);

	max_length 			= max_length & ((max_length == 0xFFFFFFFF) - 1);
#elif AVX_256
	// Static data
	static unsigned int shifts1_array[8] ALI64 = { 1,2,3,4,5,6,7,7 };
	static unsigned int shifts2_array[8] ALI64 = { 0,2,4,6,7,7,7,7 };

	unsigned int aux_array1[8] ALI64;
	unsigned int aux_array2[8] ALI64;

	// SIMD declarations
	__m256i max_zeros_block;
	__m256i max_index_block;
	__m256i samples_block_1;
	__m256i samples_block_2;
	__m256i sizes_block;
	__m256i zeros_block;
	__m256i index_block;
	__m256i mask_block;

	// Single value copy in block
	__m256i sample_copy;
	__m256i size_copy;
	__m256i one_copy;
	__m256i max_copy;

	// Workers
	__m256i shifts1;
	__m256i shifts2;
	__m256i aux1;
	__m256i aux2;
	__m256i aux3;

	// Macros
	__m256i macro_aux1;
	__m256i macro_aux2;

	#define _mm256_blendv_epi32(src1, src2, mask) \
	(__m256i) _mm256_blendv_ps((__m256) src1, (__m256) src2, (__m256) mask);

	#define _mm256_cmplt_epi32(src1, src2, dest) \
	{ \
		macro_aux1 = _mm256_cmpgt_epi32(src1, src2); \
		macro_aux2 = _mm256_cmpeq_epi32(src1, src2); \
		macro_aux1 = _mm256_or_si256(macro_aux1, macro_aux2); \
		dest = _mm256_andnot_si256(macro_aux1, max_copy); \
	}

	/*
	 * Phase 0, initialization
	 */

	max_zeros_block = _mm256_setzero_si256();
	max_index_block = _mm256_set1_epi32(0x7FFFFFFF);
	shifts2 		= _mm256_load_si256((__m256i *) shifts2_array);
	shifts1 		= _mm256_load_si256((__m256i *) shifts1_array);

	sample_copy 	= _mm256_set1_epi64x(sample);
	size_copy   	= _mm256_set1_epi32(size);
	one_copy    	= _mm256_set1_epi32(1);
	max_copy    	= _mm256_set1_epi8(0xFF);

	/*
	 * Phase 1, computation
	 */

	for (k = 0, i = 0; k < _window; k += 8, i += 1)
	{
		// Loading samples and sizes (comparing phase)
		samples_block_1 = _mm256_load_si256((__m256i *) &samples[k]);
		samples_block_2 = _mm256_load_si256((__m256i *) &samples[k + 4]);
		sizes_block     = _mm256_load_si256((__m256i *) &sizes[k]);

		// Getting a mask of equal samples
		aux1 = _mm256_cmpeq_epi64(samples_block_1, sample_copy);
		aux2 = _mm256_cmpeq_epi64(samples_block_2, sample_copy);
		aux1 = _mm256_permutevar8x32_epi32(aux1, shifts2);
		aux2 = _mm256_permutevar8x32_epi32(aux2, shifts2);
		aux1 = _mm256_permute2f128_si256(aux1, aux2, 32);

		// Getting a mask of equal sizes
		aux2 = _mm256_cmpeq_epi32(sizes_block, size_copy);

		// Getting a final mask of equal samples and sizes
		mask_block = _mm256_and_si256(aux1, aux2);

		// Loading zeros and indexes (circular phase)
		zeros_block = _mm256_load_si256((__m256i *) &zeros[k]);
		index_block = _mm256_load_si256((__m256i *) &indes[k]);

		// Moving data to the left
		zeros_block = _mm256_permutevar8x32_epi32(zeros_block, shifts1);
		index_block = _mm256_permutevar8x32_epi32(index_block, shifts1);
		zeros_block = _mm256_insert_epi32(zeros_block, zeros[k + 8], 7);
		index_block = _mm256_insert_epi32(index_block, indes[k + 8], 7);

		// Adding 1 to zeros block and cleaning
		zeros_block = _mm256_add_epi32(zeros_block, one_copy );
		zeros_block = _mm256_and_si256(zeros_block, mask_block);

		// Zeros and index storing
		_mm256_store_si256 ((__m256i *) &zeros[k], zeros_block);
		_mm256_store_si256 ((__m256i *) &indes[k], index_block);

		// Comparing zeros with index (When index is less than zero)
		_mm256_cmplt_epi32(index_block, zeros_block, mask_block);

		if (_mm256_testz_si256(mask_block, mask_block) == 1) continue;

		zeros_block = _mm256_and_si256(zeros_block, mask_block);
		index_block = _mm256_and_si256(index_block, mask_block);

		// zeros_block  > max_zeros_block: obligadamente reemplazable
		// zeros_block == max_zeros_block: posiblemente reemplazable
		aux1 = _mm256_cmpgt_epi32(zeros_block, max_zeros_block);
		aux2 = _mm256_cmpeq_epi32(zeros_block, max_zeros_block);

		// If index is less than the stored one but the zeros are equal (aux1)
	  /*aux3  =*/ _mm256_cmplt_epi32(index_block, max_index_block, aux3);

		// aux1 = masks of z greaters
		// aux2 = masks of z equals
		// aux3 = masks of i lessers
		aux2 = _mm256_and_si256(aux2, aux3);
		aux1 = _mm256_or_si256( aux1, aux2);

		//
		max_zeros_block = _mm256_blendv_epi32(max_zeros_block, zeros_block, aux1);
		max_index_block = _mm256_blendv_epi32(max_index_block, index_block, aux1);
	}

	_mm256_store_si256 ((__m256i *) aux_array1, max_zeros_block);
	_mm256_store_si256 ((__m256i *) aux_array2, max_index_block);

	for (i = 0; i < 8; ++i)
	{
		if (aux_array1[i] > max_zeros)
		{
			max_zeros  = aux_array1[i];
			max_length = aux_array2[i];
		}
	}
#else
	unsigned int m;

	// Indexing
	i = index + 1;
	i = i & ((i == _window) - 1);
	m = i + 1;
	m = m & ((m == _window) - 1);

	for (k = 1; k <= limit; ++k)
	{
		if (sample == samples[i] && size == sizes[i])
		{
			zeros[i] = zeros[m] + 1;

			if (zeros[i] > max_zeros && k < zeros[i])
			{
				max_length = k;
				max_zeros = zeros[i];
			}
		}
		zeros[m] = 0;

		i = i + 1;
		i = i & ((i == _window) - 1);
		m = m + 1;
		m = m & ((m == _window) - 1);
	}
#endif
	//
	samples[index] 	= sample;
	sizes[index] 	= size;

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
	sizes = sizes_vec[level];

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

	// DynAIS basic algorithm call.
	level_result[level] = dynais_basic(sample, size, level);

	// If new loop is detected, the sample and the size
	// is passed recursively to dynais_hierarchical.
	if (level_result[level] >= NEW_LOOP)
	{
		// Finding the correct size of the loop. If the level
		// is 0 then is not needed, because all level 0 inputs
		// size is 1, and then is equal to level size.
		if (level > 0) {
			new_sample_size = dynais_add_samples_size(level_length[level], level);
		} else {
			new_sample_size = level_length[level];
		}

		return dynais_hierarchical(sample, new_sample_size, level + 1);
	}

	// If is not a NEW_LOOP.
	return level;
}

int dynais_old(unsigned short sample_s, unsigned short *size, unsigned short *govern_level)
{
	int end_loop = 0;
	int in_loop = 0;
	int result;
	int reach;
	int l, ll;

	unsigned long sample = (unsigned long) sample_s;

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
			*govern_level = (unsigned short) l;
			*size = (unsigned short) level_length[l];

			// END_LOOP is detected above, it means that in this and
			// below levels the status is NEW_LOOP or END_NEW_LOOP,
			// because the only way to break a loop is with the
			// detection of a new loop.
			if (end_loop) {
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
						return IN_LOOP;
					}
				}
			}

			if (end_loop) {
				return END_NEW_LOOP;
			}

			return level_result[l];
		}
	}


	// In case no loop were found: NO_LOOP or END_LOOP
	// in level 0, size and government level are 0.
	*size = 0;
	*govern_level = 0;

	return -end_loop;
}
