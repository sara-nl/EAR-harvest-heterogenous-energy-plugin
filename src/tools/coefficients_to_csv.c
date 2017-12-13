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
#include <ear_models/ear_models.h>

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

void read_file(const char *path, const char *name, int i_node, int wfd)
{
    char cp_name[PATH_MAX];
    struct Coefficients_info *coeffs;
    int size = sizeof(struct Coefficients_info);
    int rfd = open(path, O_RDONLY);
    int i_freq;
    int i_host;
    int length;

    coeffs = (struct Coefficients_info *) malloc(size);
    strcpy(cp_name, name);
    
    i_freq = strlst(cp_name, '.');
    cp_name[i_freq] = '\0';
    i_freq += 1;

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

    if (argc != 2) {
        printf("Usage: coefficients_to_csv ear_db_pathname\n");
        printf("\t- ear_db_pathname: the content of that EAR environment variable.\n");
        printf("\tIf node files doesn't contain any suffix, i.e. in coef.node01.2500000\n");
        printf("\tthe suffix is 'coef.', add a slash '/' at the end of ear_db_pathname.\n");
        exit(1);
    }

    dir_path = argv[1];
    
    if ((i_root = strlst(dir_path, '/')) == -1) {
        printf("Invalid path\n");
        exit(1);
    }
   
    dir_path[i_root++] = '\0';
    i_node = strlen(&dir_path[i_root]);
    dd = opendir(dir_path);

    if (dd != NULL)
    {
        sprintf(wfile_path, "%s/coefficients_list.csv", dir_path); 
	wfd = open(wfile_path, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);

        if (wfd < 0) {
            printf("Error opening coefficients list file %s\n", wfile_path);
        }

        printf("Reading files in '%s' with suffix '%s'\n", dir_path, &dir_path[i_root]);
        
        while ((directory = readdir(dd)) != NULL)
        {
            sprintf(rfile_path, "%s/%s", dir_path, directory->d_name);
            
            if (is_regular_file(rfile_path) && strpcmp(&dir_path[i_root], directory->d_name) == 0) {
                read_file(rfile_path, directory->d_name, i_node, wfd);
            }
        }

        close(wfd);
        closedir(dd);
        printf("Reading completed, saved csv '%s'\n", wfile_path);
    }
    else
    {
        printf("Error: the path doesn't exists\n");
        exit(1);
    }

    return 0;
}
