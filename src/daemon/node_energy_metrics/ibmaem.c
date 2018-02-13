/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <ear_verbose.h>
#ifndef PATH_TO_AEM
#define PATH_TO_AEM "/sys/devices/platform/aem.0/"
#endif
#define MAX_LINE_LEN 255
#define SYSFS_PATH_MAX 255
#define ENERGY1_INPUT 0

static unsigned ear_ibmaem=-1;

/* helper function to read file from /sys into given buffer */
/* fname is a relative path under "aem.0" dir */
unsigned int sysfs_read_file( char *buf, size_t buflen)
{
	size_t numread;

	if (lseek(ear_ibmaem,0,SEEK_SET)<0){
		return 0;
	}

	numread = read(ear_ibmaem, buf, buflen - 1);
	if ( numread < 1 )
	{
		ear_verbose(0,"EAR(%s): Warning, reading 0 bytes from energy1_input file closing file:%s\n",__FILE__,strerror(errno));
		close(ear_ibmaem);
		ear_ibmaem=-1;
		return 0;
	}

	buf[numread] = '\0';
	return numread;
}

//  energy value provided by IBMAEM. Energy is reported in mJ
// Returns 0 on successand -1 on error
int ibmaem_read_dc_energy(unsigned long *energy)
{
 	unsigned long value;
	unsigned int len;
	char linebuf[MAX_LINE_LEN];
	char *endp;
	*energy = 0;

	if (ear_ibmaem < 0) {
		ear_verbose(0,"EAR: IBMAEM not available in ibmaem_read_dc_energy\n");
		return -1;
	}

	if ((len = sysfs_read_file(linebuf, sizeof(linebuf))) == 0)
	{
		ear_verbose(0,"EAR(%s): sys_read returns 0:%s\n",__FILE__,strerror(errno));
		return -1;
	}

	value = strtoul(linebuf, &endp, 0);
	if ((endp == linebuf) || ((value == ULONG_MAX) && (errno == ERANGE))){

		ear_debug(0,"EAR(%s) WARNING: energy value out of range (endp %x linebuf %x) (errno %d ERANGE %d) ENERGY=%s\n",__FILE__,endp,linebuf,errno,ERANGE,linebuf);
		return -1;
	}
	// IBMAEM returns uJ
	*energy=value/1000;
	return 0;
}

/* AC energy is not supported */
int ibmaem_read_ac_energy(unsigned long *energy)
{
	*energy=0;
	return 0;
}
// Returns -1 if DC file doesn't exist or we cannot access it
// Returns 0 on success -1 on error
int ibmaem_node_energy_init()
{
	int fd;
	fd=open("/sys/devices/platform/aem.0/energy1_input",O_RDONLY);
	if (fd<0){ 
		ear_verbose(0,"EAR(%s) ERROR IBMAEM file not available (/sys/devices/platform/aem.0/energy1_input).%s\n",__FILE__,strerror(errno));
		ear_ibmaem=-1;
		return -1;
	}
	ear_ibmaem=fd;
	return 0;
}

int ibmaem_node_energy_dispose()
{
	if (ear_ibmaem>0) close(ear_ibmaem);
	ear_ibmaem=-1;
	return 0;
}

int ibmaem_count_energy_data_length()
{
    int ret;
    return sizeof(unsigned long);
}

