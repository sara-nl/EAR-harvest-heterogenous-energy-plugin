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


void read_file_v2(const char *path, char *node,int wfd,ulong ref)
{
    coefficient_t *coeffs;
	int size;
	int pstates;
	char buffer[256];

    int rfd = open(path, O_RDONLY);
	if (rfd<0){
		printf("Error opening file %s\n",strerror(errno));
		return;
	}
	/* Size */
	size=lseek(rfd,0,SEEK_END);
	lseek(rfd,0,SEEK_SET);
	pstates=size/sizeof(coefficient_t);

	printf("%d pstates found in %s\n",pstates,path);
		
    coeffs = (coefficient_t *) malloc(size);
    

	if (read(rfd, coeffs, size)!=size){
		printf("Error reading coefficients (%s)\n",strerror(errno));
	}
	int i;
	for ( i=0;i<pstates;i++){
		print_coefficient(&coeffs[i]);
		dprintf(wfd,"%s;%lu;%lu;%u;%lf;%lf;%lf;%lf;%lf;%lf\n",
		node,ref,coeffs[i].pstate,coeffs[i].available,coeffs[i].A,coeffs[i].B,coeffs[i].C,coeffs[i].D,coeffs[i].E,coeffs[i].F);
		
	}

    close(rfd);
}

void read_file_v3(char *path,char * node,int wfd)
{
    coefficient_v3_t *coeffs;
    int size;
    int pstates,ncoeffs;
    char buffer[256];

    int rfd = open(path, O_RDONLY);
    if (rfd<0){
        printf("Error opening file %s\n",strerror(errno));
        return;
    }   
    /* Size */
    size=lseek(rfd,0,SEEK_END);
	printf("File size %d\n",size);
    lseek(rfd,0,SEEK_SET);
	ncoeffs=size/sizeof(coefficient_v3_t);
    printf("%d ncoeffs found in %s\n",ncoeffs,path);
    
    coeffs = (coefficient_v3_t *) malloc(size);
    

    if (read(rfd, coeffs, size)!=size){
        printf("Error reading coefficients (%s)\n",strerror(errno));
    }   
	int i;
	for (i=0;i<ncoeffs;i++){
        print_coefficient_v3(&coeffs[i]);
        dprintf(wfd,"%s;%lu;%lu;%u;%lf;%lf;%lf;%lf;%lf;%lf\n",
        node,coeffs[i].pstate_ref,coeffs[i].pstate,coeffs[i].available,coeffs[i].A,coeffs[i].B,coeffs[i].C,coeffs[i].D,coeffs[i].E,coeffs[i].F);

	}

    close(rfd);

}


int main(int argc, char *argv[])
{
    struct dirent *directory = NULL;
    char rfile_path[PATH_MAX];
    char wfile_path[PATH_MAX];
    char *dir_path;
    int i_root;
    int i_node;
    int wfd;
    DIR *dd;
	uint version;
	char *nodename;
	uint island;
	ulong max_freq,min_freq;

    if (!((argc == 6 ) || (argc == 7))){ 
        printf("Usage: %s version=2,3 path nodename max_freq min_freq [island_number]\n",argv[0]);
		printf("\t Version is EAR version: 2 or 3\n");
        printf("\t- path: the coefficients path (root for v2, folder for v3)\n");
        printf("\tnodename is short version of hostname\n");
		printf("\tmax_freq min_freq are range of freqs to be considered, 0 for v3 coeffs\n");
		printf("\t island_number is only mandatory for v3 version\n");
		printf("\t coefficients are reported both in stdout and csv file \n");
        exit(1);
    }
	version=atoi(argv[1]);
    dir_path = argv[2];
	nodename=argv[3];
	max_freq=(ulong)atol(argv[4]);
	min_freq=(ulong)atol(argv[5]);
	if (argc==7) island=atoi(argv[6]);
	
	printf("Converting coefficients from %s node %s from %lu to %lu freqs ", dir_path,nodename,max_freq,min_freq);
	if (version==2)
	{
		printf("\n");
	}else if (version==3){
		printf("Version 3 , island %d\n",island);
	}else{ 
		printf("Invalid version\n");
		exit(0);
	}
	/* The program reports coefficients in stdout and csv file */
    sprintf(wfile_path, "./coefficients_list.csv"); 
	wfd = open(wfile_path, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    if (wfd < 0) {
    	printf("Error opening coefficients list file %s (%s)\n", wfile_path,strerror(errno));
    }
   	if (version==2){ 

        dprintf(wfd, "nodename;F_ref;F_n;AVAIL;A;B;C;D;E;F\n");
		ulong cfreq;
		for (cfreq=min_freq;cfreq<=max_freq;cfreq+=100000)
        {
            sprintf(rfile_path, "%s%s.%lu", dir_path, nodename,cfreq);
            if (is_regular_file(rfile_path)) {
				printf("Processing %s file\n",rfile_path);
                read_file_v2(rfile_path, nodename,wfd,cfreq);
            }else{
				printf("%s not a regular file, ignoring it\n",rfile_path);
			}
        }

        printf("Reading completed, saved csv '%s'\n", wfile_path);
	}else{
        dprintf(wfd, "nodename;F_ref;F_n;AVAIL;A;B;C;D;E;F\n");
		ulong cfreq;
        sprintf(rfile_path, "%s/island%d/coeffs.%s", dir_path,island, nodename);
        if (is_regular_file(rfile_path)) {
			printf("Processing %s file\n",rfile_path);
           read_file_v3(rfile_path, nodename,wfd);
        }else{
			printf("%s not a regular file, ignoring it\n",rfile_path);
		}

        printf("Reading completed, saved csv '%s'\n", wfile_path);
	}
    close(wfd);

    return 0;
}
