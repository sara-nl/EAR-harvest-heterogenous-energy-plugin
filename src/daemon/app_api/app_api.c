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
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <common/config.h>
#include <common/states.h>
#include <common/types/generic.h>
#include <daemon/app_api/app_conf_api.h>

static int fd_app_to_eard;
static int fd_eard_to_app;
static char app_to_eard[MAX_PATH_SIZE];
static char eard_to_app[MAX_PATH_SIZE];

static int create_connection()
{
	char *tmp;
    /* Creating the connection */

    tmp=getenv("EAR_TMP");
    if (tmp==NULL){
        fprintf(stderr,"Error, EAR_TMP not defined. Load ear module\n");
        return EAR_ERROR;
    }
    sprintf(app_to_eard,"%s/.app_to_eard",tmp);
    sprintf(eard_to_app,"%s/.eard_to_app",tmp);
    fd_app_to_eard=open(app_to_eard,O_WRONLY);
	if (fd_app_to_eard<0) return EAR_ERROR;
    fd_eard_to_app=open(eard_to_app,O_RDONLY);
	if (fd_eard_to_app<0) return EAR_ERROR;

	return EAR_SUCCESS;
}
static int send_request(app_send_t *req)
{
	int ret=EAR_SUCCESS;

	if (write(fd_app_to_eard,req,sizeof(app_send_t))!=sizeof(app_send_t)) return EAR_ERROR;

	return ret;
}
static int wait_answer(app_recv_t *rec)
{
	int ret=EAR_SUCCESS;
	if (read(fd_eard_to_app,rec,sizeof(app_recv_t))!=sizeof(app_recv_t)) return EAR_ERROR;
	return ret;
}

static void close_connection()
{
	close(fd_app_to_eard);
	close(fd_app_to_eard);
}

int ear_energy(ulong *energy_mj,ulong *time_ms)
{
	int ret=EAR_ERROR;
	app_send_t my_req;
	app_recv_t my_data;	

	/* Creating the connection */
	if ((ret=create_connection())!= EAR_SUCCESS) return ret;

	/* Preparing the request */
	my_req.req=ENERGY_TIME;

	/* Sending the request */
	if ((ret=send_request(&my_req))!=EAR_SUCCESS) return ret;

	/* Waiting for an answer */
	if (wait_answer(&my_data)!=EAR_SUCCESS) return ret;

	/* Parsing the output */

	*energy_mj=my_data.my_data.my_energy.energy_mj;
	*time_ms=my_data.my_data.my_energy.time_ms;

	/* Closing connection */
	close_connection();

	return my_data.ret;
}
