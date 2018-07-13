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
#include <strings.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <common/ear_verbose.h>
#include <common/states.h>
#include <common/types/configuration/cluster_conf.h>
#include <common/types/coefficient.h>
#include <daemon/shared_configuration.h>

static int fd;
static const char *__NAME__ = "EARD_shared";
extern char *__HOST__;


/** These functions created path names, just to avoid problems if changing the path name in the future */
/** This functions creates the name of the file mapping the shared memory for the dynamic power settings, it is placed at TMP 
 */
int  get_settings_conf_path(char *tmp,char *path)
{
	sprintf(path,"%s/.ear_settings_conf",tmp);
	
}
/** This functions creates the name of the file mapping the shared memory for the resched flag, it is placed at TMP 
 */
int  get_resched_path(char *tmp,char *path)
{
	sprintf(path,"%s/.ear_resched",tmp);
}

int get_coeffs_path(char *tmp,char *path)
{
	sprintf(path,"%s/.ear_coeffs",tmp);
}

/** BASIC FUNCTIONS */

void *create_shared_area(char *path,char *data,int area_size,int *shared_fd,int must_clean)
{
	/* This function creates a shared memory region based on files and mmap */
	int ret;
	void * my_shared_region=NULL;		
	char buff[256];
	mode_t my_mask;
	my_mask=umask(0);
	strcpy(buff,path);
	eard_verbose(0,"creating file %s for shared memory\n",buff);
	fd=open(buff,O_CREAT|O_RDWR|O_TRUNC,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if (fd<0){
		eard_verbose(0,"error creating sharing memory (%s)\n",strerror(errno));
		umask(my_mask);
		return NULL;
	}
	eard_verbose(0,"shared file for mmap created\n");
	umask(my_mask);
	// Default values
	if (must_clean) bzero(data,area_size);
	eard_verbose(0,"writting default values\n");
	ret=write(fd,data,area_size);
	if (ret<0){
		eard_verbose(0,"error creating sharing memory (%s)\n",strerror(errno));
		close(fd);
		return NULL;
	}
	eard_verbose(0,"mapping shared memory\n");
	my_shared_region= mmap(NULL, area_size,PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);                                     
	if ((my_shared_region == MAP_FAILED) || (my_shared_region == NULL)){
		eard_verbose(0," error creating sharing memory (%s)\n",strerror(errno));
		close(fd);
		return NULL;
	}
	*shared_fd=fd;
	return my_shared_region;
}

void * attach_shared_area(char *path,int area_size,uint perm,int *shared_fd,int *s)
{
    int ret;
    void * my_shared_region=NULL;
	char buff[256];
	int flags;
	int size;
	strcpy(buff,path);
    fd=open(buff,perm);
    if (fd<0){
        eard_verbose(0,"error attaching to sharing memory (%s)\n",strerror(errno));
        return NULL;
    }
	if (area_size==0){
		size=lseek(fd,0,SEEK_END);
		if (size<0){
			eard_verbose(0,"Error computing shared memory size (%s) ",strerror(errno));
		}else area_size=size;
		if (s!=NULL) *s=size;
	}
	if (perm==O_RDWR){
		flags=PROT_READ|PROT_WRITE;
	}else{
		flags=PROT_READ;
	}
    my_shared_region= mmap(NULL, area_size,flags, MAP_SHARED, fd, 0);
    if ((my_shared_region == MAP_FAILED) || (my_shared_region == NULL)){
        eard_verbose(0,"error attaching to sharing memory (%s)\n",strerror(errno));
        close(fd);
        return NULL;
    }
	*shared_fd=fd;
    return my_shared_region;
}

void dettach_shared_area(int fd)
{
	close(fd);
}
void dispose_shared_area(char *path,int fd)
{
	close(fd);
	unlink(path);
}

/***** SPECIFIC FUNCTIONS *******************/

static int fd_cluster,fd_settings,fd_resched,fd_coeffs;

//// SETTINGS

// Creates a shared memory region between eard and ear_lib. returns NULL if error.
settings_conf_t * create_settings_conf_shared_area(char * path)  
{      	
	settings_conf_t my_settings;
	
	return (settings_conf_t *)create_shared_area(path,(char *)&my_settings,sizeof(my_settings),&fd_settings,1);
	
}

settings_conf_t * attach_settings_conf_shared_area(char * path)
{
    return (settings_conf_t *)attach_shared_area(path,sizeof(settings_conf_t),O_RDONLY,&fd_settings,NULL);
}                                
void dettach_settings_conf_shared_area()
{
	dettach_shared_area(fd_settings);
}

void settings_conf_shared_area_dispose(char * path)
{
	dispose_shared_area(path,fd_settings);
}


/// RESCHED

// Creates a shared memory region between eard and ear_lib. returns NULL if error.
resched_t * create_resched_shared_area(char * path)  
{      	
	resched_t my_settings;
	
	return (resched_t *)create_shared_area(path,(char *)&my_settings,sizeof(my_settings),&fd_resched,1);
	
}

resched_t * attach_resched_shared_area(char * path)
{
    return (resched_t *)attach_shared_area(path,sizeof(resched_t),O_RDWR,&fd_resched,NULL);
}                                
void dettach_resched_shared_area()
{
	dettach_shared_area(fd_resched);
}
void resched_shared_area_dispose(char * path)
{
	dispose_shared_area(path,fd_resched);
}

/* COEFFS */

coefficient_v3_t * create_coeffs_shared_area(char * path,coefficient_v3_t *coeffs,int size)
{

    return (coefficient_v3_t *)create_shared_area(path,(char *)coeffs,size,&fd_coeffs,0);

}

coefficient_v3_t * attach_coeffs_shared_area(char * path,int *size)
{
    return (coefficient_v3_t *)attach_shared_area(path,0,O_RDONLY,&fd_coeffs,size);
}

void coeffs_shared_area_dispose(char * path)
{
    dispose_shared_area(path,fd_coeffs);
}

void dettach_coeffs_shared_area()
{
    dettach_shared_area(fd_coeffs);
}



