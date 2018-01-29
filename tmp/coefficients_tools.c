/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
        BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <ear_models/ear_models.h>

struct Coefficients_info *read_coefficients_from_file(char *filename,int *p_states)
{
	int fd,size,ret;
	struct Coefficients_info *my_coeffs;
        fd=open(filename,O_RDONLY);
        if (fd<0){
                fprintf(stderr,"Coefficients file error: %s\n",strerror(errno));
		return NULL;
        }
        size=lseek(fd,0,SEEK_END);
        lseek(fd,0,SEEK_SET);
        // The number of p_states depends on the architecture
        my_coeffs=(struct Coefficients_info *)malloc(size);
        if (my_coeffs==NULL){
                fprintf(stderr,"Erroc malloc returns null (%s)\n",strerror(errno));
		return NULL;
        }
        // Reset the memory to zeroes
        memset(my_coeffs,0,sizeof(struct Coefficients_info));
        *p_states=size/sizeof(struct Coefficients_info);
        fprintf(stderr,"There are %d p_states in the file\n",*p_states);
        // We read the coefficients
        if ((ret=read(fd,my_coeffs,size))!=size){
                fprintf(stderr,"Error when readin coefficients file (%s)\n",strerror(errno));
		free(my_coeffs);
		return NULL;
        }
	return my_coeffs;
}

