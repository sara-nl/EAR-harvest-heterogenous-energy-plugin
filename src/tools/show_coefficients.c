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


int main(int argc, char *argv[])
{
    int fd;
    int i;

    if (argc<2){ 
        printf("Usage: %s coeffs_file\n",argv[0]);
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
    printf("Coefficients file size %d, pstates %d\n",file_size,num_pstates);
    coeffs=(coefficient_t*)calloc(file_size,1);
    if (coeffs==NULL) {
	printf("Not enough memory\n");
	exit(1);
    }
    /* The program reports coefficients in stdout and csv file */
    read_file(argv[1],coeffs);
    print_coefficients(coeffs);

    return 0;
}
