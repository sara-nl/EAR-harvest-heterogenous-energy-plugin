#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <x86intrin.h>
#include <emmintrin.h> // -msse2
#include <immintrin.h> // -mavx -mfma


// PAST		0, 10, 11, 12, 13, 14, 15,  1,
//			1,  2,  3,  4,  5,  6,  7,  8,
//			9, 10, 11, 12, 13, 14, 15,  1,
//			1,  2,  3,  4,  5,  6,  7,  8,
//			9, 10, 11, 12, 13, 14, 15, 16
			

unsigned long samples[40] = {  0,  9, 10, 11, 12, 13, 14, 15,
							   1,  1,  2,  3,  4,  5,  6,  7,
							   8,  9, 10, 11, 12, 13, 14, 15,
							   1,  1,  2,  3,  4,  5,  6,  7,
							   8,  9, 10, 11, 12, 13, 14, 15  };

unsigned int sizes[40]    = {  1,  1,  1,  1,  1,  1,  1,  1,
                               1,  1,  1,  1,  1,  1,  1,  1,
                               1,  1,  1,  1,  1,  1,  1,  1,
                               1,  1,  1,  1,  1,  1,  1,  1,
                               1,  1,  1,  1,  1,  1,  1,  1  };
								
unsigned int zeros[40]    = {  0,  0,  0,  0,  0,  0,  0,  0,
							   0,  0,  0,  0,  0,  0,  0,  0,
							   0, 23,  0,  0,  0,  0,  0,  0,
							   0,  0,  0,  0,  0,  0,  0,  0,
							   0,  7,  0,  0,  0,  0,  0,  0  };

unsigned int ones[8]      = {  1,  1,  1,  1,  1,  1,  1,  1  };
unsigned int shifts[8]    = {  1,  2,  3,  4,  5,  6,  7,  7  };

void print_buffer_uint(unsigned int *buffer, int positions)
{
	int i;

	for (i = 0; i < positions; ++i) {
		if ((i % 8)  == 0 && i != 0) printf("\n");
		printf("%u ", buffer[i]);
	}
	printf("\n");
}

int main(int argc, char **argv)
{
	unsigned long sample = 8;
	unsigned int size = 1;
	
	unsigned int limit = 40;
	unsigned int index = 0;
	int i, j, k;

	unsigned int max_length = 0;
	unsigned int max_zeros = 0;

    __m512i sample_comparer;
    __m512i sample_loader;
    __m256i size_comparer;
    __m256i size_loader;

	__m256i shifts_static;
	__m256i zeros_dynamic;
	__m256i ones_static;
    __mmask8 mmask;

    sample_comparer = _mm512_set1_epi64(sample);
    size_comparer   = _mm256_set1_epi32(size);

	shifts_static   = _mm256_load_si256((__m256i *) &shifts);
    ones_static     = _mm256_set1_epi32(1);

	for (i = 0, k = 0; k <= limit; k += 8, i++) 
    {
        sample_loader = _mm512_load_epi64(&samples[k]);
        size_loader   = _mm256_load_si256((__m256i *) &sizes[k]);

        // Con estas comparación se busca descartar un bloque de 8 muestras.
        mmask = _mm512_cmp_epu64_mask(sample_comparer, sample_loader, _MM_CMPINT_EQ);
        mmask = _mm256_mask_cmp_epi32_mask(mmask, size_comparer, size_loader, _MM_CMPINT_EQ);
        
		zeros_dynamic = _mm256_load_si256((__m256i *) &zeros[k]);
		zeros_dynamic = _mm256_permutevar8x32_epi32(zeros_dynamic, shifts_static);
        zeros_dynamic = _mm256_maskz_add_epi32(mmask, zeros_dynamic, ones_static);
		_mm256_store_si256 ((__m256i *) &zeros[k], zeros_dynamic);

		if (mmask != 0) {
			printf("Bloque %d disponible\n", i);


			for (j = k; j < k + 8; ++j)
			{
				if (zeros[j] > max_zeros && k < zeros[j])
            	{
                	max_length = k;
                	max_zeros = zeros[j];
            	}
			}
		}
	}

	print_buffer_uint(zeros, 40);
	printf("max_length: %u\n", max_length);
	printf("max_zeros: %u\n", max_zeros);

	return 0;
}
