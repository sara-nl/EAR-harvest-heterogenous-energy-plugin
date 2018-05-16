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
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <common/ear_verbose.h>
#include <common/shared_configuration.h>
#include <common/states.h>

static int fd;
static const char *__NAME__ = "shared_area:";

// Creates a shared memory region between eard and ear_lib. returns NULL if error.
ear_conf_t * create_ear_conf_shared_area(char * ear_conf_path, ulong max_freq)  
{      	
	int ret;
	void * my_shared_region=NULL;		
	ear_conf_t my_conf;
	char buff[256];
	mode_t my_mask;
	my_mask=umask(0);
	sprintf(buff,"%s/.ear_dyn_conf",ear_conf_path);
	VERBOSE_N(0,"creating file %s for shared memory\n",buff);
	fd=open(buff,O_CREAT|O_RDWR|O_TRUNC,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if (fd<0){
		VERBOSE_N(0,"error creating sharing memory (%s)\n",strerror(errno));
		umask(my_mask);
		return NULL;
	}
	VERBOSE_N(0,"shared file for mmap created\n");
	umask(my_mask);
	// Default values
	my_conf.max_freq=max_freq;
	my_conf.th=PERFORMANCE_GAIN;
	my_conf.force_rescheduling=0;
	VERBOSE_N(0,"writting default values\n");
	ret=write(fd,&my_conf,sizeof(my_conf));
	if (ret<0){
		VERBOSE_N(0,"error creating sharing memory (%s)\n",strerror(errno));
		close(fd);
		return NULL;
	}
	VERBOSE_N(0,"mapping shared memory\n");
	my_shared_region= mmap(NULL, sizeof(my_conf),PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);                                     
	if ((my_shared_region == MAP_FAILED) || (my_shared_region == NULL)){
		VERBOSE_N(0," error creating sharing memory (%s)\n",strerror(errno));
		close(fd);
		return NULL;
	}
	return (ear_conf_t *)my_shared_region;
}

ear_conf_t * attach_ear_conf_shared_area(char * ear_conf_path)
{
    int ret;
    void * my_shared_region=NULL;
    ear_conf_t my_conf;
	char buff[256];
	sprintf(buff,"%s/.ear_dyn_conf",ear_conf_path);
    fd=open(buff,O_RDWR);
    if (fd<0){
        VERBOSE_N(0,"error attaching to sharing memory (%s)\n",strerror(errno));
        return NULL;
    }
    my_shared_region= mmap(NULL, sizeof(my_conf),PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if ((my_shared_region == MAP_FAILED) || (my_shared_region == NULL)){
        VERBOSE_N(0,"error attaching to sharing memory (%s)\n",strerror(errno));
        close(fd);
        return NULL;
    }
    return (ear_conf_t *)my_shared_region;
}                                
void dettach_ear_conf_shared_area()
{
	close(fd);
}
void ear_conf_shared_area_dispose(char * ear_conf_path)
{
	char buff[256];
    sprintf(buff,"%s/.ear_dyn_conf",ear_conf_path);

	close(fd);
	unlink(buff);
}
