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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <math.h>
#include <unistd.h>

#include <linux/limits.h>
#include <common/types/coefficient.h>

#define TURBO_FREQ 2401000

coefficient_t *coeffs;
int num_pstates;


char buffer[1024];
int file_size;
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

    while (string[i] != '\0') {
        if (string[i] == c) {
            last_i = i;
        }
        i++;
    }

    return last_i;
}



void read_file(char *path,coefficient_t *coeffs)
{

    int rfd = open(path, O_RDONLY);
    if (rfd<0){
        printf("Error opening file %s\n",strerror(errno));
        return;
    }   
    if (read(rfd, coeffs, file_size)!=file_size){
        printf("Error reading coefficients for path %s (%s)\n",path,strerror(errno));
    }   

    close(rfd);

}


void print_coefficients(coefficient_t *avg)
{
	int i;
	for (i=0;i<num_pstates;i++){
		coeff_print(&avg[i]);
	}
}

coefficient_t *generate_new_coeffs(int *pstates, int n_pstates, coefficient_t *old_coeffs)
{
    coefficient_t *new_coeffs = calloc(n_pstates*n_pstates, sizeof(coefficient_t));
    int old_n_pstates = (int)sqrt(num_pstates);

    int i, j;
    for (i = 0; i < n_pstates; i++)
    {
        new_coeffs[i].pstate_ref = pstates[0];
        new_coeffs[i].pstate = pstates[i];
    }

    for (i = 1; i < num_pstates; i++)
    {
        memset(&new_coeffs[i*n_pstates], 0, sizeof(coefficient_t));
        new_coeffs[i*n_pstates].pstate_ref = pstates[i];
        new_coeffs[i*n_pstates].pstate = pstates[0];
        
        for (j = 1; j < num_pstates; j++)
        {
            int offset = i*n_pstates+j;
            int second_offset = (i-1)*old_n_pstates+(j-1);
            memcpy(&new_coeffs[offset], &coeffs[second_offset], sizeof(coefficient_t));
        }
    }
    return new_coeffs;

}

int main(int argc, char *argv[])
{
    int fd;
    int i;
    int *reference_pstates;
    int new_num_pstates;
    if (argc<3){ 
        printf("Usage: %s coeffs_file new_coeffs_file\n",argv[0]);
        exit(1);
    }

    fd=open(argv[1],O_RDONLY);
    if (fd<0){
        sprintf(buffer,"Invalid coeffs path %s (%s)\n",argv[1],strerror(errno));
        printf(buffer);
        exit(1);
    }
    file_size=lseek(fd,0,SEEK_END);
    close(fd);
    num_pstates=file_size/sizeof(coefficient_t);
    new_num_pstates = (int)sqrt(num_pstates);
    new_num_pstates++;
    reference_pstates = calloc(new_num_pstates, sizeof(int));
    //new_num_pstates*=new_num_pstates;

    printf("num_states: %d\n", new_num_pstates);
    printf("Coefficients file size %d, pstates %d\n",file_size,num_pstates);
    coeffs=(coefficient_t*)calloc(file_size,1);
    if (coeffs==NULL) {
	    printf("Not enough memory\n");
	    exit(1);
    }
    /* The program reports coefficients in stdout and csv file */
    read_file(argv[1],coeffs);

    reference_pstates[0] = TURBO_FREQ;
    for (i = 1; i < sqrt(num_pstates)+1; i++)
        reference_pstates[i] = coeffs[i-1].pstate;

    coefficient_t *new_coeffs = generate_new_coeffs(reference_pstates, new_num_pstates, coeffs);
    num_pstates = new_num_pstates* new_num_pstates;
    print_coefficients(new_coeffs);

    fd=open(argv[2], O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    write(fd, new_coeffs, sizeof(coefficient_t)*num_pstates);
    free(reference_pstates);
    return 0;
}
