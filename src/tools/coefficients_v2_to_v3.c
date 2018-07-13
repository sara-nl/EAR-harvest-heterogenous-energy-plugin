/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

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

void from_v2_to_v3(ulong ref,coefficient_t *v2,coefficient_v3_t *v3)
{
	v3->pstate_ref=ref;
	v3->pstate=v2->pstate;
	v3->available=v2->available;
	v3->A=v2->A;
	v3->B=v2->B;
	v3->C=v2->C;
	v3->D=v2->D;
	v3->E=v2->E;
	v3->F=v2->F;
}

void convert_coeffs_v2(const char *path, char *node,int wfd,ulong ref)
{
    coefficient_t *coeffs;
    coefficient_v3_t *coeffs_v3;
	int size;
	int pstates;
	char buffer[256];
	int min_pstate;

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
    coeffs_v3 = (coefficient_v3_t *) malloc(pstates*sizeof(coefficient_v3_t));
    

	if (read(rfd, coeffs, size)!=size){
		printf("Error reading coefficients (%s)\n",strerror(errno));
	}
	int i;
	for ( i=0;i<pstates;i++){
		if ((i>0) && (coeffs[i].pstate!=0)){
			print_coefficient(&coeffs[i]);
			from_v2_to_v3(ref,&coeffs[i],&coeffs_v3[i]);
			write(wfd,&coeffs_v3[i],sizeof(coefficient_v3_t));
		}
	}
    close(rfd);
}



int main(int argc, char *argv[])
{
    struct dirent *directory = NULL;
    char rfile_path[PATH_MAX];
    char wfile_path[PATH_MAX];
    char dir_path_v3_island[PATH_MAX];
    char *dir_path,*dir_path_v3;
    int i_root;
    int i_node;
    int wfd;
    DIR *dd;
	uint version;
	char *nodename;
	uint island;
	ulong max_freq,min_freq;

    if (argc !=7 ) { 
        printf("Usage: %s path_v2 path_v3 nodename max_freq min_freq island_number\n",argv[0]);
        printf("\t- path_v2: the coefficients path  v2\n");
        printf("\t- path_v3: the coefficients path  v3\n");
        printf("\tnodename is short version of hostname\n");
		printf("\tmax_freq min_freq are range of freqs to be considered\n");
		printf("\t island_number \n");
		printf("\t path_v3 must exist, island_number directory is created\n");
        exit(1);
    }
    dir_path = argv[1];
    dir_path_v3 = argv[2];
	nodename=argv[3];
	max_freq=(ulong)atol(argv[4]);
	min_freq=(ulong)atol(argv[5]);
	island=atoi(argv[6]);
	
	printf("Converting coefficients v2 from %s node %s from %lu to %lu freqs , island %d\n", dir_path,nodename,max_freq,min_freq,island);
	/* The program reports coefficients in stdout and csv file */
	sprintf(dir_path_v3_island,"%s/island%d",dir_path_v3,island);
	if ((mkdir(dir_path_v3_island,S_IRWXU|S_IRGRP|S_IROTH)<0) && (errno!=EEXIST)){	
		printf("Error creating %s folder (%s)\n",dir_path_v3_island,strerror(errno));
		exit(0);
	}
    sprintf(wfile_path, "%s/coeffs.%s", dir_path_v3_island, nodename);
	wfd = open(wfile_path, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    if (wfd < 0) {
    	printf("Error opening coefficients list file %s (%s)\n", wfile_path,strerror(errno));
    }

	ulong cfreq;
	for (cfreq=max_freq;cfreq>=min_freq;cfreq-=100000)
    {
            sprintf(rfile_path, "%s%s.%lu", dir_path, nodename,cfreq);
            if (is_regular_file(rfile_path)) {
				printf("Processing %s file\n",rfile_path);
                convert_coeffs_v2(rfile_path, nodename,wfd,cfreq);
            }else{
				printf("%s not a regular file, ignoring it\n",rfile_path);
			}
    }

    printf("Coefficients v3 generated at '%s'\n", wfile_path);
    close(wfd);

    return 0;
}
