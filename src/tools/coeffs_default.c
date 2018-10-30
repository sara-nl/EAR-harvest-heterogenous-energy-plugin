#include <math.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/limits.h>
#include <common/sizes.h>
#include <common/string_enhanced.h>
#include <common/types/coefficient.h>

// Buffers
char buffer[SZ_PATH];
char buffer_output[SZ_PATH];

// Coefficients
coefficient_t *coeffs_accum;
int *coeffs_num;
int coeffs_max;

// Customize program
int armonize;
int output;

#define SZ_COEFF sizeof(coefficient_t)

/*
 *
 *
 *
 */

int is_regular_file(const char *path)
{
	struct stat path_stat;
	stat(path, &path_stat);
	return S_ISREG(path_stat.st_mode);
}

int find(coefficient_t *coeff)
{
	int found;
	int last;
	int i;

	for (i = 0; i < 200; ++i)
	{
		last  = (coeffs_accum[i].pstate_ref == 0);
		found = (coeffs_accum[i].pstate_ref == coeff->pstate_ref) &&
				(coeffs_accum[i].pstate     == coeff->pstate);

		if (last) {
			coeffs_max = i + 1;
		}

		if (found || last) {
			return i;
		}
	}

	return -1;
}

void accum(coefficient_t *coeff, int i)
{
    coeffs_accum[i].pstate_ref = coeff->pstate_ref;
    coeffs_accum[i].pstate = coeff->pstate;

	if(!coeff->available) {
		return;
	}
	
	coeffs_accum[i].available = 1;
	coeffs_num[i] += 1;

	if (coeff->pstate_ref == coeff->pstate) {
		return;
	}

	if (armonize) {
		coeffs_accum[i].A += 1.0 / coeff->A;
		coeffs_accum[i].B += 1.0 / coeff->B;
		coeffs_accum[i].C += 1.0 / coeff->C;
		coeffs_accum[i].D += 1.0 / coeff->D;
		coeffs_accum[i].E += 1.0 / coeff->E;
		coeffs_accum[i].F += 1.0 / coeff->F;
	} else {
		coeffs_accum[i].A += coeff->A;
		coeffs_accum[i].B += coeff->B;
		coeffs_accum[i].C += coeff->C;
		coeffs_accum[i].D += coeff->D;
		coeffs_accum[i].E += coeff->E;
		coeffs_accum[i].F += coeff->F;
	}
}

void write_file(char *path)
{
    coefficient_t coeff;
    double num;
	size_t r;
    int fd;
    int i;

	#define FD_WRITE 1

	//
	#if FD_WRITE
	fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH);

    if (fd < 0) {
        printf("ERROR while opening file %s (%s)\n", path, strerror(errno));
        return;
    }
	#endif

	//
	tprintf_init(stderr, STR_MODE_DEF, "10 10 6 2 13 10 10 10 10 10");

	//
	tprintf("F. from||F. to||N||Av|| |  A|| B|| C|| D|| E|| F|| G");

	//
	for (i = 0; i < coeffs_max; ++i)
    {
        if (coeffs_accum[i].pstate_ref == coeffs_accum[i].pstate) {
            coeffs_accum[i].A = 1.0;
            coeffs_accum[i].D = 1.0;
        } else if (coeffs_accum[i].available)
        {
            num = (double) coeffs_num[i];

            if (armonize) {
				coeffs_accum[i].A = num / coeffs_accum[i].A;
				coeffs_accum[i].B = num / coeffs_accum[i].B;
				coeffs_accum[i].C = num / coeffs_accum[i].C;
				coeffs_accum[i].D = num / coeffs_accum[i].D;
				coeffs_accum[i].E = num / coeffs_accum[i].E;
				coeffs_accum[i].F = num / coeffs_accum[i].F;
			} else {
				coeffs_accum[i].A = coeffs_accum[i].A / num;
				coeffs_accum[i].B = coeffs_accum[i].B / num;
				coeffs_accum[i].C = coeffs_accum[i].C / num;
				coeffs_accum[i].D = coeffs_accum[i].D / num;
				coeffs_accum[i].E = coeffs_accum[i].E / num;
				coeffs_accum[i].F = coeffs_accum[i].F / num;
			}
        }

		tprintf("%lu||%lu||%d||%d|| | %- 0.3lf||%- 0.3lf||%- 0.3lf||%- 0.3lf||%- 0.3lf||%- 0.3lf",
			coeffs_accum[i].pstate_ref, coeffs_accum[i].pstate, coeffs_num[i], coeffs_accum[i].available,
			coeffs_accum[i].A, coeffs_accum[i].B, coeffs_accum[i].C,
			coeffs_accum[i].D, coeffs_accum[i].E, coeffs_accum[i].F);


		#if FD_WRITE
		if (write(fd, &coeffs_accum[i], SZ_COEFF) != SZ_COEFF) {
 			printf("ERROR while writting coefficients file\n");
            exit(1);
		}
		#endif
		
        #if 0   
        coeff_print(&coeffs_accum[i]);
        #endif
    }

	close(fd);
}

void read_file(char *path, char *node)
{
	coefficient_t coeff;
	size_t r;
	int fd;
	int i;

	fd = open(path, O_RDONLY);

	if (fd < 0) {
		printf("ERROR while opening file %s (%s)\n", path, strerror(errno));
 		exit(1);
	}

	//
	while((r = read(fd, &coeff, SZ_COEFF)) > 0)
	{
		if (r == SZ_COEFF)
		{
			#if 0
			coeff_print(&coeff);
			#endif
			i = find(&coeff);
	
			if (i >= 0) {
				accum(&coeff, i);
			}
		} else {
			printf("ERROR while reading file %s\n", strerror(errno));
			exit(1);
		}
	}

	close(fd);
}

int usage(int argc, char *argv[])
{
	int i = 0;

	if (argc < 3)
	{
		fprintf(stdout, "Usage: %s coeffs.path node.list [OPTIONS...]\n\n", argv[0]);
		fprintf(stdout, "  The coeffs.path includes the island.\n");
		fprintf(stdout, "  The node.list is splitted by spaces.\n");
		fprintf(stdout, "\nOptions:\n");
		fprintf(stdout, "\t-A, --armonize\tDoes the armonic mean, reducing the weight\n");
		fprintf(stdout, "\t\t\tof radical coefficient values.\n");
		fprintf(stdout, "\t-O <file.path>\tSaves the default coefficients file in a\n");
		fprintf(stdout, "\t\t\tfile of custom location.\n");
		
		exit(1);
	}

	for (i = 3; i < argc; ++i)
	{
		if (!armonize)
			armonize = ((strcmp(argv[i], "-A") == 0) ||
						(strcmp(argv[i], "--armonize") == 0));
		if (!output) {
			output = (strcmp(argv[i], "-O") == 0);
			strcpy(buffer_output, argv[i+1]);
		}
	}

	return argc - armonize - (output*2);
}

int main(int argc, char *argv[])
{
	char *path_file;
	char *path_root;
	char *node_name;
	int n;
	int i;

	n = usage(argc, argv);

	// Number of P_STATEs, more than 200 is impossible
	coeffs_accum = calloc(200, sizeof(coefficient_t));
	coeffs_num   = calloc(200, sizeof(unsigned int));	
	path_root = argv[1];
	path_file = buffer;

	//
	for (i = 2; i < n; i++)
	{
		node_name = argv[i];

		/* The program reports coefficients in stdout and csv file */
		sprintf(path_file, "%s/coeffs.%s", path_root, node_name);

		if (is_regular_file(path_file))
		{
			read_file(path_file, node_name);
		}
	}

	//
	if (output) {
		path_file = buffer_output;
	} else {
		sprintf(path_file, "%s/coeffs.default", path_root);
	}
	write_file(path_file);

	return 0;
}
