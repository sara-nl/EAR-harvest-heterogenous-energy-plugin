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
#include <common/types/coefficient.h>

coefficient_t *coeffs_per_node;
coefficient_t *coeffs_accum;
int num_pstates;

#define IS_DIFFERENT	1
#define IS_EQUAL		0
#define MAX_DIFF		0.1
#define DEF_FREQ		2401000

int file_size;
int num_nodes;
int valid_nodes;
char buffer[1024];

int is_regular_file(const char *path)
{
	struct stat path_stat;
	stat(path, &path_stat);
	return S_ISREG(path_stat.st_mode);
}

int strlst(const char *string, char c)
{
	int last_i = -1;
	int i = 0;

	while (string[i] != '\0')
	{
		if (string[i] == c) {
			last_i = i;
		}
		i++;
	}

	return last_i;
}


void read_file(char *path, coefficient_t *coeffs)
{
	int rfd = open(path, O_RDONLY);

	if (rfd < 0) {
		printf("Error opening file %s\n", strerror(errno));
		return;
	}

	if (read(rfd, coeffs, file_size) != file_size) {
		printf("Error reading coefficients for path %s (%s)\n", path, strerror(errno));
	}

	close(rfd);
}

int equal(double A, double B, double th)
{
	double d;

	if (A > B) {
		d = (A - B) / B;
	} else {
		d = (B - A) / A;
	}

	return (d < th);
}

int compare_coefficients(coefficient_t *avg, coefficient_t *per_node)
{
	float diff;
	int are_equal = 1;
	int i = 0;

	while ((are_equal) && (i < num_pstates))
	{
		if (!equal(avg[i].A, per_node[i].A, MAX_DIFF)) are_equal = 0;
		if (!are_equal || (!equal(avg[i].B, per_node[i].B, MAX_DIFF))) are_equal = 0;
		if (!are_equal || (!equal(avg[i].C, per_node[i].C, MAX_DIFF))) are_equal = 0;
		if (!are_equal || (!equal(avg[i].D, per_node[i].D, MAX_DIFF))) are_equal = 0;
		if (!are_equal || (!equal(avg[i].E, per_node[i].E, MAX_DIFF))) are_equal = 0;
		if (!are_equal || (!equal(avg[i].F, per_node[i].F, MAX_DIFF))) are_equal = 0;

		i++;
	}

	if (are_equal) {
		return IS_EQUAL;
	} else {
		return IS_DIFFERENT;
	}
}

void accum_coeffs(coefficient_t *avg, coefficient_t *cnew)
{
	int i;

	for (i = 0; i<num_pstates; i++)
	{
		avg[i].pstate_ref += cnew[i].pstate_ref;
		avg[i].available  += cnew[i].available;
		avg[i].pstate     += cnew[i].pstate;

		avg[i].A += cnew[i].A;
		avg[i].B += cnew[i].B;
		avg[i].C += cnew[i].C;
		avg[i].D += cnew[i].D;
		avg[i].E += cnew[i].E;
		avg[i].F += cnew[i].F;
	}
}

void compute_average(coefficient_t *avg, int valid_nodes)
{
	int i;

	for (i = 0; i < num_pstates; i++)
	{
		avg[i].pstate_ref = avg[i].pstate_ref / valid_nodes;
		avg[i].available  = avg[i].available / valid_nodes;
		avg[i].pstate     = avg[i].pstate / valid_nodes;

		avg[i].A = avg[i].A / (double) valid_nodes;
		avg[i].B = avg[i].B / (double) valid_nodes;
		avg[i].C = avg[i].C / (double) valid_nodes;
		avg[i].D = avg[i].D / (double) valid_nodes;
		avg[i].E = avg[i].E / (double) valid_nodes;
		avg[i].F = avg[i].F / (double) valid_nodes;
	}
}

void print_coefficients(coefficient_t *avg)
{
	int i;

	for (i = 0; i < num_pstates; i++)
	{
		coeff_print(&avg[i]);

		#if 0
		printf("Pstate %d A=%lf B=%lf C=%lf D=%lf E=%lf F=%lf\n",
			i, avg[i].A, avg[i].B, avg[i].C, avg[i].D, avg[i].E, avg[i].F);
		#endif
	}
}


int main(int argc, char *argv[])
{
	char *dir_path;
	char rfile_path[PATH_MAX];
	int total_warning = 0;
	int curr_node = 2;
	int fd;
	int i;

	if (argc < 4) {
		printf("Usage: %s coeffs_path(including island) node1 node2 [...noden]\n", argv[0]);
		exit(1);
	}

	dir_path = argv[1];
	num_nodes = argc - 2;

	sprintf(rfile_path, "%s/coeffs.%s", dir_path, argv[2]);
	fd = open(rfile_path, O_RDONLY);

	if (fd < 0)
	{
		sprintf(buffer, "Invalid coeffs path %s (%s)\n", rfile_path, strerror(errno));
		printf(buffer);
		exit(1);
	}

	//
	file_size = lseek(fd, 0, SEEK_END);

	close(fd);

	num_pstates = file_size / sizeof(coefficient_t);
	printf("Coefficients file size %d, pstates %d\n", file_size, num_pstates);
	printf("Computed with %s\n", rfile_path);

	//
	coeffs_per_node = (coefficient_t *) calloc(file_size * (argc - 2), 1);
	coeffs_accum = (coefficient_t *) calloc(file_size, 1);

	if ((coeffs_accum == NULL) || (coeffs_per_node == NULL))
	{
		printf("Not enough memory\n");
		exit(1);
	}

	for (i = 2; i < argc; i++)
	{
		/* The program reports coefficients in stdout and csv file */
		sprintf(rfile_path, "%s/coeffs.%s", dir_path, argv[i]);

		if (is_regular_file(rfile_path))
		{
			printf("Processing %s file\n", rfile_path);
			read_file(rfile_path, &coeffs_per_node[(i - 2) * num_pstates]);

			if (coeffs_per_node[(i - 2) * num_pstates].pstate_ref == DEF_FREQ)
			{
				accum_coeffs(coeffs_accum, &coeffs_per_node[(i - 2) * num_pstates]);

				#if 0
				print_coefficients(&coeffs_per_node[(i-2)*num_pstates]);
				printf("%s is a valid coefficient file\n",argv[i]);
				#endif

				valid_nodes++;
			} else {
				#if 0
				printf("Default freq not valid %lu\n",coeffs_per_node[(i-2)*num_pstates].pstate_ref);
				#endif
			}

			#if 0
			print_coefficients(&coeffs_per_node[(i-2)*num_pstates]);
			getchar();
			#endif
		} else {
			printf("%s not a regular file, ignoring it\n", rfile_path);
		}
	}

	printf("There are %d valid nodes \n", valid_nodes);
	compute_average(coeffs_accum, valid_nodes);
	printf("Average coefficients-----\n");
	print_coefficients(coeffs_accum);

	for (i = 0; i < argc - 2; i++)
	{
		if (coeffs_per_node[i * num_pstates].pstate_ref == DEF_FREQ)
		{
			if (compare_coefficients(coeffs_accum, &coeffs_per_node[i * num_pstates]) == IS_DIFFERENT)
			{
				printf("Node %s is (at least) %lf different than average in some coefficient\n", argv[i + 2], MAX_DIFF);
				total_warning++;

				#if 0
				print_coefficients(&coeffs_per_node[i*num_pstates]);
				#endif
			}
		}
	}

	printf("Total warnings %d\n", total_warning);

#if 0
	printf("Generating coeffs.default at /tmp/coeffs.default\n");

	fd = open("/tmp/coeffs.default", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	if (fd < 0) {
		printf("Error, default file can not be generated (%s)\n", strerror(errno));
		exit(1);
	}

	if (write(fd, coeffs_accum, file_size) != file_size) {
		printf("Error writting default file (%s)\n", strerror(errno));
	}

	close(fd);
#endif

	return 0;
}