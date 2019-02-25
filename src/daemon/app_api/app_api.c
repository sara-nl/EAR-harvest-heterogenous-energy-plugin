/**************************************************************
*   Energy Aware Runtime (EAR)
*   This program is part of the Energy Aware Runtime (EAR).
*
*   EAR provides a dynamic, transparent and ligth-weigth solution for
*   Energy management.
*
*       It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017
*   BSC Contact     mailto:ear-support@bsc.es
*   Lenovo contact  mailto:hpchelp@lenovo.com
*
*   EAR is free software; you can redistribute it and/or
*   modify it under the terms of the GNU Lesser General Public
*   License as published by the Free Software Foundation; either
*   version 2.1 of the License, or (at your option) any later version.
*
*   EAR is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   Lesser General Public License for more details.
*
*   You should have received a copy of the GNU Lesser General Public
*   License along with EAR; if not, write to the Free Software
*   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*   The GNU LEsser General Public License is contained in the file COPYING
*/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>


#include <common/config.h>
#include <common/states.h>
#include <common/types/generic.h>
#include <common/output/verbose.h>
#define SHOW_DEBUGS 1
#include <common/output/debug.h>
#include <daemon/app_api/app_conf_api.h>

static int fd_app_to_eard;
static int fd_eard_to_app;
static char app_to_eard[MAX_PATH_SIZE];
static char eard_to_app[MAX_PATH_SIZE];
static char my_app_to_eard[MAX_PATH_SIZE];

static int create_connection()
{
	char *tmp;
	int pid;
	app_send_t req;
    /* Creating the connection */
	debug("create_connection\n");

    tmp=getenv("EAR_TMP");
    if (tmp==NULL){
        verbose(0, "Error, EAR_TMP not defined. Load ear module"); //error
        return EAR_ERROR;
    }
    sprintf(app_to_eard,"%s/.app_to_eard",tmp);
	req.pid=pid=getpid();
	req.req=CONNECT;
	pid=req.pid;
    sprintf(eard_to_app,"%s/.eard_to_app.%d",tmp,pid);
    sprintf(my_app_to_eard,"%s/.app_to_eard.%d",tmp,pid);
	/* This pipe is to send data */

	debug("Creating pipe %s\n",my_app_to_eard);
    if (mknod(my_app_to_eard,S_IFIFO|S_IWUSR|S_IRGRP|S_IROTH,0)<0){
		if (errno!=EEXIST) return EAR_ERROR;
	}
	/* This pipe is to receive data */
	debug("Creating pipe %s\n",eard_to_app);
    if (mknod(eard_to_app,S_IFIFO|S_IRUSR|S_IWGRP|S_IWOTH,0)<0){
		if (errno!=EEXIST) return EAR_ERROR;
    }
	/* EARD connection */	
	debug("opening pipe and sending request\n");
    fd_app_to_eard=open(app_to_eard,O_WRONLY|O_NONBLOCK);
	if (fd_app_to_eard<0) return EAR_ERROR;
	if (write(fd_app_to_eard,&req,sizeof(req))!=sizeof(req)) return EAR_ERROR;
	close(fd_app_to_eard);
	
	/* Now we open our specific connection */
	debug("Connecting\n");
    fd_app_to_eard=open(my_app_to_eard,O_WRONLY|O_NONBLOCK);
	if (fd_app_to_eard<0) return EAR_ERROR;
    fd_eard_to_app=open(eard_to_app,O_RDONLY|O_NONBLOCK);
	if (fd_eard_to_app<0) return EAR_ERROR;

	return EAR_SUCCESS;
}
static int send_request(app_send_t *req)
{
	int ret=EAR_SUCCESS;
	debug("Send request\n");
	if (write(fd_app_to_eard,req,sizeof(app_send_t))!=sizeof(app_send_t)) return EAR_ERROR;

	return ret;
}
static int wait_answer(app_recv_t *rec)
{
        int ret=EAR_SUCCESS;
        fd_set rfds;
        struct timeval tv;
        int retval;

		debug("wait for data\n");
        /* Addin fd_eard_to_app to the fdset */
        FD_ZERO(&rfds);
        FD_SET(fd_eard_to_app, &rfds);

        tv.tv_sec = 0;
        tv.tv_usec = 5000;

        retval = select(fd_eard_to_app+1, &rfds, NULL, NULL, &tv);
        if (retval<=0) return EAR_ERROR;

        /* we can read now */
        if (read(fd_eard_to_app,rec,sizeof(app_recv_t))!=sizeof(app_recv_t)) return EAR_ERROR;
		debug("Data received\n");
        return ret;
}

static void close_connection()
{
	app_send_t req;

	debug("Closing connection\n");
	req.pid=getpid();
	req.req=DISCONNECT;
	if (write(fd_app_to_eard,&req,sizeof(req))!=sizeof(req)) return;
	close(fd_app_to_eard);
	close(fd_app_to_eard);
	unlink(eard_to_app);
	unlink(my_app_to_eard);
}

int ear_energy(ulong *energy_mj,ulong *time_ms)
{
	int ret=EAR_ERROR;
	app_send_t my_req;
	app_recv_t my_data;	

	#if 0
	/* Creating the connection */
	if ((ret=create_connection())!= EAR_SUCCESS) return ret;
	#endif

	/* Preparing the request */
	my_req.req=ENERGY_TIME;

	/* Sending the request */
	if ((ret=send_request(&my_req))!=EAR_SUCCESS) return ret;

	/* Waiting for an answer */
	if (wait_answer(&my_data)!=EAR_SUCCESS) return ret;

	/* Parsing the output */

	*energy_mj=my_data.my_data.my_energy.energy_mj;
	*time_ms=my_data.my_data.my_energy.time_ms;

	#if 0
	/* Closing connection */
	close_connection();
	#endif

	return my_data.ret;
}

int ear_debug_energy(ulong *energy_j,ulong *energy_mj,ulong *time_sec,ulong *time_ms,ulong *os_time_sec,ulong *os_time_ms)
{
        int ret=EAR_ERROR;
        app_send_t my_req;
        app_recv_t my_data;
        *energy_mj=0;
        *energy_j=0;
        *time_ms=0;
        *time_sec=0;
        *os_time_sec=0;
        *os_time_ms=0;

		#if 0
        /* Creating the connection */
        if ((ret=create_connection())!= EAR_SUCCESS) return ret;
		#endif

        /* Preparing the request */
        my_req.req=ENERGY_TIME_DEBUG;

        /* Sending the request */
        if ((ret=send_request(&my_req))!=EAR_SUCCESS) return ret;

        /* Waiting for an answer */
        if ((ret=wait_answer(&my_data))!=EAR_SUCCESS) return ret;

        /* Parsing the output */

        *energy_j=my_data.my_data.my_energy.energy_j;
        *energy_mj=my_data.my_data.my_energy.energy_mj;
        *time_sec=my_data.my_data.my_energy.time_sec;
        *time_ms=my_data.my_data.my_energy.time_ms;
        *os_time_sec=my_data.my_data.my_energy.os_time_sec;
        *os_time_ms=my_data.my_data.my_energy.os_time_ms;
		#if 0
        /* Closing connection */
        close_connection();
		#endif

        return my_data.ret;
}


int ear_connect()
{
	
	return create_connection();
}

int ear_disconnect()
{
	close_connection();
}


/****** UTILS *****/
static unsigned long ulong_diff_overflow(unsigned long begin, unsigned long end)
{
    unsigned long max_16 = USHRT_MAX;
    unsigned long max_32 = ULONG_MAX;
    unsigned long max_48 = 281474976710656; //2^48
    unsigned long max_64 = ULLONG_MAX;

    unsigned long ret = 0;

    if (begin < max_16 && end < max_16)
    {
        ret = max_16 - begin + end;
    }
    else if (begin < max_32 && end < max_32 && max_32 < max_64)
    {
        ret = max_32 - begin + end;
    }
    if (begin < max_48 && end < max_48)
    {
        ret = max_48 - begin + end;
    }
    else
    {
        ret = max_64 - begin + end;
    }
    return ret;
}


void ear_energy_diff(unsigned long ebegin,unsigned long eend, unsigned long *ediff, unsigned long tbegin, unsigned long tend, unsigned long *tdiff)
{
	/* Energy */
	if (eend>ebegin) *ediff=eend-ebegin;
	else *ediff=ulong_diff_overflow(ebegin, eend);
	/* Time */
	if (tend>tbegin) *tdiff=tend-tbegin;
	else *tdiff=ulong_diff_overflow(tbegin,tend);
}
