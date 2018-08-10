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
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>


#include <common/config.h>
#include <common/states.h>
#include <common/types/generic.h>
#include <common/ear_verbose.h>
#include <common/types/configuration/cluster_conf.h>
#include <daemon/app_api/app_conf_api.h>
#include <metrics/ipmi/energy_node.h>

#define close_app_connection()

static char app_to_eard[MAX_PATH_SIZE];
static char eard_to_app[MAX_PATH_SIZE];

static int fd_app_to_eard=-1;
static int fd_eard_to_app=-1;

extern cluster_conf_t my_cluster_conf;
extern int eard_must_exit;
static char *__NAME__="EARD";
extern char *__HOST__;

/*********************************************************/
/***************** PRIVATE FUNCTIONS in this module ******/
/*********************************************************/

static void app_api_set_sigterm()
{
    sigset_t set;
    struct  sigaction sa;
    sigfillset(&set);
    pthread_sigmask(SIG_SETMASK,&set,NULL); 

}


static int connect_with_app()
{
    if (fd_eard_to_app>=0) return EAR_SUCCESS;
    else return EAR_ERROR;
}

static int send_app_answer(app_recv_t *data)
{
	if (write(fd_eard_to_app,data,sizeof(app_recv_t))!=sizeof(app_recv_t))	return EAR_ERROR;
	return EAR_SUCCESS;
}



/*********************************************************/
/***************** FUNCTIONS USED BY EARD ************/
/*********************************************************/
int create_app_connection(char *root)
{
	mode_t old_mask;
    sprintf(app_to_eard,"%s/.app_to_eard",root);
    sprintf(eard_to_app,"%s/.eard_to_app",root);
	old_mask=umask(0);
	// app_to_eard is used to send requests from app to eard
    if (mknod(app_to_eard,S_IFIFO|S_IRUSR|S_IRGRP|S_IWUSR|S_IWGRP|S_IROTH|S_IWOTH,0)<0){
        if (errno!=EEXIST){
			return EAR_ERROR;
        }
    }
	if (mknod(eard_to_app,S_IFIFO|S_IRUSR|S_IRGRP|S_IWUSR|S_IWGRP|S_IROTH|S_IWOTH,0)<0){
        if (errno!=EEXIST){
            return EAR_ERROR;
        }
    }
	umask(old_mask);
	fd_app_to_eard=open(app_to_eard,O_RDWR);
	if (fd_app_to_eard<0){
		return EAR_ERROR;
	}
	fd_eard_to_app=open(eard_to_app,O_RDWR);
	if (fd_eard_to_app<0)	return EAR_ERROR;
	return EAR_SUCCESS;
}

uint read_app_command(app_send_t *app_req)
{
	if (read(fd_app_to_eard,app_req,sizeof(app_send_t))!=sizeof(app_send_t)){
		eard_verbose(0,"Error reading NON-EARL application request\n");
		return INVALID_COMMAND;
	}
	return app_req->req;
}


/** Releases resources to connect with applications */
void dispose_app_connection()
{
	close(fd_app_to_eard);
	close(fd_eard_to_app);
	unlink(app_to_eard);
	unlink(eard_to_app);
}

/*********************************************************/
/***************** FUNCTIONS EXPORTED BY EARD ************/
/*********************************************************/

/** Returns the energy in mJ and the time in ms  */
void ear_energy()
{
	int fd_ack;
	app_recv_t data;
	ulong energy_mj,time_ms;
	
	/* Execute specific request */

	read_dc_energy_time(&energy_mj,&time_ms);

	/* Create connection */
	if (connect_with_app()!=EAR_SUCCESS){
		eard_verbose(0,"Error connecting with NON-EARL application \n");
		return;
	}


	/* Prepare the answer */
	data.ret=EAR_SUCCESS;
	data.my_data.my_energy.energy_mj=energy_mj;
	data.my_data.my_energy.time_ms=time_ms;

	send_app_answer(&data);

	/* Close the connection with app */
	close_app_connection();	
	
	return;

}




/***************************************************************/
/***************** THREAD IMPLEMENTING NON-EARL API ************/
/***************************************************************/

void process_request()
{
	int req;
	app_send_t app_req;
	req=read_app_command(&app_req);
	switch (req){
	case ENERGY_TIME:
		ear_energy();
		break;
	case INVALID_COMMAND:
		eard_verbose(0,"PANIC, invalid command received and not recognized\n");
		break;
	default:
		eard_verbose(0,"PANIC, non-earl command received and not recognized\n");
		break;
	}
	
}

void *eard_non_earl_api_service(void *noinfo)
{
	fd_set rfds;
    fd_set rfds_basic;
	int numfds_ready,numfds_req;
	int max_fd;

	app_api_set_sigterm();

	/* Create connections */
	if (create_app_connection(my_cluster_conf.tmp_dir)!= EAR_SUCCESS){ 
		eard_verbose(0,"Error creating files for non-EARL requests\n");
		pthread_exit(0);
	}
	FD_ZERO(&rfds);
	FD_SET(fd_app_to_eard, &rfds);
	/* fd_eard_to_app is created the last */
    max_fd=fd_app_to_eard;
    numfds_req=max_fd+1;
	rfds_basic=rfds;
	/* Wait for messages */
	eard_verbose(0,"Waiting for non-earl requestst\n");
	while ((eard_must_exit==0) && (numfds_ready=select(numfds_req,&rfds,NULL,NULL,NULL))>=0){
		if (numfds_ready>0){
			/* There is only one fd, it MUST be this one */
        	if (FD_ISSET(fd_app_to_eard,&rfds)){
				/* Process the request  and send the answer */
				process_request();	
			}
		}
		rfds=rfds_basic;	
	}
	/* Close and remove files , never reached if thread is killed */
	dispose_app_connection();
	pthread_exit(0);
}


