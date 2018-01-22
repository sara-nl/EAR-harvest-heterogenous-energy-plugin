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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <ear_dynais/ear_dynais.h>

#if ANALYSIS
#include <nmmintrin.h>
#endif

// General indexes.
unsigned int _levels;
unsigned int _window;
unsigned int calls;

// Per level data
unsigned long *sample_vec[MAX_LEVELS];
unsigned int  *zero_vec[MAX_LEVELS];
unsigned int  *size_vec[MAX_LEVELS];

#if ANALYSIS
unsigned int *crc_vec[MAX_LEVELS];
#endif

unsigned int level_saved_size[MAX_LEVELS];
unsigned int level_size[MAX_LEVELS];
unsigned int level_limit[MAX_LEVELS];
unsigned int level_index[MAX_LEVELS];
  signed int level_result[MAX_LEVELS];

#if METRICS
// Time metrics
clock_t call_t1, elapsed_t1, elapsed_t2;
unsigned int call_accum;
double call_taccum;

// Summary metrics (not precise)
unsigned int summ_loops[METRICS_CHUNCK];
unsigned int summ_iters[METRICS_CHUNCK];
unsigned int summ_calls[METRICS_CHUNCK];
unsigned int summ_ends[METRICS_CHUNCK];

// Current loop metrics
unsigned int iter_calls[MAX_LEVELS];
unsigned int iter_ended[MAX_LEVELS];
unsigned int loop_iters[MAX_LEVELS];
unsigned int loop_calls[MAX_LEVELS];
unsigned int loop_size[MAX_LEVELS];
signed int loop_state[MAX_LEVELS];

//
//
// DynAIS Metrics
//
//

unsigned int dynais_metrics_get_loop_calls(unsigned int *level)
{
    int l;
    for (l = _levels - 1; l >= 0; --l) {
        if (loop_state[l] >= IN_LOOP && iter_ended[l]) {
            *level = l;
            return iter_calls[l];
        }
        else if (loop_state[l] >= IN_LOOP) {
            *level = l;
            return 0;
        }
    }
    *level = 0;
    return 0;
}

void dynais_metrics_level_close(unsigned int level)
{
    unsigned int calls_per_iter;
    unsigned int calls_per_loop;
    unsigned int sum_index = METRICS_WINDOW * level + loop_size[level];

    // When level is 0, we add two additional iterations, completing the
    // loop, because dynais() just starts counting at the third iteration.
    // This doesn't no affect higher level because for higher level loops the
    // precision is lost, because at the third iteration, we don't know how
    // many calls are included in this higher loop composed of lower loops.
    calls_per_loop = calls - loop_calls[level];
    if (level == 0) calls_per_loop += (2 * loop_size[level] + 1);

    // Finally
    calls_per_iter = calls_per_loop / loop_iters[level];
    summ_calls[sum_index] += calls_per_loop;
}

void dynais_metrics_sample_add(unsigned long value, int status, unsigned int size, unsigned int level)
{
    unsigned int loop_index = METRICS_WINDOW * level + size;
    unsigned int sum_index = METRICS_WINDOW * level + loop_size[level];

    // End of the loop, then loop data must be processed.
    if (status == END_LOOP || status == END_NEW_LOOP) {
        dynais_metrics_level_close(level);
        iter_ended[level] = 0;
    }

    //
    if (status == NEW_ITERATION && !iter_ended[level]) {
        iter_calls[level] = calls - loop_calls[level] + 1;
        iter_ended[level] = 1;
    }

    // The number of lowest level calls (the MPI calls).
    calls += (level == 0);

    // Current loop iterations.
    loop_state[level] = status;
    loop_iters[level] += (status == NEW_ITERATION);

    // Summary metrics.
    summ_ends[sum_index]   += (status == END_LOOP || status == END_NEW_LOOP);
    summ_iters[loop_index] += (status == NEW_ITERATION) + 3 * (status >= NEW_LOOP);
    summ_loops[loop_index] += (status >= NEW_LOOP);

    // New loop, some data must be reset.
    if (status >= NEW_LOOP) {
        loop_size[level] = size;
        loop_calls[level] = calls;
        loop_iters[level] = 3;
        iter_ended[level] = 0;
    }
}

void dynais_metrics_print()
{
    unsigned int level, index, i;
    unsigned int calls_per_iter;
    unsigned int calls_estimate;
    int status;

    printf("SUMMARY METRICS (no precision, just averages)\n");
    for (level = 0; level < _levels; ++level)
    {
        // If the last status of the level shows that the
        // loop in that level isn't finished, it has to
        // to be closed before to complete statistics.
        status = loop_state[level];

        if (status != END_LOOP && status != NO_LOOP) {
            dynais_metrics_level_close(level);
        }

        printf("LEVEL %i\n", level);
        index = METRICS_WINDOW * level;

        for (i = 0; i < METRICS_WINDOW; ++i, ++index)
        {
            if (summ_loops[index] > 0)
            {
                calls_estimate = summ_calls[index];

                if (level > 0) {
                    calls_estimate = calls_estimate / (summ_iters[index] - 2);
                    calls_estimate = summ_calls[index] + (calls_estimate * 2);
                }

                calls_per_iter = calls_estimate / summ_iters[index];

                printf("- Size: %u, n/e: %u/%u", i, summ_loops[index], summ_ends[index]);
                printf(", iters: %u, MPI_det/est: %u/%u", summ_iters[index], summ_calls[index], calls_estimate);
                printf(", est_calls_iter: %u\n", calls_per_iter);
            }
        }
    }
}
#endif

#if ANALYSIS
unsigned int dynais_loop_data(unsigned long *_samples,
                              unsigned int *_sizes,
                              unsigned int *_crcs,
                              unsigned int level,
                              unsigned int size)
{
    unsigned long *samples;
    unsigned int *sizes;
    unsigned int *crcs;
    unsigned int limit;
    unsigned int index;
    unsigned int i, k;

    //
    index = level_index[level];
    limit = level_limit[level];

    //
    samples = sample_vec[level];
    sizes = size_vec[level];
    crcs = crc_vec[level];

    // Indexing
    i = index + 2;
    i = (i & ((i >= _window) - 1)) + (i > _window);

    // Copying the content
    for (k = 0; k < size; ++k)
    {
        _samples[k] = samples[i];
        _sizes[k] = sizes[i];
        _crcs[k] = crcs[i];

        i = i + 1;
        i = i & ((i == _window) - 1);
    }

    return k;
}
#endif

//
//
// Dynamic Application Iterative Structure Detection (DynAIS)
//
//

int dynais_init(unsigned int window, unsigned int levels)
{
    unsigned long *p_data;
    unsigned int *p_zero, *p_size, *p_crc;
    int mem_res1, mem_res2, mem_res3, mem_res4 = 0;
    int i;

    _window = (window < METRICS_WINDOW) ? window : METRICS_WINDOW;
    _levels = (levels < MAX_LEVELS) ? levels : MAX_LEVELS;
    mem_res1 = posix_memalign((void *) &p_data, sizeof(long), sizeof(long) * window * levels);
    mem_res2 = posix_memalign((void *) &p_zero, sizeof(long), sizeof(int) * window * levels);
    mem_res3 = posix_memalign((void *) &p_size, sizeof(long), sizeof(int) * window * levels);

    #if ANALYSIS
    mem_res4 = posix_memalign((void *) &p_crc, sizeof(long), sizeof(int) * window * levels);
    #endif

    //EINVAL = 22, ENOMEM = 12
    if (mem_res1 != 0 || mem_res2 != 0 || mem_res3 != 0 || mem_res4 != 0) {
        return -1;
    }

    for (i = 0; i < levels; ++i)
    {
        level_limit[i] = 0;
        level_index[i] = 0;
        level_result[i] = NO_LOOP;
        level_size[i] = 0;

        sample_vec[i] = &p_data[i * window];
        zero_vec[i] = &p_zero[i * window];
        size_vec[i] = &p_size[i * window];

        #if ANALYSIS
        crc_vec[i] = &p_crc[i * window];
        #endif
    }

    return 0;
}

void dynais_dispose()
{
    free((void *) sample_vec[0]);
    free((void *) zero_vec[0]);
    free((void *) size_vec[0]);

    #if ANALYSIS
    free((void *) crc_vec[0]);
    #endif
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
int dynais_basic(unsigned long sample, unsigned int *size, unsigned int level)
{
    unsigned long *samples;
    unsigned int *zeros, *sizes, *crcs;
    unsigned int index, limit, last_size, current_size;

    unsigned int end_loop, in_loop, new_loop, new_iteration;
    unsigned int max_size = 0, max_zeros = 0;
    unsigned int i, k, m;
    unsigned int mask;
    int result = 0;

    // Getting level data (index, limit...)
    index = level_index[level];
    limit = level_limit[level];
    last_size = level_saved_size[level];

    samples = sample_vec[level];
    zeros = zero_vec[level];
    sizes = size_vec[level];

    #if ANALYSIS
    crcs = crc_vec[level];
    #endif

    current_size = *size;
    sizes[index] = current_size;
    samples[index] = sample;

    // Indexing
    i = index + 1;
    i = i & ((i == _window) - 1);
    m = i + 1;
    m = m & ((m == _window) - 1);

    for (k = 1; k <= limit; ++k)
    {
        if (sample == samples[i] && current_size == sizes[i])
        {
            zeros[i] = zeros[m] + 1;

            if (zeros[i] > max_zeros && k < zeros[i])
            {
                max_size = k;
                max_zeros = zeros[i];

                // Caution, CRC code isn't implemented inter-level.
                // To do that there are two possibilities:
                // - Use the CRC code as upper level sample.
                // - Initialize CRC vector with the level number.
                #if ANALYSIS
                crcs[i] = crcs[m];
                #endif
            }
            #if ANALYSIS
            else {
                crcs[i] = _mm_crc32_u64(crcs[m], sample);
            }
            #endif
        }
        zeros[m] = 0;
        crcs[m] = 0;

        i = i + 1;
        i = i & ((i == _window) - 1);
        m = m + 1;
        m = m & ((m == _window) - 1);
    }

    // Mask is 0xFFFFFFFF (-1) when _index is greater than 0
    // or 0x00000000 when _index is equal to 0.
    mask = (index == 0) - 1;
    index = index + mask + (~mask & (_window - 1));

    // Mask is 0xFFFFFFFF (-1) when limit is inferior than
    // window -1, and 0x00000000 when is equal.
    mask = (limit == (_window - 1)) - 1;
    limit = ((limit + 1) & mask) + ((_window - 1) & ~mask);

    // STATUS is obtained. If max_size is greater than 0
    // and if max_zeros is greater than the max_size, which
    // means we are in a loop (equal is the last sample of the
    // second iteration).
    in_loop = max_size > 0 & (max_zeros > max_size);

    // We know that we are at the begining of a loop or an
    // iteration because the module between max_zeros and
    // max_size is 1. We also protect this operation
    // with the last boolean sample, because if max_size
    // were 0, the application would crash.
    new_iteration = in_loop && (max_size == 1 || (max_zeros % max_size) == 1);

    // If max_zeros is equal to max_size plus one, means
    // that we are exactly at the point that the loop starts
    // not the iteration.
    new_loop = new_iteration & (last_size != max_size);

    // If the last loop size is different than 0, which means
    // that we come from another loop, and the detected size is
    // different than the last saved size, it means that this is
    // an end loop status.
    end_loop = (last_size != max_size && last_size != 0);

    result = 0;
    result -= !in_loop & end_loop; // -1 = end lopp
    result += in_loop;       // 1 = in loop
    result += new_iteration; // 2 = new iteration
    result += new_loop;      // 3 = new loop
    result += new_loop & end_loop; // 4 = end and new loop

    // Saving the loop length if we are in a loop.
    if (result >= 3) last_size = max_size;
    else if (result <= 0) last_size = 0;

    // Length of the loop is obtained.
    *size = max_size;

    level_index[level] = index;
    level_limit[level] = limit;
    level_saved_size[level] = last_size;

    return result;
}

// Returns the highest level.
int dynais_hierarchical(unsigned long sample, unsigned int size, unsigned int level)
{
    if (level >= _levels) return level - 1;

    level_size[level] = size;
    // DynAIS basic algorithm call.
    level_result[level] = dynais_basic(sample, &level_size[level], level);

    // Metrics
    #if METRICS
    dynais_metrics_sample_add(sample, level_result[level], level_size[level], level);
    #endif

    // If new loop is detected, the sample and the size
    // is passed recursively to dynais_hierarchical.
    if (level_result[level] >= NEW_LOOP) {
        return dynais_hierarchical(sample, level_size[level], level + 1);
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

    #if METRICS
    call_accum += 1;
    call_t1 = clock();
    #endif

    // Hierarchical algorithm call. The maximum level
    // reached is returned. All those values were updated
    // by the basic DynAIS algorithm call.
    reach = dynais_hierarchical(sample, 1, 0);

    #if METRICS
    call_taccum += (double) (clock() - call_t1);
    #endif

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
            *size = level_size[l];

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