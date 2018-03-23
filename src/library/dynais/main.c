#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#include <library/dynais/dynais.h>

void usage()
{
	printf("Usage: dynais_input_file windows_length n_levels output_file\n");
	printf("- input_file: path to the input file (binary)\n");
	printf("- windows_length: size of each level window\n");
	printf("- n_levels: number of levels\n");
	printf("- max_inputs: number of inputs to read (0 = unlimited)\n");
	exit(1);
}

int main(int argc, char **argv)
{
	FILE *input;
	double total_td;
	clock_t init_t, total_t;
	unsigned int level, size;
	unsigned long value[5];
	unsigned int i, panic;
	int result;

	// Application
	if (argc != 5) {
		usage();
	}

	if ((input = fopen(argv[1], "r")) < 0) {
		perror("File read failed.");
		return 0;
	}

	// Algorithms
	dynais_init(atoi(argv[2]), atoi(argv[3]));
	panic = atoi(argv[4]);
	i = 0;

	if (panic == 0) panic = UINT_MAX;
	init_t = clock();

	unsigned long fake_input[10] = { 1, 2, 3, 1, 2, 3, 1, 2, 3, 4 };

	// Reading
	while (fread(&value, 5, sizeof(unsigned long), input) > 0 && i < panic) {
		//value[3] = fake_input[i];
		//printf("-------------------------------------------------\n");
		//printf("Sample %lu\n", value[3]);
		result = dynais(value[3], &size, &level);
		if (result >= 0) printf("Result in %u: l%u, s%u, r%d\n", i, level, size, result);
		++i;
	}

	total_t = clock() - init_t;
	total_td = (double) total_t / ((double) CLOCKS_PER_SEC / 1000000.f);
	//
	fclose(input);

	// Algorithms
	dynais_dispose();

	//
	printf("----------------------------------\n");
	printf("Total inputs: %u (max %u)\n", i, UINT_MAX);
	printf("Total elapsed time: %0.lf us\n", total_td);

	return 0;
}
