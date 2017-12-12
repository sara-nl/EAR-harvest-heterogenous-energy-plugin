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
#include <ear_models/ear_models.h>
#include <linux/limits.h>

void read_file(char *path)
{
    printf("\tOpening %s\n", path);
    struct Coefficients_info *coeffs;
    int fd = open(path, O_RDONLY);
    int size = sizeof(struct Coefficients_info);
    int length;

    coeffs = (struct Coefficients_info *) malloc(size);

    while((length = read(fd, coeffs, size)) == size)
    {
	if (coeffs->pstate > 0)
        printf("\t\t(%lu,%u,%f,%f,%f,%f,%f,%f)\n",
               coeffs->pstate, coeffs->available,
               coeffs->A, coeffs->B, coeffs->C,
               coeffs->D, coeffs->E, coeffs->F);
    }

    close(fd);
}

int main(int argc, char *argv[])
{
    struct dirent *directory = NULL;
    char file_path[PATH_MAX];
    char *dir_path;
    DIR *dd;

    dir_path = argv[1];
    dd = opendir(dir_path);

    if (dd != NULL)
    {
        printf("Reading files in %s\n", dir_path);

        while ((directory = readdir(dd)) != NULL)
        {
            if (strcmp(directory->d_name, "..") != 0 &&
                strcmp(directory->d_name, ".") != 0)
            {
                sprintf(file_path, "%s/%s", dir_path, directory->d_name);
                read_file(file_path);
            }
        }

        closedir(dd);
        printf("Reading completed.\n");
    }
    else
    {
        printf("Error: the path doesn't exists.\n");
        exit(1);
    }

    return 0;
}
