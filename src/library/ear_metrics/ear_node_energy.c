/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ear_daemon_client.h>
#include <ear_verbose.h>

#define MAX_LINE_LEN 	255
#define SYSFS_PATH_MAX 	255
#define ENERGY1_INPUT 	0
#define NODE_ENERGY

#ifndef PATH_TO_AEM
#define PATH_TO_AEM "/sys/devices/platform/aem.0/"
#endif
#ifdef NODE_ENERGY
static unsigned long *energy_init,*energy_end,*acum_energy;
#endif
static unsigned ear_ibmaem=1;

/* helper function to read file from /sys into given buffer */
/* fname is a relative path under "aem.0" dir */
unsigned int sysfs_read_file(const char *fname, char *buf, size_t buflen)
{
	char path[SYSFS_PATH_MAX];
	int fd;
	size_t numread;

	snprintf(path, sizeof(path), PATH_TO_AEM "%s", fname);

	if ( ( fd = open(path, O_RDONLY) ) == -1 ){
		ear_verbose(0,"EAR(%s) Opening /sys/devices/platform/aem.0/energy1_input file :%s\n",__FILE__,strerror(errno));
		return 0;
	}

	numread = read(fd, buf, buflen - 1);
	if ( numread < 1 )
	{
		ear_verbose(0,"EAR(%s): Warning, reading 0 bytes from energy1_input file closing file:%s\n",__FILE__,strerror(errno));
		close(fd);
		return 0;
	}

	buf[numread] = '\0';
	close(fd);

	return numread;
}

/* read access to files which contain one numeric value (unsigned long int) */

unsigned long int read_dc_energy()
{
 	unsigned long value;
#ifdef IBMAEM
	unsigned int len;
	char linebuf[MAX_LINE_LEN];
	char *endp;

	if (!ear_ibmaem) return 0;

	if ( ( len = sysfs_read_file("energy1_input", linebuf, sizeof(linebuf))) == 0 )
	{
		ear_verbose(0,"EAR(%s): sys_read returns 0:%s\n",__FILE__,strerror(errno));
		return 0;
	}
	//strtol(str, &temp, 0);

	value = strtoul(linebuf, &endp, 0);
	if ((endp == linebuf) || ((value == ULONG_MAX) && (errno == ERANGE))){

	//if ( (endp == linebuf) || (errno == ERANGE) ){
		ear_debug(0,"EAR(%s) WARNING: energy value out of range (endp %x linebuf %x) (errno %d ERANGE %d) ENERGY=%s\n",__FILE__,endp,linebuf,errno,ERANGE,linebuf);
		return 0;
	}
#else
	ear_daemon_client_node_dc_energy(acum_energy);
	value=*acum_energy*1000;	
#endif

	return value;
}
// Returns -1 if DC file doesn't exist or we cannot access it
int init_dc_energy()
{
#ifdef IBMAEM
	int fd;
	fd=open("/sys/devices/platform/aem.0/energy1_input",O_RDONLY);
	if (fd<0){ 
		ear_verbose(0,"EAR(%s) ERROR IBMAEM file not available (/sys/devices/platform/aem.0/energy1_input).%s\n",__FILE__,strerror(errno));
		ear_ibmaem=0;
		return -1;
	}
#else
	int node_energy_size;
	node_energy_size=ear_daemon_client_node_energy_data_size();	
	energy_init=(unsigned long *)malloc(node_energy_size);
	energy_end=(unsigned long *)malloc(node_energy_size);
	acum_energy=(unsigned long *)malloc(node_energy_size);
	ear_daemon_client_node_dc_energy(energy_init);
#endif
	return 0;
}
int end_dc_energy()
{
#ifdef IBMAEM
#else
	ear_daemon_client_node_dc_energy(energy_end);
	//ear_verbose(1,"Total energy consumed %luJ \n",(*energy_end-*energy_init)/1000);
	return 0;
#endif
}

unsigned long int energy_diff(unsigned long int end,unsigned long int init)
{
    unsigned long int to_max;
    // ULONG_MAX
    if (end<init){
        ear_debug(0,"EAR(%s) ENERGY END < INIT %llu - %llu (max %llu)\n",__FILE__,end,init,ULONG_MAX);
        to_max=ULONG_MAX-init;
        return (to_max+end);    
    }else return (end-init);
}

