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

int strpcmp(const char *string1, const char *string2)
{
    int i = 0;

    while (string1[i] != '\0' && string2[i] != '\0')
    {
        if (string1[i] != string2[i]) {
            return -1;
        }
        i++;
    }

    return 0;
}

void read_file(const char *path, char *nodename,int wfd)
{
    coefficient_t *coeffs;
	int size;
	int pstates;

    int rfd = open(path, O_RDONLY);
	if (rfd<0){
		printf("Error opening file %s\n",strerrno(errno));
		return;
	}
	/* Size */
	size=lseek(rfd,0,SEEK_END);
	lseek(rfd,0,SEEK_SET);
	pstates=size/sizeof(coefficient_t);
		
    coeffs = (coefficient_t *) malloc(size);
    

    while((length = read(rfd, coeffs, size)) == size)
    {

	if (coeffs->pstate > 0)
        {
            printf("%s;%s;%lu;%u;%f;%f;%f;%f;%f;%f\n",
               &cp_name[i_node], &cp_name[i_freq],
               coeffs->pstate, coeffs->available,
               coeffs->A, coeffs->B, coeffs->C,
               coeffs->D, coeffs->E, coeffs->F);
            dprintf(wfd, "%s;%s;%lu;%u;%f;%f;%f;%f;%f;%f\n",
               &cp_name[i_node], &cp_name[i_freq],
               coeffs->pstate, coeffs->available,
               coeffs->A, coeffs->B, coeffs->C,
               coeffs->D, coeffs->E, coeffs->F);
        }
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
        printf("Usage: coefficients_to_csv version=2,3 path nodename max_freq min_freq [island_number]\n");
		printf("\t Version is EAR version: 2 or 3\n");
        printf("\t- path: the coefficients path (root for v2, folder for v3)\n");
        printf("\tnodename is short version of hostname\n");
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
	
	printf("Converting coefficients from % node %s from %lu to %lu freqs ", dir_path,nodename,max_freq,min_freq);
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
    sprintf(wfile_path, "%s/coefficients_list.csv", dir_path); 
	wfd = open(wfile_path, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    if (wfd < 0) {
    	printf("Error opening coefficients list file %s (%s)\n", wfile_path,strerror(errno));
    }
   	if (version==2){ 

        printf("Reading files in '%s' with suffix '%s'\n", dir_path, &dir_path[i_root]);       
        dprintf(wfd, "nodename;F_ref;F_n;AVAIL;A;B;C;D;E;F\n");
		ulong cfreq;
		for (cfreq=min_freq;cfreq<=max_freq;cfreq+=100000){
        {
            sprintf(rfile_path, "%s.%s.%lu", dir_path, nodename,cfreq);
            if (is_regular_file(rfile_path)) {
				printf("Processing % file\n",rfile_path);
                read_file(rfile_path, wfd);
            }else{
				printf("%s not a regular file, ignoring it\n",rfile_path);
			}
        }

        close(wfd);
        printf("Reading completed, saved csv '%s'\n", wfile_path);
    }
    else
    {
        printf("Error: the path doesn't exists\n");
        exit(1);
    }
	}else{
	}

    return 0;
}
